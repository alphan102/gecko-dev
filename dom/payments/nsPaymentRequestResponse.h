/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_nsPaymentRequestResponse_h
#define mozilla_dom_nsPaymentRequestResponse_h

#include "nsIPaymentRequestResponse.h"

namespace mozilla {
namespace dom {

class PaymentRequestParent;

class nsPaymentRequestResponse : public nsIPaymentRequestResponse
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTREQUESTRESPONSE

  nsPaymentRequestResponse();

protected:
  virtual ~nsPaymentRequestResponse();

  nsString mRequestId;
  uint32_t mType;
};

class nsPaymentRequestCanMakeResponse final : public nsIPaymentRequestCanMakeResponse
                                            , public nsPaymentRequestResponse
{
public:
  NS_DECL_ISUPPORTS_INHERITED
  NS_FORWARD_NSIPAYMENTREQUESTRESPONSE(nsPaymentRequestResponse::)
  NS_DECL_NSIPAYMENTREQUESTCANMAKERESPONSE

  nsPaymentRequestCanMakeResponse();

protected:
  ~nsPaymentRequestCanMakeResponse();

  bool mResult;
};

class nsPaymentRequestAbortResponse final : public nsIPaymentRequestAbortResponse
                                          , public nsPaymentRequestResponse
{
public:
  NS_DECL_ISUPPORTS_INHERITED
  NS_FORWARD_NSIPAYMENTREQUESTRESPONSE(nsPaymentRequestResponse::)
  NS_DECL_NSIPAYMENTREQUESTABORTRESPONSE

  nsPaymentRequestAbortResponse();

protected:
  ~nsPaymentRequestAbortResponse();

  uint32_t mAbortStatus;
};

class nsPaymentRequestShowResponse final : public nsIPaymentRequestShowResponse
                                         , public nsPaymentRequestResponse
{
public:
  NS_DECL_ISUPPORTS_INHERITED
  NS_FORWARD_NSIPAYMENTREQUESTRESPONSE(nsPaymentRequestResponse::)
  NS_DECL_NSIPAYMENTREQUESTSHOWRESPONSE

  nsPaymentRequestShowResponse();
protected:
  ~nsPaymentRequestShowResponse();

  uint32_t mAcceptStatus;
  nsString mMethodName;
  nsString mData;
  nsString mPayerName;
  nsString mPayerEmail;
  nsString mPayerPhone;
};

class nsPaymentRequestCompleteResponse final : public nsIPaymentRequestCompleteResponse
                                             , public nsPaymentRequestResponse
{
public:
  NS_DECL_ISUPPORTS_INHERITED
  NS_FORWARD_NSIPAYMENTREQUESTRESPONSE(nsPaymentRequestResponse::)
  NS_DECL_NSIPAYMENTREQUESTCOMPLETERESPONSE

  nsPaymentRequestCompleteResponse();

protected:
  ~nsPaymentRequestCompleteResponse();

  uint32_t mCompleteStatus;
};

class nsPaymentRequestCallback final : public nsIPaymentRequestCallback
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTREQUESTCALLBACK

  nsPaymentRequestCallback(PaymentRequestParent* aParent);

private:
  RefPtr<PaymentRequestParent> mParent;

  ~nsPaymentRequestCallback();
};
} // end of dom
} // end of namespace mozilla
#endif
