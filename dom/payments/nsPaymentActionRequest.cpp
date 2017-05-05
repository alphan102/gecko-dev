/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsPaymentActionRequest.h"
#include "nsPaymentRequest.h"

namespace mozilla {
namespace dom {

/* nsPaymentActionRequest */

NS_IMPL_ISUPPORTS(nsPaymentActionRequest,
                  nsIPaymentActionRequest)

NS_IMETHODIMP
nsPaymentActionRequest::Init(const nsAString& aRequestId,
                              const uint32_t aType)
{
  mRequestId = aRequestId;
  mType = aType;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentActionRequest::GetRequestId(nsAString& aRequestId)
{
  aRequestId = mRequestId;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentActionRequest::GetType(uint32_t* aType)
{
  *aType = mType;
  return NS_OK;
}

/* nsPaymentCreateActionRequest */

NS_IMPL_ISUPPORTS_INHERITED(nsPaymentCreateActionRequest,
                            nsPaymentActionRequest,
                            nsIPaymentCreateActionRequest)

NS_IMETHODIMP
nsPaymentCreateActionRequest::InitRequest(const nsAString& aRequestId,
                                           const uint64_t aTabId,
                                           nsIArray* aMethodData,
                                           nsIPaymentDetails* aDetails,
                                           nsIPaymentOptions* aOptions)
{
  Init(aRequestId, nsIPaymentActionRequest::CREATE_ACTION);
  mTabId = aTabId;
  mMethodData = aMethodData;
  mDetails = aDetails;
  mOptions = aOptions;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentCreateActionRequest::GetTabId(uint64_t* aTabId)
{
  *aTabId = mTabId;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentCreateActionRequest::GetMethodData(nsIArray** aMethodData)
{
  NS_ENSURE_ARG_POINTER(aMethodData);
  nsCOMPtr<nsIMutableArray> methodData = do_CreateInstance(NS_ARRAY_CONTRACTID);
  uint32_t length;
  nsresult rv = mMethodData->GetLength(&length);
  if (NS_FAILED(rv)) {
    return NS_ERROR_FAILURE;
  }
  for (uint32_t idx = 0; idx < length; ++idx) {
    nsCOMPtr<nsIPaymentMethodData> method =
      do_QueryElementAt(mMethodData, idx);
    methodData->AppendElement(method, false);
  }
  methodData.forget(aMethodData);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentCreateActionRequest::GetDetails(nsIPaymentDetails** aDetails)
{
  NS_ENSURE_ARG_POINTER(aDetails);
  nsString id;
  nsCOMPtr<nsIPaymentItem> totalItem;
  nsCOMPtr<nsIArray> displayItems;
  nsCOMPtr<nsIArray> shippingOptions;
  nsCOMPtr<nsIArray> modifiers;
  nsString error;
  mDetails->GetId(id);
  mDetails->GetTotalItem(getter_AddRefs(totalItem));
  mDetails->GetDisplayItems(getter_AddRefs(displayItems));
  mDetails->GetShippingOptions(getter_AddRefs(shippingOptions));
  mDetails->GetModifiers(getter_AddRefs(modifiers));
  mDetails->GetError(error);
  nsCOMPtr<nsIPaymentDetails> details =
    new nsPaymentDetails(id, totalItem, displayItems, shippingOptions, modifiers, error);
  details.forget(aDetails);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentCreateActionRequest::GetOptions(nsIPaymentOptions** aOptions)
{
  NS_ENSURE_ARG_POINTER(aOptions);
  bool requestPayerName;
  bool requestPayerEmail;
  bool requestPayerPhone;
  bool requestShipping;
  nsString shippingType;
  mOptions->GetRequestPayerName(&requestPayerName);
  mOptions->GetRequestPayerEmail(&requestPayerEmail);
  mOptions->GetRequestPayerPhone(&requestPayerPhone);
  mOptions->GetRequestShipping(&requestShipping);
  mOptions->GetShippingType(shippingType);
  nsCOMPtr<nsIPaymentOptions> options =
    new nsPaymentOptions(requestPayerName,
                         requestPayerEmail,
                         requestPayerPhone,
                         requestShipping,
                         shippingType);
  options.forget(aOptions);
  return NS_OK;
}

} // end of namespace dom
} // end of namespace mozilla
