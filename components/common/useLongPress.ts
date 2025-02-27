// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'

const DEFAULT_THRESHOLD = 500

export interface LongPressProps {
  /**
   * Handler that is called when the threshold time is met while
   * the press is over the target.
   */
  onLongPress: (e: React.TouchEvent) => void
  /**
   * Handler that is called when a press move interaction starts
   */
  onTouchMove?: (e: React.TouchEvent) => void
  /**
   * The amount of time in milliseconds to wait before triggering a long press.
   * @default 500ms
   */
  delay?: number
}

export default function useLongPress(props: LongPressProps) {
  const touchTimer = React.useRef<NodeJS.Timeout | number>()

  const handleTouchStart = React.useCallback(
    (e: React.TouchEvent) => {
      // The TouchEvent `e` gets nullified and is not available to the caller
      // We must persist it
      // https://legacy.reactjs.org/docs/legacy-event-pooling.html
      e.persist()
      touchTimer.current = setTimeout(
        () => props.onLongPress(e),
        props.delay === undefined ? DEFAULT_THRESHOLD : props.delay
      )
    },
    [props.onLongPress, props.delay]
  )

  const handleTouchEnd = React.useCallback(() => {
    if (touchTimer.current) {
      clearTimeout(touchTimer.current)
      touchTimer.current = undefined
    }
  }, [])

  const handleTouchMove = React.useCallback((e: React.TouchEvent) => {
    handleTouchEnd()
    props.onTouchMove?.(e)
  }, [])

  return {
    onTouchStart: handleTouchStart,
    onTouchEnd: handleTouchEnd,
    onTouchMove: handleTouchMove
  }
}
