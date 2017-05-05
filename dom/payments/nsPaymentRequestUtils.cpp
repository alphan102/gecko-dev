/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsArrayUtils.h"
#include "nsPaymentRequestUtils.h"
#include "nsIMutableArray.h"
#include "nsISupportsPrimitives.h"

namespace mozilla {
namespace dom {

nsresult
ConvertStringstoISupportsStrings(const nsTArray<nsString>& aStrings,
                                 nsIArray** aIStrings)
{
  NS_ENSURE_ARG_POINTER(aIStrings);
  nsCOMPtr<nsIMutableArray> iStrings = do_CreateInstance(NS_ARRAY_CONTRACTID);
  for (const nsString& string : aStrings) {
    nsCOMPtr<nsISupportsString> iString =
      do_CreateInstance(NS_SUPPORTS_STRING_CONTRACTID);
    iString->SetData(string);
    iStrings->AppendElement(iString, false);
  }
  iStrings.forget(aIStrings);
  return NS_OK;
}

nsresult
ConvertISupportsStringstoStrings(nsIArray* aIStrings,
                                 nsTArray<nsString>& aStrings)
{
  uint32_t length;
  aStrings.Clear();
  aIStrings->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsISupportsString> iString = do_QueryElementAt(aIStrings, index);
    MOZ_ASSERT(iString);
    nsString string;
    iString->GetData(string);
    aStrings.AppendElement(string);
  }
  return NS_OK;
}

nsresult
CopyISupportsStrings(nsIArray* aSourceStrings, nsIArray** aTargetStrings)
{
  NS_ENSURE_ARG_POINTER(aTargetStrings);
  nsCOMPtr<nsIMutableArray> strings = do_CreateInstance(NS_ARRAY_CONTRACTID);
  uint32_t length;
  aSourceStrings->GetLength(&length);
  for (uint32_t index = 0; index < length; ++index) {
    nsCOMPtr<nsISupportsString> string = do_QueryElementAt(aSourceStrings, index);
    MOZ_ASSERT(string);
    strings->AppendElement(string, false);
  }
  strings.forget(aTargetStrings);
  return NS_OK;
}

} // end of namespace dom
} // end of namespace mozilla
