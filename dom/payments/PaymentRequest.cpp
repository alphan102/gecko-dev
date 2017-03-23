/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/PaymentRequest.h"
#include "mozilla/dom/PaymentResponse.h"
#include "nsContentUtils.h"

namespace mozilla {
namespace dom {


// [TODO] Revisit here once |mPaymentAddress| is declared
NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION_INHERITED(PaymentRequest)
NS_INTERFACE_MAP_END_INHERITING(DOMEventTargetHelper)

NS_IMPL_ADDREF_INHERITED(PaymentRequest, DOMEventTargetHelper)
NS_IMPL_RELEASE_INHERITED(PaymentRequest, DOMEventTargetHelper)



bool
PaymentRequest::IsVaildNumber(const nsAString& aItem,
                              const nsAString& aStr,
                              nsAString& aErrorMsg)
{
  nsAutoString aValue(aStr);
  nsresult error = NS_OK;
  aValue.ToFloat(&error);
  if (NS_FAILED(error)) {
    aErrorMsg.AssignLiteral("The amount.value of \"");
    aErrorMsg.Append(aItem);
    aErrorMsg.AppendLiteral("\"(");
    aErrorMsg.Append(aValue);
    aErrorMsg.AppendLiteral(") must be a valid decimal monetary value.");
    return false;
  }
  return true;
}

bool
PaymentRequest::IsPositiveNumber(const nsAString& aItem,
                                 const nsAString& aStr,
                                 nsAString& aErrorMsg)
{
  nsAutoString aValue(aStr);
  nsresult error = NS_OK;
  float value = aValue.ToFloat(&error);
  if (NS_FAILED(error) || value < 0) {
    aErrorMsg.AssignLiteral("The amount.value of \"");
    aErrorMsg.Append(aItem);
    aErrorMsg.AppendLiteral("\"(");
    aErrorMsg.Append(aValue);
    aErrorMsg.AppendLiteral(") must be a valid and positive decimal monetary value.");
    return false;
  }
  return true;
}

already_AddRefed<PaymentRequest>
PaymentRequest::Constructor(const GlobalObject& aGlobal,
                            const Sequence<PaymentMethodData>& aMethodData,
                            const PaymentDetailsInit& aDetails,
                            const PaymentOptions& aOptions,
                            ErrorResult& aRv)
{
  nsCOMPtr<nsPIDOMWindowInner> window = do_QueryInterface(aGlobal.GetAsSupports());
  if (!window) {
    aRv.Throw(NS_ERROR_UNEXPECTED);
    return nullptr;
  }

  // Check payment methods is done by webidl

  // Check the amount.value of detail.total
  nsString message;
  if (!IsPositiveNumber(NS_LITERAL_STRING("details.total"),
                        aDetails.mTotal.mAmount.mValue, message)) {
    aRv.ThrowTypeError<MSG_ILLEGAL_PR_CONSTRUCTOR>(message);
    return nullptr;
  }

  // Check the amount.value of each item in the display items
  if (aDetails.mDisplayItems.WasPassed()) {
    const Sequence<PaymentItem>& displayItems = aDetails.mDisplayItems.Value();
    for (const PaymentItem& displayItem : displayItems) {
      if (!IsVaildNumber(displayItem.mLabel,
                         displayItem.mAmount.mValue, message)) {
        aRv.ThrowTypeError<MSG_ILLEGAL_PR_CONSTRUCTOR>(message);
        return nullptr;
      }
    }
  }

  // Check the shipping option
  if (aDetails.mShippingOptions.WasPassed()) {
    const Sequence<PaymentShippingOption>& shippingOptions = aDetails.mShippingOptions.Value();
    for (const PaymentShippingOption& shippingOption : shippingOptions) {
      if (!IsVaildNumber(NS_LITERAL_STRING("details.shippingOptions"),
                         shippingOption.mAmount.mValue, message)) {
        aRv.ThrowTypeError<MSG_ILLEGAL_PR_CONSTRUCTOR>(message);
        return nullptr;
      }
    }
  }

  // Check payment details modifiers
  if (aDetails.mModifiers.WasPassed()) {
    const Sequence<PaymentDetailsModifier>& modifiers = aDetails.mModifiers.Value();
    for (const PaymentDetailsModifier& modifier : modifiers) {
      if (!IsPositiveNumber(NS_LITERAL_STRING("details.modifiers.total"),
                            modifier.mTotal.mAmount.mValue, message)) {
        aRv.ThrowTypeError<MSG_ILLEGAL_PR_CONSTRUCTOR>(message);
        return nullptr;
      }
    }
  }

  // TODO : Use PaymentRequestManager::CreatePayment to create PaymentRequest
  // Need Bug 1345390 to complete the functionality.
  // We also set mId in "CreatePayment()"
  return nullptr;
}

PaymentRequest::PaymentRequest(nsPIDOMWindowInner* aWindow)
  : DOMEventTargetHelper(aWindow)
  , mWindow(aWindow)
  , mState(eUnknown)
{
  // Generate a unique id for identification
  nsID uuid;
  nsContentUtils::GenerateUUIDInPlace(uuid);
  char buffer[NSID_LENGTH];
  uuid.ToProvidedString(buffer);
  CopyASCIItoUTF16(buffer, mInternalId);

  mState = eCreated;
}

already_AddRefed<Promise>
PaymentRequest::Show(ErrorResult& aRv)
{
  return nullptr;
}

void
PaymentRequest::RespondCanMakePayment(bool aResult)
{
}

already_AddRefed<Promise>
PaymentRequest::CanMakePayment(ErrorResult& aRv)
{
  return nullptr;
}

already_AddRefed<Promise>
PaymentRequest::Abort(ErrorResult& aRv)
{
  return nullptr;
}

void
PaymentRequest::RespondAbortPayment(bool aSuccess)
{
}

void
PaymentRequest::RespondShowPayment(bool aAccept,
                                   const nsAString& aMethodName,
                                   const nsAString& aPayerName,
                                   const nsAString& aPayerEmail,
                                   const nsAString& aPayerPhone)
{
}

void
PaymentRequest::GetId(nsAString& aRetVal) const
{
  aRetVal = mId;
}

void
PaymentRequest::GetInternalId(nsAString& aRetVal)
{
  aRetVal = mInternalId;
}

void
PaymentRequest::SetId(const nsAString& aId)
{
  mId = aId;
}

already_AddRefed<PaymentAddress>
PaymentRequest::GetShippingAddress() const
{
  return nullptr;
}

void
PaymentRequest::GetShippingOption(nsAString& aRetVal) const
{
}

Nullable<PaymentShippingType>
PaymentRequest::GetShippingType() const
{
  return nullptr;
}

PaymentRequest::~PaymentRequest()
{
}

JSObject*
PaymentRequest::WrapObject(JSContext* aCx, JS::Handle<JSObject*> aGivenProto)
{
  return PaymentRequestBinding::Wrap(aCx, this, aGivenProto);
}


} // namespace dom
} // namespace mozilla
