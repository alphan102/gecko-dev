"use strict";

// kTestRoot is from head.js
const kTestPage = kTestRoot + "payment_request.html";

add_task(function*() {
  yield BrowserTestUtils.withNewTab(kTestPage,
    function*(browser) {

      const paymentSrv = Cc["@mozilla.org/dom/payments/payment-request-service;1"].getService(Ci.nsIPaymentRequestService);
      ok(paymentSrv, "Fail to get PaymentRequestService.");

      const paymentEnum = paymentSrv.enumerate();
      ok(paymentEnum.hasMoreElements(), "PaymentRequestService should have at least one payment request.");
      while (paymentEnum.hasMoreElements()) {
        let payment = paymentEnum.getNext().QueryInterface(Ci.nsIPaymentRequest);
        ok(payment, "Fail to get existing payment request.");

        // checking the passed PaymentMethods parameter
        is(payment.paymentMethods.length, 1, "paymentMethods' length should be 1.");

        const methodData = payment.paymentMethods.queryElementAt(0, Ci.nsIPaymentMethodData);
        ok(methodData, "Fail to get payment methodData.");
        is(methodData.supportedMethods.length, 2, "supportedMethods' length should be 2.");
        let supportedMethod = methodData.supportedMethods.queryElementAt(0, Ci.nsISupportsString);
        is(supportedMethod, "MyPay", "1st supported method should be 'MyPay'.");
        supportedMethod = methodData.supportedMethods.queryElementAt(1, Ci.nsISupportsString);
        is(supportedMethod, "TestPay", "2nd supported method should be 'TestPay'.");
        is(methodData.data, "", "method data should be empty");

        // checking the passed PaymentDetails parameter
        const details = payment.paymentDetails;
        is(details.id, "test details", "details.id should be 'test details'.");
        is(details.totalItem.label, "Donation", "total item's label should be 'Donation'.");
        is(details.totalItem.amount.currency, "USD", "total item's currency should be 'USD'.");
        is(details.totalItem.amount.value, "55.00", "total item's value should be '55.00'.");

        const displayItems = details.displayItems;
        is(displayItems.length, 2, "displayItems' length should be 2.");
        let item = displayItems.queryElementAt(0, Ci.nsIPaymentItem);
        is(item.label, "Original donation amount", "1st display item's label should be 'Original donation amount'.");
        is(item.amount.currency, "USD", "1st display item's currency should be 'USD'.");
        is(item.amount.value, "-65.00", "1st display item's value should be '-65.00'.");
        item = displayItems.queryElementAt(1, Ci.nsIPaymentItem);
        is(item.label, "Friends and family discount", "2nd display item's label should be 'Friends and family discount'.");
        is(item.amount.currency, "USD", "2nd display item's currency should be 'USD'.");
        is(item.amount.value, "10.00", "2nd display item's value should be '10.00'.");

        const modifiers = details.modifiers;
        is(modifiers.length, 1, "modifiers' length should be 1.");

        const modifier = modifiers.queryElementAt(0, Ci.nsIPaymentDetailsModifier);
        const modifierSupportedMethods = modifier.supportedMethods;
        is(modifierSupportedMethods.length, 1, "modifier's supported methods length should be 1.");
        supportedMethod = modifierSupportedMethods.queryElementAt(0, Ci.nsISupportsString);
        is(supportedMethod, "MyPay", "modifier's supported method name should be 'MyPay'.");
        is(modifier.total.label, "Discounted donation", "modifier's total label should be 'Discounted donation'.");
        is(modifier.total.amount.currency, "USD", "modifier's total currency should be 'USD'.");
        is(modifier.total.amount.value, "45.00", "modifier's total value should be '45.00'.");

        const additionalItems = modifier.additionalDisplayItems;
        is(additionalItems.length, "1", "additionalDisplayItems' length should be 1.");
        const additionalItem = additionalItems.queryElementAt(0, Ci.nsIPaymentItem);
        is(additionalItem.label, "MyPay discount", "additional item's label should be 'MyPay discount'.");
        is(additionalItem.amount.currency, "USD", "additional item's currency should be 'USD'.");
        is(additionalItem.amount.value, "-10.00", "additional item's value should be '-10.00'.");
        is(modifier.data, "{\"discountProgramParticipantId\":\"86328764873265\"}",
           "modifier's data should be '{\"discountProgramParticipantId\":\"86328764873265\"}'.");

        // checking the passed PaymentOptions parameter
        const paymentOptions = payment.paymentOptions;
        ok(!paymentOptions.payerName, "payerName option should be false");
        ok(!paymentOptions.payerEmail, "payerEmail option should be false");
        ok(!paymentOptions.payerPhone, "payerPhone option should be false");
        ok(!paymentOptions.requestShipping, "requestShipping option should be false");
        is(paymentOptions.shippingType, "shipping", "shippingType option should be 'shipping'");
      }
    }
  );
});
