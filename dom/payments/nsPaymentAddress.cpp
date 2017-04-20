/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsArrayUtils.h"
#include "nsIMutableArray.h"
#include "nsISupportsPrimitives.h"
#include "nsPaymentAddress.h"
#include "nsPaymentRequestUtils.h"

namespace mozilla {
namespace dom {

NS_IMPL_ISUPPORTS(nsPaymentAddress, nsIPaymentAddress)

nsresult
nsPaymentAddress::Init(const nsAString& aCountry,
                       nsIArray* aAddressLine,
                       const nsAString& aRegion,
                       const nsAString& aCity,
                       const nsAString& aDependentLocality,
                       const nsAString& aPostalCode,
                       const nsAString& aSortingCode,
                       const nsAString& aLanguageCode,
                       const nsAString& aOrganization,
                       const nsAString& aRecipient,
                       const nsAString& aPhone)
{
  mCountry = aCountry;
  mRegion = aRegion;
  mCity = aCity;
  mDependentLocality = aDependentLocality;
  mPostalCode = aPostalCode;
  mSortingCode = aSortingCode;
  mLanguageCode = aLanguageCode;
  mOrganization = aOrganization;
  mRecipient = aRecipient;
  mPhone = aPhone;
  return ConvertISupportsStringstoStrings(aAddressLine, mAddressLine);
}

nsresult
nsPaymentAddress::GetCountry(nsAString& aCountry)
{
  aCountry = mCountry;
  return NS_OK;
}

nsresult
nsPaymentAddress::GetAddressLine(nsIArray** aAddressLine)
{
  NS_ENSURE_ARG_POINTER(aAddressLine);
  return ConvertStringstoISupportsStrings(mAddressLine, aAddressLine);
}

nsresult
nsPaymentAddress::GetRegion(nsAString& aRegion)
{
  aRegion = mRegion;
  return NS_OK;
}

nsresult
nsPaymentAddress::GetCity(nsAString& aCity)
{
  aCity = mCity;
  return NS_OK;
}

nsresult
nsPaymentAddress::GetDependentLocality(nsAString& aDependentLocality)
{
  aDependentLocality = mDependentLocality;
  return NS_OK;
}

nsresult
nsPaymentAddress::GetPostalCode(nsAString& aPostalCode)
{
  aPostalCode = mPostalCode;
  return NS_OK;
}

nsresult
nsPaymentAddress::GetSortingCode(nsAString& aSortingCode)
{
  aSortingCode = mSortingCode;
  return NS_OK;
}

nsresult
nsPaymentAddress::GetLanguageCode(nsAString& aLanguageCode)
{
  aLanguageCode = mLanguageCode;
  return NS_OK;
}

nsresult
nsPaymentAddress::GetOrganization(nsAString& aOrganization)
{
  aOrganization = mOrganization;
  return NS_OK;
}

nsresult
nsPaymentAddress::GetRecipient(nsAString& aRecipient)
{
  aRecipient = mRecipient;
  return NS_OK;
}

nsresult
nsPaymentAddress::GetPhone(nsAString& aPhone)
{
  aPhone = mPhone;
  return NS_OK;
}
} // end of namespace dom
} // end of namespace mozilla
