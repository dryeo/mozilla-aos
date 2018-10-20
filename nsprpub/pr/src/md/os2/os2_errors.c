/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "prerror.h"
#include "primpl.h"

/* POSIX erros (copied from unix_errors.c w/some additions at the end) */
void _MD_os2_map_default_error(PRInt32 err)
{
    PRErrorCode prError;

    switch (err ) {
        case EACCES:
            prError = PR_NO_ACCESS_RIGHTS_ERROR;
            break;
        case EADDRINUSE:
            prError = PR_ADDRESS_IN_USE_ERROR;
            break;
        case EADDRNOTAVAIL:
            prError = PR_ADDRESS_NOT_AVAILABLE_ERROR;
            break;
        case EAFNOSUPPORT:
            prError = PR_ADDRESS_NOT_SUPPORTED_ERROR;
            break;
        case EAGAIN:
            prError = PR_WOULD_BLOCK_ERROR;
            break;
        /*
         * On QNX and Neutrino, EALREADY is defined as EBUSY.
         */
#if EALREADY != EBUSY
        case EALREADY:
            prError = PR_ALREADY_INITIATED_ERROR;
            break;
#endif
        case EBADF:
            prError = PR_BAD_DESCRIPTOR_ERROR;
            break;
#ifdef EBADMSG
        case EBADMSG:
            prError = PR_IO_ERROR;
            break;
#endif
        case EBUSY:
            prError = PR_FILESYSTEM_MOUNTED_ERROR;
            break;
        case ECONNABORTED:
            prError = PR_CONNECT_ABORTED_ERROR;
            break;
        case ECONNREFUSED:
            prError = PR_CONNECT_REFUSED_ERROR;
            break;
        case ECONNRESET:
            prError = PR_CONNECT_RESET_ERROR;
            break;
        case EDEADLK:
            prError = PR_DEADLOCK_ERROR;
            break;
#ifdef EDIRCORRUPTED
        case EDIRCORRUPTED:
            prError = PR_DIRECTORY_CORRUPTED_ERROR;
            break;
#endif
#ifdef EDQUOT
        case EDQUOT:
            prError = PR_NO_DEVICE_SPACE_ERROR;
            break;
#endif
        case EEXIST:
            prError = PR_FILE_EXISTS_ERROR;
            break;
        case EFAULT:
            prError = PR_ACCESS_FAULT_ERROR;
            break;
        case EFBIG:
            prError = PR_FILE_TOO_BIG_ERROR;
            break;
        case EHOSTUNREACH:
        case EHOSTDOWN:
            prError = PR_HOST_UNREACHABLE_ERROR;
            break;
        case EINPROGRESS:
            prError = PR_IN_PROGRESS_ERROR;
            break;
        case EINTR:
            prError = PR_PENDING_INTERRUPT_ERROR;
            break;
        case EINVAL:
            prError = PR_INVALID_ARGUMENT_ERROR;
            break;
        case EIO:
            prError = PR_IO_ERROR;
            break;
        case EISCONN:
            prError = PR_IS_CONNECTED_ERROR;
            break;
        case EISDIR:
            prError = PR_IS_DIRECTORY_ERROR;
            break;
        case ELOOP:
            prError = PR_LOOP_ERROR;
            break;
        case EMFILE:
            prError = PR_PROC_DESC_TABLE_FULL_ERROR;
            break;
        case EMLINK:
            prError = PR_MAX_DIRECTORY_ENTRIES_ERROR;
            break;
        case EMSGSIZE:
            prError = PR_INVALID_ARGUMENT_ERROR;
            break;
#ifdef EMULTIHOP
        case EMULTIHOP:
            prError = PR_REMOTE_FILE_ERROR;
            break;
#endif
        case ENAMETOOLONG:
            prError = PR_NAME_TOO_LONG_ERROR;
            break;
        case ENETUNREACH:
            prError = PR_NETWORK_UNREACHABLE_ERROR;
            break;
        case ENFILE:
            prError = PR_SYS_DESC_TABLE_FULL_ERROR;
            break;
        /*
         * On SCO OpenServer 5, ENOBUFS is defined as ENOSR.
         */
#if defined(ENOBUFS) && (ENOBUFS != ENOSR)
        case ENOBUFS:
            prError = PR_INSUFFICIENT_RESOURCES_ERROR;
            break;
#endif
        case ENODEV:
            prError = PR_FILE_NOT_FOUND_ERROR;
            break;
        case ENOENT:
            prError = PR_FILE_NOT_FOUND_ERROR;
            break;
        case ENOLCK:
            prError = PR_FILE_IS_LOCKED_ERROR;
            break;
#ifdef ENOLINK
        case ENOLINK:
            prError = PR_REMOTE_FILE_ERROR;
            break;
#endif
        case ENOMEM:
            prError = PR_OUT_OF_MEMORY_ERROR;
            break;
        case ENOPROTOOPT:
            prError = PR_INVALID_ARGUMENT_ERROR;
            break;
        case ENOSPC:
            prError = PR_NO_DEVICE_SPACE_ERROR;
            break;
#ifdef ENOSR
        case ENOSR:
            prError = PR_INSUFFICIENT_RESOURCES_ERROR;
            break;
#endif
        case ENOSYS:
            prError = PR_NOT_IMPLEMENTED_ERROR;
            break;
        case ENOTCONN:
            prError = PR_NOT_CONNECTED_ERROR;
            break;
        case ENOTDIR:
            prError = PR_NOT_DIRECTORY_ERROR;
            break;
        case ENOTSOCK:
            prError = PR_NOT_SOCKET_ERROR;
            break;
        case ENXIO:
            prError = PR_FILE_NOT_FOUND_ERROR;
            break;
        case EOPNOTSUPP:
            prError = PR_NOT_TCP_SOCKET_ERROR;
            break;
#ifdef EOVERFLOW
        case EOVERFLOW:
            prError = PR_BUFFER_OVERFLOW_ERROR;
            break;
#endif
        case EPERM:
            prError = PR_NO_ACCESS_RIGHTS_ERROR;
            break;
        case EPIPE:
            prError = PR_CONNECT_RESET_ERROR;
            break;
#ifdef EPROTO
        case EPROTO:
            prError = PR_IO_ERROR;
            break;
#endif
        case EPROTONOSUPPORT:
            prError = PR_PROTOCOL_NOT_SUPPORTED_ERROR;
            break;
        case EPROTOTYPE:
            prError = PR_ADDRESS_NOT_SUPPORTED_ERROR;
            break;
        case ERANGE:
            prError = PR_INVALID_METHOD_ERROR;
            break;
        case EROFS:
            prError = PR_READ_ONLY_FILESYSTEM_ERROR;
            break;
        case ESPIPE:
            prError = PR_INVALID_METHOD_ERROR;
            break;
        case ETIMEDOUT:
            prError = PR_IO_TIMEOUT_ERROR;
            break;
#if EWOULDBLOCK != EAGAIN
        case EWOULDBLOCK:
            prError = PR_WOULD_BLOCK_ERROR;
            break;
#endif
        case EXDEV:
            prError = PR_NOT_SAME_DEVICE_ERROR;
            break;
        /*
         * OS/2-specific errors
         */
        case ENETDOWN:
            prError = PR_NETWORK_DOWN_ERROR;
            break;
#ifdef SOCEFAULT
        case SOCEFAULT:
            prError = PR_ACCESS_FAULT_ERROR;
            break;
#endif
        default:
            prError = PR_UNKNOWN_ERROR;
            break;
    }
    PR_SetError(prError, err);
}

