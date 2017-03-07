/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_PaymentRequest_h
#define mozilla_dom_PaymentRequest_h

#include "mozilla/DOMEventTargetHelper.h"
#include "mozilla/ErrorResult.h"
#include "mozilla/dom/PaymentRequestBinding.h"
#include "nsWrapperCache.h"

namespace mozilla {
namespace dom {

class EventHandlerNonNull;
class PaymentAddress;
class Promise;

} // namespace dom
} // namespace mozilla

namespace mozilla {
namespace dom {

class PaymentRequest final : public DOMEventTargetHelper
{
public:
  NS_DECL_ISUPPORTS_INHERITED

  PaymentRequest();

  nsPIDOMWindowInner* GetParentObject() const
  {
    return GetOwner();
  }

  virtual JSObject* WrapObject(JSContext* aCx,
                               JS::Handle<JSObject*> aGivenProto) override;

  static already_AddRefed<PaymentRequest>
    Constructor(const GlobalObject& global,
                const Sequence<PaymentMethodData>& methodData,
                const PaymentDetailsInit& details,
                const PaymentOptions& options,
                ErrorResult& aRv)
  { return nullptr; }

  // Return a raw pointer here to avoid refcounting, but make sure it's safe
  // (the object should be kept alive by the callee).
  already_AddRefed<Promise> Show() { return nullptr; }

  // Return a raw pointer here to avoid refcounting, but make sure it's safe
  // (the object should be kept alive by the callee).
  already_AddRefed<Promise> Abort() { return nullptr; }

  // Return a raw pointer here to avoid refcounting, but make sure it's safe
  // (the object should be kept alive by the callee).
  already_AddRefed<Promise> CanMakePayment() { return nullptr; }

  void GetPaymentRequestId(nsString& aRetVal) const { }

  // Return a raw pointer here to avoid refcounting, but make sure it's safe
  // (the object should be kept alive by the callee).
  already_AddRefed<PaymentAddress> GetShippingAddress() const { return nullptr; }

  void GetShippingOption(nsString& aRetVal) const { }

  Nullable<PaymentShippingType> GetShippingType() const { return nullptr; }

  IMPL_EVENT_HANDLER(shippingaddresschange);
  IMPL_EVENT_HANDLER(shippingoptionchange);

protected:
  ~PaymentRequest();
};

} // namespace dom
} // namespace mozilla

#endif // mozilla_dom_PaymentRequest_h
