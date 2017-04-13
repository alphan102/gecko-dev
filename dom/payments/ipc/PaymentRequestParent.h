/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_PaymentRequestParent_h
#define mozilla_dom_PaymentRequestParent_h

#include "mozilla/dom/ipc/IdType.h"
#include "nsISupports.h"
#include "mozilla/dom/PPaymentRequestParent.h"
#include "nsIPaymentAddress.h"
#include "nsIPaymentRequestResponse.h"

namespace mozilla {
namespace dom {

class PaymentRequestParent : public nsISupports
                           , public PPaymentRequestParent
{
public:
  NS_DECL_ISUPPORTS;

  PaymentRequestParent();

  virtual nsresult RespondPayment(nsIPaymentRequestResponse* aResponse);
  virtual nsresult ChangeShippingAddress(const nsAString& aRequestId,
                                         nsIPaymentAddress* aAddress);
  virtual nsresult ChangeShippingOption(const nsAString& aRequestId,
                                        const nsAString& aOption);
protected:
  virtual mozilla::ipc::IPCResult
  RecvRequestPayment(const PaymentRequestRequest& aRequest) override;

  virtual mozilla::ipc::IPCResult Recv__delete__() override;

  virtual void ActorDestroy(ActorDestroyReason aWhy) override;
private:
  virtual ~PaymentRequestParent();

  bool mActorDestroyed;
};

} // end of namespace dom
} // end of namespace mozilla
#endif
