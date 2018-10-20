/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* OS2 IO module
 *
 * Assumes synchronous I/O.
 *
 */

#include "primpl.h"
#include "prio.h"
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <fcntl.h>
#include <io.h>

struct _MDLock               _pr_ioq_lock;

void
_PR_MD_INIT_IO()
{
}

PRStatus
_PR_MD_WAIT(PRThread *thread, PRIntervalTime ticks)
{
    PRInt32 rv;
    ULONG count;

    PRUint32 msecs = (ticks == PR_INTERVAL_NO_TIMEOUT) ?
        SEM_INDEFINITE_WAIT : PR_IntervalToMilliseconds(ticks);
    rv = SafeWaitEventSem(thread->md.blocked_sema, msecs);
    DosResetEventSem(thread->md.blocked_sema, &count); 
    switch(rv) 
    {
        case NO_ERROR:
            return PR_SUCCESS;
            break;
        case ERROR_TIMEOUT:
            _PR_THREAD_LOCK(thread);
            if (thread->state == _PR_IO_WAIT) {
			  ;
            } else {
                if (thread->wait.cvar != NULL) {
                    thread->wait.cvar = NULL;
                    _PR_THREAD_UNLOCK(thread);
                } else {
                    /* The CVAR was notified just as the timeout
                     * occurred.  This led to us being notified twice.
                     * call SemRequest() to clear the semaphore.
                     */
                    _PR_THREAD_UNLOCK(thread);
                    rv = SafeWaitEventSem(thread->md.blocked_sema, 0);
                    DosResetEventSem(thread->md.blocked_sema, &count); 
                    PR_ASSERT(rv == NO_ERROR);
                }
            }
            return PR_SUCCESS;
            break;
        default:
            break;
    }
    return PR_FAILURE;
}
PRStatus
_PR_MD_WAKEUP_WAITER(PRThread *thread)
{
    if ( _PR_IS_NATIVE_THREAD(thread) ) 
    {
        if (DosPostEventSem(thread->md.blocked_sema) != NO_ERROR)
            return PR_FAILURE;
        else
            return PR_SUCCESS;
    }
}

PRInt32
_PR_MD_OPEN(const char *name, PRIntn flags, int mode)
{
    /* NOTE: the below code is mostly borrowed from unix.c */
    PRInt32 osflags;
    PRInt32 rv;

    if (flags & PR_RDWR) {
        osflags = O_RDWR;
    } else if (flags & PR_WRONLY) {
        osflags = O_WRONLY;
    } else {
        osflags = O_RDONLY;
    }

    if (flags & PR_EXCL)
        osflags |= O_EXCL;
    if (flags & PR_APPEND)
        osflags |= O_APPEND;
    if (flags & PR_TRUNCATE)
        osflags |= O_TRUNC;
    if (flags & PR_SYNC) {
#if defined(O_SYNC)
        osflags |= O_SYNC;
#elif defined(O_FSYNC)
        osflags |= O_FSYNC;
#else
#error "Neither O_SYNC nor O_FSYNC is defined on this platform"
#endif
    }

    if (flags & PR_CREATE_FILE)
        osflags |= O_CREAT;

    /* Files opened by NSPR are not expected to perform EOL translation */
    rv = open(name, osflags | O_BINARY, mode);

    if (rv < 0) {
        _PR_MD_MAP_OPEN_ERROR(_MD_ERRNO());
    }

    return rv;
}

PRInt32
_PR_MD_READ(PRFileDesc *fd, void *buf, PRInt32 amount)
{
    PRInt32 rv;

    rv = read(fd->secret->md.osfd, buf, amount);
    if (rv < 0) {
        _PR_MD_MAP_READ_ERROR(_MD_ERRNO());
    }

    return rv;
}

PRInt32
_PR_MD_WRITE(PRFileDesc *fd, const void *buf, PRInt32 amount)
{
    PRInt32 rv;

    rv = write(fd->secret->md.osfd, buf, amount);
    if (rv < 0) {
        _PR_MD_MAP_READ_ERROR(_MD_ERRNO());
    }

    return rv;
}

