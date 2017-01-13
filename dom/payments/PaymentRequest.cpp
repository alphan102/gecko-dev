/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/PaymentRequest.h"

namespace mozilla {
namespace dom {


// [TODO] Revisit here once |mPaymentAddress| is declared
NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION_INHERITED(PaymentRequest)
NS_INTERFACE_MAP_END_INHERITING(DOMEventTargetHelper)

NS_IMPL_ADDREF_INHERITED(PaymentRequest, DOMEventTargetHelper)
NS_IMPL_RELEASE_INHERITED(PaymentRequest, DOMEventTargetHelper)

PaymentRequest::PaymentRequest()
{
  // Add |MOZ_COUNT_CTOR(PaymentRequest);| for a non-refcounted object.
}

PaymentRequest::~PaymentRequest()
{
  // Add |MOZ_COUNT_DTOR(PaymentRequest);| for a non-refcounted object.
}

JSObject*
PaymentRequest::WrapObject(JSContext* aCx, JS::Handle<JSObject*> aGivenProto)
{
  return PaymentRequestBinding::Wrap(aCx, this, aGivenProto);
}


} // namespace dom
} // namespace mozilla
