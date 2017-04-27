/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "PaymentRequestManager.h"
#include "mozilla/dom/ContentChild.h"
#include "mozilla/dom/TabChild.h"
#include "mozilla/dom/PaymentRequestChild.h"
#include "nsContentUtils.h"
#include "nsIJSON.h"
#include "nsString.h"

namespace mozilla {
namespace dom {

namespace {

PaymentRequestChild* gPaymentRequestChild = nullptr;

/*
 *  Following Convert* functions are used for convert PaymentRequest structs
 *  to transferable structs for IPC.
 */
nsString
SerializeFromJSObject(JSObject* aObject, nsresult& aRv)
{
  nsCOMPtr<nsIJSON> serializer = do_CreateInstance("@mozilla.org/dom/json;1");
  if (!serializer) {
    aRv = NS_ERROR_FAILURE;
    return EmptyString();
  }
  JS::Value value = JS::ObjectValue(*aObject);
  JSContext* cx = nsContentUtils::GetCurrentJSContext();
  MOZ_ASSERT(cx);
  nsString serializedObject;
  aRv = serializer->EncodeFromJSVal(&value, cx, serializedObject);
  if (NS_FAILED(aRv)) {
    return EmptyString();
  }
  return serializedObject;
}

IPCPaymentMethodData
ConvertMethodData(const PaymentMethodData& aMethodData, nsresult& aRv)
{
  aRv = NS_OK;
  nsTArray<nsString> supportedMethods;
  for (const nsString& method : aMethodData.mSupportedMethods) {
    supportedMethods.AppendElement(method);
  }
  nsString serializedData;
  if (aMethodData.mData.WasPassed()) {
    serializedData = SerializeFromJSObject(aMethodData.mData.Value(), aRv);
  }
  return IPCPaymentMethodData(supportedMethods, serializedData);
}

IPCPaymentCurrencyAmount
ConvertCurrencyAmount(const PaymentCurrencyAmount& aAmount)
{
  return IPCPaymentCurrencyAmount(aAmount.mCurrency,
                                  aAmount.mValue);
}

IPCPaymentItem
ConvertItem(const PaymentItem& item)
{
  IPCPaymentCurrencyAmount amount = ConvertCurrencyAmount(item.mAmount);
  return IPCPaymentItem(item.mLabel, amount, item.mPending);
}

IPCPaymentDetailsModifier
ConvertModifier(const PaymentDetailsModifier& aModifier, nsresult& aRv)
{
  aRv = NS_OK;
  nsTArray<nsString> supportedMethods;
  for (const nsString& method : aModifier.mSupportedMethods) {
    supportedMethods.AppendElement(method);
  }
  nsString serializedData;
  if (aModifier.mData.WasPassed()) {
    serializedData = SerializeFromJSObject(aModifier.mData.Value(), aRv);
  }
  IPCPaymentItem total = ConvertItem(aModifier.mTotal);
  nsTArray<IPCPaymentItem> additionalDisplayItems;
  if (aModifier.mAdditionalDisplayItems.WasPassed()) {
    for (const PaymentItem& item : aModifier.mAdditionalDisplayItems.Value()) {
      IPCPaymentItem displayItem = ConvertItem(item);
      additionalDisplayItems.AppendElement(displayItem);
    }
  }
  return IPCPaymentDetailsModifier(supportedMethods,
                                   total,
                                   additionalDisplayItems,
                                   serializedData);
}

IPCPaymentShippingOption
ConvertShippingOption(const PaymentShippingOption& aOption)
{
  IPCPaymentCurrencyAmount amount = ConvertCurrencyAmount(aOption.mAmount);
  return IPCPaymentShippingOption(aOption.mId, aOption.mLabel, amount, aOption.mSelected);
}

IPCPaymentDetails
ConvertDetailsInit(const PaymentDetailsInit& aDetails, nsresult& aRv)
{
  aRv = NS_OK;
  IPCPaymentItem total = ConvertItem(aDetails.mTotal);
  nsTArray<IPCPaymentItem> displayItems;
  if (aDetails.mDisplayItems.WasPassed()) {
    for (const PaymentItem& item : aDetails.mDisplayItems.Value()) {
      IPCPaymentItem displayItem = ConvertItem(item);
      displayItems.AppendElement(displayItem);
    }
  }
  nsTArray<IPCPaymentShippingOption> shippingOptions;
  if (aDetails.mShippingOptions.WasPassed()) {
    for (const PaymentShippingOption& option : aDetails.mShippingOptions.Value()) {
      IPCPaymentShippingOption shippingOption =
        ConvertShippingOption(option);
      shippingOptions.AppendElement(shippingOption);
    }
  }
  nsTArray<IPCPaymentDetailsModifier> modifiers;
  if (aDetails.mModifiers.WasPassed()) {
    for (const PaymentDetailsModifier& modifier : aDetails.mModifiers.Value()) {
      IPCPaymentDetailsModifier detailsModifier =
        ConvertModifier(modifier, aRv);
      if (NS_FAILED(aRv)) {
        break;
      }
      modifiers.AppendElement(detailsModifier);
    }
  }
  nsString id;
  if (aDetails.mId.WasPassed()) {
    id = aDetails.mId.Value();
  }
  return IPCPaymentDetails(id,
                           total,
                           displayItems,
                           shippingOptions,
                           modifiers,
                           EmptyString());
}

IPCPaymentDetails
ConvertDetailsUpdate(const PaymentDetailsUpdate& aDetails, nsresult& aRv)
{
  aRv = NS_OK;
  IPCPaymentItem total = ConvertItem(aDetails.mTotal);
  nsTArray<IPCPaymentItem> displayItems;
  if (aDetails.mDisplayItems.WasPassed()) {
    for (const PaymentItem& item : aDetails.mDisplayItems.Value()) {
      IPCPaymentItem displayItem = ConvertItem(item);
      displayItems.AppendElement(displayItem);
    }
  }
  nsTArray<IPCPaymentShippingOption> shippingOptions;
  if (aDetails.mShippingOptions.WasPassed()) {
    for (const PaymentShippingOption& option : aDetails.mShippingOptions.Value()) {
      IPCPaymentShippingOption shippingOption =
        ConvertShippingOption(option);
      shippingOptions.AppendElement(shippingOption);
    }
  }
  nsTArray<IPCPaymentDetailsModifier> modifiers;
  if (aDetails.mModifiers.WasPassed()) {
    for (const PaymentDetailsModifier& modifier : aDetails.mModifiers.Value()) {
      IPCPaymentDetailsModifier detailsModifier =
        ConvertModifier(modifier, aRv);
      if (NS_FAILED(aRv)) {
        break;
      }
      modifiers.AppendElement(detailsModifier);
    }
  }
  nsString error;
  if (aDetails.mError.WasPassed()) {
    error = aDetails.mError.Value();
  }
  return IPCPaymentDetails(EmptyString(),
                           total,
                           displayItems,
                           shippingOptions,
                           modifiers,
                           error);
}

IPCPaymentOptions
ConvertOptions(const PaymentOptions& aOptions)
{
  uint8_t shippingTypeIndex = static_cast<uint8_t>(aOptions.mShippingType);
  nsString shippingType(NS_LITERAL_STRING("shipping"));
  if (shippingTypeIndex < ArrayLength(PaymentShippingTypeValues::strings)) {
    shippingType.AssignASCII(
      PaymentShippingTypeValues::strings[shippingTypeIndex].value);
  }
  return IPCPaymentOptions(aOptions.mRequestPayerName,
                           aOptions.mRequestPayerEmail,
                           aOptions.mRequestPayerPhone,
                           aOptions.mRequestShipping,
                           shippingType);
}
} // end of namespace

/* PaymentRequestManager */

NS_IMPL_ISUPPORTS0(PaymentRequestManager);
StaticRefPtr<PaymentRequestManager> PaymentRequestManager::sSingleton;

PaymentRequestManager::PaymentRequestManager()
  : mRequestQueue()
{
  ContentChild* contentChild = ContentChild::GetSingleton();
  if (!gPaymentRequestChild) {
    gPaymentRequestChild = new PaymentRequestChild();
  }
  if (contentChild) {
    contentChild->SendPPaymentRequestConstructor(gPaymentRequestChild);
  }
}

PaymentRequestManager::~PaymentRequestManager()
{
  gPaymentRequestChild = nullptr;
}

already_AddRefed<PaymentRequestManager>
PaymentRequestManager::GetSingleton()
{
  if (!sSingleton) {
    sSingleton = new PaymentRequestManager();
  }
  RefPtr<PaymentRequestManager> manager = sSingleton.get();
  return manager.forget();
}

already_AddRefed<PaymentRequest>
PaymentRequestManager::GetPaymentRequestById(const nsAString& aRequestId)
{
  for (const RefPtr<PaymentRequest>& request : mRequestQueue) {
    nsString requestId;
    request->GetInternalId(requestId);
    if (aRequestId == requestId) {
      RefPtr<PaymentRequest> paymentRequest = request;
      return paymentRequest.forget();
    }
  }
  return nullptr;
}

nsresult
PaymentRequestManager::CreatePayment(nsPIDOMWindowInner* aWindow,
                                     const Sequence<PaymentMethodData>& aMethodData,
                                     const PaymentDetailsInit& aDetails,
                                     const PaymentOptions& aOptions,
                                     PaymentRequest** aRequest)
{
  NS_ENSURE_ARG_POINTER(aRequest);
  if (!gPaymentRequestChild) {
    aRequest = nullptr;
    return NS_ERROR_NOT_INITIALIZED;
  }

  TabChild* child = TabChild::GetFrom(aWindow->GetDocShell());
  NS_ENSURE_TRUE(child, NS_ERROR_FAILURE);

  nsresult rv;
  nsTArray<IPCPaymentMethodData> methodData;
  for (const PaymentMethodData& data : aMethodData) {
    IPCPaymentMethodData iData = ConvertMethodData(data, rv);
    if (NS_FAILED(rv)) {
      return rv;
    }
    methodData.AppendElement(iData);
  }

  IPCPaymentDetails details = ConvertDetailsInit(aDetails, rv);
  if (NS_FAILED(rv)) {
    return rv;
  }

  IPCPaymentOptions options = ConvertOptions(aOptions);

  RefPtr<PaymentRequest> paymentRequest = new PaymentRequest(aWindow);
  nsString requestId;
  paymentRequest->GetInternalId(requestId);

  /*
   *  Set request's |mId| to details.id if details.id exists.
   *  Otherwise, set |mId| to internal id.
   */
  if (aDetails.mId.WasPassed() && !aDetails.mId.Value().IsEmpty()) {
    paymentRequest->SetId(aDetails.mId.Value());
  } else {
    paymentRequest->SetId(requestId);
  }

  PaymentRequestCreateRequest request(child->GetTabId(),
                                      requestId,
                                      methodData,
                                      details,
                                      options);
  gPaymentRequestChild->SendRequestPayment(request);

  mRequestQueue.AppendElement(paymentRequest);
  paymentRequest.forget(aRequest);
  return NS_OK;
}

nsresult
PaymentRequestManager::ShowPayment(const nsAString& aRequestId)
{
  if (!gPaymentRequestChild) {
    return NS_ERROR_NOT_INITIALIZED;
  }

  RefPtr<PaymentRequest> request = GetPaymentRequestById(aRequestId);
  if (!request) {
    return NS_ERROR_UNEXPECTED;
  }

  nsString requestId(aRequestId);
  PaymentRequestShowRequest paymentAction(requestId);
  gPaymentRequestChild->SendRequestPayment(paymentAction);
  return NS_OK;
}

nsresult
PaymentRequestManager::AbortPayment(const nsAString& aRequestId)
{
  if (!gPaymentRequestChild) {
    return NS_ERROR_NOT_INITIALIZED;
  }

  RefPtr<PaymentRequest> request = GetPaymentRequestById(aRequestId);
  if (!request) {
    return NS_ERROR_UNEXPECTED;
  }

  nsString requestId(aRequestId);
  PaymentRequestAbortRequest paymentAction(requestId);
  gPaymentRequestChild->SendRequestPayment(paymentAction);
  return NS_OK;
}

nsresult
PaymentRequestManager::CanMakePayment(const nsAString& aRequestId)
{
  if (!gPaymentRequestChild) {
    return NS_ERROR_NOT_INITIALIZED;
  }

  RefPtr<PaymentRequest> request = GetPaymentRequestById(aRequestId);
  if (!request) {
    return NS_ERROR_UNEXPECTED;
  }

  nsString requestId(aRequestId);
  PaymentRequestCanMakeRequest paymentAction(requestId);
  gPaymentRequestChild->SendRequestPayment(paymentAction);
  return NS_OK;
}

nsresult
PaymentRequestManager::UpdatePayment(const nsAString& aRequestId,
                                     const PaymentDetailsUpdate& aDetails)
{
  if (!gPaymentRequestChild) {
    return NS_ERROR_NOT_INITIALIZED;
  }

  RefPtr<PaymentRequest> request = GetPaymentRequestById(aRequestId);
  if (!request) {
    return NS_ERROR_UNEXPECTED;
  }

  nsresult rv;
  IPCPaymentDetails details = ConvertDetailsUpdate(aDetails, rv);
  if (NS_FAILED(rv)) {
    return rv;
  }

  nsString requestId(aRequestId);
  PaymentRequestUpdateRequest paymentAction(requestId, details);
  gPaymentRequestChild->SendRequestPayment(paymentAction);
  return NS_OK;
}

nsresult
PaymentRequestManager::CompletePayment(const nsAString& aRequestId,
                                       const PaymentComplete& aComplete)
{
  if (!gPaymentRequestChild) {
    return NS_ERROR_NOT_INITIALIZED;
  }

  RefPtr<PaymentRequest> request = GetPaymentRequestById(aRequestId);
  if (!request) {
    return NS_ERROR_UNEXPECTED;
  }

  nsString completeStatusString(NS_LITERAL_STRING("unknown"));
  uint8_t completeIndex = static_cast<uint8_t>(aComplete);
  if (completeIndex < ArrayLength(PaymentCompleteValues::strings)) {
    completeStatusString.AssignASCII(
      PaymentCompleteValues::strings[completeIndex].value);
  }

  nsString requestId(aRequestId);
  PaymentRequestCompleteRequest paymentAction(requestId, completeStatusString);
  gPaymentRequestChild->SendRequestPayment(paymentAction);
  return NS_OK;
}

nsresult
PaymentRequestManager::RespondPayment(const PaymentRequestResponse& aResponse)
{
  switch (aResponse.type()) {
    case PaymentRequestResponse::TPaymentRequestCanMakeResponse: {
      PaymentRequestCanMakeResponse response = aResponse;
      RefPtr<PaymentRequest> request = GetPaymentRequestById(response.requestId());
      if (!request) {
        return NS_ERROR_FAILURE;
      }
      request->RespondCanMakePayment(response.result());
      break;
    }
    case PaymentRequestResponse::TPaymentRequestAbortResponse: {
      PaymentRequestAbortResponse response = aResponse;
      RefPtr<PaymentRequest> request = GetPaymentRequestById(response.requestId());
      if (!request) {
        return NS_ERROR_FAILURE;
      }
      request->RespondAbortPayment(response.isSucceeded());
      if (response.isSucceeded()) {
        mRequestQueue.RemoveElement(request);
      }
      break;
    }
    case PaymentRequestResponse::TPaymentRequestShowResponse: {
      PaymentRequestShowResponse response = aResponse;
      RefPtr<PaymentRequest> request = GetPaymentRequestById(response.requestId());
      if (!request) {
        return NS_ERROR_FAILURE;
      }
      /*
        response.data() is a serialized JSON object, and should be deserialized
        in PaymentResponse interface. Don't deserialize here because manager don't
        have proper JSContext to create JSObject.
      */
      request->RespondShowPayment(response.isAccepted(),
                                  response.methodName(),
                                  response.data(),
                                  response.payerName(),
                                  response.payerEmail(),
                                  response.payerPhone());
      break;
    }
    case PaymentRequestResponse::TPaymentRequestCompleteResponse: {
      PaymentRequestCompleteResponse response = aResponse;
      RefPtr<PaymentRequest> request = GetPaymentRequestById(response.requestId());
      if (!request) {
        return NS_ERROR_FAILURE;
      }
      request->RespondComplete();
      mRequestQueue.RemoveElement(request);
      break;
    }
    default: {
      return NS_ERROR_UNEXPECTED;
    }
  }
  return NS_OK;
}

nsresult
PaymentRequestManager::ChangeShippingAddress(const nsAString& aRequestId,
                                             const IPCPaymentAddress& aAddress)
{
  RefPtr<PaymentRequest> request = GetPaymentRequestById(aRequestId);
  if (!request) {
    return NS_ERROR_UNEXPECTED;
  }
  return request->UpdateShippingAddress(aAddress.country(),
                                        aAddress.addressLine(),
                                        aAddress.region(),
                                        aAddress.city(),
                                        aAddress.dependentLocality(),
                                        aAddress.postalCode(),
                                        aAddress.sortingCode(),
                                        aAddress.languageCode(),
                                        aAddress.organization(),
                                        aAddress.recipient(),
                                        aAddress.phone());
}

nsresult
PaymentRequestManager::ChangeShippingOption(const nsAString& aRequestId,
                                            const nsAString& aOption)
{
  RefPtr<PaymentRequest> request = GetPaymentRequestById(aRequestId);
  if (!request) {
    return NS_ERROR_UNEXPECTED;
  }

  return request->UpdateShippingOption(aOption);
}
} // end of namespace dom
} // end of namespace mozilla
