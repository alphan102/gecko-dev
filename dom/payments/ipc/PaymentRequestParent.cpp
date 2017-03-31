/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "PaymentRequestParent.h"
#include "mozilla/ipc/InputStreamUtils.h"
#include "nsServiceManagerUtils.h"
#include "nsPaymentRequest.h"
#include "nsIPaymentRequestRequest.h"
#include "nsIPaymentRequestService.h"
#include "nsIMutableArray.h"
#include "nsISupportsPrimitives.h"
#include "nsArrayUtils.h"
#include "nsCOMPtr.h"

using namespace mozilla::dom;

namespace mozilla {
namespace dom {

NS_IMPL_ISUPPORTS0(PaymentRequestParent);

PaymentRequestParent::PaymentRequestParent()
  : mActorDestroyed(false)
{
}

PaymentRequestParent::~PaymentRequestParent()
{
}

mozilla::ipc::IPCResult
PaymentRequestParent::RecvRequestPayment(const PaymentRequestRequest& aRequest)
{
  nsCOMPtr<nsIPaymentRequestCallback> callback = new nsPaymentRequestCallback(this);
  nsCOMPtr<nsIPaymentRequestRequest> nsrequest;
  switch (aRequest.type()) {
    case PaymentRequestRequest::TPaymentRequestCreateRequest: {
      PaymentRequestCreateRequest request = aRequest;
      nsCOMPtr<nsIMutableArray> nsMethodData =
        do_CreateInstance(NS_ARRAY_CONTRACTID);
      for (IPCPaymentMethodData data : request.methodData()) {
        nsCOMPtr<nsIPaymentMethodData> nsData =
          new nsPaymentMethodData(data);
        nsMethodData->AppendElement(nsData, false);
      }

      nsCOMPtr<nsIPaymentDetails> nsDetails =
        new nsPaymentDetails(request.details());

      nsCOMPtr<nsIPaymentOptions> nsOptions =
        new nsPaymentOptions(request.options());

      nsCOMPtr<nsIPaymentRequestCreateRequest> createRequest =
        do_CreateInstance(NS_PAYMENT_REQUEST_CREATE_REQUEST_CONTRACT_ID);
      createRequest->InitRequest(request.requestId(),
                                 callback,
                                 request.tabId(),
                                 nsMethodData,
                                 nsDetails,
                                 nsOptions);
      nsrequest = do_QueryInterface(createRequest);
      break;
    }
    case PaymentRequestRequest::TPaymentRequestCanMakeRequest: {
      PaymentRequestCanMakeRequest request = aRequest;
      nsrequest = do_CreateInstance(NS_PAYMENT_REQUEST_REQUEST_CONTRACT_ID);
      nsrequest->Init(request.requestId(),
                      nsIPaymentRequestRequest::CANMAKE_REQUEST,
		      callback);
      break;
    }
    case PaymentRequestRequest::TPaymentRequestShowRequest: {
      return IPC_FAIL(this, "Not yet implemented");
    }
    case PaymentRequestRequest::TPaymentRequestAbortRequest: {
      PaymentRequestAbortRequest request = aRequest;
      nsrequest = do_CreateInstance(NS_PAYMENT_REQUEST_REQUEST_CONTRACT_ID);
      nsrequest->Init(request.requestId(),
                      nsIPaymentRequestRequest::ABORT_REQUEST,
		      callback);
      break;
    }
    case PaymentRequestRequest::TPaymentRequestUpdateRequest: {
      return IPC_FAIL(this, "Not yet implemented");
    }
    default: {
      return IPC_FAIL(this, "Unexpected request type");
    }
  }
  nsCOMPtr<nsIPaymentRequestService> service =
    do_GetService(NS_PAYMENT_REQUEST_SERVICE_CONTRACT_ID);
  MOZ_ASSERT(service);
  nsresult rv = service->RequestPayment(nsrequest);
  if (NS_FAILED(rv)) {
    return IPC_FAIL_NO_REASON(this);
  }
  return IPC_OK();
}

mozilla::ipc::IPCResult
PaymentRequestParent::Recv__delete__()
{
  return IPC_OK();
}

void
PaymentRequestParent::ActorDestroy(ActorDestroyReason aWhy)
{
  mActorDestroyed = true;
}

nsresult
PaymentRequestParent::RespondPayment(nsIPaymentRequestResponse* aResponse)
{
  if (mActorDestroyed) {
    return NS_ERROR_FAILURE;
  }
  uint32_t type;
  aResponse->GetType(&type);
  nsString requestId;
  aResponse->GetRequestId(requestId);
  switch (type) {
    case nsIPaymentRequestResponse::CANMAKE_RESPONSE: {
      nsCOMPtr<nsIPaymentRequestCanMakeResponse> nsresponse =
        do_QueryInterface(aResponse);
      bool result;
      nsresponse->GetResult(&result);
      PaymentRequestCanMakeResponse response(requestId, result);
      if (!SendRespondPayment(response)) {
	return NS_ERROR_FAILURE;
      }
      break;
    }
    case nsIPaymentRequestResponse::ABORT_RESPONSE: {
      nsCOMPtr<nsIPaymentRequestAbortResponse> nsresponse =
        do_QueryInterface(aResponse);
      bool isSucceeded;
      nsresponse->IsSucceeded(&isSucceeded);
      PaymentRequestAbortResponse response(requestId, isSucceeded);
      if (!SendRespondPayment(response)) {
	return NS_ERROR_FAILURE;
      }
      break;
    }
    case nsIPaymentRequestResponse::SHOW_RESPONSE: {
      break;
    }
    default: {
      break;
    }
  }
  return NS_OK;
}
} // end of namespace dom
} // end of namespace mozilla
