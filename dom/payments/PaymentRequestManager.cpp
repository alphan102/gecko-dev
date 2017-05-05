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
                                   serializedData,
                                   aModifier.mAdditionalDisplayItems.WasPassed());
}

IPCPaymentShippingOption
ConvertShippingOption(const PaymentShippingOption& aOption)
{
  IPCPaymentCurrencyAmount amount = ConvertCurrencyAmount(aOption.mAmount);
  return IPCPaymentShippingOption(aOption.mId, aOption.mLabel, amount, aOption.mSelected);
}

void
ConvertDetailsBase(const PaymentDetailsBase& aDetails,
                   nsTArray<IPCPaymentItem>& aDisplayItems,
                   nsTArray<IPCPaymentShippingOption>& aShippingOptions,
                   nsTArray<IPCPaymentDetailsModifier>& aModifiers,
                   nsresult& aRv)
{
  aRv = NS_OK;
  if (aDetails.mDisplayItems.WasPassed()) {
    for (const PaymentItem& item : aDetails.mDisplayItems.Value()) {
      IPCPaymentItem displayItem = ConvertItem(item);
      aDisplayItems.AppendElement(displayItem);
    }
  }
  if (aDetails.mShippingOptions.WasPassed()) {
    for (const PaymentShippingOption& option : aDetails.mShippingOptions.Value()) {
      IPCPaymentShippingOption shippingOption =
        ConvertShippingOption(option);
      aShippingOptions.AppendElement(shippingOption);
    }
  }
  if (aDetails.mModifiers.WasPassed()) {
    for (const PaymentDetailsModifier& modifier : aDetails.mModifiers.Value()) {
      IPCPaymentDetailsModifier detailsModifier =
        ConvertModifier(modifier, aRv);
      if (NS_FAILED(aRv)) {
        break;
      }
      aModifiers.AppendElement(detailsModifier);
    }
  }
}

IPCPaymentDetails
ConvertDetailsInit(const PaymentDetailsInit& aDetails, nsresult& aRv)
{
  // Convert PaymentDetailsBase members
  nsTArray<IPCPaymentItem> displayItems;
  nsTArray<IPCPaymentShippingOption> shippingOptions;
  nsTArray<IPCPaymentDetailsModifier> modifiers;
  ConvertDetailsBase(aDetails, displayItems, shippingOptions, modifiers, aRv);

  // Convert |id|
  nsString id(EmptyString());
  if (aDetails.mId.WasPassed()) {
    id = aDetails.mId.Value();
  }

  // Convert required |total|
  IPCPaymentItem total = ConvertItem(aDetails.mTotal);

  return IPCPaymentDetails(id,
                           total,
                           displayItems,
                           shippingOptions,
                           modifiers,
                           EmptyString(), // error message
                           aDetails.mDisplayItems.WasPassed(),
                           aDetails.mShippingOptions.WasPassed(),
                           aDetails.mModifiers.WasPassed());
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
  : mPaymentRequestChild(nullptr)
  , mRequestQueue()
{
}

PaymentRequestManager::~PaymentRequestManager()
{
  ClearPaymentRequestChild();
}

void
PaymentRequestManager::CreatePaymentRequestChild()
{
  if (!mPaymentRequestChild) {
    mPaymentRequestChild = new PaymentRequestChild();
    ContentChild* contentChild = ContentChild::GetSingleton();
    if (contentChild) {
      contentChild->SendPPaymentRequestConstructor(mPaymentRequestChild);
    }
  }
}

void
PaymentRequestManager::ClearPaymentRequestChild()
{
  if (mPaymentRequestChild) {
    PPaymentRequestChild::Send__delete__(mPaymentRequestChild);
    mPaymentRequestChild = nullptr;
  }
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
  MOZ_ASSERT(NS_IsMainThread());
  NS_ENSURE_ARG_POINTER(aRequest);

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

  CreatePaymentRequestChild();

  PaymentCreateActionRequest request(child->GetTabId(),
                                     requestId,
                                     methodData,
                                     details,
                                     options);
  mPaymentRequestChild->SendRequestPayment(request);

  ClearPaymentRequestChild();

  mRequestQueue.AppendElement(paymentRequest);
  paymentRequest.forget(aRequest);
  return NS_OK;
}

} // end of namespace dom
} // end of namespace mozilla
