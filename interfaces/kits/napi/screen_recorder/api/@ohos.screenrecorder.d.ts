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
 * interface of screen recorder.
 * @syscap SystemCapability.WindowManager.WindowManager.Core
 * @systemapi Hide this for inner system use.
 * @since 9
 */
declare namespace screenrecorder {
  /**
   * Get virtual screen recorder.
   * @param screenId Indicates the screen id of the virtual screen.
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  function getVirtualScreenRecorder(screenId:number, callback: AsyncCallback<VirtualScreenRecorder>): void;
  function getVirtualScreenRecorder(screenId:number): Promise<VirtualScreenRecorder>;

  /**
   * interface for VirtualScreenRecorder.
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  interface VirtualScreenRecorder {
    /**
     * screen id
     */
    readonly id: number;

    /**
     * Get input surface.
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @systemapi Hide this for inner system use.
     * @since 9
     */
    getInputSurface(callback: AsyncCallback<string>): void;
    getInputSurface(): Promise<string>;

    /**
     * Start virtual screen recorder.
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @systemapi Hide this for inner system use.
     * @since 9
     */
    start(fds:Array<number>, callback: AsyncCallback<void>): void;
    start(fds:Array<number>): Promise<void>;

    /**
     * Stop virtual screen recorder.
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @systemapi Hide this for inner system use.
     * @since 9
     */
    stop(callback: AsyncCallback<void>): void;
    stop(): Promise<void>;
  }
}

export default screenrecorder;