/* Native OS/2 API errors */
#if 0
/*
 * NOTE: Disabled fo now since no longer used anywhere. However, there are
 * some places in NSPR where Dos* APIs are used directly and where errors are
 * reports ted like PR_SetError(PR_UNKNOWN_ERROR, rc). Such calls should be
 * replaced with _MD_os2_map_default_dos_error for better invormativeness.
 */
static void _MD_os2_map_default_dos_error(PRInt32 err)
{
    PRErrorCode prError;

    switch (err) {
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            prError = PR_FILE_NOT_FOUND_ERROR;
            break;
        case ERROR_ACCESS_DENIED:
        case ERROR_WRITE_PROTECT:
            prError = PR_NO_ACCESS_RIGHTS_ERROR;
            break;
        case ERROR_INVALID_ADDRESS:
        case ERROR_INVALID_ACCESS:
            prError = PR_ACCESS_FAULT_ERROR;
            break;
        case ERROR_INVALID_NAME:
        case ERROR_INVALID_PARAMETER:
            prError = PR_INVALID_ARGUMENT_ERROR;
            break;
        case ERROR_INVALID_HANDLE:
            prError = PR_BAD_DESCRIPTOR_ERROR;
        case ERROR_ALREADY_EXISTS:
        case ERROR_FILE_EXISTS:
            prError = PR_FILE_EXISTS_ERROR;
            break;
        case ERROR_TOO_MANY_OPEN_FILES:
            prError = PR_SYS_DESC_TABLE_FULL_ERROR;
            break;
        case ERROR_NOT_DOS_DISK:
        case ERROR_OPEN_FAILED:
        case ERROR_NOT_READY:
        case ERROR_PATH_BUSY:
        case ERROR_CANNOT_MAKE:
        case ERROR_BROKEN_PIPE:
        case ERROR_LOCK_VIOLATION:
        case ERROR_SEEK_ON_DEVICE:
            prError = PR_IO_ERROR;
            break;
        case ERROR_DRIVE_LOCKED:
        case ERROR_LOCKED:
        case ERROR_DEVICE_IN_USE:
        case ERROR_SHARING_VIOLATION:
            prError = PR_FILE_IS_LOCKED_ERROR;
            break;
        case ERROR_FILENAME_EXCED_RANGE:
            prError = PR_NAME_TOO_LONG_ERROR;
            break;
        case ERROR_NOT_ENOUGH_MEMORY:
            prError = PR_OUT_OF_MEMORY_ERROR;
            break;
        case ERROR_MORE_DATA:
        case ERROR_SHARING_BUFFER_EXCEEDED:
            prError = PR_INSUFFICIENT_RESOURCES_ERROR;
            break;
        case ERROR_NO_MORE_FILES:
            prError = PR_NO_MORE_FILES_ERROR;
            break;
        case ERROR_DISK_FULL:
        case ERROR_HANDLE_DISK_FULL:
            prError = PR_NO_DEVICE_SPACE_ERROR;
            break;
        case ERROR_NETNAME_DELETED:
            prError = PR_CONNECT_RESET_ERROR;
            break;
        default:
            prError = PR_UNKNOWN_ERROR;
            break;
    }
    PR_SetError(prError, err);
}
#endif

