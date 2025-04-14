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

class WindowStage {
  constructor(obj) {
    this.windowStage = obj;
  }

  setUIContent(context, url, storage) {
    return this.windowStage.setUIContent(context, url, storage);
  }

  loadContent(url, storage, asyncCallback) {
    return this.windowStage.loadContent(url, storage, asyncCallback);
  }

  loadContentByName(name, storage, asyncCallback) {
    return this.windowStage.loadContentByName(name, storage, asyncCallback);
  }

  getWindowMode(asyncCallback) {
    return this.windowStage.getWindowMode(asyncCallback);
  }

  getMainWindow(asyncCallback) {
    return this.windowStage.getMainWindow(asyncCallback);
  }

  getMainWindowSync() {
    return this.windowStage.getMainWindowSync();
  }

  createSubWindow(windowName, asyncCallback) {
    return this.windowStage.createSubWindow(windowName, asyncCallback);
  }

  createSubWindowWithOptions(windowName, options, asyncCallback) {
    return this.windowStage.createSubWindowWithOptions(windowName, options, asyncCallback);
  }

  getSubWindow(asyncCallback) {
    return this.windowStage.getSubWindow(asyncCallback);
  }

  setWindowModal(isModal) {
    return this.windowStage.setWindowModal(isModal);
  }

  on(type, callback) {
    return this.windowStage.on(type, callback);
  }

  off(type, callback) {
    return this.windowStage.off(type, callback);
  }

  setShowOnLockScreen(showOnLockScreen) {
    return this.windowStage.setShowOnLockScreen(showOnLockScreen);
  }

  disableWindowDecor() {
    return this.windowStage.disableWindowDecor();
  }

  setDefaultDensityEnabled(enabled) {
    return this.windowStage.setDefaultDensityEnabled(enabled);
  }

  setCustomDensity(density) {
    return this.windowStage.setCustomDensity(density);
  }

  removeStartingWindow() {
    return this.windowStage.removeStartingWindow();
  }

  setWindowRectAutoSave(enabled, isSaveBySpecifiedFlag) {
    return this.windowStage.setWindowRectAutoSave(enabled, isSaveBySpecifiedFlag);
  }

  isWindowRectAutoSave() {
    return this.windowStage.isWindowRectAutoSave();
  }

  setSupportedWindowModes(supportedWindowModes) {
    return this.windowStage.setSupportedWindowModes(supportedWindowModes);
  }
}

export default WindowStage;
