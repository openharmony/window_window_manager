/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
import colorSpaceManager from '@ohos.graphics.colorSpaceManager';
import hdrCapability from '@ohos.graphics.hdrCapability';

namespace PiPWindow {
  export interface PiPController {
    startPiP(): Promise<void>;
    stopPiP(): Promise<void>;
  }
  export class PiPControllerInternal implements PiPController {
    static { loadLibrary('pipwindowani_kit.z') }
    private nativeObj : long;
    setNativeObj(nativeObj: long):void {
       this.nativeObj = nativeObj;
    }

    private native startPiP(nativeObj: long): void;
    private native stopPiP(nativeObj: long): void;'

    public startPiP(): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void) :void => {
            taskpool.execute((): void => {
                this.startPiP(this.nativeObj);
            }).then((ret: NullishType) :void => {
                resolve(undefined);
            }).catch((err: NullishType) :void => {
                reject(err as BusinessError);
            });
        });
    }

    public stopPiP(): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void) :void => {
            taskpool.execute((): void => {
                this.stopPiP(this.nativeObj);
            }).then((ret: NullishType) :void => {
                resolve(undefined);
            }).catch((err: NullishType) :void => {
                reject(err as BusinessError);
            });
        });
    }
  }

  let nativeObj: long;
  function setNativeObj(nativeObj: long):void {
    nativeObj = nativeObj;
  }

  native function isPiPEnabled(nativeObj: long): boolean;
  export function isPiPEnabled(): boolean {
    return new Promise<boolean>((resolve: (value: boolean) => void, reject: (error: BusinessError) => void ) => {
    taskpool.execute((): boolean => {
      return isPiPEnabled(nativeObj);
    }).then((ret: NullishType) => {
      resolve(ret as boolean);
    }).catch((err: NullishType) => {
      reject(err as BusinessError);
    });
    }); 
  }
}

export default PiPWindow;