PROffset64
_PR_MD_LSEEK64(PRFileDesc *fd, PROffset64 offset, PRSeekWhence whence)
{
    PR_STATIC_ASSERT(sizeof(off_t) == sizeof(PROffset64));

    PROffset64 rv, where;

    switch (whence) {
        case PR_SEEK_SET:
            where = SEEK_SET;
            break;
        case PR_SEEK_CUR:
            where = SEEK_CUR;
            break;
        case PR_SEEK_END:
            where = SEEK_END;
            break;
        default:
            PR_SetError(PR_INVALID_ARGUMENT_ERROR, 0);
            return -1;
    }
    rv = lseek(fd->secret->md.osfd, offset, where);
    if (rv == -1) {
        _PR_MD_MAP_LSEEK_ERROR(_MD_ERRNO());
    }

    return rv;
}

PROffset32
_PR_MD_LSEEK(PRFileDesc *fd, PROffset32 offset, PRSeekWhence whence)
{
    return _PR_MD_LSEEK64(fd, offset, whence);
}

PRInt32
_PR_MD_FSYNC(PRFileDesc *fd)
{
    PRInt32 rv;

    rv = fsync(fd->secret->md.osfd);
    if (rv == -1) {
        _PR_MD_MAP_FSYNC_ERROR(_MD_ERRNO());
    }

    return rv;
}

PRInt32
_MD_CloseFile(PRInt32 osfd)
{
    PRInt32 rv;

    rv = close(osfd);
    if (rv == -1) {
        _PR_MD_MAP_CLOSE_ERROR(_MD_ERRNO());
    }

    return rv;
}

PRInt32
_PR_MD_CLOSE_DIR(_MDDir *d)
{
    PRInt32 rv = 0;

    if (d->d) {
        rv = closedir(d->d);
        if (rv == -1) {
            _PR_MD_MAP_CLOSEDIR_ERROR(_MD_ERRNO());
        }
    }
    return rv;
}

PRStatus
_PR_MD_OPEN_DIR(_MDDir *d, const char *name)
{
    d->d = opendir(name);
    if (!d->d) {
        _PR_MD_MAP_OPENDIR_ERROR(_MD_ERRNO());
        return PR_FAILURE;
    }
    return PR_SUCCESS;
}

char *
_PR_MD_READ_DIR(_MDDir *d, PRIntn flags)
{
    struct dirent *de;

    for (;;) {
        /*
          * XXX: readdir() is not MT-safe. There is an MT-safe version
          * readdir_r() on some systems.
          */
        _MD_ERRNO() = 0;
        de = readdir(d->d);
        if (!de) {
            _PR_MD_MAP_READDIR_ERROR(_MD_ERRNO());
            return 0;
        }
        if ((flags & PR_SKIP_DOT) &&
            (de->d_name[0] == '.') && (de->d_name[1] == 0))
            continue;
        if ((flags & PR_SKIP_DOT_DOT) &&
            (de->d_name[0] == '.') && (de->d_name[1] == '.') &&
            (de->d_name[2] == 0))
            continue;
        /* TODO: Respect only the OS/2 attribute for hidden files for now */
#if 1
        if ((flags & PR_SKIP_HIDDEN) && (de->d_attr & FILE_HIDDEN))
#else
        if ((flags & PR_SKIP_HIDDEN) && (de->d_name[0] == '.'))
#endif
            continue;
        break;
    }
    return de->d_name;
}

PRInt32
_PR_MD_DELETE(const char *name)
{
    PRInt32 rv;

    rv = unlink(name);
    if (rv == -1) {
        _PR_MD_MAP_DELETE_ERROR(_MD_ERRNO());
    }

    return rv;
}

PRInt32
_PR_MD_STAT(const char *fn, struct stat *info)
{
    PRInt32 rv;

    rv = _stat(fn, info);
    if (rv == -1) {
        /*
         * Check for MSVC runtime library _stat() bug.
         * (It's really a bug in FindFirstFile().)
         * If a pathname ends in a backslash or slash,
         * e.g., c:\temp\ or c:/temp/, _stat() will fail.
         * Note: a pathname ending in a slash (e.g., c:/temp/)
         * can be handled by _stat() on NT but not on Win95.
         *
         * We remove the backslash or slash at the end and
         * try again.  
         *
         * Not sure if this happens on OS/2 or not,
         * but it doesn't hurt to be careful.
         */

        int len = strlen(fn);
        if (len > 0 && len <= _MAX_PATH
                && (fn[len - 1] == '\\' || fn[len - 1] == '/')) {
            char newfn[_MAX_PATH + 1];

            strcpy(newfn, fn);
            newfn[len - 1] = '\0';
            rv = _stat(newfn, info);
        }
    }

    if (rv == -1) {
        _PR_MD_MAP_STAT_ERROR(_MD_ERRNO());
    }
    return rv;
}

