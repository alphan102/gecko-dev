/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsPaymentRequestService.h"
#include "nsPaymentRequest.h"
#include "nsArrayEnumerator.h"

namespace mozilla {
namespace dom {

/* nsPaymentRequestService */

NS_IMPL_ISUPPORTS(nsPaymentRequestService,
                  nsIPaymentRequestService)

StaticRefPtr<nsPaymentRequestService> nsPaymentRequestService::sSingleton;

already_AddRefed<nsPaymentRequestService>
nsPaymentRequestService::GetSingleton()
{
  MOZ_ASSERT(NS_IsMainThread());
  if (!sSingleton) {
    sSingleton = new nsPaymentRequestService();
  }
  RefPtr<nsPaymentRequestService> service = sSingleton.get();
  return service.forget();
}

nsPaymentRequestService::nsPaymentRequestService()
{
}

nsPaymentRequestService::~nsPaymentRequestService()
{
}

NS_IMETHODIMP
nsPaymentRequestService::GetPaymentRequestById(const nsAString& aRequestId,
                                               nsIPaymentRequest** aRequest)
{
  NS_ENSURE_ARG_POINTER(aRequest);
  uint32_t numRequests = mRequestQueue.Length();
  for (uint32_t index = 0; index < numRequests; ++index) {
    nsCOMPtr<nsIPaymentRequest> request = mRequestQueue[index];
    nsString requestId;
    nsresult rv = request->GetRequestId(requestId);
    NS_ENSURE_SUCCESS(rv, rv);
    if (requestId == aRequestId) {
      request.forget(aRequest);
      break;
    }
  }
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestService::Enumerate(nsISimpleEnumerator** aEnumerator)
{
  NS_ENSURE_ARG_POINTER(aEnumerator);
  return NS_NewArrayEnumerator(aEnumerator, mRequestQueue);
}

NS_IMETHODIMP
nsPaymentRequestService::RequestPayment(nsIPaymentActionRequest* aRequest)
{
  nsCOMPtr<nsIPaymentRequest> payment;
  uint32_t type;
  aRequest->GetType(&type);
  switch (type) {
    case nsIPaymentActionRequest::CREATE_ACTION: {
      nsCOMPtr<nsIPaymentCreateActionRequest> request =
        do_QueryInterface(aRequest);
      uint64_t tabId;
      nsString requestId;
      nsCOMPtr<nsIArray> methodData;
      nsCOMPtr<nsIPaymentDetails> details;
      nsCOMPtr<nsIPaymentOptions> options;
      request->GetTabId(&tabId);
      request->GetRequestId(requestId);
      request->GetMethodData(getter_AddRefs(methodData));
      request->GetDetails(getter_AddRefs(details));
      request->GetOptions(getter_AddRefs(options));
      payment = new nsPaymentRequest(tabId, requestId, methodData, details, options);
      mRequestQueue.AppendElement(payment);
      break;
    }
    default: {
      return NS_ERROR_FAILURE;
    }
  }
  return NS_OK;
}

} // end of namespace dom
} // end of namespace mozilla
