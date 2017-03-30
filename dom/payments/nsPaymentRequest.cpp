/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsPaymentRequest.h"
#include "nsIMutableArray.h"
#include "nsISupportsPrimitives.h"
#include "nsArrayUtils.h"

namespace mozilla {
namespace dom{

/* nsPaymentMethodData */

NS_IMPL_ISUPPORTS(nsPaymentMethodData,
                  nsIPaymentMethodData)

nsPaymentMethodData::nsPaymentMethodData(nsIArray* aSupportedMethods,
                                         const nsAString& aData)
  : mData(aData)
{
  uint32_t length;
  aSupportedMethods->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsString method;
    nsCOMPtr<nsISupportsString> iMethod =
      do_QueryElementAt(aSupportedMethods, index);
    iMethod->GetData(method);
    mSupportedMethods.AppendElement(method);
  }
}

nsPaymentMethodData::nsPaymentMethodData(const IPCPaymentMethodData& aMethodData)
  : mSupportedMethods(aMethodData.supportedMethods())
  , mData(aMethodData.data())
{
}

nsPaymentMethodData::~nsPaymentMethodData()
{
}

NS_IMETHODIMP
nsPaymentMethodData::GetSupportedMethods(nsIArray** aSupportedMethods)
{
  NS_ENSURE_ARG_POINTER(aSupportedMethods);
  nsCOMPtr<nsIMutableArray> methods = do_CreateInstance(NS_ARRAY_CONTRACTID);
  for (const nsString& supportedMethod : mSupportedMethods) {
    nsCOMPtr<nsISupportsString> method =
      do_CreateInstance(NS_SUPPORTS_STRING_CONTRACTID);
    method->SetData(supportedMethod);
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
  : mTotal(aTotal)
  , mData(aData)
{
  uint32_t length;
  aSupportedMethods->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsString modifier;
    nsCOMPtr<nsISupportsString> iModifier =
      do_QueryElementAt(aSupportedMethods, index);
    iModifier->GetData(modifier);
    mSupportedMethods.AppendElement(modifier);
  }

  aAdditionalDisplayItems->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsIPaymentItem> item =
      do_QueryElementAt(aAdditionalDisplayItems, index);
    mAdditionalDisplayItems.AppendElement(item);
  }
}

nsPaymentDetailsModifier::nsPaymentDetailsModifier(const IPCPaymentDetailsModifier& aModifier)
  : mSupportedMethods(aModifier.supportedMethods())
  , mData(aModifier.data())
{
  mTotal = new nsPaymentItem(aModifier.total());

  for (const IPCPaymentItem& item : aModifier.additionalDisplayItems()) {
    nsCOMPtr<nsIPaymentItem> iitem = new nsPaymentItem(item);
    mAdditionalDisplayItems.AppendElement(iitem);
  }
}

nsPaymentDetailsModifier::~nsPaymentDetailsModifier()
{
}

NS_IMETHODIMP
nsPaymentDetailsModifier::GetSupportedMethods(nsIArray** aSupportedMethods)
{
  NS_ENSURE_ARG_POINTER(aSupportedMethods);
  nsCOMPtr<nsIMutableArray> modifiers = do_CreateInstance(NS_ARRAY_CONTRACTID);
  for (const nsString& method : mSupportedMethods) {
    nsCOMPtr<nsISupportsString> modifier =
      do_CreateInstance(NS_SUPPORTS_STRING_CONTRACTID);
    modifier->SetData(method);
    modifiers->AppendElement(modifier, false);
  }
  modifiers.forget(aSupportedMethods);
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
  nsCOMPtr<nsIMutableArray> items = do_CreateInstance(NS_ARRAY_CONTRACTID);
  for (uint32_t index = 0; index < mAdditionalDisplayItems.Length(); ++index) {
    nsCOMPtr<nsIPaymentItem> item = mAdditionalDisplayItems[index];
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
  , mError(aError)
{
  uint32_t length;
  aDisplayItems->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsIPaymentItem> item =
      do_QueryElementAt(aDisplayItems, index);
    mDisplayItems.AppendElement(item);
  }

  aShippingOptions->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsIPaymentShippingOption> option =
      do_QueryElementAt(aShippingOptions, index);
    mShippingOptions.AppendElement(option);
  }

  aModifiers->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsIPaymentDetailsModifier> modifier =
      do_QueryElementAt(aModifiers, index);
    mModifiers.AppendElement(modifier);
  }
}

