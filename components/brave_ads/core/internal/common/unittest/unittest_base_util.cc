/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/internal/common/unittest/unittest_base_util.h"

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "base/check.h"
#include "base/containers/extend.h"
#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/no_destructor.h"
#include "base/strings/strcat.h"
#include "base/time/time.h"
#include "base/values.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_command_line_switch_util.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_current_test_util.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_file_path_util.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_file_util.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_local_state_pref_value_util.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_profile_pref_value_util.h"
#include "brave/components/brave_ads/core/internal/global_state/global_state.h"
#include "brave/components/brave_ads/core/mojom/brave_ads.mojom.h"
#include "brave/components/brave_ads/core/public/ad_units/notification_ad/notification_ad_info.h"
#include "brave/components/brave_ads/core/public/database/database.h"
#include "brave/components/brave_ads/core/public/flags/flags_util.h"

namespace brave_ads {

namespace {

using AdEventHistoryMap =
    base::flat_map</*type_id*/ std::string, std::vector<base::Time>>;
using AdEventMap = base::flat_map</*uuid*/ std::string, AdEventHistoryMap>;

AdEventMap& AdEventCache() {
  static base::NoDestructor<AdEventMap> ad_events;
  return *ad_events;
}

}  // namespace

void MockFlags() {
  GlobalState::GetInstance()->Flags() = *BuildFlags();

  // Use the staging environment for tests if we did not append command line
  // switches in `SetUpMocks()`.
  if (!DidAppendCommandLineSwitches()) {
    CHECK(GlobalState::HasInstance());
    GlobalState::GetInstance()->Flags().environment_type =
        mojom::EnvironmentType::kStaging;
  }
}

void MockShowNotificationAd(AdsClientMock& mock) {
  ON_CALL(mock, ShowNotificationAd)
      .WillByDefault(::testing::Invoke([](const NotificationAdInfo& ad) {
        // TODO(https://github.com/brave/brave-browser/issues/29587): Decouple
        // reminders from push notification ads.
        const bool is_reminder_valid = !ad.placement_id.empty() &&
                                       !ad.title.empty() && !ad.body.empty() &&
                                       ad.target_url.is_valid();

        CHECK(ad.IsValid() || is_reminder_valid);
      }));
}

void MockCloseNotificationAd(AdsClientMock& mock) {
  ON_CALL(mock, CloseNotificationAd)
      .WillByDefault(::testing::Invoke([](const std::string& placement_id) {
        CHECK(!placement_id.empty());
      }));
}

void MockCacheAdEventForInstanceId(const AdsClientMock& mock) {
  ON_CALL(mock, CacheAdEventForInstanceId)
      .WillByDefault(::testing::Invoke(
          [](const std::string& id, const std::string& ad_type,
             const std::string& confirmation_type, const base::Time time) {
            CHECK(!id.empty());
            CHECK(!ad_type.empty());
            CHECK(!confirmation_type.empty());

            const std::string uuid = GetUuidForCurrentTestAndValue(id);
            const std::string type_id =
                base::StrCat({ad_type, confirmation_type});
            AdEventCache()[uuid][type_id].push_back(time);
          }));
}

void MockGetCachedAdEvents(const AdsClientMock& mock) {
  ON_CALL(mock, GetCachedAdEvents)
      .WillByDefault(::testing::Invoke(
          [](const std::string& ad_type,
             const std::string& confirmation_type) -> std::vector<base::Time> {
            CHECK(!ad_type.empty());
            CHECK(!confirmation_type.empty());

            const std::string uuid_for_current_test = GetUuidForCurrentTest();

            const std::string type_id =
                base::StrCat({ad_type, confirmation_type});

            std::vector<base::Time> cached_ad_events;

            for (const auto& [uuid, history] : AdEventCache()) {
              if (!uuid.ends_with(base::StrCat({":", uuid_for_current_test}))) {
                // Only get ad events for current test.
                continue;
              }

              for (const auto& [ad_event_type_id, timestamps] : history) {
                if (ad_event_type_id == type_id) {
                  base::Extend(cached_ad_events, timestamps);
                }
              }
            }

            return cached_ad_events;
          }));
}

void MockResetAdEventCacheForInstanceId(const AdsClientMock& mock) {
  ON_CALL(mock, ResetAdEventCacheForInstanceId)
      .WillByDefault(::testing::Invoke([](const std::string& id) {
        CHECK(!id.empty());

        const std::string uuid = GetUuidForCurrentTestAndValue(id);
        AdEventCache()[uuid] = {};
      }));
}

void MockSave(AdsClientMock& mock) {
  ON_CALL(mock, Save)
      .WillByDefault(::testing::Invoke([](const std::string& /*name*/,
                                          const std::string& /*value*/,
                                          SaveCallback callback) {
        std::move(callback).Run(/*success=*/true);
      }));
}

void MockLoad(AdsClientMock& mock, const base::ScopedTempDir& temp_dir) {
  ON_CALL(mock, Load)
      .WillByDefault(::testing::Invoke(
          [&temp_dir](const std::string& name, LoadCallback callback) {
            base::FilePath path = temp_dir.GetPath().AppendASCII(name);
            if (!base::PathExists(path)) {
              // If path does not exist load the file from the test path.
              path = TestDataPath().AppendASCII(name);
            }

            std::string value;
            if (!base::ReadFileToString(path, &value)) {
              return std::move(callback).Run(std::nullopt);
            }

            std::move(callback).Run(value);
          }));
}

void MockLoadComponentResource(AdsClientMock& mock,
                               const base::ScopedTempDir& temp_dir) {
  ON_CALL(mock, LoadComponentResource)
      .WillByDefault(::testing::Invoke(
          [&temp_dir](const std::string& id, const int /*version*/,
                      LoadFileCallback callback) {
            base::FilePath path = temp_dir.GetPath().AppendASCII(id);

            if (!base::PathExists(path)) {
              // If path does not exist load the file from the test path.
              path = ComponentResourcesTestDataPath().AppendASCII(id);
            }

            base::File file(path, base::File::Flags::FLAG_OPEN |
                                      base::File::Flags::FLAG_READ);
            std::move(callback).Run(std::move(file));
          }));
}

void MockLoadDataResource(AdsClientMock& mock) {
  ON_CALL(mock, LoadDataResource)
      .WillByDefault(
          ::testing::Invoke([](const std::string& name) -> std::string {
            return MaybeReadDataResourceToString(name).value_or("");
          }));
}

void MockRunDBTransaction(AdsClientMock& mock, Database& database) {
  ON_CALL(mock, RunDBTransaction)
      .WillByDefault(
          ::testing::Invoke([&database](mojom::DBTransactionInfoPtr transaction,
                                        RunDBTransactionCallback callback) {
            CHECK(transaction);

            mojom::DBCommandResponseInfoPtr command_response =
                mojom::DBCommandResponseInfo::New();

            database.RunTransaction(std::move(transaction), &*command_response);

            std::move(callback).Run(std::move(command_response));
          }));
}

void MockGetProfilePref(const AdsClientMock& mock) {
  ON_CALL(mock, GetProfilePref)
      .WillByDefault(::testing::Invoke(
          [](const std::string& path) -> std::optional<base::Value> {
            return GetProfilePrefValue(path);
          }));
}

void MockClearProfilePref(AdsClientMock& mock) {
  ON_CALL(mock, ClearProfilePref)
      .WillByDefault(::testing::Invoke(
          [](const std::string& path) { ClearProfilePrefValue(path); }));
}

void MockHasProfilePrefPath(const AdsClientMock& mock) {
  ON_CALL(mock, HasProfilePrefPath)
      .WillByDefault(::testing::Invoke([](const std::string& path) -> bool {
        return HasProfilePrefPathValue(path);
      }));
}

void MockGetLocalStatePref(const AdsClientMock& mock) {
  ON_CALL(mock, GetLocalStatePref)
      .WillByDefault(::testing::Invoke(
          [](const std::string& path) -> std::optional<base::Value> {
            return GetLocalStatePrefValue(path);
          }));
}

void MockClearLocalStatePref(AdsClientMock& mock) {
  ON_CALL(mock, ClearLocalStatePref)
      .WillByDefault(::testing::Invoke(
          [](const std::string& path) { ClearLocalStatePrefValue(path); }));
}

void MockHasLocalStatePrefPath(const AdsClientMock& mock) {
  ON_CALL(mock, HasLocalStatePrefPath)
      .WillByDefault(::testing::Invoke([](const std::string& path) -> bool {
        return HasLocalStatePrefPathValue(path);
      }));
}

}  // namespace brave_ads
