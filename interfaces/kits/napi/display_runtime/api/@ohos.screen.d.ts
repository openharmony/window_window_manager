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
  function on(eventType: 'screenConnectEvent' | 'screenChangeEvent', callback: Callback<ScreenEvent>): void;
  function off(eventType: 'screenConnectEvent' | 'screenChangeEvent', callback?: Callback<ScreenEvent>): void;

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

    /**
     * get the rotation of the screen
     * @devices tv, phone, tablet, wearable
     * @since 8
     */
    readonly rotation: Rotation;

    requestRotation(rotation: Rotation): Promise<boolean>;

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

  /**
   * screen connect or disconnect event
   * @devices tv, phone, tablet, wearable
   * @since 8
   */
  type ScreenEvent = {
    screenId: number;
    type: ScreenConnectEventType;
  } | {
    screenId: Array<number>;
    type: ScreenChangeEventType;
  }

  enum ScreenConnectEventType {
    DISCONNECT = 0,
    CONNECT = 1,
  }

  enum ScreenChangeEventType {
    ADD_TO_GROUP = 0,
    REMOVE_FROM_GROUP = 1,
    CHANGE_GROUP = 2,
    UPDATE_ROTATION = 3,
    CHANGE_MODE = 4,
  }

  /**
   * rotation degree of screen
   * @devices tv, phone, tablet, wearable
   * @since 8
   */
   enum Rotation {
    /**
     * 0 degree
     */
    ROTATION_0 = 0,
    /**
     * 90 degree
     */
    ROTATION_90 = 1,
    /**
     * 180 degree
     */
    ROTATION_180 = 2,
    /**
     * 270 degree
     */
    ROTATION_270 = 3,
  }

  interface ScreenModeInfo {
    id: number;
    width: number;
    height: number;
    freshRate: number;
  }
}

export default screen;
