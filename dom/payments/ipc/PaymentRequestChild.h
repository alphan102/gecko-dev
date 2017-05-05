/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_PaymentRequestChild_h
#define mozilla_dom_PaymentRequestChild_h

#include "mozilla/dom/PPaymentRequestChild.h"

namespace mozilla {
namespace dom {

class PaymentRequestChild final : public PPaymentRequestChild
{
public:
  PaymentRequestChild();

  virtual void ActorDestroy(ActorDestroyReason aWhy) override;
private:
  ~PaymentRequestChild();

  bool mActorDestroyed;
};

} // end of namespace dom
} // end of namespace mozilla
#endif
