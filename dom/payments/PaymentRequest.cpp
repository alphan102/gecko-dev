/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/PaymentRequest.h"
#include "mozilla/dom/PaymentResponse.h"
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
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE(mResponse)
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE(mShippingAddress)
NS_IMPL_CYCLE_COLLECTION_TRAVERSE_END

NS_IMPL_CYCLE_COLLECTION_UNLINK_BEGIN_INHERITED(PaymentRequest,
                                                DOMEventTargetHelper)
  NS_IMPL_CYCLE_COLLECTION_UNLINK(mResponse)
  NS_IMPL_CYCLE_COLLECTION_UNLINK(mShippingAddress)
NS_IMPL_CYCLE_COLLECTION_UNLINK_END

NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION_INHERITED(PaymentRequest)
NS_INTERFACE_MAP_END_INHERITING(DOMEventTargetHelper)

NS_IMPL_ADDREF_INHERITED(PaymentRequest, DOMEventTargetHelper)
NS_IMPL_RELEASE_INHERITED(PaymentRequest, DOMEventTargetHelper)

bool
PaymentRequest::IsValidNumber(const nsAString& aItem,
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
PaymentRequest::IsValidDetailsInit(const PaymentDetailsInit& aDetails, nsAString& aErrorMsg)
{
  // Check the amount.value of detail.total
  if (!IsPositiveNumber(NS_LITERAL_STRING("details.total"),
                        aDetails.mTotal.mAmount.mValue, aErrorMsg)) {
    return false;
  }

  return IsValidDetailsBase(aDetails, aErrorMsg);
}

bool
PaymentRequest::IsValidDetailsUpdate(const PaymentDetailsUpdate& aDetails)
{
  nsString message;
  // Check the amount.value of detail.total
  if (!IsPositiveNumber(NS_LITERAL_STRING("details.total"),
                        aDetails.mTotal.mAmount.mValue, message)) {
    return false;
  }

  return IsValidDetailsBase(aDetails, message);
}

bool
PaymentRequest::IsValidDetailsBase(const PaymentDetailsBase& aDetails, nsAString& aErrorMsg)
{
  // Check the amount.value of each item in the display items
  if (aDetails.mDisplayItems.WasPassed()) {
    const Sequence<PaymentItem>& displayItems = aDetails.mDisplayItems.Value();
    for (const PaymentItem& displayItem : displayItems) {
      if (!IsValidNumber(displayItem.mLabel,
                         displayItem.mAmount.mValue, aErrorMsg)) {
        return false;
      }
    }
  }

  // Check the shipping option
  if (aDetails.mShippingOptions.WasPassed()) {
    const Sequence<PaymentShippingOption>& shippingOptions = aDetails.mShippingOptions.Value();
    for (const PaymentShippingOption& shippingOption : shippingOptions) {
      if (!IsValidNumber(NS_LITERAL_STRING("details.shippingOptions"),
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
          if (!IsValidNumber(displayItem.mLabel,
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

  // Check payment methods is done by webidl

  // Check payment details
  nsString message;
  if (!IsValidDetailsInit(aDetails, message)) {
    aRv.ThrowTypeError<MSG_ILLEGAL_PR_CONSTRUCTOR>(message);
    return nullptr;
  }

  // [TODO]
  // If the data member of modifier is present,
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
  , mShippingAddress(nullptr)
  , mUpdating(false)
  , mUpdateError(NS_OK)
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
  if (mState != eCreated) {
    aRv.Throw(NS_ERROR_DOM_INVALID_STATE_ERR);
    return nullptr;
  }

  nsCOMPtr<nsIGlobalObject> global = do_QueryInterface(GetOwner());
  ErrorResult result;
  RefPtr<Promise> promise = Promise::Create(global, result);
  if (result.Failed()) {
    aRv.Throw(NS_ERROR_FAILURE);
    mState = eClosed;
    return nullptr;
  }

  RefPtr<PaymentRequestManager> manager = PaymentRequestManager::GetSingleton();
  nsresult rv = manager->ShowPayment(mInternalId);
  if (NS_FAILED(rv)) {
    promise->MaybeReject(NS_ERROR_FAILURE);
    mState = eClosed;
    return promise.forget();
  }

  mAcceptPromise = promise;
  mState = eInteractive;
  return promise.forget();
}

nsresult
PaymentRequest::UpdatePayment(const PaymentDetailsUpdate& aDetails)
{
  RefPtr<PaymentRequestManager> manager = PaymentRequestManager::GetSingleton();
  return manager->UpdatePayment(mInternalId, aDetails);
}

void
PaymentRequest::AbortUpdate(nsresult aRv)
{
  MOZ_ASSERT(NS_FAILED(aRv));

  // Close down any remaining user interface.
  RefPtr<PaymentRequestManager> manager = PaymentRequestManager::GetSingleton();
  nsresult rv = manager->AbortPayment(mInternalId);
  if (NS_FAILED(rv)) {
    return;
  }

  // Remember update error |aRv| and do the following steps in RespondShowPayment.
  // 1. Set target.state to closed
  // 2. Reject the promise target.acceptPromise with exception "aRv"
  // 3. Abort the algorithm with update error
  mUpdateError = aRv;
}

void
PaymentRequest::RespondShowPayment(bool aAccept,
                                   const nsAString& aMethodName,
                                   const nsAString& aDetails,
                                   const nsAString& aPayerName,
                                   const nsAString& aPayerEmail,
                                   const nsAString& aPayerPhone,
                                   nsresult aRv)
{
  MOZ_ASSERT(mAcceptPromise);
  MOZ_ASSERT(ReadyForUpdate());

  if (aAccept) {
    // TODO : need to add aDetails into paymentResponse
    // TODO : need to add shipping option, hard-code "AIR" here
    RefPtr<PaymentResponse> paymentResponse =
      new PaymentResponse(GetOwner(), mInternalId, mId, aMethodName,
                          NS_LITERAL_STRING("AIR"), aPayerName,
                          aPayerEmail, aPayerPhone);
    mResponse = paymentResponse;
    mAcceptPromise->MaybeResolve(paymentResponse);
  } else {
    mAcceptPromise->MaybeReject(aRv);
  }

  mState = eClosed;
  mAcceptPromise = nullptr;
}

void
PaymentRequest::RespondComplete()
{
  MOZ_ASSERT(mResponse);

  mResponse->RespondComplete();
}

already_AddRefed<Promise>
PaymentRequest::CanMakePayment(ErrorResult& aRv)
{
  if (mState != eCreated) {
    aRv.Throw(NS_ERROR_DOM_INVALID_STATE_ERR);
    return nullptr;
  }

  if (mResultPromise) {
    aRv.Throw(NS_ERROR_DOM_NOT_ALLOWED_ERR);
    return nullptr;
  }

  nsCOMPtr<nsIGlobalObject> global = do_QueryInterface(GetOwner());
  ErrorResult result;
  RefPtr<Promise> promise = Promise::Create(global, result);
  if (result.Failed()) {
    aRv.Throw(NS_ERROR_FAILURE);
    return nullptr;
  }

  RefPtr<PaymentRequestManager> manager = PaymentRequestManager::GetSingleton();
  nsresult rv = manager->CanMakePayment(mInternalId);
  if (NS_FAILED(rv)) {
    promise->MaybeReject(NS_ERROR_FAILURE);
    return promise.forget();
  }

  mResultPromise = promise;
  return promise.forget();
}

void
PaymentRequest::RespondCanMakePayment(bool aResult)
{
  MOZ_ASSERT(mResultPromise);

  mResultPromise->MaybeResolve(aResult);
  mResultPromise = nullptr;
}

already_AddRefed<Promise>
PaymentRequest::Abort(ErrorResult& aRv)
{
  if (mState != eInteractive) {
    aRv.Throw(NS_ERROR_DOM_INVALID_STATE_ERR);
    return nullptr;
  }

  if (mAbortPromise) {
    aRv.Throw(NS_ERROR_DOM_NOT_ALLOWED_ERR);
    return nullptr;
  }

  nsCOMPtr<nsIGlobalObject> global = do_QueryInterface(GetOwner());
  ErrorResult result;
  RefPtr<Promise> promise = Promise::Create(global, result);
  if (result.Failed()) {
    aRv.Throw(NS_ERROR_FAILURE);
    return nullptr;
  }

  RefPtr<PaymentRequestManager> manager = PaymentRequestManager::GetSingleton();
  nsresult rv = manager->AbortPayment(mInternalId);
  if (NS_FAILED(rv)) {
    aRv.Throw(NS_ERROR_FAILURE);
    return nullptr;
  }

  mAbortPromise = promise;
  return promise.forget();
}

void
PaymentRequest::RespondAbortPayment(bool aSuccess)
{
  // Check whether we are aborting the update:
  //
  // - If |mUpdateError| is not NS_OK, we are aborting the update as
  //   |mUpdateError| was set in method |AbortUpdate|.
  //   => Reject |mAcceptPromise| and reset |mUpdateError| to complete
  //      the action, regardless of |aSuccess|.
  //
  // - Otherwise, we are handling |Abort| method call from merchant.
  //   => Resolve/Reject |mAbortPromise| based on |aSuccess|.
  if (NS_FAILED(mUpdateError)) {
    RespondShowPayment(false, EmptyString(), EmptyString(), EmptyString(),
                       EmptyString(), EmptyString(), mUpdateError);
    mUpdateError = NS_OK;
    return;
  }

  MOZ_ASSERT(mAbortPromise);

  if (aSuccess) {
    mAbortPromise->MaybeResolve(JS::UndefinedHandleValue);
    mState = eClosed;
  } else {
    mAbortPromise->MaybeReject(NS_ERROR_DOM_ABORT_ERR);
  }

  mAbortPromise = nullptr;
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

bool
PaymentRequest::ReadyForUpdate()
{
  return mState == eInteractive && !mUpdating;
}

void
PaymentRequest::SetUpdating(bool aUpdating)
{
  mUpdating = aUpdating;
}

already_AddRefed<PaymentAddress>
PaymentRequest::GetShippingAddress() const
{
  RefPtr<PaymentAddress> address = mShippingAddress;
  return address.forget();
}

nsresult
PaymentRequest::DispatchUpdateEvent(const nsAString& aType)
{
  MOZ_ASSERT(ReadyForUpdate());

  PaymentRequestUpdateEventInit init;
  init.mBubbles = false;
  init.mCancelable = false;

  RefPtr<PaymentRequestUpdateEvent> event =
    PaymentRequestUpdateEvent::Constructor(this, aType, init);
  event->SetTrusted(true);

  return DispatchDOMEvent(nullptr, event, nullptr, nullptr);
}

nsresult
PaymentRequest::UpdateShippingAddress(const nsAString& aCountry,
                                      const nsTArray<nsString>& aAddressLine,
                                      const nsAString& aRegion,
                                      const nsAString& aCity,
                                      const nsAString& aDependentLocality,
                                      const nsAString& aPostalCode,
                                      const nsAString& aSortingCode,
                                      const nsAString& aLanguageCode,
                                      const nsAString& aOrganization,
                                      const nsAString& aRecipient,
                                      const nsAString& aPhone)
{
  mShippingAddress = new PaymentAddress(GetOwner(), aCountry, aAddressLine,
                                        aRegion, aCity, aDependentLocality,
                                        aPostalCode, aSortingCode, aLanguageCode,
                                        aOrganization, aRecipient, aPhone);

  // Fire shippingaddresschange event
  return DispatchUpdateEvent(NS_LITERAL_STRING("shippingaddresschange"));
}

void
PaymentRequest::GetShippingOption(nsAString& aRetVal) const
{
  aRetVal = mShippingOption;
}

nsresult
PaymentRequest::UpdateShippingOption(const nsAString& aShippingOption)
{
  mShippingOption = aShippingOption;

  // Fire shippingaddresschange event
  return DispatchUpdateEvent(NS_LITERAL_STRING("shippingoptionchange"));
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