PRInt32
_PR_MD_GETFILEINFO64(const char *fn, PRFileInfo64 *info)
{
    struct stat sb;
    PRInt32 rv;
    PRInt64 s, s2us;

    PR_STATIC_ASSERT(sizeof(sb.st_size) == sizeof(info->size));

    rv = _PR_MD_STAT(fn, &sb);
    if (rv == -1) {
        _PR_MD_MAP_STAT_ERROR(_MD_ERRNO());
        return rv;
    }

    if (info) {
        if (S_IFREG & sb.st_mode)
            info->type = PR_FILE_FILE;
        else if (S_IFDIR & sb.st_mode)
            info->type = PR_FILE_DIRECTORY;
        else
            info->type = PR_FILE_OTHER;
        info->size = sb.st_size;
        LL_I2L(s2us, PR_USEC_PER_SEC);
        LL_I2L(s, sb.st_mtime);
        LL_MUL(s, s, s2us);
        info->modifyTime = s;
        LL_I2L(s, sb.st_ctime);
        LL_MUL(s, s, s2us);
        info->creationTime = s;
    }

    return rv;
}

PRInt32
_PR_MD_GETFILEINFO(const char *fn, PRFileInfo *info)
{
    PRFileInfo64 info64;
    PRInt32 rv = _PR_MD_GETFILEINFO64(fn, &info64);
    if (rv == -1) {
        return rv;
    }

    if (info) {
        info->type = info64.type;
        info->size = info64.size;
        info->modifyTime = info64.modifyTime;
        info->creationTime = info64.creationTime;
    }
    
    return rv;
}

PRInt32
_PR_MD_GETOPENFILEINFO64(const PRFileDesc *fd, PRFileInfo64 *info)
{
    struct stat sb;
    PRInt32 rv;
    PRInt64 s, s2us;

    PR_STATIC_ASSERT(sizeof(sb.st_size) == sizeof(info->size));

    rv = fstat(fd->secret->md.osfd, &sb);
    if (rv == -1) {
        _PR_MD_MAP_FSTAT_ERROR(_MD_ERRNO());
        return rv;
    }

    if (info) {
        if (S_IFREG & sb.st_mode)
            info->type = PR_FILE_FILE;
        else if (S_IFDIR & sb.st_mode)
            info->type = PR_FILE_DIRECTORY;
        else
            info->type = PR_FILE_OTHER;
        info->size = sb.st_size;
        LL_I2L(s2us, PR_USEC_PER_SEC);
        LL_I2L(s, sb.st_mtime);
        LL_MUL(s, s, s2us);
        info->modifyTime = s;
        LL_I2L(s, sb.st_ctime);
        LL_MUL(s, s, s2us);
        info->creationTime = s;
    }

    return rv;
}

PRInt32
_PR_MD_GETOPENFILEINFO(const PRFileDesc *fd, PRFileInfo *info)
{
    PRFileInfo64 info64;
    PRInt32 rv = _PR_MD_GETOPENFILEINFO64(fd, &info64);
    if (rv == -1)  {
        return rv;
    }

    if (info) {
        info->type = info64.type;
        info->size = info64.size;
        info->modifyTime = info64.modifyTime;
        info->creationTime = info64.creationTime;
    }

    return rv;
}

PRInt32
_PR_MD_RENAME(const char *from, const char *to)
{
   PRInt32 rv;

   rv = rename(from, to);
   if (rv == -1) {
       _PR_MD_MAP_RENAME_ERROR(_MD_ERRNO());
   }

   return rv;
}

