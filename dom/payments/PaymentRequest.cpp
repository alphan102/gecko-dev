/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/PaymentRequest.h"
#include "nsContentUtils.h"
#include "PaymentRequestManager.h"

namespace mozilla {
namespace dom {

NS_IMPL_CYCLE_COLLECTION_CLASS(PaymentRequest)


NS_IMPL_CYCLE_COLLECTION_TRACE_BEGIN_INHERITED(PaymentRequest,
                                               DOMEventTargetHelper)
  // Don't need NS_IMPL_CYCLE_COLLECTION_TRACE_PRESERVED_WRAPPER because
  // DOMEventTargetHelper does it for us.
NS_IMPL_CYCLE_COLLECTION_TRACE_END

NS_IMPL_CYCLE_COLLECTION_TRAVERSE_BEGIN_INHERITED(PaymentRequest,
                                                  DOMEventTargetHelper)
NS_IMPL_CYCLE_COLLECTION_TRAVERSE_END

NS_IMPL_CYCLE_COLLECTION_UNLINK_BEGIN_INHERITED(PaymentRequest,
                                                DOMEventTargetHelper)
NS_IMPL_CYCLE_COLLECTION_UNLINK_END

NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION_INHERITED(PaymentRequest)
NS_INTERFACE_MAP_END_INHERITING(DOMEventTargetHelper)

NS_IMPL_ADDREF_INHERITED(PaymentRequest, DOMEventTargetHelper)
NS_IMPL_RELEASE_INHERITED(PaymentRequest, DOMEventTargetHelper)

bool
PaymentRequest::PrefEnabled(JSContext* aCx, JSObject* aObj)
{
  return Preferences::GetBool("dom.payments.request.enabled");
}

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

bool
PaymentRequest::IsVaildDetailsInit(const PaymentDetailsInit& aDetails, nsAString& aErrorMsg)
{
  // Check the amount.value of detail.total
  if (!IsPositiveNumber(NS_LITERAL_STRING("details.total"),
                        aDetails.mTotal.mAmount.mValue, aErrorMsg)) {
    return false;
  }

  return IsVaildDetailsBase(aDetails, aErrorMsg);
}

bool
PaymentRequest::IsVaildDetailsBase(const PaymentDetailsBase& aDetails, nsAString& aErrorMsg)
{
  // Check the amount.value of each item in the display items
  if (aDetails.mDisplayItems.WasPassed()) {
    const Sequence<PaymentItem>& displayItems = aDetails.mDisplayItems.Value();
    for (const PaymentItem& displayItem : displayItems) {
      if (!IsVaildNumber(displayItem.mLabel,
                         displayItem.mAmount.mValue, aErrorMsg)) {
        return false;
      }
    }
  }

  // Check the shipping option
  if (aDetails.mShippingOptions.WasPassed()) {
    const Sequence<PaymentShippingOption>& shippingOptions = aDetails.mShippingOptions.Value();
    for (const PaymentShippingOption& shippingOption : shippingOptions) {
      if (!IsVaildNumber(NS_LITERAL_STRING("details.shippingOptions"),
                         shippingOption.mAmount.mValue, aErrorMsg)) {
        return false;
      }
    }
  }

  // Check payment details modifiers
  if (aDetails.mModifiers.WasPassed()) {
    const Sequence<PaymentDetailsModifier>& modifiers = aDetails.mModifiers.Value();
    for (const PaymentDetailsModifier& modifier : modifiers) {
      if (!IsPositiveNumber(NS_LITERAL_STRING("details.modifiers.total"),
                            modifier.mTotal.mAmount.mValue, aErrorMsg)) {
        return false;
      }
      if (modifier.mAdditionalDisplayItems.WasPassed()) {
        const Sequence<PaymentItem>& displayItems = modifier.mAdditionalDisplayItems.Value();
        for (const PaymentItem& displayItem : displayItems) {
          if (!IsVaildNumber(displayItem.mLabel,
                             displayItem.mAmount.mValue, aErrorMsg)) {
            return false;
          }
        }
      }
    }
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

  // [TODO] Bug 1318988 - Implement `allowPaymentRequest` on iframe

  // Check payment methods is done by webidl

  // Check payment details
  nsString message;
  if (!IsVaildDetailsInit(aDetails, message)) {
    aRv.ThrowTypeError<MSG_ILLEGAL_PR_CONSTRUCTOR>(message);
    return nullptr;
  }

  // [TODO]
  // If the data member of modifier(aDetails.mModifiers) is present,
  // let serializedData be the result of JSON-serializing modifier.data into a string.
  // null if it is not.

  RefPtr<PaymentRequestManager> manager = PaymentRequestManager::GetSingleton();

  // Create PaymentRequest and set its |mId|
  RefPtr<PaymentRequest> request;
  nsresult rv = manager->CreatePayment(window, aMethodData, aDetails, aOptions, getter_AddRefs(request));

  return NS_FAILED(rv) ? nullptr : request.forget();
}

PaymentRequest::PaymentRequest(nsPIDOMWindowInner* aWindow)
  : DOMEventTargetHelper(aWindow)
  , mUpdating(false)
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

void
PaymentRequest::SetUpdating(bool aUpdating)
{
  mUpdating = aUpdating;
}

void
PaymentRequest::GetShippingOption(nsAString& aRetVal) const
{
  aRetVal = mShippingOption;
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
