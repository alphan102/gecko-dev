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
#include "mozilla/dom/Promise.h"

namespace mozilla {
namespace dom {

class EventHandlerNonNull;
class PaymentAddress;
class PaymentResponse;

} // namespace dom
} // namespace mozilla

namespace mozilla {
namespace dom {

class PaymentRequest final : public DOMEventTargetHelper
{
public:
  NS_DECL_ISUPPORTS_INHERITED

  PaymentRequest(nsPIDOMWindowInner* aWindow);

  nsPIDOMWindowInner* GetParentObject() const
  {
    return GetOwner();
  }

  virtual JSObject* WrapObject(JSContext* aCx,
                               JS::Handle<JSObject*> aGivenProto) override;

  static bool
    IsVaildNumber(const nsAString& aItem,
                  const nsAString& aStr,
                  nsAString& aErrorMsg);
  static bool
    IsPositiveNumber(const nsAString& aItem,
                     const nsAString& aStr,
                     nsAString& aErrorMsg);

  static already_AddRefed<PaymentRequest>
    Constructor(const GlobalObject& aGlobal,
                const Sequence<PaymentMethodData>& aMethodData,
                const PaymentDetailsInit& aDetails,
                const PaymentOptions& aOptions,
                ErrorResult& aRv);

  already_AddRefed<Promise> Show(ErrorResult& aRv);
  void RespondShowPayment(bool aAccept,
                          const nsAString& aMethodName,
                          const nsAString& aPayerName,
                          const nsAString& aPayerEmail,
                          const nsAString& aPayerPhone);

  already_AddRefed<Promise> Abort(ErrorResult& aRv);
  void RespondAbortPayment(bool aResult);

  already_AddRefed<Promise> CanMakePayment(ErrorResult& aRv);
  void RespondCanMakePayment(bool aResult);

  void GetId(nsAString& aRetVal) const;
  void GetInternalId(nsAString& aRetVal);
  void SetId(const nsAString& aId);

  already_AddRefed<PaymentAddress> GetShippingAddress() const;
  void GetShippingOption(nsAString& aRetVal) const;

  Nullable<PaymentShippingType> GetShippingType() const;

  IMPL_EVENT_HANDLER(shippingaddresschange);
  IMPL_EVENT_HANDLER(shippingoptionchange);

protected:
  ~PaymentRequest();

  nsCOMPtr<nsPIDOMWindowInner> mWindow;

  // Id for internal identification
  nsString mInternalId;
  // Id for communicating with merchant side
  // mId is initialized to details.id if it exists
  // otherwise, mId has the same value as mInternalId.
  nsString mId;
  // Promise for "PaymentRequest::CanMakePayment"
  RefPtr<Promise> mResultPromise;
  // Promise for "PaymentRequest::Show"
  RefPtr<Promise> mAcceptPromise;
  // Promise for "PaymentRequest::Abort"
  RefPtr<Promise> mAbortPromise;
  // Resolve mAcceptPromise with mResponse if user accepts the request.
  RefPtr<PaymentResponse> mResponse;
  enum {
    eUnknown,
    eCreated,
    eInteractive,
    eClosed
  } mState;
};

} // namespace dom
} // namespace mozilla

#endif // mozilla_dom_PaymentRequest_h
