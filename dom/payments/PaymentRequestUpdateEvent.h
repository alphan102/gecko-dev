/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_PaymentRequestUpdateEvent_h
#define mozilla_dom_PaymentRequestUpdateEvent_h

#include "mozilla/Attributes.h"
#include "mozilla/ErrorResult.h"
#include "mozilla/dom/Event.h"
#include "mozilla/dom/PaymentRequestUpdateEventBinding.h"

namespace mozilla {
namespace dom {

class Promise;

} // namespace dom
} // namespace mozilla

namespace mozilla {
namespace dom {

class PaymentRequestUpdateEvent final : public Event
{
public:
  NS_DECL_ISUPPORTS_INHERITED

  PaymentRequestUpdateEvent(EventTarget* aOwner);

  virtual JSObject*
    WrapObjectInternal(JSContext* aCx,
                       JS::Handle<JSObject*> aGivenProto) override;

  static already_AddRefed<PaymentRequestUpdateEvent>
    Constructor(const GlobalObject& global,
                const nsAString& type,
                const PaymentRequestUpdateEventInit& eventInitDict,
                ErrorResult& aRv)
  { return nullptr; }

  void UpdateWith(Promise& d) { }

  bool IsTrusted() const { return true; }

protected:
  ~PaymentRequestUpdateEvent();
};

} // namespace dom
} // namespace mozilla

#endif // mozilla_dom_PaymentRequestUpdateEvent_h
