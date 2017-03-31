/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsPaymentRequestResponse.h"
#include "nsIRunnable.h"

namespace mozilla {
namespace dom {

/* nsPaymentRequestResponse */

NS_IMPL_ISUPPORTS(nsPaymentRequestResponse,
                  nsIPaymentRequestResponse)

nsPaymentRequestResponse::nsPaymentRequestResponse()
  : mType(nsIPaymentRequestResponse::NO_TYPE)
{
}

nsPaymentRequestResponse::~nsPaymentRequestResponse()
{
}

NS_IMETHODIMP
nsPaymentRequestResponse::GetRequestId(nsAString& aRequestId)
{
  aRequestId = mRequestId;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestResponse::GetType(uint32_t* aType)
{
  *aType = mType;
  return NS_OK;
}

/* nsPaymentRequestCreateRequest */

NS_IMPL_ISUPPORTS_INHERITED(nsPaymentRequestCanMakeResponse,
                            nsPaymentRequestResponse,
                            nsIPaymentRequestCanMakeResponse)

nsPaymentRequestCanMakeResponse::nsPaymentRequestCanMakeResponse()
{
  mType = nsIPaymentRequestResponse::CANMAKE_RESPONSE;
}

nsPaymentRequestCanMakeResponse::~nsPaymentRequestCanMakeResponse()
{
}

NS_IMETHODIMP
nsPaymentRequestCanMakeResponse::GetResult(bool* aResult)
{
  *aResult = mResult;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestCanMakeResponse::Init(const nsAString& aRequestId, const bool aResult)
{
  mRequestId = aRequestId;
  mResult = aResult;
  return NS_OK;
}

/* nsPaymentRequestAbortResponse */

NS_IMPL_ISUPPORTS_INHERITED(nsPaymentRequestAbortResponse,
                            nsPaymentRequestResponse,
                            nsIPaymentRequestAbortResponse)

nsPaymentRequestAbortResponse::nsPaymentRequestAbortResponse()
{
  mType = nsIPaymentRequestResponse::ABORT_RESPONSE;
}

nsPaymentRequestAbortResponse::~nsPaymentRequestAbortResponse()
{
}

NS_IMETHODIMP
nsPaymentRequestAbortResponse::GetAbortStatus(uint32_t* aAbortStatus)
{
  *aAbortStatus = mAbortStatus;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestAbortResponse::Init(const nsAString& aRequestId,
                                    const uint32_t aAbortStatus)
{
  mRequestId = aRequestId;
  mAbortStatus = aAbortStatus;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestAbortResponse::IsSucceeded(bool* aIsSucceeded)
{
  *aIsSucceeded = (mAbortStatus == nsIPaymentRequestResponse::ABORT_SUCCEEDED);
  return NS_OK;
}

/* nsPaymentRequestShowResponse */

NS_IMPL_ISUPPORTS_INHERITED(nsPaymentRequestShowResponse,
                            nsPaymentRequestResponse,
                            nsIPaymentRequestShowResponse)

nsPaymentRequestShowResponse::nsPaymentRequestShowResponse()
{
  mType = nsIPaymentRequestResponse::SHOW_RESPONSE;
}

nsPaymentRequestShowResponse::~nsPaymentRequestShowResponse()
{
}

NS_IMETHODIMP
nsPaymentRequestShowResponse::GetAcceptStatus(uint32_t* aAcceptStatus)
{
  *aAcceptStatus = mAcceptStatus;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestShowResponse::GetMethodName(nsAString& aMethodName)
{
  aMethodName = mMethodName;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestShowResponse::GetData(nsAString& aData)
{
  aData = mData;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestShowResponse::GetPayerName(nsAString& aPayerName)
{
  aPayerName = mPayerName;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestShowResponse::GetPayerEmail(nsAString& aPayerEmail)
{
  aPayerEmail = mPayerEmail;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestShowResponse::GetPayerPhone(nsAString& aPayerPhone)
{
  aPayerPhone = mPayerPhone;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestShowResponse::Init(const nsAString& aRequestId,
                                   const uint32_t aAcceptStatus,
                                   const nsAString& aMethodName,
                                   const nsAString& aData,
                                   const nsAString& aPayerName,
                                   const nsAString& aPayerEmail,
                                   const nsAString& aPayerPhone)
{
  mRequestId = aRequestId;
  mAcceptStatus = aAcceptStatus;
  mMethodName = aMethodName;
  mData = aData;
  mPayerName = aPayerName;
  mPayerEmail = aPayerEmail;
  mPayerPhone = aPayerPhone;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequestShowResponse::IsAccepted(bool* aIsAccepted)
{
  *aIsAccepted = (mAcceptStatus == nsIPaymentRequestResponse::PAYMENT_ACCEPTED);
  return NS_OK;
}

/* nsPaymentRequestCallback */

NS_IMPL_ISUPPORTS(nsPaymentRequestCallback,
                  nsIPaymentRequestCallback)

nsPaymentRequestCallback::nsPaymentRequestCallback(PaymentRequestParent* aParent)
  : mParent(aParent)
{
}

nsPaymentRequestCallback::~nsPaymentRequestCallback()
{
}

NS_IMETHODIMP
nsPaymentRequestCallback::RespondPayment(nsIPaymentRequestResponse* aResponse)
{
  if (!mParent) {
    return NS_ERROR_UNEXPECTED;
  }
  if (!NS_IsMainThread()) {
    RefPtr<nsIPaymentRequestCallback> self = this;
    RefPtr<nsIPaymentRequestResponse> response = aResponse;
    nsCOMPtr<nsIRunnable> r = NS_NewRunnableFunction([self, response] ()
    {
      self->RespondPayment(response);
    });
    return NS_DispatchToMainThread(r);
  }
  return mParent->RespondPayment(aResponse);
}
} // end of namespace dom
} // end of namespace mozilla
