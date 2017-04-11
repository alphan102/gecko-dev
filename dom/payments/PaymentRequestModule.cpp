/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/ModuleUtils.h"
#include "nsPaymentRequestRequest.h"
#include "nsPaymentRequestResponse.h"
#include "nsPaymentRequestService.h"

using mozilla::dom::nsPaymentRequestRequest;
using mozilla::dom::nsPaymentRequestCreateRequest;
using mozilla::dom::nsPaymentRequestUpdateRequest;
using mozilla::dom::nsPaymentRequestCompleteRequest;
using mozilla::dom::nsPaymentRequestCanMakeResponse;
using mozilla::dom::nsPaymentRequestAbortResponse;
using mozilla::dom::nsPaymentRequestShowResponse;
using mozilla::dom::nsPaymentRequestCompleteResponse;
using mozilla::dom::nsPaymentRequestService;

NS_GENERIC_FACTORY_CONSTRUCTOR(nsPaymentRequestRequest)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsPaymentRequestCreateRequest)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsPaymentRequestUpdateRequest)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsPaymentRequestCompleteRequest)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsPaymentRequestCanMakeResponse)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsPaymentRequestAbortResponse)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsPaymentRequestShowResponse)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsPaymentRequestCompleteResponse)
NS_GENERIC_FACTORY_SINGLETON_CONSTRUCTOR(nsPaymentRequestService,
                                         nsPaymentRequestService::GetSingleton)

NS_DEFINE_NAMED_CID(NS_PAYMENT_REQUEST_REQUEST_CID);
NS_DEFINE_NAMED_CID(NS_PAYMENT_REQUEST_CREATE_REQUEST_CID);
NS_DEFINE_NAMED_CID(NS_PAYMENT_REQUEST_UPDATE_REQUEST_CID);
NS_DEFINE_NAMED_CID(NS_PAYMENT_REQUEST_COMPLETE_REQUEST_CID);
NS_DEFINE_NAMED_CID(NS_PAYMENT_REQUEST_CANMAKE_RESPONSE_CID);
NS_DEFINE_NAMED_CID(NS_PAYMENT_REQUEST_ABORT_RESPONSE_CID);
NS_DEFINE_NAMED_CID(NS_PAYMENT_REQUEST_SHOW_RESPONSE_CID);
NS_DEFINE_NAMED_CID(NS_PAYMENT_REQUEST_COMPLETE_RESPONSE_CID);
NS_DEFINE_NAMED_CID(NS_PAYMENT_REQUEST_SERVICE_CID);

static const mozilla::Module::CIDEntry kPaymentRequestCIDs[] = {
  { &kNS_PAYMENT_REQUEST_REQUEST_CID, false, nullptr, nsPaymentRequestRequestConstructor},
  { &kNS_PAYMENT_REQUEST_CREATE_REQUEST_CID, false, nullptr, nsPaymentRequestCreateRequestConstructor},
  { &kNS_PAYMENT_REQUEST_UPDATE_REQUEST_CID, false, nullptr, nsPaymentRequestUpdateRequestConstructor},
  { &kNS_PAYMENT_REQUEST_COMPLETE_REQUEST_CID, false, nullptr, nsPaymentRequestCompleteRequestConstructor},
  { &kNS_PAYMENT_REQUEST_CANMAKE_RESPONSE_CID, false, nullptr, nsPaymentRequestCanMakeResponseConstructor},
  { &kNS_PAYMENT_REQUEST_ABORT_RESPONSE_CID, false, nullptr, nsPaymentRequestAbortResponseConstructor},
  { &kNS_PAYMENT_REQUEST_SHOW_RESPONSE_CID, false, nullptr, nsPaymentRequestShowResponseConstructor},
  { &kNS_PAYMENT_REQUEST_COMPLETE_RESPONSE_CID, false, nullptr, nsPaymentRequestCompleteResponseConstructor},
  { &kNS_PAYMENT_REQUEST_SERVICE_CID, true, nullptr, nsPaymentRequestServiceConstructor },
  { nullptr }
};

static const mozilla::Module::ContractIDEntry kPaymentRequestContracts[] = {
  { NS_PAYMENT_REQUEST_REQUEST_CONTRACT_ID, &kNS_PAYMENT_REQUEST_REQUEST_CID },
  { NS_PAYMENT_REQUEST_CREATE_REQUEST_CONTRACT_ID, &kNS_PAYMENT_REQUEST_CREATE_REQUEST_CID },
  { NS_PAYMENT_REQUEST_UPDATE_REQUEST_CONTRACT_ID, &kNS_PAYMENT_REQUEST_UPDATE_REQUEST_CID },
  { NS_PAYMENT_REQUEST_COMPLETE_REQUEST_CONTRACT_ID, &kNS_PAYMENT_REQUEST_COMPLETE_REQUEST_CID },
  { NS_PAYMENT_REQUEST_CANMAKE_RESPONSE_CONTRACT_ID, &kNS_PAYMENT_REQUEST_CANMAKE_RESPONSE_CID },
  { NS_PAYMENT_REQUEST_ABORT_RESPONSE_CONTRACT_ID, &kNS_PAYMENT_REQUEST_ABORT_RESPONSE_CID },
  { NS_PAYMENT_REQUEST_SHOW_RESPONSE_CONTRACT_ID, &kNS_PAYMENT_REQUEST_SHOW_RESPONSE_CID },
  { NS_PAYMENT_REQUEST_COMPLETE_RESPONSE_CONTRACT_ID, &kNS_PAYMENT_REQUEST_COMPLETE_RESPONSE_CID },
  { NS_PAYMENT_REQUEST_SERVICE_CONTRACT_ID, &kNS_PAYMENT_REQUEST_SERVICE_CID },
  { nullptr }
};

static const mozilla::Module::CategoryEntry kPaymentRequestCategories[] = {
  { "payment-request", "nsPaymentRequestRequest", NS_PAYMENT_REQUEST_REQUEST_CONTRACT_ID },
  { "payment-request", "nsPaymentRequestCreateRequest", NS_PAYMENT_REQUEST_CREATE_REQUEST_CONTRACT_ID },
  { "payment-request", "nsPaymentRequestUpdateRequest", NS_PAYMENT_REQUEST_UPDATE_REQUEST_CONTRACT_ID },
  { "payment-request", "nsPaymentRequestCompleteRequest", NS_PAYMENT_REQUEST_COMPLETE_REQUEST_CONTRACT_ID },
  { "payment-request", "nsPaymentRequestCanMakeResponse", NS_PAYMENT_REQUEST_CANMAKE_RESPONSE_CONTRACT_ID },
  { "payment-request", "nsPaymentRequestAbortResponse", NS_PAYMENT_REQUEST_ABORT_RESPONSE_CONTRACT_ID },
  { "payment-request", "nsPaymentRequestShowResponse", NS_PAYMENT_REQUEST_SHOW_RESPONSE_CONTRACT_ID },
  { "payment-request", "nsPaymentRequestCompleteResponse", NS_PAYMENT_REQUEST_COMPLETE_RESPONSE_CONTRACT_ID },
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
