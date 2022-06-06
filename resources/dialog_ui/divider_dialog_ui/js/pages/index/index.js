/*
    Copyright (c) 2022 Huawei Device Co., Ltd.
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

import router from '@ohos.router'
import window from '@ohos.window';

export default {
    data: {
        backgroundColor:"black"
    },
    onInit() {
       callNativeHandler("EVENT_INIT", "");
    },
    onCancel() {
        callNativeHandler("EVENT_CANCEL", "");
    },
    onExit() {
        callNativeHandler("EVENT_CANCEL", "");
    },
    onExchange() {
    },
    onExchangeDoubleClick() {
        console.info('double click exchange');
    },
    onFocus(){
        this.backgroundColor = "black"
    },
    recoveryColor(){
        this.backgroundColor = "gray"
    },
    onTouching(){
        this.backgroundColor = "gray"
    },
    onTouchEnd(){
        this.backgroundColor = "black"
    }
}