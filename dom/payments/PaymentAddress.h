/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_PaymentAddress_h
#define mozilla_dom_PaymentAddress_h

#include "nsPIDOMWindow.h"
#include "nsWrapperCache.h"

namespace mozilla {
namespace dom {

class PaymentAddress final : public nsISupports,
                             public nsWrapperCache
{
public:
  NS_DECL_CYCLE_COLLECTING_ISUPPORTS
  NS_DECL_CYCLE_COLLECTION_SCRIPT_HOLDER_CLASS(PaymentAddress)

  PaymentAddress();

  nsPIDOMWindowInner* GetParentObject() const
  {
    return mOwner;
  }

  virtual JSObject* WrapObject(JSContext* aCx,
                               JS::Handle<JSObject*> aGivenProto) override;

  // Getter functions
  void GetCountry(nsAString& aRetVal) const { }

  void GetAddressLine(nsTArray<nsString>& aRetVal) const { }

  void GetRegion(nsAString& aRetVal) const { }

  void GetCity(nsAString& aRetVal) const { }

  void GetDependentLocality(nsAString& aRetVal) const { }

  void GetPostalCode(nsAString& aRetVal) const { }

  void GetSortingCode(nsAString& aRetVal) const { }

  void GetLanguageCode(nsAString& aRetVal) const { }

  void GetOrganization(nsAString& aRetVal) const { }

  void GetRecipient(nsAString& aRetVal) const { }

  void GetPhone(nsAString& aRetVal) const { }

private:
  ~PaymentAddress();

  nsCOMPtr<nsPIDOMWindowInner> mOwner;
};

} // namespace dom
} // namespace mozilla

#endif // mozilla_dom_PaymentAddress_h
