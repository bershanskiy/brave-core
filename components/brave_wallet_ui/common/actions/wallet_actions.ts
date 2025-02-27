/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import { WalletActions } from '../slices/wallet.slice'

// We must re-export actions here until we remove all imports of this file
export const {
  activeOriginChanged,
  addUserAsset,
  addUserAssetError,
  autoLockMinutesChanged,
  backedUp,
  defaultBaseCryptocurrencyChanged,
  defaultBaseCurrencyChanged,
  getAllNetworks,
  getAllTokensList,
  initialize,
  initialized,
  walletCreated,
  walletReset,
  walletRestored,
  locked,
  refreshAll,
  refreshBalancesAndPriceHistory,
  refreshNetworksAndTokens,
  removeUserAsset,
  setAllTokensList,
  setAssetAutoDiscoveryCompleted,
  setPasswordAttempts,
  setSelectedAccountFilterItem,
  setSelectedAssetFilterItem,
  setSelectedGroupAssetsByItem,
  setSelectedNetworkFilter,
  setUserAssetVisible,
  setVisibleTokensInfo,
  unlocked,
  updateUserAsset,
  setHidePortfolioGraph,
  setHidePortfolioBalances,
  setRemovedFungibleTokenIds,
  setRemovedNonFungibleTokenIds,
  setDeletedNonFungibleTokenIds,
  setDeletedNonFungibleTokens,
  setHidePortfolioNFTsTab,
  setRemovedNonFungibleTokens,
  setFilteredOutPortfolioNetworkKeys,
  setFilteredOutPortfolioAccountIds,
  setHidePortfolioSmallBalances,
  setShowNetworkLogoOnNfts,
  setIsRefreshingNetworksAndTokens,
  setAllowedNewWalletAccountTypeNetworkIds
} = WalletActions
