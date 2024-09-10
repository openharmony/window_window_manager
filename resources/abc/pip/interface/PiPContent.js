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
const NodeController = requireNapi('arkui.node').NodeController;
const FrameNode = requireNapi('arkui.node').FrameNode;

const TAG = 'PiPContent';
const ABOUT_TO_STOP = 3;

class XCNodeController extends NodeController {
    constructor(k2) {
        super();
        this.node = null;
        this.mXComponent = k2;
    }

    makeNode(i2) {
        let j2;
        this.node = new FrameNode(i2);
        this.node.appendChild(this.mXComponent);
        return this.node;
    }

    removeNode() {
        let h2;
        (h2 = this.node) === null || h2 === void 0 ? void 0 : h2.removeChild(this.mXComponent);
    }
}

export class PiPContent extends ViewPU {
    constructor(b2, c2, d2, e2 = -1, f2 = undefined, g2) {
        super(b2, d2, e2, g2);
        if (typeof f2 === 'function') {
            this.paramsGenerator_ = f2;
        }
        this.xComponentController = new XComponentController;
        this.nodeController = null;
        this.mXCNodeController = null;
        this.useNode = false;
        this.xComponentId = 'pipContent';
        this.xComponentType = 'surface';
        this.xComponent = null;
        this.setInitiallyProvidedValue(c2);
    }

    setInitiallyProvidedValue(a2) {
        if (a2.xComponentController !== undefined) {
            this.xComponentController = a2.xComponentController;
        }
        if (a2.nodeController !== undefined) {
            this.nodeController = a2.nodeController;
        }
        if (a2.mXCNodeController !== undefined) {
            this.mXCNodeController = a2.mXCNodeController;
        }
        if (a2.useNode !== undefined) {
            this.useNode = a2.useNode;
        }
        if (a2.xComponentId !== undefined) {
            this.xComponentId = a2.xComponentId;
        }
        if (a2.xComponentType !== undefined) {
            this.xComponentType = a2.xComponentType;
        }
        if (a2.xComponent !== undefined) {
            this.xComponent = a2.xComponent;
        }
    }

    updateStateVars(z1) {
    }

    purgeVariableDependenciesOnElmtId(y1) {
    }

    aboutToAppear() {
        this.nodeController = pip.getCustomUIController();
        this.xComponent = pip.getTypeNode();
        if (this.xComponent === null || this.xComponent === undefined) {
            console.error(TAG, `xComponent node is null`);
            return;
        }
        let u1 = this.xComponent.getNodeType();
        if (u1 !== 'XComponent') {
            console.error(`xComponent type mismatch: ${u1}`);
            return;
        }
        this.useNode = true;
        if (!this.xComponent.isAttached()) {
            pip.markPipNodeType(this.xComponent, false);
        } else {
            pip.markPipNodeType(this.xComponent, true);
            this.xComponent.getParent().removeChild(this.xComponent);
        }
        pip.setTypeNodeEnabled();
        this.mXCNodeController = new XCNodeController(this.xComponent);
        console.info(TAG, 'use Node Controller');
        pip.on('stateChange', (w1) => {
            let x1;
            console.info(TAG, `stateChange state: ${w1}`);
            if (w1 === ABOUT_TO_STOP) {
                (x1 = this.mXCNodeController) === null || x1 === void 0 ? void 0 : x1.removeNode();
            }
        });
    }

    aboutToBeDeleted() {
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }

    aboutToDisappear() {
        pip.off('stateChange');
    }

    initialRender() {
        this.observeComponentCreation2((s1, t1) => {
            Stack.create();
            Stack.size({ width: '100%', height: '100%' });
        }, Stack);
        this.observeComponentCreation2((h1, i1) => {
            If.create();
            if (this.useNode) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.buildNode.bind(this)();
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.buildXComponent.bind(this)();
                });
            }
        }, If);
        If.pop();
        this.observeComponentCreation2((a1, b1) => {
            If.create();
            if (this.nodeController !== null) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.buildCustomUI.bind(this)();
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        Stack.pop();
    }

    buildCustomUI(g1 = null) {
        this.observeComponentCreation2((i1, j1) => {
            NodeContainer.create(this.nodeController);
            NodeContainer.size({ width: '100%', height: '100%'});
        }, NodeContainer);
    }

    buildXComponent(b1 = null) {
        this.observeComponentCreation2((d1, e1) => {
            XComponent.create({
                id: this.xComponentId,
                type: this.xComponentType,
                controller: this.xComponentController
            }, 'pipContent_XComponent');
            XComponent.onLoad((() => {
                pip.initXComponentController(this.xComponentController);
                console.info(TAG, 'XComponent onLoad done');
            }));
            XComponent.size({ width: '100%', height: '100%' });
            XComponent.backgroundColor(Color.Transparent);
        }, XComponent);
    }

    buildNode(x = null) {
        this.observeComponentCreation2((z, a1) => {
            NodeContainer.create(this.mXCNodeController);
            NodeContainer.size({ width: '100%', height: '100%' });
        }, NodeContainer);
    }

    rerender() {
        this.updateDirtyElements();
    }

    static getEntryName() {
        return 'PiPContent';
    }
}

ViewStackProcessor.StartGetAccessRecordingFor(ViewStackProcessor.AllocateNewElmetIdForNextComponent());
loadDocument(new PiPContent(void 0, {}));
ViewStackProcessor.StopGetAccessRecording();