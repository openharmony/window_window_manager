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
import hilog from '@ohos.hilog';
import window from '@ohos.window';
import { Callback } from '@ohos.base';

/**
 * uiExtensionHost.
 *
 * @namespace uiExtensionHost
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @systemapi
 * @since 11
 */
namespace uiExtensionHost {
  export native function createExtensionWindow(win: long, hostId: int): UIExtensionHostWindowProxy;

  // UIExtension WmsLogTag
  const WMS_UIEXT: int = 0xC0420D;
  const WMS_UIEXT_TAG: string = "WMSUiext";
  const enum WmErrorCode {
    WM_OK = 0,
    WM_ERROR_INVALID_PARAM = 401
  }
  const WM_ERRORCODE_INVALID_PARAM_MSG: string = `Parameter error. Possible causes: ` +
    `1. Mandatory parameters are left unspecified; ` +
    `2. Incorrect parameters types; ` +
    `3. Parameter verification failed.`;
  const WINDOW_SIZE_CHANGE_CB: string = "windowSizeChange";
  const AVOID_AREA_CHANGE_CB: string = "avoidAreaChange";

  // internal class and function
  export type CallbackSize = (size: window.Size) => void;
  export type CallbackArea = (area: window.AvoidAreaOptions) => void;

  function sizeChangeCallback(fn: object, fnArg: object): void {
    let f = fn as (d: object) => void;
    f(fnArg);
  }

  function avoidAreaChangeCallback(cb: object, area: object, type: int): void {
    const func = cb as (data: window.AvoidAreaOptions) => void;
    func({type: type as window.AvoidAreaType, area: area as window.AvoidArea});
  }

  function keyboardHeightChangeCallback(cb: object, cbArg: int): void {
    const func = cb as (cbArg: number) => void;
    func(cbArg as number);
  }

  class UIESize implements window.Size {
    width: double;
    height: double;
  }

  class UIERect implements window.Rect {
    left: double;
    top: double;
    width: double;
    height: double;
  }

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
    static { loadLibrary("embeddablewindowstageani_kit.z") }
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
    public native setWaterMarkFlag(nativeObj: long, enable: boolean): int;
    public native hidePrivacyContentForHost(nativeObj: long, shouldHide: boolean): int;
    public native onAvoidAreaChange(nativeObj: long, callback: object, callbackData: object): int;
    public native offAvoidAreaChange(nativeObj: long, callback?: object): int;
    public native onWindowSizeChange(nativeObj: long, callback: object, callbackData: object): int;
    public native offWindowSizeChange(nativeObj: long, callback?: object): int;

    get properties(): UIExtensionHostWindowProxyProperties {
      let property = new UIEProperties();
      let ret = this.getProperties(this.nativeObj, property as object);
      if (ret !== WmErrorCode.WM_OK) {
        hilog.error(WMS_UIEXT, WMS_UIEXT_TAG, `getProperties failed, ret: ${ret}`);
      }
      return property;
    }

    set properties(properties: UIExtensionHostWindowProxyProperties) {
      this.setProperties(this.nativeObj, properties as object);
    }

    public on<T>(type: string, callback: Callback<T>): void {
      if (type === AVOID_AREA_CHANGE_CB) {
        let area = new UIEAreaAndType();
        let ret = this.onAvoidAreaChange(this.nativeObj, callback as object, area as object);
        if (ret !== WmErrorCode.WM_OK) {
          hilog.error(WMS_UIEXT, WMS_UIEXT_TAG, `register ${type} callback failed, ret: ${ret}`);
        }
      } else if (type === WINDOW_SIZE_CHANGE_CB) {
        let size = new UIESize();
        let ret = this.onWindowSizeChange(this.nativeObj, callback as object, size as object);
        if (ret !== WmErrorCode.WM_OK) {
          hilog.error(WMS_UIEXT, WMS_UIEXT_TAG, `register ${type} callback failed, ret: ${ret}`);
        }
      } else {
        hilog.error(WMS_UIEXT, WMS_UIEXT_TAG, `type ${type} is not supported`);
        throw {
          code: WmErrorCode.WM_ERROR_INVALID_PARAM,
          message: WM_ERRORCODE_INVALID_PARAM_MSG
        } as BusinessError<string> as Error;
      }
    }

    public off<T>(type: string, callback?: Callback<T>): void {
      if (type === AVOID_AREA_CHANGE_CB) {
        let ret = this.offAvoidAreaChange(this.nativeObj, callback);
        if (ret !== WmErrorCode.WM_OK) {
          hilog.error(WMS_UIEXT, WMS_UIEXT_TAG, `unregister ${type} callback failed, ret: ${ret}`);
        }
      } else if (type === WINDOW_SIZE_CHANGE_CB) {
        let ret = this.offWindowSizeChange(this.nativeObj, callback);
        if (ret !== WmErrorCode.WM_OK) {
          hilog.error(WMS_UIEXT, WMS_UIEXT_TAG, `unregister ${type} callback failed, ret: ${ret}`);
        }
      } else {
        hilog.error(WMS_UIEXT, WMS_UIEXT_TAG, `type ${type} is not supported`);
        throw {
          code: WmErrorCode.WM_ERROR_INVALID_PARAM,
          message: WM_ERRORCODE_INVALID_PARAM_MSG
        } as BusinessError<string> as Error;
      }
    }

    getWindowAvoidArea(type: window.AvoidAreaType): window.AvoidArea {
      let area: window.AvoidArea = new UIEAvoidArea();
      let ret = this.getWindowAvoidArea(this.nativeObj, type as int, area as object);
      if (ret !== WmErrorCode.WM_OK) {
        hilog.error(WMS_UIEXT, WMS_UIEXT_TAG, `getWindowAvoidArea failed, ret: ${ret}`);
      }
      return area;
    }

    setWaterMarkFlag(enable: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          let ret = this.setWaterMarkFlag(this.nativeObj, enable);
          if (ret !== WmErrorCode.WM_OK) {
            hilog.error(WMS_UIEXT, WMS_UIEXT_TAG, `setWaterMarkFlag failed, ret: ${ret}`);
          }
        }).then((ret: NullishType) :void => {
          resolve(undefined);
        }).catch((err: NullishType) :void => {
          reject(err as BusinessError);
        });
      });
    }

    hidePrivacyContentForHost(shouldHide: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          let ret = this.hidePrivacyContentForHost(this.nativeObj, shouldHide);
          if (ret !== WmErrorCode.WM_OK) {
            hilog.error(WMS_UIEXT, WMS_UIEXT_TAG, `hidePrivacyContentForHost failed, ret: ${ret}`);
          }
        }).then((ret: NullishType) :void => {
          resolve(undefined);
        }).catch((err: NullishType) :void => {
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