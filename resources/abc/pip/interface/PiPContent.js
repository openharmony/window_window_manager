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

    replaceNode(r2) {
        let s2;
        let t2;
        (s2 = this.node) === null || s2 === void 0 ? void 0 : s2.removeChild(this.mXComponent);
        this.mXComponent = r2;
        (t2 = this.node) === null || t2 === void 0 ? void 0 : t2.appendChild(this.mXComponent);
    }

    removeNode() {
        let h2;
        (h2 = this.node) === null || h2 === void 0 ? void 0 : h2.removeChild(this.mXComponent);
    }
}

class PiPContent extends ViewPU {
    constructor(b2, c2, d2, e2 = -1, f2 = undefined, g2) {
        super(b2, d2, e2, g2);
        if (typeof f2 === 'function') {
            this.paramsGenerator_ = f2;
        }
        this.xComponentController = new XComponentController;
        this.nodeController = null;
        this.mXCNodeController = null;
        this.__useNode = new ObservedPropertySimplePU(false, this, 'useNode');
        this.__nodeChange = new ObservedPropertySimplePU(false, this, 'nodeChange');
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
        if (a2.nodeChange !== undefined) {
            this.nodeChange = a2.nodeChange;
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

    purgeVariableDependenciesOnElmtId(h2) {
        this.__useNode.purgeDependencyOnElmtId(h2);
        this.__nodeChange.purgeDependencyOnElmtId(h2);
    }

    aboutToBeDeleted() {
        this.__useNode.aboutToBeDeleted();
        this.__nodeChange.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }

    get useNode() {
        return this.__useNode.get();
    }

    set useNode(b) {
        this.__useNode.set(b);
    }

    get nodeChange() {
        return this.__nodeChange.get();
    }

    set nodeChange(a) {
        this.__nodeChange.set(a);
    }

    validateNode(f2) {
        if (f2 === null || f2 === undefined) {
            console.error(TAG, `validateNode node is null`);
            return false;
        }
        let g2 = f2.getNodeType();
        if (g2 !== 'XComponent') {
            console.error(TAG, `node type mismatch: ${g2}`);
            return false;
        }
        return true;
    }

    aboutToAppear() {
        this.nodeController = pip.getCustomUIController();
        this.registerUpdateNodeListener();
        this.xComponent = pip.getTypeNode();
        if (!this.validateNode(this.xComponent)) {
            return;
        }
        this.updatePipNodeType(this.xComponent);
        this.useNode = true;
        pip.setTypeNodeEnabled();
        this.mXCNodeController = new XCNodeController(this.xComponent);
        console.info(TAG, 'use Node Controller');
        this.registerStateChangeListener();
    }

    registerStateChangeListener() {
        pip.on('stateChange', (y1) => {
            let z1;
            console.info(TAG, `stateChange state:${y1}`);
            if (y1 === ABOUT_TO_STOP) {
                (z1 = this.mXCNodeController) === null || z1 === void 0 ? void 0 : z1.removeNode();
            }
        });
    }

    registerUpdateNodeListener() {
        pip.on('nodeUpdate', (d2) => {
            let e2;
            console.info(TAG, `nodeUpdate`);
            if (!this.validateNode(d2)) {
                return;
            }
            if (this.useNode) {
                this.updatePipNodeType(d2);
                (e2 = this.mXCNodeController) === null || e2 === void 0 ? void 0 : e2.replaceNode(d2);
                this.nodeChange = true;
            } else {
                this.updatePipNodeType(d2);
                this.mXCNodeController = new XCNodeController(d2);
                console.info(TAG, 'update to Node Controller');
                this.registerStateChangeListener();
                this.useNode = true;
            }
        });
    }

    updatePipNodeType(a2) {
        let b2 = a2.getParent();
        if (b2 === null || b2 === undefined) {
            pip.setPipNodeType(a2, false);
        } else {
            pip.setPipNodeType(a2, true);
            b2.removeChild(a2);
        }
    }


    aboutToDisappear() {
        pip.off('stateChange');
        pip.off('nodeUpdate');
    }

    initialRender() {
        this.observeComponentCreation2((s1, t1) => {
            Stack.create();
            Stack.size({ width: '100%', height: '100%' });
        }, Stack);
        this.observeComponentCreation2((h1, i1) => {
            If.create();
            if (this.useNode || this.nodeChange) {
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