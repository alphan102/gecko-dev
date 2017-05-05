/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/ModuleUtils.h"
#include "nsPaymentActionRequest.h"
#include "nsPaymentRequestService.h"

using mozilla::dom::nsPaymentActionRequest;
using mozilla::dom::nsPaymentCreateActionRequest;
using mozilla::dom::nsPaymentRequestService;

NS_GENERIC_FACTORY_CONSTRUCTOR(nsPaymentActionRequest)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsPaymentCreateActionRequest)
NS_GENERIC_FACTORY_SINGLETON_CONSTRUCTOR(nsPaymentRequestService,
                                         nsPaymentRequestService::GetSingleton)

NS_DEFINE_NAMED_CID(NS_PAYMENT_ACTION_REQUEST_CID);
NS_DEFINE_NAMED_CID(NS_PAYMENT_CREATE_ACTION_REQUEST_CID);
NS_DEFINE_NAMED_CID(NS_PAYMENT_REQUEST_SERVICE_CID);

static const mozilla::Module::CIDEntry kPaymentRequestCIDs[] = {
  { &kNS_PAYMENT_ACTION_REQUEST_CID, false, nullptr, nsPaymentActionRequestConstructor},
  { &kNS_PAYMENT_CREATE_ACTION_REQUEST_CID, false, nullptr, nsPaymentCreateActionRequestConstructor},
  { &kNS_PAYMENT_REQUEST_SERVICE_CID, true, nullptr, nsPaymentRequestServiceConstructor },
  { nullptr }
};

static const mozilla::Module::ContractIDEntry kPaymentRequestContracts[] = {
  { NS_PAYMENT_ACTION_REQUEST_CONTRACT_ID, &kNS_PAYMENT_ACTION_REQUEST_CID },
  { NS_PAYMENT_CREATE_ACTION_REQUEST_CONTRACT_ID, &kNS_PAYMENT_CREATE_ACTION_REQUEST_CID },
  { NS_PAYMENT_REQUEST_SERVICE_CONTRACT_ID, &kNS_PAYMENT_REQUEST_SERVICE_CID },
  { nullptr }
};

static const mozilla::Module::CategoryEntry kPaymentRequestCategories[] = {
  { "payment-request", "nsPaymentActionRequest", NS_PAYMENT_ACTION_REQUEST_CONTRACT_ID },
  { "payment-request", "nsPaymentCreateActionRequest", NS_PAYMENT_CREATE_ACTION_REQUEST_CONTRACT_ID },
  { "payment-request", "nsPaymentRequestService", NS_PAYMENT_REQUEST_SERVICE_CONTRACT_ID },
  { nullptr }
};

static const mozilla::Module kPaymentRequestModule = {
  mozilla::Module::kVersion,
  kPaymentRequestCIDs,
  kPaymentRequestContracts,
  kPaymentRequestCategories
};

NSMODULE_DEFN(PaymentRequestModule) = &kPaymentRequestModule;
