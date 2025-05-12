/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import type { AsyncCallback, Callback} from '@ohos.base';
import { BusinessError } from '@ohos.base';
import { NodeController, XComponentController } from '@ohos.arkui.component';
import common from '@ohos.app.ability.common';

export default namespace PiPWindow {
  loadLibrary('pipwindowani_kit.z');
  
  export interface PiPController {
    startPiP(): Promise<void>;
    stopPiP(): Promise<void>;
  }
  
  export class PiPControllerInternal implements PiPController {
    static { loadLibrary('pipwindowani_kit.z') };
    private nativeObj: long;
    public setNativeObj(nativeObj: long): void {
        this.nativeObj = nativeObj;
    }

    public native startPiPNative(nativeObj: long): void;
    public native stopPiPNative(nativeObj: long): void;

    public startPiP(): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.startPiPNative(this.nativeObj);
            }).then((ret: NullishType) :void => {
                resolve(undefined);
            }).catch((err: NullishType) :void => {
                reject(err as BusinessError);
            });
        });
    }

    public stopPiP(): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.stopPiPNative(this.nativeObj);
            }).then((ret: NullishType) :void => {
                resolve(undefined);
            }).catch((err: NullishType) :void => {
                reject(err as BusinessError);
            });
        });
    }
  }

  export enum PiPTemplateType {
    VIDEO_PLAY,
    VIDEO_CALL,
    VIDEO_MEETING,
    VIDEO_LIVE,
  }

  export type PiPControlGroup = VideoPlayControlGroup | VideoCallControlGroup | VideoMeetingControlGroup | VideoLiveControlGroup;

  export enum VideoPlayControlGroup {
    VIDEO_PREVIOUS_NEXT = 101,
    FAST_FORWARD_BACKWARD = 102,
  }

  export enum VideoCallControlGroup {
    MICROPHONE_SWITCH = 201,
    HANG_UP_BUTTON = 202,
    CAMERA_SWITCH = 203,
    MUTE_SWITCH = 204,
  }

  export enum VideoMeetingControlGroup {
    HANG_UP_BUTTON = 301,
    CAMERA_SWITCH = 302,
    MUTE_SWITCH = 303,
    MICROPHONE_SWITCH = 304,
  }

  export enum VideoLiveControlGroup {
    VIDEO_PLAY_PAUSE = 401,
    MUTE_SWITCH = 402,
  }

  export interface PiPConfiguration {
    context: common.BaseContext;
    componentController: XComponentController;
    navigationId?: string;
    templateType?: PiPTemplateType;
    contentWidth?: number;
    contentHeight?: number;
    controlGroups?: Array<PiPControlGroup>;
    customUIController?: NodeController;
  }

  let nativeObj: long;
  export function setNativeObj(nativeObj: long):void {
    nativeObj = nativeObj;
  }

  export native function isPiPEnabled(): boolean;
}
