/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

class EmbeddableWindowStage {
  constructor(obj) {
    this.__embeddable_window_stage__ = obj;
  }

  on(type, callback) {
    return this.__embeddable_window_stage__.on(type, callback);
  }

  off(type, callback) {
    return this.__embeddable_window_stage__.off(type, callback);
  }

  loadContent(url, storage, asyncCallback) {
    return this.__embeddable_window_stage__.loadContent(url, storage, asyncCallback);
  }

  loadContentByName(name, storage, asyncCallback) {
    return this.__embeddable_window_stage__.loadContentByName(name, storage, asyncCallback);
  }

  getMainWindow(asyncCallback) {
    return this.__embeddable_window_stage__.getMainWindow(asyncCallback);
  }

  getMainWindowSync() {
    return this.__embeddable_window_stage__.getMainWindowSync();
  }

  createSubWindow(windowName, asyncCallback) {
    return this.__embeddable_window_stage__.createSubWindow(windowName, asyncCallback);
  }

  getSubWindow(asyncCallback) {
    return this.__embeddable_window_stage__.getSubWindow(asyncCallback);
  }

  createSubWindowWithOptions(windowName, option, asyncCallback) {
    return this.__embeddable_window_stage__.createSubWindowWithOptions(windowName, option, asyncCallback);
  }

  setUIContent(context, url, storage) {
    return this.__embeddable_window_stage__.setUIContent(context, url, storage);
  }

  setDefaultDensityEnabled(enabled) {
    return this.__embeddable_window_stage__.setDefaultDensityEnabled(enabled);
  }

  setCustomDensity(density) {
    return this.__embeddable_window_stage__.setCustomDensity(density);
  }

  removeStartingWindow(asyncCallback) {
    return this.__embeddable_window_stage__.removeStartingWindow(asyncCallback);
  }

  setWindowModal(isModal, asyncCallback) {
    return this.__embeddable_window_stage__.setWindowModal(isModal, asyncCallback);
  }

  setWindowRectAutoSave(enabled, isSaveBySpecifiedFlag) {
    return this.__embeddable_window_stage__.setWindowRectAutoSave(enabled, isSaveBySpecifiedFlag);
  }

  isWindowRectAutoSave(asyncCallback) {
    return this.__embeddable_window_stage__.isWindowRectAutoSave(asyncCallback);
  }

  setSupportedWindowModes(supportedWindowModes, asyncCallback) {
    return this.__embeddable_window_stage__.setSupportedWindowModes(supportedWindowModes, asyncCallback);
  }
}

export default EmbeddableWindowStage;
