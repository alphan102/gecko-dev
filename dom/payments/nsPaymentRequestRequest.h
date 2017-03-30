/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_nsPaymentRequestRequest_h
#define mozilla_dom_nsPaymentRequestRequest_h

#include "nsIPaymentRequestRequest.h"
#include "nsCOMPtr.h"
#include "nsCOMArray.h"
#include "nsIArray.h"

namespace mozilla {
namespace dom {

class nsPaymentRequestRequest : public nsIPaymentRequestRequest
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTREQUESTREQUEST

  nsPaymentRequestRequest() {};

protected:
  virtual ~nsPaymentRequestRequest() {};

  nsString mRequestId;
  uint32_t mType;
  nsCOMPtr<nsIPaymentRequestCallback> mCallback;
};

class nsPaymentRequestCreateRequest final : public nsIPaymentRequestCreateRequest
                                          , public nsPaymentRequestRequest
{
public:
  NS_DECL_ISUPPORTS_INHERITED
  NS_FORWARD_NSIPAYMENTREQUESTREQUEST(nsPaymentRequestRequest::)
  NS_DECL_NSIPAYMENTREQUESTCREATEREQUEST

  nsPaymentRequestCreateRequest() = default;

private:
  ~nsPaymentRequestCreateRequest() {};

  uint64_t mTabId;
  nsCOMPtr<nsIArray> mMethodData;
  nsCOMPtr<nsIPaymentDetails> mDetails;
  nsCOMPtr<nsIPaymentOptions> mOptions;
};

class nsPaymentRequestUpdateRequest final : public nsIPaymentRequestUpdateRequest
                                          , public nsPaymentRequestRequest
{
  NS_DECL_ISUPPORTS_INHERITED
  NS_FORWARD_NSIPAYMENTREQUESTREQUEST(nsPaymentRequestRequest::)
  NS_DECL_NSIPAYMENTREQUESTUPDATEREQUEST

  nsPaymentRequestUpdateRequest() = default;

private:
  ~nsPaymentRequestUpdateRequest() {};

  nsCOMPtr<nsIPaymentDetails> mDetails;
};
} // end of namespace dom
} // end of namespace mozilla
#endif
