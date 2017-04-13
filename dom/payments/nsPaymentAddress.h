/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_nsPaymentAddress_h
#define mozilla_dom_nsPaymentAddress_h

#include "nsIPaymentAddress.h"
#include "nsIArray.h"
#include "nsTArray.h"
#include "nsString.h"

namespace mozilla {
namespace dom {

class nsPaymentAddress final : public nsIPaymentAddress
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTADDRESS
  nsPaymentAddress() {}

protected:
  ~nsPaymentAddress() {}

  nsString mCountry;
  nsTArray<nsString> mAddressLine;
  nsString mRegion;
  nsString mCity;
  nsString mDependentLocality;
  nsString mPostalCode;
  nsString mSortingCode;
  nsString mLanguageCode;
  nsString mOrganization;
  nsString mRecipient;
  nsString mPhone;
};

} // end of namespace dom
} // end of namespace mozilla
#endif
