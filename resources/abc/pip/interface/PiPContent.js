/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

const pip = requireNapi("pip");

const TAG = "PiPContent";

export class PiPContent extends ViewPU {
    constructor(e, o, t, n = -1, i = void 0) {
        super(e, t, n);
        "function" == typeof i && (this.paramsGenerator_ = i);
        this.xComponentController = new XComponentController;
        this.xComponentId = "pipContent";
        this.setInitiallyProvidedValue(o);
    }

    setInitiallyProvidedValue(e) {
        void 0 !== e.xComponentController && (this.xComponentController = e.xComponentController);
        void 0 !== e.xComponentId && (this.xComponentId = e.xComponentId);
    }

    updateStateVars(e) {
    }

    purgeVariableDependenciesOnElmtId(e) {
    }

    aboutToBeDeleted() {
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }

    initialRender() {
        this.observeComponentCreation2(((e, o) => {
            Stack.create();
            Stack.size({ width: "100%", height: "100%" });
        }), Stack);
        this.observeComponentCreation2(((e, o) => {
            XComponent.create({
                id: this.xComponentId,
                type: "surface",
                controller: this.xComponentController
            }, "pipContent_XComponent");
            XComponent.onLoad((() => {
                pip.initXComponentController(this.xComponentController);
                console.debug(TAG, "XComponent onLoad done");
            }));
            XComponent.size({ width: "100%", height: "100%" });
        }), XComponent);
        Stack.pop();
    }

    rerender() {
        this.updateDirtyElements();
    }
}

ViewStackProcessor.StartGetAccessRecordingFor(ViewStackProcessor.AllocateNewElmtIdForNextComponent());
loadDocument(new PiPContent(void 0, {}));
ViewStackProcessor.StopGetAccessRecording();