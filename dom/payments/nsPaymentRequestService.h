/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_nsPaymentRequestService_h
#define mozilla_dom_nsPaymentRequestService_h

#include "nsIPaymentRequest.h"
#include "nsIPaymentRequestService.h"
#include "nsCOMPtr.h"
#include "nsCOMArray.h"

namespace mozilla {
namespace dom {

// The implmentation of nsIPaymentRequestService

class nsPaymentRequestService final : public nsIPaymentRequestService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPAYMENTREQUESTSERVICE

  nsPaymentRequestService();

  static already_AddRefed<nsPaymentRequestService> GetSingleton();

private:
  ~nsPaymentRequestService();

  static StaticRefPtr<nsPaymentRequestService> sSingleton;
  nsCOMArray<nsIPaymentRequest> mRequestQueue;
};

} // end of namespace dom
} // end of namespace mozilla
#endif
