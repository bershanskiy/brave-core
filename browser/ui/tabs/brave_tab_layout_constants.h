/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_TABS_BRAVE_TAB_LAYOUT_CONSTANTS_H_
#define BRAVE_BROWSER_UI_TABS_BRAVE_TAB_LAYOUT_CONSTANTS_H_

namespace brave_tabs {

// Horizontal tab layout:
//
// The upstream tab implemenation assumes that tab view bounds overlap. In order
// to create a gap between tabs without violating these assumptions, tabs views
// are given a small overlap. Rounded tab rectangles are drawn centered and
// inset horizontally by an amount that will create the required visual gap.

// The height of tab views in horizontal tabs mode. Note that the height of the
// view may be greater than the visual height of the tab shape. See also
// `kHorizontalTabVerticalSpacing`.
constexpr int kHorizontalTabViewHeight = 40;

// The amount of space before the first tab.
constexpr int kHorizontalTabStripLeftMargin = 8;

// The amount of visual spacing between the top and bottom of tabs and the
// bounds of the tab strip region. The portion of this space below tabs will be
// occupied by tab group underlines.
constexpr int kHorizontalTabVerticalSpacing = 4;

// The visual gap between tabs.
constexpr int kHorizontalTabGap = 4;

// The amount of overlap between tabs. Based on upstream assumptions, tab views
// must have a non-negative overlap. Furthermore, tab separators will not render
// correctly if the tab overlap is zero.
constexpr int kHorizontalTabOverlap = 2;

// The horizontal difference between the edge of the tab view and the visual
// edge of the rendered tab.
constexpr int kHorizontalTabInset =
    (kHorizontalTabGap + kHorizontalTabOverlap) / 2;

// The content padding within a tab.
constexpr int kHorizontalTabPadding = 6;

// The horizontal difference between the visual edge of a tab group and the
// bounds of the group underline.
constexpr int kHorizontalGroupUnderlineInset = 2;

// The tab border radius.
constexpr int kTabBorderRadius = 8;

// The size of the group header slot when the title is empty.
constexpr int kEmptyGroupTitleSize = 32;

// The line height of the tab group header text.
constexpr int kTabGroupLineHeight = 24;

// The amount of padding at the top and bottom of tab group header "chips"
// (i.e. the rectangle enclosing the tab group title).
constexpr int kTabGroupTitleVerticalInset = 4;

}  // namespace brave_tabs

#endif  // BRAVE_BROWSER_UI_TABS_BRAVE_TAB_LAYOUT_CONSTANTS_H_
