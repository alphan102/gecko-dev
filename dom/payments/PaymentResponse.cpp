/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/PaymentResponse.h"

namespace mozilla {
namespace dom {

// [TODO] Revisit here once |mShippingAddress| is declared
NS_IMPL_CYCLE_COLLECTION_WRAPPERCACHE(PaymentResponse, mOwner)

NS_IMPL_CYCLE_COLLECTING_ADDREF(PaymentResponse)
NS_IMPL_CYCLE_COLLECTING_RELEASE(PaymentResponse)

NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION(PaymentResponse)
  NS_WRAPPERCACHE_INTERFACE_MAP_ENTRY
  NS_INTERFACE_MAP_ENTRY(nsISupports)
NS_INTERFACE_MAP_END

PaymentResponse::PaymentResponse()
{
  // Add |MOZ_COUNT_CTOR(PaymentResponse);| for a non-refcounted object.

  // [TODO] Assign |mOwner| for method |GetParentObject| to return
}

PaymentResponse::~PaymentResponse()
{
  // Add |MOZ_COUNT_DTOR(PaymentResponse);| for a non-refcounted object.
}

JSObject*
PaymentResponse::WrapObject(JSContext* aCx, JS::Handle<JSObject*> aGivenProto)
{
  return PaymentResponseBinding::Wrap(aCx, this, aGivenProto);
}


} // namespace dom
} // namespace mozilla
