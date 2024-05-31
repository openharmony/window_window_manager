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

const pip = requireNapi('pip');

const TAG = 'PiPContent';

export class PiPContent extends ViewPU {
    constructor(e, o, t, n = -1, i = void 0) {
        super(e, t, n);
        'function' === typeof i && (this.paramsGenerator_ = i);
        this.xComponentController = new XComponentController;
        this.nodeController = null;
        this.xComponentId = 'pipContent';
        this.xComponentType = 'surface';
        this.setInitiallyProvidedValue(o);
    }

    setInitiallyProvidedValue(e) {
        void 0 !== e.xComponentController && (this.xComponentController = e.xComponentController);
        void 0 !== e.xComponentId && (this.xComponentId = e.xComponentId);
        void 0 !== e.xComponentType && (this.xComponentType = e.xComponentType);
        void 0 !== e.nodeController && (this.nodeController = e.nodeController);
    }

    updateStateVars(e) {
    }

    purgeVariableDependenciesOnElmtId(e) {
    }

    aboutToAppear() {
        this.nodeController = pip.getCustomUIController();
    }

    aboutToBeDeleted() {
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }

    initialRender() {
        this.observeComponentCreation2(((e, o) => {
            Stack.create();
            Stack.size({ width: '100%', height: '100%' });
        }), Stack);
        this.observeComponentCreation2(((e, o) => {
            XComponent.create({
                id: this.xComponentId,
                type: this.xComponentType,
                controller: this.xComponentController
            }, 'pipContent_XComponent');
            XComponent.onLoad((() => {
                pip.initXComponentController(this.xComponentController);
                console.debug(TAG, 'XComponent onLoad done');
            }));
            XComponent.size({ width: '100%', height: '100%' });
            XComponent.backgroundColor(Color.Transparent);
        }), XComponent);
        this.observeComponentCreation2((d, e) => {
            If.create();
            if (this.nodeController != null) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((i, j) => {
                        NodeContainer.create(this.nodeController);
                        NodeContainer.size({ width: '100%', height: '100%'});
                    }, NodeContainer);
                });
            } else {
                this.ifElseBranchUpdateFunction(1, ()=> {
                });
            }
        }, If);
        If.pop();
        Stack.pop();
    }

    rerender() {
        this.updateDirtyElements();
    }
}

ViewStackProcessor.StartGetAccessRecordingFor(ViewStackProcessor.AllocateNewElmetIdForNextComponent());
loadDocument(new PiPContent(void 0, {}));
ViewStackProcessor.StopGetAccessRecording();