nsPaymentDetails::nsPaymentDetails(const IPCPaymentDetails& aDetails)
  : mId(aDetails.id())
  , mError(aDetails.error())
{
  mTotalItem = new nsPaymentItem(aDetails.total());
  for (const IPCPaymentItem& displayItem : aDetails.displayItems()) {
    nsCOMPtr<nsIPaymentItem> item = new nsPaymentItem(displayItem);
    mDisplayItems.AppendElement(item);
  }

  for (const IPCPaymentShippingOption& shippingOption : aDetails.shippingOptions()) {
    nsCOMPtr<nsIPaymentShippingOption> option =
      new nsPaymentShippingOption(shippingOption);
    mShippingOptions.AppendElement(option);
  }

  for (const IPCPaymentDetailsModifier& modifier : aDetails.modifiers()) {
    nsCOMPtr<nsIPaymentDetailsModifier> imodifier = new nsPaymentDetailsModifier(modifier);
    mModifiers.AppendElement(imodifier);
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
  nsCOMPtr<nsIMutableArray> items = do_CreateInstance(NS_ARRAY_CONTRACTID);
  for (uint32_t index = 0; index < mDisplayItems.Length(); ++index) {
    nsString label;
    nsCOMPtr<nsIPaymentCurrencyAmount> amount;
    bool pending;
    mDisplayItems[index]->GetLabel(label);
    mDisplayItems[index]->GetAmount(getter_AddRefs(amount));
    mDisplayItems[index]->GetPending(&pending);
    nsCOMPtr<nsIPaymentItem> item = new nsPaymentItem(label, amount, pending);
    items->AppendElement(item, false);
  }
  items.forget(aDisplayItems);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentDetails::GetShippingOptions(nsIArray** aShippingOptions)
{
  NS_ENSURE_ARG_POINTER(aShippingOptions);
  nsCOMPtr<nsIMutableArray> options = do_CreateInstance(NS_ARRAY_CONTRACTID);
  for (uint32_t index = 0; index < mShippingOptions.Length(); ++index) {
    nsString id;
    nsString label;
    nsCOMPtr<nsIPaymentCurrencyAmount> amount;
    bool selected;
    mShippingOptions[index]->GetId(id);
    mShippingOptions[index]->GetLabel(label);
    mShippingOptions[index]->GetAmount(getter_AddRefs(amount));
    mShippingOptions[index]->GetSelected(&selected);
    nsCOMPtr<nsIPaymentShippingOption> option =
      new nsPaymentShippingOption(id, label, amount, selected);
    options->AppendElement(option, false);
  }
  options.forget(aShippingOptions);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentDetails::GetModifiers(nsIArray** aModifiers)
{
  NS_ENSURE_ARG_POINTER(aModifiers);
  nsCOMPtr<nsIMutableArray> modifiers = do_CreateInstance(NS_ARRAY_CONTRACTID);
  for (uint32_t index = 0; index < mModifiers.Length(); ++index) {
    nsCOMPtr<nsIArray> supportedModifiers;
    nsCOMPtr<nsIPaymentItem> total;
    nsCOMPtr<nsIArray> additionalDisplayItems;
    nsString data;
    mModifiers[index]->GetSupportedMethods(getter_AddRefs(supportedModifiers));
    mModifiers[index]->GetTotal(getter_AddRefs(total));
    mModifiers[index]->GetAdditionalDisplayItems(getter_AddRefs(additionalDisplayItems));
    mModifiers[index]->GetData(data);
    nsCOMPtr<nsIPaymentDetailsModifier> modifier =
      new nsPaymentDetailsModifier(supportedModifiers, total, additionalDisplayItems, data);
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
  , mPaymentDetails(aPaymentDetails)
  , mPaymentOptions(aPaymentOptions)
{
  uint32_t length;
  aPaymentMethods->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsIPaymentMethodData> methodData =
      do_QueryElementAt(aPaymentMethods, index);
    mPaymentMethods.AppendElement(methodData);
  }
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
  nsCOMPtr<nsIMutableArray> methods =
    do_CreateInstance(NS_ARRAY_CONTRACTID);
  for (uint32_t index = 0; index < mPaymentMethods.Length(); ++index) {
    nsCOMPtr<nsIArray> supportedMethods;
    nsString data;
    mPaymentMethods[index]->GetSupportedMethods(getter_AddRefs(supportedMethods));
    mPaymentMethods[index]->GetData(data);
    nsCOMPtr<nsIPaymentMethodData> method = new nsPaymentMethodData(supportedMethods, data);
    methods->AppendElement(method, false);
  }
  methods.forget(aPaymentMethods);
  return NS_OK;
}

NS_IMETHODIMP
nsPaymentRequest::GetPaymentDetails(nsIPaymentDetails** aPaymentDetails)
{
  NS_ENSURE_ARG_POINTER(aPaymentDetails);
  nsString id;
  nsCOMPtr<nsIPaymentItem> totalItem;
  nsCOMPtr<nsIArray> displayedItems;
  nsCOMPtr<nsIArray> shippingOptions;
  nsCOMPtr<nsIArray> modifiers;
  nsString error;
  mPaymentDetails->GetId(id);
  mPaymentDetails->GetTotalItem(getter_AddRefs(totalItem));
  mPaymentDetails->GetDisplayItems(getter_AddRefs(displayedItems));
  mPaymentDetails->GetShippingOptions(getter_AddRefs(shippingOptions));
  mPaymentDetails->GetModifiers(getter_AddRefs(modifiers));
  mPaymentDetails->GetError(error);
  nsCOMPtr<nsIPaymentDetails> details =
    new nsPaymentDetails(id, totalItem, displayedItems, shippingOptions, modifiers, error);
  details.forget(aPaymentDetails);
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
