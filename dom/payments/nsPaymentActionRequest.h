/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_nsPaymentActionRequest_h
#define mozilla_dom_nsPaymentActionRequest_h

#include "nsIPaymentActionRequest.h"
#include "nsCOMPtr.h"
#include "nsCOMArray.h"
#include "nsIArray.h"

namespace mozilla {
namespace dom {

class nsPaymentActionRequest : public nsIPaymentActionRequest
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTACTIONREQUEST

  nsPaymentActionRequest() {};

protected:
  virtual ~nsPaymentActionRequest() {};

  nsString mRequestId;
  uint32_t mType;
};

class nsPaymentCreateActionRequest final : public nsIPaymentCreateActionRequest
                                         , public nsPaymentActionRequest
{
public:
  NS_DECL_ISUPPORTS_INHERITED
  NS_FORWARD_NSIPAYMENTACTIONREQUEST(nsPaymentActionRequest::)
  NS_DECL_NSIPAYMENTCREATEACTIONREQUEST

  nsPaymentCreateActionRequest() = default;

private:
  ~nsPaymentCreateActionRequest() {};

  uint64_t mTabId;
  nsCOMPtr<nsIArray> mMethodData;
  nsCOMPtr<nsIPaymentDetails> mDetails;
  nsCOMPtr<nsIPaymentOptions> mOptions;
};

} // end of namespace dom
} // end of namespace mozilla
#endif