void _MD_os2_map_opendir_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_closedir_error(PRInt32 err)
{
    PRErrorCode prError;

    switch (err) {
        case EINVAL:
            prError = PR_BAD_DESCRIPTOR_ERROR;
            break;
        default:
            _MD_os2_map_default_error(err);
            return;
    }
    PR_SetError(prError, err);
}

void _MD_os2_readdir_error(PRInt32 err)
{
    PRErrorCode prError;

    switch (err) {
        case 0:
        case ENOENT:
            prError = PR_NO_MORE_FILES_ERROR;
            break;
#ifdef EOVERFLOW
        case EOVERFLOW:
            prError = PR_IO_ERROR;
            break;
#endif
        case EINVAL:
            prError = PR_IO_ERROR;
            break;
        case ENXIO:
            prError = PR_IO_ERROR;
            break;
        default:
            _MD_os2_map_default_error(err);
            return;
    }
    PR_SetError(prError, err);
}

void _MD_os2_map_delete_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_stat_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_fstat_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_rename_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_access_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_mkdir_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_rmdir_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_read_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_transmitfile_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_write_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_lseek_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_fsync_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_close_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_socket_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_recv_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_recvfrom_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_send_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_sendto_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_writev_error(int err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_accept_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_acceptex_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

/*
 * An error code of 0 means that the nonblocking connect succeeded.
 */

int _MD_os2_get_nonblocking_connect_error(int osfd)
{
    int err;
    int len = sizeof(err);
    if (getsockopt(osfd, SOL_SOCKET, SO_ERROR, (char *) &err, &len) == -1) {
        return sock_errno();
    } else {
        return err;
    }
}

void _MD_os2_map_connect_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_bind_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_listen_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_shutdown_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_socketpair_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_getsockname_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_getpeername_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_getsockopt_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_setsockopt_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_open_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_gethostname_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}

void _MD_os2_map_select_error(PRInt32 err)
{
    PRErrorCode prerror;

    switch (err) {
        /*
         * OS/2 select() only works on sockets.  So in this
         * context, ENOTSOCK is equivalent to EBADF on Unix.
         */
        case ENOTSOCK:
            prerror = PR_BAD_DESCRIPTOR_ERROR;
            break;
        default:
            _MD_os2_map_default_error(err);
            return;
    }
    PR_SetError(prerror, err);
}

void _MD_os2_map_lockf_error(PRInt32 err)
{
    _MD_os2_map_default_error(err);
}
