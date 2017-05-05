/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/ipc/InputStreamUtils.h"
#include "nsArrayUtils.h"
#include "nsCOMPtr.h"
#include "nsIMutableArray.h"
#include "nsIPaymentActionRequest.h"
#include "nsIPaymentRequestService.h"
#include "nsISupportsPrimitives.h"
#include "nsPaymentRequest.h"
#include "nsServiceManagerUtils.h"
#include "PaymentRequestParent.h"

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
PaymentRequestParent::RecvRequestPayment(const PaymentActionRequest& aRequest)
{
  nsCOMPtr<nsIPaymentActionRequest> nsrequest;
  switch (aRequest.type()) {
    case PaymentActionRequest::TPaymentCreateActionRequest: {
      PaymentCreateActionRequest request = aRequest;
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

      nsCOMPtr<nsIPaymentCreateActionRequest> createRequest =
        do_CreateInstance(NS_PAYMENT_CREATE_ACTION_REQUEST_CONTRACT_ID);
      createRequest->InitRequest(request.requestId(),
                                 request.tabId(),
                                 nsMethodData,
                                 nsDetails,
                                 nsOptions);
      nsrequest = do_QueryInterface(createRequest);
      break;
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
} // end of namespace dom
} // end of namespace mozilla
