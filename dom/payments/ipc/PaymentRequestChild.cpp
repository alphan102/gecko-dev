/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "PaymentRequestChild.h"
#include "PaymentRequestManager.h"

namespace mozilla {
namespace dom {

PaymentRequestChild::PaymentRequestChild()
  : mActorDestroyed(false)
{
  MOZ_COUNT_CTOR(PaymentRequestChild);
}

PaymentRequestChild::~PaymentRequestChild()
{
  MOZ_COUNT_DTOR(PaymentRequestChild);
  if (!mActorDestroyed) {
    Send__delete__(this);
  }
}

mozilla::ipc::IPCResult
PaymentRequestChild::RecvRespondPayment(const PaymentRequestResponse& aResponse)
{
  PaymentRequestResponse response = aResponse;
  RefPtr<PaymentRequestManager> manager = PaymentRequestManager::GetSingleton();
  MOZ_ASSERT(manager);
  if (NS_FAILED(manager->RespondPayment(response))) {
    return IPC_FAIL_NO_REASON(this);
  }
  return IPC_OK();
}

mozilla::ipc::IPCResult
PaymentRequestChild::RecvChangeShippingAddress(const nsString& aRequestId,
                                               const IPCPaymentAddress& aAddress)
{
  RefPtr<PaymentRequestManager> manager = PaymentRequestManager::GetSingleton();
  MOZ_ASSERT(manager);
  if (NS_FAILED(manager->ChangeShippingAddress(aRequestId, aAddress))) {
    return IPC_FAIL_NO_REASON(this);
  }
  return IPC_OK();
}

mozilla::ipc::IPCResult
PaymentRequestChild::RecvChangeShippingOption(const nsString& aRequestId,
                                              const nsString& option)
{
  return IPC_OK();
}

void
PaymentRequestChild::ActorDestroy(ActorDestroyReason aWhy)
{
  mActorDestroyed = true;
}
} // end of namespace dom
} // end of namespace mozilla
