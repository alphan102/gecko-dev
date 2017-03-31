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
nsPaymentRequestService::RequestPayment(nsIPaymentRequestRequest* aRequest)
{
  nsCOMPtr<nsIPaymentRequest> payment;
  uint32_t type;
  aRequest->GetType(&type);
  if (type != nsIPaymentRequestRequest::CREATE_REQUEST) {
    nsString requestId;
    aRequest->GetRequestId(requestId);
    nsresult rv = GetPaymentRequestById(requestId, getter_AddRefs(payment));
    if (NS_FAILED(rv)) {
      return NS_ERROR_FAILURE;
    }
    nsCOMPtr<nsIPaymentRequestCallback> callback;
    aRequest->GetCallback(getter_AddRefs(callback));
    payment->SetCallback(callback);
  }
  switch (type) {
    case nsIPaymentRequestRequest::CREATE_REQUEST: {
      nsCOMPtr<nsIPaymentRequestCreateRequest> request =
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
    case nsIPaymentRequestRequest::CANMAKE_REQUEST: {
      /*
       *  TODO: 1. Check basic card support once the Basic Card Payment spec is
       *           implemented.
       *        2. Check third party payment app support by traversing all
       *           registered third party payment apps.
       *  Currently we always return true for testing. Once above TODO are
       *  implemented, this code should be removed.
       */
      nsString requestId;
      payment->GetRequestId(requestId);
      nsCOMPtr<nsIPaymentRequestCanMakeResponse> response =
        do_CreateInstance(NS_PAYMENT_REQUEST_CANMAKE_RESPONSE_CONTRACT_ID);;
      response->Init(requestId, true);
      RespondPayment(response);
      break;
    }
    case nsIPaymentRequestRequest::ABORT_REQUEST:
    case nsIPaymentRequestRequest::SHOW_REQUEST: {
      /*
       *  TODO: Launch/inform payment UI here once the UI module is implemented.
       */
      break;
    }
    default: {
      return NS_ERROR_FAILURE;
    }
  }
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestService::RespondPayment(nsIPaymentRequestResponse* aResponse)
{
  nsString requestId;
  aResponse->GetRequestId(requestId);
  nsCOMPtr<nsIPaymentRequest> request;
  nsresult rv = GetPaymentRequestById(requestId, getter_AddRefs(request));
  if (NS_FAILED(rv)) {
    return NS_ERROR_FAILURE;
  }
  nsCOMPtr<nsIPaymentRequestCallback> callback;
  rv = request->GetCallback(getter_AddRefs(callback));
  if (NS_FAILED(rv)) {
    return NS_ERROR_FAILURE;
  }
  callback->RespondPayment(aResponse);
  request->SetCallback(nullptr);
  uint32_t type;
  aResponse->GetType(&type);
  switch (type) {
    case nsIPaymentRequestResponse::ABORT_RESPONSE:
    case nsIPaymentRequestResponse::SHOW_RESPONSE:
    {
      mRequestQueue.RemoveElement(request);
      break;
    }
    default: {
      break;
    }
  }
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestService::ChangeShippingAddress(const nsAString& aRequestId)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsPaymentRequestService::ChangeShippingOption(const nsAString& aRequestId,
                                              const nsAString& aOption)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

} // end of namespace dom
} // end of namespace mozilla
