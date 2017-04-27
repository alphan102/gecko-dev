/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_nsPaymentRequest_h
#define mozilla_dom_nsPaymentRequest_h

#include "nsIPaymentRequest.h"
#include "mozilla/dom/PPaymentRequest.h"
#include "nsCOMPtr.h"
#include "nsCOMArray.h"
#include "nsString.h"

namespace mozilla {
namespace dom {

class nsPaymentMethodData final : public nsIPaymentMethodData
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTMETHODDATA

  nsPaymentMethodData(nsIArray* aSupportedMethods,
                      const nsAString& aData);
  nsPaymentMethodData(const IPCPaymentMethodData& aMethodData);

protected:
  ~nsPaymentMethodData();

  nsCOMPtr<nsIArray> mSupportedMethods;
  nsString mData;
};

class nsPaymentCurrencyAmount final : public nsIPaymentCurrencyAmount
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTCURRENCYAMOUNT

  nsPaymentCurrencyAmount(const nsAString& aCurrency,
                          const nsAString& aValue);
  nsPaymentCurrencyAmount(const IPCPaymentCurrencyAmount& aCurrencyAmount);

protected:
  ~nsPaymentCurrencyAmount();

  nsString mCurrency;
  nsString mValue;
};

class nsPaymentItem final : public nsIPaymentItem
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTITEM

  nsPaymentItem(const nsAString& aLabel,
                nsIPaymentCurrencyAmount* aAmount,
                const bool aPending);
  nsPaymentItem(const IPCPaymentItem& aItem);

protected:
  ~nsPaymentItem();

  nsString mLabel;
  nsCOMPtr<nsIPaymentCurrencyAmount> mAmount;
  bool mPending;
};

class nsPaymentDetailsModifier final : public nsIPaymentDetailsModifier
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTDETAILSMODIFIER

  nsPaymentDetailsModifier(nsIArray* aSupportedMethods,
                           nsIPaymentItem* aTotal,
                           nsIArray* aAdditionalDisplayItems,
                           const nsAString& aData);
  nsPaymentDetailsModifier(const IPCPaymentDetailsModifier& modifier);

protected:
  ~nsPaymentDetailsModifier();

  nsCOMPtr<nsIArray> mSupportedMethods;
  nsCOMPtr<nsIPaymentItem> mTotal;
  nsCOMPtr<nsIArray> mAdditionalDisplayItems;
  nsString mData;
};

class nsPaymentShippingOption final : public nsIPaymentShippingOption
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTSHIPPINGOPTION

  nsPaymentShippingOption(const nsAString& aId,
                          const nsAString& aLabel,
                          nsIPaymentCurrencyAmount* aAmount,
                          const bool aSelected=false);
  nsPaymentShippingOption(const IPCPaymentShippingOption& aShippingOption);

protected:
  ~nsPaymentShippingOption();

  nsString mId;
  nsString mLabel;
  nsCOMPtr<nsIPaymentCurrencyAmount> mAmount;
  bool mSelected;
};

class nsPaymentDetails final : public nsIPaymentDetails
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTDETAILS

  nsPaymentDetails(const nsAString& aId,
                   nsIPaymentItem* aTotalItem,
                   nsIArray* aDisplayItems,
                   nsIArray* aShippingOptions,
                   nsIArray* aModifiers,
                   const nsAString& aError);
  nsPaymentDetails(const IPCPaymentDetails& aDetails);

protected:
  ~nsPaymentDetails();

  nsString mId;
  nsCOMPtr<nsIPaymentItem> mTotalItem;
  nsCOMPtr<nsIArray> mDisplayItems;
  nsCOMPtr<nsIArray> mShippingOptions;
  nsCOMPtr<nsIArray> mModifiers;
  nsString mError;
};

class nsPaymentOptions final : public nsIPaymentOptions
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTOPTIONS

  nsPaymentOptions(const bool aRequestPayerName,
                   const bool aRequestPayerEmail,
                   const bool aRequestPayerPhone,
                   const bool aRequestShipping,
                   const nsAString& aShippintType);
  nsPaymentOptions(const IPCPaymentOptions& aOptions);

protected:
  ~nsPaymentOptions();

  bool mRequestPayerName;
  bool mRequestPayerEmail;
  bool mRequestPayerPhone;
  bool mRequestShipping;
  nsString mShippingType;
};

class nsPaymentRequest final : public nsIPaymentRequest
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTREQUEST

  nsPaymentRequest(const uint64_t aTabId,
                   const nsAString& aRequestId,
                   nsIArray* aPaymentMethods,
                   nsIPaymentDetails* aPaymentDetails,
                   nsIPaymentOptions* aPaymentOptions);

protected:
  ~nsPaymentRequest();

  uint64_t mTabId;
  nsString mRequestId;
  nsCOMPtr<nsIArray> mPaymentMethods;
  nsCOMPtr<nsIPaymentDetails> mPaymentDetails;
  nsCOMPtr<nsIPaymentOptions> mPaymentOptions;

  nsCOMPtr<nsIPaymentRequestCallback> mCallback;
};

} // end of namespace dom
} // end of namespace mozilla
#endif
