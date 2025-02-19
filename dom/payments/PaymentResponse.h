/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_PaymentResponse_h
#define mozilla_dom_PaymentResponse_h

#include "mozilla/dom/PaymentResponseBinding.h" // PaymentComplete
#include "nsPIDOMWindow.h"
#include "nsWrapperCache.h"

namespace mozilla {
namespace dom {

class PaymentAddress;
class Promise;

} // namespace dom
} // namespace mozilla

namespace mozilla {
namespace dom {

class PaymentResponse final : public nsISupports,
                              public nsWrapperCache
{
public:
  NS_DECL_CYCLE_COLLECTING_ISUPPORTS
  NS_DECL_CYCLE_COLLECTION_SCRIPT_HOLDER_CLASS(PaymentResponse)

  PaymentResponse();

  nsPIDOMWindowInner* GetParentObject() const
  {
    return mOwner;
  }

  virtual JSObject* WrapObject(JSContext* aCx,
                               JS::Handle<JSObject*> aGivenProto) override;

  void GetRequestId(nsString& aRetVal) const { }

  void GetMethodName(nsString& aRetVal) const { }

  void GetDetails(JSContext* cx, JS::MutableHandle<JSObject*> aRetVal) const { }

  // Return a raw pointer here to avoid refcounting, but make sure it's safe
  // (the object should be kept alive by the callee).
  already_AddRefed<PaymentAddress> GetShippingAddress() const { return nullptr; }

  void GetShippingOption(nsString& aRetVal) const { }

  void GetPayerName(nsString& aRetVal) const { }

  void GetPayerEmail(nsString& aRetVal) const { }

  void GetPayerPhone(nsString& aRetVal) const { }

  // Return a raw pointer here to avoid refcounting, but make sure it's safe
  // (the object should be kept alive by the callee).
  already_AddRefed<Promise> Complete(PaymentComplete result) { return nullptr; }

protected:
  ~PaymentResponse();

private:
  nsCOMPtr<nsPIDOMWindowInner> mOwner;
};

} // namespace dom
} // namespace mozilla

#endif // mozilla_dom_PaymentResponse_h
