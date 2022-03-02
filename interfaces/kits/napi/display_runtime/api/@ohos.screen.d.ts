/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

import { AsyncCallback, Callback } from './basic';

/**
 * interface of screen manager
 * @devices tv, phone, tablet, wearable
 * @systemapi Hide this for inner system use.
 * @since 8
 */
declare namespace screen {
  /**
   * get all screen
   * @devices tv, phone, tablet, wearable
   * @since 8
   */
  function getAllScreen(): Promise<Array<Screen>>;

  // Screen plug-in event; Screen resolution ratio and other parameters, combination relationship changes
  function on(eventType: 'connect' | 'disconnect' | 'change', callback: Callback<number>): void;
  function off(eventType: 'connect' | 'disconnect' | 'change', callback?: Callback<number>): void;

  /**
   * make screens as expand-screen
   * @devices tv, phone, tablet, wearable
   * @since 8
   */
  function makeExpand(options:Array<ExpandOption>): Promise<number>;

  /**
   * make screens as mirror-screen
   * @devices tv, phone, tablet, wearable
   * @since 8
   */
  function makeMirror(mainScreen:number, mirrorScreen:Array<number>): Promise<number>;

  /**
   * the paramter of making expand screen
   * @devices tv, phone, tablet, wearable
   * @since 8
   */
  interface ExpandOption {
    /**
     * screen id
     */
    screenId: number;

    /**
     * the start coordinate X of the screen origin
     */
    startX: number;

    /**
     * the start coordinate Y of the screen origin
     */
    startY: number;
  }

  /**
   * interface for screen
   * @devices tv, phone, tablet, wearable
   * @since 8
   */
  interface Screen {
    /**
     * get screen id
     * @devices tv, phone, tablet, wearable
     * @since 8
     */
    readonly id: number;

    // return group id
    readonly parent: number;

    readonly supportedModeInfo: Array<ScreenModeInfo>;

    readonly activeModeIndex: number;

    readonly orientation: Orientation;

    /**
     * set the orientation of the screen
     * @devices tv, phone, tablet, wearable
     * @since 8
     */
    setOrientation(orientation: Orientation): Promise<boolean>;

    setScreenActiveMode(modeIndex: number): Promise<boolean>;
  }

  /**
   * interface for screen group
   * @devices tv, phone, tablet, wearable
   * @since 8
   */
   interface ScreenGroup {
    /**
     * get screen group id
     * @devices tv, phone, tablet, wearable
     * @since 8
     */
    id: number;

    combinationInfo: ExpandInfo | MirrorInfo;
  }

  interface ExpandInfo {
    screenId: Array<number>;
    startX: Array<number>;
    startY: Array<number>;
  }

  interface MirrorInfo {
    mainScreenId: number;
    mirrorScreenId: Array<number>;
  }

  enum Orientation {
    UNSPECIFIED = 0,
    VERTICAL = 1,
    HORIZONTAL = 2,
    REVERSE_VERTICAL = 3,
    REVERSE_HORIZONTAL = 4,
    SENSOR = 5,
    SENSOR_VERTICAL = 6,
    SENSOR_HORIZONTAL = 7,
  }

  interface ScreenModeInfo {
    id: number;
    width: number;
    height: number;
    freshRate: number;
  }
}

export default screen;