PRInt32
_PR_MD_ACCESS(const char *name, PRAccessHow how)
{
    PRInt32 rv;
    int amode;

    switch (how) {
        case PR_ACCESS_WRITE_OK:
            amode = W_OK;
            break;
        case PR_ACCESS_READ_OK:
            amode = R_OK;
            break;
        case PR_ACCESS_EXISTS:
            amode = F_OK;
            break;
        default:
            PR_SetError(PR_INVALID_ARGUMENT_ERROR, 0);
            return -1;
    }
    rv = access(name, amode);

    if (rv < 0)
        _PR_MD_MAP_ACCESS_ERROR(_MD_ERRNO());

    return rv;
}

PRInt32
_PR_MD_MKDIR(const char *name, PRIntn mode)
{
    PRInt32 rv;

    rv = mkdir(name, mode);
    if (rv < 0) {
        _PR_MD_MAP_MKDIR_ERROR(_MD_ERRNO());
    }

    return rv;
}

PRInt32
_PR_MD_RMDIR(const char *name)
{
    PRInt32 rv;

    rv = rmdir(name);
    if (rv < 0) {
        _PR_MD_MAP_RMDIR_ERROR(_MD_ERRNO());
    }

    return rv;
}

/*
 * NOTE: Under kLIBC 0.6.6 and around, NSPR must be linked against LIBCx in
 * order for the fcntl locking code to work correctly.
 */

PRStatus
_PR_MD_LOCKFILE(PRInt32 f)
{
    PRInt32 rv;
    struct flock arg;

    arg.l_type = F_WRLCK;
    arg.l_whence = SEEK_SET;
    arg.l_start = 0;
    arg.l_len = 0;  /* until EOF */
    rv = fcntl(f, F_SETLKW, &arg);
    if (rv == 0)
        return PR_SUCCESS;
    _PR_MD_MAP_LOCKF_ERROR(_MD_ERRNO());
    return PR_FAILURE;
}

PRStatus
_PR_MD_TLOCKFILE(PRInt32 f)
{
    PRInt32 rv;
    struct flock arg;

    arg.l_type = F_WRLCK;
    arg.l_whence = SEEK_SET;
    arg.l_start = 0;
    arg.l_len = 0;  /* until EOF */
    rv = fcntl(f, F_SETLK, &arg);
    if (rv == 0)
        return PR_SUCCESS;
    _PR_MD_MAP_LOCKF_ERROR(_MD_ERRNO());
    return PR_FAILURE;
}

PRStatus
_PR_MD_UNLOCKFILE(PRInt32 f)
{
    PRInt32 rv;
    struct flock arg;

    arg.l_type = F_UNLCK;
    arg.l_whence = SEEK_SET;
    arg.l_start = 0;
    arg.l_len = 0;  /* until EOF */
    rv = fcntl(f, F_SETLK, &arg);
    if (rv == 0)
        return PR_SUCCESS;
    _PR_MD_MAP_LOCKF_ERROR(_MD_ERRNO());
    return PR_FAILURE;
}

PRStatus
_PR_MD_SET_FD_INHERITABLE(PRFileDesc *fd, PRBool inheritable)
{
    PRInt32 rv;

    rv = fcntl(fd->secret->md.osfd, F_SETFD, inheritable ? 0 : FD_CLOEXEC);
    if (-1 == rv) {
        PR_SetError(PR_UNKNOWN_ERROR, _MD_ERRNO());
        return PR_FAILURE;
    }
    return PR_SUCCESS;
}

void
_PR_MD_INIT_FD_INHERITABLE(PRFileDesc *fd, PRBool imported)
{
    if (imported) {
        fd->secret->inheritable = _PR_TRI_UNKNOWN;
    } else {
        /* By default, an OS/2 fd is not closed on exec. */
#ifdef DEBUG
        {
            int flags = fcntl(fd->secret->md.osfd, F_GETFD, 0);
            PR_ASSERT(0 == flags);
        }
#endif
        fd->secret->inheritable = _PR_TRI_TRUE;
    }
}

void
_PR_MD_QUERY_FD_INHERITABLE(PRFileDesc *fd)
{
    int flags;

    PR_ASSERT(_PR_TRI_UNKNOWN == fd->secret->inheritable);
    flags = fcntl(fd->secret->md.osfd, F_GETFD, 0);
    PR_ASSERT(-1 != flags);
    fd->secret->inheritable = (flags & FD_CLOEXEC) ?
        _PR_TRI_FALSE : _PR_TRI_TRUE;
}
