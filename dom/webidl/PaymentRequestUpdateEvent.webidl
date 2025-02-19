/* -*- Mode: IDL; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this WebIDL file is
 *   https://www.w3.org/TR/payment-request/#paymentrequestupdateevent-interface
 */

dictionary PaymentDetailsUpdate : PaymentDetailsBase {
  DOMString   error;
  PaymentItem total;
};

[Constructor(DOMString type,
             optional PaymentRequestUpdateEventInit eventInitDict),
 SecureContext]
interface PaymentRequestUpdateEvent : Event {
  void updateWith(Promise<PaymentDetailsUpdate> d);
};

dictionary PaymentRequestUpdateEventInit : EventInit {
};
