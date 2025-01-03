/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsUnicodeToCP869_h___
#define nsUnicodeToCP869_h___

#include "nsID.h"

class nsISupports;

/**
 * A character set converter from Unicode to CP869.
 */
nsresult
nsUnicodeToCP869Constructor(nsISupports *aOuter, REFNSIID aIID,
                            void **aResult);

#endif /* nsUnicodeToCP869_h___ */
