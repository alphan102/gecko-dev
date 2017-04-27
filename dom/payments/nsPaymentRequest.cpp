/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsArrayUtils.h"
#include "nsIMutableArray.h"
#include "nsISupportsPrimitives.h"
#include "nsPaymentRequest.h"
#include "nsPaymentRequestUtils.h"

namespace mozilla {
namespace dom{

/* nsPaymentMethodData */

NS_IMPL_ISUPPORTS(nsPaymentMethodData,
                  nsIPaymentMethodData)

nsPaymentMethodData::nsPaymentMethodData(nsIArray* aSupportedMethods,
                                         const nsAString& aData)
  : mSupportedMethods(aSupportedMethods)
  , mData(aData)
{
}

nsPaymentMethodData::nsPaymentMethodData(const IPCPaymentMethodData& aMethodData)
  : mData(aMethodData.data())
{
  ConvertStringstoISupportsStrings(aMethodData.supportedMethods(),
                                   getter_AddRefs(mSupportedMethods));
}

nsPaymentMethodData::~nsPaymentMethodData()
{
}

NS_IMETHODIMP
nsPaymentMethodData::GetSupportedMethods(nsIArray** aSupportedMethods)
{
  NS_ENSURE_ARG_POINTER(aSupportedMethods);
  MOZ_ASSERT(mSupportedMethods);
  nsCOMPtr<nsIMutableArray> methods = do_CreateInstance(NS_ARRAY_CONTRACTID);
  uint32_t length;
  mSupportedMethods->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsISupportsString> method = do_QueryElementAt(mSupportedMethods, index);
    methods->AppendElement(method, false);
  }
  methods.forget(aSupportedMethods);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentMethodData::GetData(nsAString& aData)
{
  aData = mData;
  return NS_OK;
}

/* nsPaymentCurrencyAmount */

NS_IMPL_ISUPPORTS(nsPaymentCurrencyAmount,
                  nsIPaymentCurrencyAmount)

nsPaymentCurrencyAmount::nsPaymentCurrencyAmount(const nsAString& aCurrency,
                                                 const nsAString& aValue)
  : mCurrency(aCurrency)
  , mValue(aValue)
{
}

nsPaymentCurrencyAmount::nsPaymentCurrencyAmount(const IPCPaymentCurrencyAmount& aAmount)
  : mCurrency(aAmount.currency())
  , mValue(aAmount.value())
{
}

nsPaymentCurrencyAmount::~nsPaymentCurrencyAmount()
{
}

NS_IMETHODIMP
nsPaymentCurrencyAmount::GetCurrency(nsAString& aCurrency)
{
  aCurrency = mCurrency;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentCurrencyAmount::GetValue(nsAString& aValue)
{
  aValue = mValue;
  return NS_OK;
}

/* nsPaymentItem */

NS_IMPL_ISUPPORTS(nsPaymentItem,
                  nsIPaymentItem)

nsPaymentItem::nsPaymentItem(const nsAString& aLabel,
                             nsIPaymentCurrencyAmount* aAmount,
                             const bool aPending)
  : mLabel(aLabel)
  , mAmount(aAmount)
  , mPending(aPending)
{
}

nsPaymentItem::nsPaymentItem(const IPCPaymentItem& aItem)
  : mLabel(aItem.label())
  , mPending(aItem.pending())
{
  mAmount = new nsPaymentCurrencyAmount(aItem.amount());
}

nsPaymentItem::~nsPaymentItem()
{
}

NS_IMETHODIMP
nsPaymentItem::GetLabel(nsAString& aLabel)
{
  aLabel = mLabel;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentItem::GetAmount(nsIPaymentCurrencyAmount** aAmount)
{
  NS_ENSURE_ARG_POINTER(aAmount);
  nsString system;
  nsString currency;
  nsString value;
  mAmount->GetCurrency(currency);
  mAmount->GetValue(value);
  RefPtr<nsPaymentCurrencyAmount> amount =
    new nsPaymentCurrencyAmount(currency, value);
  amount.forget(aAmount);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentItem::GetPending(bool* aPending)
{
  NS_ENSURE_ARG_POINTER(aPending);
  *aPending = mPending;
  return NS_OK;
}

/* nsPaymentDetailsModifier */

NS_IMPL_ISUPPORTS(nsPaymentDetailsModifier,
                  nsIPaymentDetailsModifier)

nsPaymentDetailsModifier::nsPaymentDetailsModifier(nsIArray* aSupportedMethods,
                                                   nsIPaymentItem* aTotal,
                                                   nsIArray* aAdditionalDisplayItems,
                                                   const nsAString& aData)
  : mSupportedMethods(aSupportedMethods)
  , mTotal(aTotal)
  , mAdditionalDisplayItems(aAdditionalDisplayItems)
  , mData(aData)
{
}

nsPaymentDetailsModifier::nsPaymentDetailsModifier(const IPCPaymentDetailsModifier& aModifier)
  : mAdditionalDisplayItems(nullptr)
  , mData(aModifier.data())
{
  mTotal = new nsPaymentItem(aModifier.total());

  ConvertStringstoISupportsStrings(aModifier.supportedMethods(),
                                   getter_AddRefs(mSupportedMethods));

  if (aModifier.additionalDisplayItemsPassed()) {
    nsCOMPtr<nsIMutableArray> items = do_CreateInstance(NS_ARRAY_CONTRACTID);
    for (const IPCPaymentItem& item : aModifier.additionalDisplayItems()) {
      nsCOMPtr<nsIPaymentItem> iitem = new nsPaymentItem(item);
      items->AppendElement(iitem, false);
    }
    mAdditionalDisplayItems = items.forget();
  }
}

nsPaymentDetailsModifier::~nsPaymentDetailsModifier()
{
}

NS_IMETHODIMP
nsPaymentDetailsModifier::GetSupportedMethods(nsIArray** aSupportedMethods)
{
  NS_ENSURE_ARG_POINTER(aSupportedMethods);
  MOZ_ASSERT(mSupportedMethods);
  nsCOMPtr<nsIMutableArray> methods = do_CreateInstance(NS_ARRAY_CONTRACTID);
  uint32_t length;
  mSupportedMethods->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsISupportsString> method = do_QueryElementAt(mSupportedMethods, index);
    methods->AppendElement(method, false);
  }
  methods.forget(aSupportedMethods);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentDetailsModifier::GetTotal(nsIPaymentItem** aTotal)
{
  NS_ENSURE_ARG_POINTER(aTotal);
  nsString label;
  nsCOMPtr<nsIPaymentCurrencyAmount> amount;
  bool pending;
  mTotal->GetLabel(label);
  mTotal->GetAmount(getter_AddRefs(amount));
  mTotal->GetPending(&pending);
  RefPtr<nsPaymentItem> total = new nsPaymentItem(label, amount, pending);
  total.forget(aTotal);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentDetailsModifier::GetAdditionalDisplayItems(nsIArray** aAdditionalDisplayItems)
{
  NS_ENSURE_ARG_POINTER(aAdditionalDisplayItems);
  if (!mAdditionalDisplayItems) {
    return NS_OK;
  }
  nsCOMPtr<nsIMutableArray> items = do_CreateInstance(NS_ARRAY_CONTRACTID);
  uint32_t length;
  mAdditionalDisplayItems->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsIPaymentItem> item = do_QueryElementAt(mAdditionalDisplayItems, index);
    items->AppendElement(item, false);
  }
  items.forget(aAdditionalDisplayItems);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentDetailsModifier::GetData(nsAString& aData)
{
  aData = mData;
  return NS_OK;
}

/* nsPaymentShippingOption */

NS_IMPL_ISUPPORTS(nsPaymentShippingOption,
                  nsIPaymentShippingOption)

nsPaymentShippingOption::nsPaymentShippingOption(const nsAString& aId,
                                                 const nsAString& aLabel,
                                                 nsIPaymentCurrencyAmount* aAmount,
                                                 const bool aSelected)
  : mId(aId)
  , mLabel(aLabel)
  , mAmount(aAmount)
  , mSelected(aSelected)
{
}

nsPaymentShippingOption::nsPaymentShippingOption(const IPCPaymentShippingOption& aShippingOption)
  : mId(aShippingOption.id())
  , mLabel(aShippingOption.label())
  , mSelected(aShippingOption.selected())
{
  mAmount = new nsPaymentCurrencyAmount(aShippingOption.amount());
}

nsPaymentShippingOption::~nsPaymentShippingOption()
{
}

NS_IMETHODIMP
nsPaymentShippingOption::GetId(nsAString& aId)
{
  aId = mId;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentShippingOption::GetLabel(nsAString& aLabel)
{
  aLabel = mLabel;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentShippingOption::GetAmount(nsIPaymentCurrencyAmount** aAmount)
{
  NS_ENSURE_ARG_POINTER(aAmount);
  nsString system;
  nsString currency;
  nsString value;
  mAmount->GetCurrency(currency);
  mAmount->GetValue(value);
  RefPtr<nsPaymentCurrencyAmount> amount =
    new nsPaymentCurrencyAmount(currency, value);
  amount.forget(aAmount);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentShippingOption::GetSelected(bool* aSelected)
{
  NS_ENSURE_ARG_POINTER(aSelected);
  *aSelected = mSelected;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentShippingOption::SetSelected(bool aSelected)
{
  mSelected = aSelected;
  return NS_OK;
}

/* nsPaymentDetails */

NS_IMPL_ISUPPORTS(nsPaymentDetails,
                  nsIPaymentDetails)

nsPaymentDetails::nsPaymentDetails(const nsAString& aId,
                                   nsIPaymentItem* aTotalItem,
                                   nsIArray* aDisplayItems,
                                   nsIArray* aShippingOptions,
                                   nsIArray* aModifiers,
                                   const nsAString& aError)
  : mId(aId)
  , mTotalItem(aTotalItem)
  , mDisplayItems(aDisplayItems)
  , mShippingOptions(aShippingOptions)
  , mModifiers(aModifiers)
  , mError(aError)
{
}

nsPaymentDetails::nsPaymentDetails(const IPCPaymentDetails& aDetails)
  : mId(aDetails.id())
  , mDisplayItems(nullptr)
  , mShippingOptions(nullptr)
  , mModifiers(nullptr)
  , mError(aDetails.error())
{
  mTotalItem = new nsPaymentItem(aDetails.total());

  if (aDetails.displayItemsPassed()) {
    nsCOMPtr<nsIMutableArray> items = do_CreateInstance(NS_ARRAY_CONTRACTID);
    for (const IPCPaymentItem& displayItem : aDetails.displayItems()) {
      nsCOMPtr<nsIPaymentItem> item = new nsPaymentItem(displayItem);
      items->AppendElement(item, false);
    }
    mDisplayItems = items.forget();
  }

  if (aDetails.shippingOptionsPassed()) {
    nsCOMPtr<nsIMutableArray> options = do_CreateInstance(NS_ARRAY_CONTRACTID);
    for (const IPCPaymentShippingOption& shippingOption : aDetails.shippingOptions()) {
      nsCOMPtr<nsIPaymentShippingOption> option =
        new nsPaymentShippingOption(shippingOption);
      options->AppendElement(option, false);
    }
    mShippingOptions = options.forget();
  }

  if (aDetails.modifiersPassed()) {
    nsCOMPtr<nsIMutableArray> modifiers = do_CreateInstance(NS_ARRAY_CONTRACTID);
    for (const IPCPaymentDetailsModifier& modifier : aDetails.modifiers()) {
      nsCOMPtr<nsIPaymentDetailsModifier> imodifier = new nsPaymentDetailsModifier(modifier);
      modifiers->AppendElement(imodifier, false);
    }
    mModifiers = modifiers.forget();
  }
}

nsPaymentDetails::~nsPaymentDetails()
{
}

NS_IMETHODIMP
nsPaymentDetails::GetId(nsAString& aId)
{
  aId = mId;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentDetails::GetTotalItem(nsIPaymentItem** aTotalItem)
{
  NS_ENSURE_ARG_POINTER(aTotalItem);
  nsString label;
  nsCOMPtr<nsIPaymentCurrencyAmount> amount;
  bool pending;
  mTotalItem->GetLabel(label);
  mTotalItem->GetAmount(getter_AddRefs(amount));
  mTotalItem->GetPending(&pending);
  RefPtr<nsPaymentItem> totalItem = new nsPaymentItem(label, amount, pending);
  totalItem.forget(aTotalItem);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentDetails::GetDisplayItems(nsIArray** aDisplayItems)
{
  NS_ENSURE_ARG_POINTER(aDisplayItems);
  if (!mDisplayItems) {
    return NS_OK;
  }
  nsCOMPtr<nsIMutableArray> items = do_CreateInstance(NS_ARRAY_CONTRACTID);
  uint32_t length;
  mDisplayItems->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsIPaymentItem> item = do_QueryElementAt(mDisplayItems, index);
    items->AppendElement(item, false);
  }
  items.forget(aDisplayItems);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentDetails::GetShippingOptions(nsIArray** aShippingOptions)
{
  NS_ENSURE_ARG_POINTER(aShippingOptions);
  if (!mShippingOptions) {
    return NS_OK;
  }
  nsCOMPtr<nsIMutableArray> options = do_CreateInstance(NS_ARRAY_CONTRACTID);
  uint32_t length;
  mShippingOptions->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsIPaymentShippingOption> option = do_QueryElementAt(mShippingOptions, index);
    options->AppendElement(option, false);
  }
  options.forget(aShippingOptions);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentDetails::GetModifiers(nsIArray** aModifiers)
{
  NS_ENSURE_ARG_POINTER(aModifiers);
  if (!mModifiers) {
    return NS_OK;
  }
  nsCOMPtr<nsIMutableArray> modifiers = do_CreateInstance(NS_ARRAY_CONTRACTID);
  uint32_t length;
  mModifiers->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsIPaymentDetailsModifier> modifier = do_QueryElementAt(mModifiers, index);
    modifiers->AppendElement(modifier, false);
  }
  modifiers.forget(aModifiers);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentDetails::GetError(nsAString& aError)
{
  aError = mError;
  return NS_OK;
}

/* nsPaymentOptions */

NS_IMPL_ISUPPORTS(nsPaymentOptions,
                  nsIPaymentOptions)

nsPaymentOptions::nsPaymentOptions(const bool aRequestPayerName,
                                   const bool aRequestPayerEmail,
                                   const bool aRequestPayerPhone,
                                   const bool aRequestShipping,
                                   const nsAString& aShippingType)
  : mRequestPayerName(aRequestPayerName)
  , mRequestPayerEmail(aRequestPayerEmail)
  , mRequestPayerPhone(aRequestPayerPhone)
  , mRequestShipping(aRequestShipping)
  , mShippingType(aShippingType)
{
}

nsPaymentOptions::nsPaymentOptions(const IPCPaymentOptions& aOptions)
  : mRequestPayerName(aOptions.requestPayerName())
  , mRequestPayerEmail(aOptions.requestPayerEmail())
  , mRequestPayerPhone(aOptions.requestPayerPhone())
  , mRequestShipping(aOptions.requestShipping())
  , mShippingType(aOptions.shippingType())
{
}

nsPaymentOptions::~nsPaymentOptions()
{
}

NS_IMETHODIMP
nsPaymentOptions::GetRequestPayerName(bool* aRequestPayerName)
{
  NS_ENSURE_ARG_POINTER(aRequestPayerName);
  *aRequestPayerName = mRequestPayerName;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentOptions::GetRequestPayerEmail(bool* aRequestPayerEmail)
{
  NS_ENSURE_ARG_POINTER(aRequestPayerEmail);
  *aRequestPayerEmail = mRequestPayerEmail;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentOptions::GetRequestPayerPhone(bool* aRequestPayerPhone)
{
  NS_ENSURE_ARG_POINTER(aRequestPayerPhone);
  *aRequestPayerPhone = mRequestPayerPhone;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentOptions::GetRequestShipping(bool* aRequestShipping)
{
  NS_ENSURE_ARG_POINTER(aRequestShipping);
  *aRequestShipping = mRequestShipping;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentOptions::GetShippingType(nsAString& aShippingType)
{
  aShippingType = mShippingType;
  return NS_OK;
}

/* nsPaymentReqeust */

NS_IMPL_ISUPPORTS(nsPaymentRequest,
                  nsIPaymentRequest)

nsPaymentRequest::nsPaymentRequest(const uint64_t aTabId,
                                   const nsAString& aRequestId,
                                   nsIArray* aPaymentMethods,
                                   nsIPaymentDetails* aPaymentDetails,
                                   nsIPaymentOptions* aPaymentOptions)
  : mTabId(aTabId)
  , mRequestId(aRequestId)
  , mPaymentMethods(aPaymentMethods)
  , mPaymentDetails(aPaymentDetails)
  , mPaymentOptions(aPaymentOptions)
{
}

nsPaymentRequest::~nsPaymentRequest()
{
}

NS_IMETHODIMP
nsPaymentRequest::GetTabId(uint64_t* aTabId)
{
  NS_ENSURE_ARG_POINTER(aTabId);
  *aTabId = mTabId;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequest::GetRequestId(nsAString& aRequestId)
{
  aRequestId = mRequestId;
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequest::GetPaymentMethods(nsIArray** aPaymentMethods)
{
  NS_ENSURE_ARG_POINTER(aPaymentMethods);
  if (!mPaymentMethods) {
    return NS_OK;
  }
  nsCOMPtr<nsIArray> methods = mPaymentMethods;
  methods.forget(aPaymentMethods);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequest::GetPaymentDetails(nsIPaymentDetails** aPaymentDetails)
{
  NS_ENSURE_ARG_POINTER(aPaymentDetails);
  nsString id;
  nsCOMPtr<nsIPaymentItem> totalItem;
  nsCOMPtr<nsIArray> displayItems;
  nsCOMPtr<nsIArray> shippingOptions;
  nsCOMPtr<nsIArray> modifiers;
  nsString error;
  mPaymentDetails->GetId(id);
  mPaymentDetails->GetTotalItem(getter_AddRefs(totalItem));
  mPaymentDetails->GetDisplayItems(getter_AddRefs(displayItems));
  mPaymentDetails->GetShippingOptions(getter_AddRefs(shippingOptions));
  mPaymentDetails->GetModifiers(getter_AddRefs(modifiers));
  mPaymentDetails->GetError(error);
  nsCOMPtr<nsIPaymentDetails> details =
    new nsPaymentDetails(id, totalItem, displayItems, shippingOptions, modifiers, error);
  details.forget(aPaymentDetails);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequest::UpdatePaymentDetails(nsIPaymentDetails* aPaymentDetails)
{
  nsString id;
  nsCOMPtr<nsIPaymentItem> totalItem;
  nsCOMPtr<nsIArray> displayItems;
  nsCOMPtr<nsIArray> shippingOptions;
  nsCOMPtr<nsIArray> modifiers;
  nsString error;
  /*
   * According to the spec [1], update the attributes if they present in new
   * details (i.e., PaymentDetailsUpdate); otherwise, keep original value.
   * Note |id| comes only from initial details (i.e., PaymentDetailsInit) and
   * |error| only from new details.
   *
   *   [1] https://www.w3.org/TR/payment-request/#updatewith-method
   */
  mPaymentDetails->GetId(id);
  /*
   * No need to check |totalItem| existence here. If merchant doesn't pass
   * totalItem, it would fail while calling updateWith() in webidl part and
   * throw a TypeError to merchant.
   */
  aPaymentDetails->GetTotalItem(getter_AddRefs(totalItem));
  aPaymentDetails->GetDisplayItems(getter_AddRefs(displayItems));
  if (!displayItems) {
    mPaymentDetails->GetDisplayItems(getter_AddRefs(displayItems));
  }
  aPaymentDetails->GetShippingOptions(getter_AddRefs(shippingOptions));
  if (!shippingOptions) {
    mPaymentDetails->GetShippingOptions(getter_AddRefs(shippingOptions));
  }
  aPaymentDetails->GetModifiers(getter_AddRefs(modifiers));
  if (!modifiers) {
    mPaymentDetails->GetModifiers(getter_AddRefs(modifiers));
  }
  aPaymentDetails->GetError(error);

  mPaymentDetails = new nsPaymentDetails(id, totalItem, displayItems,
                                         shippingOptions, modifiers, error);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequest::GetPaymentOptions(nsIPaymentOptions** aPaymentOptions)
{
  NS_ENSURE_ARG_POINTER(aPaymentOptions);
  bool requestPayerName;
  bool requestPayerEmail;
  bool requestPayerPhone;
  bool requestShipping;
  nsString shippingType;
  mPaymentOptions->GetRequestPayerName(&requestPayerName);
  mPaymentOptions->GetRequestPayerEmail(&requestPayerEmail);
  mPaymentOptions->GetRequestPayerPhone(&requestPayerPhone);
  mPaymentOptions->GetRequestShipping(&requestShipping);
  mPaymentOptions->GetShippingType(shippingType);
  nsCOMPtr<nsIPaymentOptions> options =
    new nsPaymentOptions(requestPayerName,
                         requestPayerEmail,
                         requestPayerPhone,
                         requestShipping,
                         shippingType);
  options.forget(aPaymentOptions);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequest::GetCallback(nsIPaymentRequestCallback** aCallback)
{
  NS_ENSURE_ARG_POINTER(aCallback);
  nsCOMPtr<nsIPaymentRequestCallback> callback = mCallback;
  callback.forget(aCallback);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequest::SetCallback(nsIPaymentRequestCallback* aCallback)
{
  mCallback = aCallback;
  return NS_OK;
}
} // end of namespace dom
} // end of namespace mozilla
