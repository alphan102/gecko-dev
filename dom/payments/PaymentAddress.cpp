/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/PaymentAddress.h"
#include "mozilla/dom/PaymentAddressBinding.h"

namespace mozilla {
namespace dom {

NS_IMPL_CYCLE_COLLECTION_WRAPPERCACHE(PaymentAddress, mOwner)

NS_IMPL_CYCLE_COLLECTING_ADDREF(PaymentAddress)
NS_IMPL_CYCLE_COLLECTING_RELEASE(PaymentAddress)

NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION(PaymentAddress)
  NS_WRAPPERCACHE_INTERFACE_MAP_ENTRY
  NS_INTERFACE_MAP_ENTRY(nsISupports)
NS_INTERFACE_MAP_END

PaymentAddress::PaymentAddress()
{
  // Add |MOZ_COUNT_CTOR(PaymentAddress);| for a non-refcounted object.

  // [TODO]
  // 1) Set attributes for getter functions
  // 2) Assign |mOwner| for method |GetParentObject| to return
}

PaymentAddress::~PaymentAddress()
{
  // Add |MOZ_COUNT_DTOR(PaymentAddress);| for a non-refcounted object.
}

JSObject*
PaymentAddress::WrapObject(JSContext* aCx, JS::Handle<JSObject*> aGivenProto)
{
  return PaymentAddressBinding::Wrap(aCx, this, aGivenProto);
}


} // namespace dom
} // namespace mozilla
