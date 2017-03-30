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
    case PaymentRequestRequest::TPaymentRequestCanMakeRequest:
    case PaymentRequestRequest::TPaymentRequestShowRequest:
    case PaymentRequestRequest::TPaymentRequestAbortRequest:
    case PaymentRequestRequest::TPaymentRequestUpdateRequest: {
      /*
       *  TODO: Convert PaymentRequestRequest to nsIPaymentRequestRequest.
       */
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
    case nsIPaymentRequestResponse::CANMAKE_RESPONSE:
    case nsIPaymentRequestResponse::ABORT_RESPONSE:
    case nsIPaymentRequestResponse::SHOW_RESPONSE: {
      /*
       *  TODO: Convert nsIPaymentRequestResponse to PaymentRequestResponse, then
       *        call SendRespondPayment to pass the task result to content process
       */
      break;
    }
    default: {
      break;
    }
  }
  return NS_ERROR_NOT_IMPLEMENTED;
}
} // end of namespace dom
} // end of namespace mozilla
