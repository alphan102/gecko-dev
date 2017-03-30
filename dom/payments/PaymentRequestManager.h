/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_PaymentRequestManager_h
#define mozilla_dom_PaymentRequestManager_h

#include "nsISupports.h"
#include "PaymentRequest.h"
#include "mozilla/dom/PaymentRequestBinding.h"
#include "mozilla/dom/PaymentRequestUpdateEventBinding.h"
#include "nsCOMPtr.h"
#include "nsTArray.h"
#include "nsString.h"

namespace mozilla {
namespace dom {

class PaymentRequestResponse;

/*
 *  PaymentRequestManager is a singleton used to manage the created PaymentRequests.
 *  It is also the communication agent to chrome proces.
 */
class PaymentRequestManager : public nsISupports
{
public:
  NS_DECL_ISUPPORTS

  static already_AddRefed<PaymentRequestManager> GetSingleton();

  already_AddRefed<PaymentRequest>
  GetPaymentRequestById(const nsAString& aRequestId);

  /*
   *  This method is used to create PaymentRequest object and send corresponding
   *  data to chrome process for internal payment creation, such that content
   *  process can ask specific task by sending requestId only.
   */
  nsresult
  CreatePayment(nsPIDOMWindowInner* aWindow,
                const Sequence<PaymentMethodData>& aMethodData,
                const PaymentDetailsInit& aDetails,
                const PaymentOptions& aOptions,
                PaymentRequest** aRequest);

  /*
   *  Following methods are used for content process to ask a specific task in
   *  chrome process. The task can be CanMake, Show, Abort and Update.
   */
  nsresult CanMakePayment(const nsAString& aRequestId);
  nsresult ShowPayment(const nsAString& aRequestId);
  nsresult AbortPayment(const nsAString& aRequestId);
  nsresult UpdatePayment(const nsAString& aRequestId,
                         const PaymentDetailsUpdate& aDetails);

  /*
   *  This method is used for responding a specific task when the result
   *  is sent back from chrome process.
   */
  nsresult RespondPayment(const PaymentRequestResponse& aResponse);

  /*
   *  Following methods are used to perform shipping address/option change that
   *  sent from chrome process.
   */
  void ChangeShippingAddress(const nsAString& aRequestId);
  void ChangeShippingOption(const nsAString& aRequestId,
                            const nsAString& aOption);

protected:
  PaymentRequestManager();
  virtual ~PaymentRequestManager();

  static StaticRefPtr<PaymentRequestManager> sSingleton;

  // The container for the created PaymentRequests
  nsTArray<RefPtr<PaymentRequest>> mRequestQueue;
};
} // end of namespace dom
} // end of namespace mozilla
#endif
