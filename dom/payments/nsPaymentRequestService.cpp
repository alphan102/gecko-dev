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
    case nsIPaymentRequestRequest::ABORT_REQUEST: {
      /*
       *  TODO: Launch/inform payment UI here once the UI module is implemented.
       *  Currently we always return true for testing. This code should be
       *  removed once the UI module is implemented.
       */
      nsString requestId;
      payment->GetRequestId(requestId);
      nsCOMPtr<nsIPaymentRequestAbortResponse> response =
        do_CreateInstance(NS_PAYMENT_REQUEST_ABORT_RESPONSE_CONTRACT_ID);
      response->Init(requestId, nsIPaymentRequestResponse::ABORT_SUCCEEDED);
      RespondPayment(response);
      break;
    }
    case nsIPaymentRequestRequest::SHOW_REQUEST: {
      /*
       *  TODO: Launch/inform payment UI here once the UI module is implemented.
       *  Currently we generate a fake response for testing. This code should be
       *  removed once the UI module is implmented
       */
      nsCOMPtr<nsIPaymentRequestShowResponse> response =
        do_CreateInstance(NS_PAYMENT_REQUEST_SHOW_RESPONSE_CONTRACT_ID);
      nsString requestId;
      payment->GetRequestId(requestId);
      response->Init(requestId,
                     nsIPaymentRequestResponse::PAYMENT_ACCEPTED,
                     NS_LITERAL_STRING("VISA"),
                     NS_LITERAL_STRING("{card number:\"4485058827460159\",Expires:\"4/2018\",CVV:\"151\"}"),
                     NS_LITERAL_STRING("Bill A. Pacheco"),
                     NS_LITERAL_STRING("BillAPacheco@jourrapide.com"),
                     NS_LITERAL_STRING("+1-434-441-3879"));
      RespondPayment(response);
      break;
    }
    case nsIPaymentRequestRequest::COMPLETE_REQUEST: {
      /*
       *  TODO: Inform payment UI once the UI module is implemented.
       *  Currently we generate a fake response for testing. This code should be
       *  removed once the UI module is implmented
       */
      nsCOMPtr<nsIPaymentRequestCompleteResponse> response =
        do_CreateInstance(NS_PAYMENT_REQUEST_COMPLETE_RESPONSE_CONTRACT_ID);
      nsString requestId;
      payment->GetRequestId(requestId);
      response->Init(requestId, nsIPaymentRequestResponse::COMPLETE_SUCCEEDED);
      RespondPayment(response);
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
  uint32_t type;
  aResponse->GetType(&type);
  switch (type) {
    case nsIPaymentRequestResponse::ABORT_RESPONSE: {
      bool isSucceeded;
      nsCOMPtr<nsIPaymentRequestAbortResponse> response = do_QueryInterface(aResponse);
      response->IsSucceeded(&isSucceeded);
      if (isSucceeded) {
        request->SetCallback(nullptr);
        mRequestQueue.RemoveElement(request);
      }
      break;
    }
    case nsIPaymentRequestResponse::COMPLETE_RESPONSE: {
      request->SetCallback(nullptr);
      mRequestQueue.RemoveElement(request);
      break;
    }
    default: {
      request->SetCallback(nullptr);
      break;
    }
  }
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestService::ChangeShippingAddress(const nsAString& aRequestId,
                                               nsIPaymentAddress* aAddress)
{
  nsCOMPtr<nsIPaymentRequest> request;
  nsresult rv = GetPaymentRequestById(aRequestId, getter_AddRefs(request));
  if (NS_FAILED(rv)) {
    return NS_ERROR_FAILURE;
  }
  nsCOMPtr<nsIPaymentRequestCallback> callback;
  rv = request->GetCallback(getter_AddRefs(callback));
  if (NS_FAILED(rv)) {
    return NS_ERROR_FAILURE;
  }
  callback->ChangeShippingAddress(aRequestId, aAddress);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestService::ChangeShippingOption(const nsAString& aRequestId,
                                              const nsAString& aOption)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

} // end of namespace dom
} // end of namespace mozilla
