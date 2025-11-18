/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

import { BusinessError } from '@ohos.base';
import window from '@ohos.window';
import { Callback } from '@ohos.base'

/**
 * uiExtensionHost.
 *
 * @namespace uiExtensionHost
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @systemapi
 * @since 11
 */
namespace uiExtensionHost {
  export native function createExtentionWindow(win: long, hostId: int): UIExtensionHostWindowProxy;

  // internal class and function
  export type CallbackSize = (size: window.Size) => void;
  export type CallbackArea = (area: window.AvoidAreaOptions) => void;

  const WMS_UIEXT: int = 0xC0420D;
  const WMS_UIEXT_TAG: string = "WMSUiext";

  function callback(fn: object, fnArg: object): void {
      let f = fn as (d: object) => void;
      f(fnArg);
  }

  class UIESize implements window.Size {
    width: int;
    height: int;
  }
  class UIERect implements window.Rect {
    left: int;
    top: int;
    width: int;
    height: int;
  };
  class UIEAvoidArea implements window.AvoidArea {
    constructor() {
        this.leftRect = new UIERect();
        this.topRect = new UIERect();
        this.rightRect = new UIERect();
        this.bottomRect = new UIERect();
    }
    visible: boolean;
    leftRect: window.Rect;
    topRect: window.Rect;
    rightRect: window.Rect;
    bottomRect: window.Rect;
  }
  class UIEAreaAndType {
    type: window.AvoidAreaType;
    area: UIEAvoidArea;
  }
  class UIEProperties implements UIExtensionHostWindowProxyProperties {
    constructor() {
        this.uiExtensionHostWindowProxyRect = new UIERect();
    }
    uiExtensionHostWindowProxyRect: window.Rect;
  }

  // API implementation
  export class UIExtensionHostInternal implements UIExtensionHostWindowProxy {
    static { loadLibrary("embeddablewindowstageani_module.z") }
    private nativeObj: long = 0;
    private setNativeObj(nativeObj: long): void {
        this.nativeObj = nativeObj;
    }
    private setAreaVisible(obj: window.AvoidArea, visible: boolean) {
        obj.visible = visible;
    }

    public native getProperties(nativeObj: long, property: object): int;
    public native setProperties(nativeObj: long, property: object): int;
    public native getWindowAvoidArea(nativeObj: long, type: int, area: object): int;
    public native setWaterMarkFlag(nativeObj: long, enable: boolean): void;
    public native hidePrivacyContentForHost(nativeObj: long, shouldHide: boolean): void;
    public native onAvoidAreaChange(nativeObj: long, callback: object, callbackData: object): int;
    public native offAvoidAreaChange(nativeObj: long, callback?: object): int;
    public native onWindowSizeChange(nativeObj: long, callback: object, callbackData: object): int;
    public native offWindowSizeChange(nativeObj: long, callback?: object): int;

    get properties(): UIExtensionHostWindowProxyProperties {
        let property = new UIEProperties();
        let ret = this.getProperties(this.nativeObj, property as object);
        return property;
    }
    set properties(property: UIExtensionHostWindowProxyProperties) {
        this.setProperties(this.nativeObj, property as object);
    }
    public on<T>(type: string, callback: Callback<T>): void {
        if (type == 'avoidAreaChange') {
            let area = new UIEAreaAndType();
            this.onAvoidAreaChange(this.nativeObj, callback as object, area as object);
        } else {
            let size = new UIESize();
            this.onWindowSizeChange(this.nativeObj, callback as object, size as object);
        }
    }
    public off<T>(type: string, callback?: Callback<T>): void {
        if (type == 'avoidAreaChange') {
            this.offAvoidAreaChange(this.nativeObj, callback);
        } else {
            this.offWindowSizeChange(this.nativeObj, callback);
        }
    }
    getWindowAvoidArea(type: window.AvoidAreaType): window.AvoidArea {
        let area: window.AvoidArea = new UIEAvoidArea();
        let ret = this.getWindowAvoidArea(this.nativeObj, type as int, area as object);
        return area;
    }

    setWaterMarkFlag(enable: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWaterMarkFlag(this.nativeObj, enable);
        }).then((ret: Any) :void => {
          resolve(undefined);
        }).catch((err: Any) :void => {
          reject(err as BusinessError);
        });
      });
    }

    hidePrivacyContentForHost(shouldHide: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.hidePrivacyContentForHost(this.nativeObj, shouldHide);
        }).then((ret: Any) :void => {
          resolve(undefined);
        }).catch((err: Any) :void => {
          reject(err as BusinessError);
        });
      });
    }
  }
  /**
   * Transition Controller
   *
   * @interface UIExtensionHostWindowProxy
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @systemapi
   * @since 11
   */
  export interface UIExtensionHostWindowProxy {
    getWindowAvoidArea(type: window.AvoidAreaType): window.AvoidArea;
    setWaterMarkFlag(enable: boolean): Promise<void>;
    hidePrivacyContentForHost(shouldHide: boolean): Promise<void>;
    on<T>(type: string, callback: Callback<T>): void;
    off<T>(type: string, callback?: Callback<T>): void;
    properties: UIExtensionHostWindowProxyProperties;
  }

  /**
   * Properties of UIExtension window
   *
   * @interface UIExtensionHostWindowProxyProperties
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @systemapi
   * @since 11
   */
  interface UIExtensionHostWindowProxyProperties {
    /**
     * The position and size of the UIExtension window
     *
     * @type { window.Rect }
     * @syscap SystemCapability.ArkUI.ArkUI.Full
     * @systemapi
     * @since 11
     */
    uiExtensionHostWindowProxyRect: window.Rect;
  }
}

export default uiExtensionHost;
