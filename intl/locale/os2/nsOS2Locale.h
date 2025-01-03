/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef _nslocaleos2_h_
#define _nslocaleos2_h_


#define INCL_BASE
#define INCL_PM
#include <os2.h>

#include "nsISupports.h"
#include "nscore.h"
#include "nsString.h"
#include "nsIOS2Locale.h"


class nsOS2Locale final : public nsIOS2Locale {
  ~nsOS2Locale();

public:

  nsOS2Locale();

  NS_DECL_ISUPPORTS

  NS_IMETHOD GetPlatformLocale(const nsAString& locale, PULONG os2Codepage);
  NS_IMETHOD GetXPLocale(const char* os2Locale, nsAString& locale);

protected:
  inline bool ParseLocaleString(const char* locale_string, char* language, char* country, char* extra, char separator);
};

#endif
