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
 
import { AsyncCallback, Callback } from '@ohos.base';
import { BusinessError } from '@ohos.base';
import image from '@ohos.multimedia.image';
 
export default namespace screenshot {
 
loadLibrary('screenshotani_kit.z');
 
export function save(options: ScreenshotOptions, callback: AsyncCallback<image.PixelMap | undefined>): void {
    taskpool.execute((options: ScreenshotOptions): image.PixelMap => {
        let res: image.PixelMap = saveSync(options);
        return res;
    }).then((ret: NullishType) => {
        callback(new BusinessError(), ret as image.PixelMap);
    }).catch((err: NullishType) => {
        callback(err as BusinessError, undefined);
    });
}
 
export function save(callback: AsyncCallback<image.PixelMap | undefined>): void {
    taskpool.execute((): image.PixelMap => {
        let res: image.PixelMap = saveSync();
        return res;
    }).then((ret: NullishType) => {
        callback(new BusinessError(), ret as image.PixelMap);
    }).catch((err: NullishType) => {
        callback(err as BusinessError, undefined);
    });
}
 
export function save(options?: ScreenshotOptions): Promise<image.PixelMap> {
    return new Promise<image.PixelMap>((resolve: (value: image.PixelMap) => void, reject: (error: BusinessError) => void ) => {
        taskpool.execute((): image.PixelMap => {
            let res: image.PixelMap = saveSync();
            return res;
        }).then((ret: NullishType) => {
            resolve(ret as image.PixelMap);
        }).catch((err: NullishType) => {
            reject(err as BusinessError);
        });
    });
}
 
native function saveSync(screenshotOptions?: ScreenshotOptions): image.PixelMap;
 
export interface ScreenshotOptions {
    screenRect: Rect;
 
    imageSize: Size;
 
    rotation: number;
 
    displayId: number;
 
    isNotificationNeeded: number;
 
    isCaptureFullOfScreen: number;
}
 
export class ScreenshotOptionsImpl implements ScreenshotOptions{
    screenRect: Rect;
 
    imageSize: Size;
 
    rotation: number;
 
    displayId: number;
 
    isNotificationNeeded: number;
    
    isCaptureFullOfScreen: number;
}
 
export interface Size {
    width: number;
 
    height: number;
}
 
export class SizeImpl implements Size {
    width: number;
 
    height: number;
}
 
export interface Rect {
    left: number;
 
    top: number;
 
    width: number;
 
    height: number;
}
 
export class RectImpl implements Rect {
    left: number;
 
    top: number;
 
    width: number;
 
    height: number;
}
 
}