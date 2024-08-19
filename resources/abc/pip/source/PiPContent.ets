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

import pip from '@ohos.pip';
import { NodeController, FrameNode, TypedFrameNode, UIContext } from '@kit.ArkUI';

const TAG: string = 'PiPContent';
const ABOUT_TO_STOP = 3;

class XCNodeController extends NodeController {
  private mXComponent: TypedFrameNode<XComponentInterface, XComponentAttribute>;
  private node: FrameNode | null = null;

  constructor(xComponent: TypedFrameNode<XComponentInterface, XComponentAttribute>) {
    super();
    this.mXComponent = xComponent;
  }

  makeNode(uiContext: UIContext): FrameNode | null {
    this.node = new FrameNode(uiContext);
    this.node.appendChild(this.mXComponent);
    return this.node;
  }

  removeNode() {
    this.node?.removeChild(this.mXComponent);
  }
}

@Entry
@Component
export struct PiPContent {
  private xComponentController: XComponentController = new XComponentController();
  private nodeController: NodeController | null = null;
  private mXCNodeController: XCNodeController | null = null;
  private useNode: boolean = false;
  private xComponent: TypedFrameNode<XComponentInterface, XComponentAttribute> | null = null;
  xComponentId: string = 'pipContent';
  xComponentType: string = 'surface';

  aboutToAppear(): void {
    this.nodeController = pip.getCustomUIController();
    this.xComponent = pip.getTypeNode();
    if (this.xComponent === null || this.xComponent === undefined) {
      return;
    }
    let type: string = this.xComponent.getNodeType();
    if (type !== 'XComponent') {
      console.error(TAG, `xComponent type mismatch: ${type}`);
      return;
    }
    this.useNode = true;
    pip.setTypeNodeEnabled();
    this.mXCNodeController = new XCNodeController(this.xComponent);
    console.info(TAG, 'use Node Controller');

    pip.on('stateChange', (state: number) => {
      console.info(TAG, `stateChange state:${state}`);
      if (state === ABOUT_TO_STOP) {
        this.mXCNodeController?.removeNode();
      }
    })
  }

  aboutToDisappear(): void {
    pip.off('stateChange');
  }

  build() {
    Stack() {
      if (this.useNode) {
        this.buildNode();
      } else {
       this.buildXComponent();
      }
      if (this.nodeController !== null) {
        this.buildCustomUI();
      }
    }
    .size({ width: '100%', height: '100%' });
  }

  @Builder
  buildCustomUI() {
    NodeContainer(this.nodeController)
      .size({ width: '100%', height: '100%' });
  }

  @Builder
  buildXComponent() {
    XComponent({ id: this.xComponentId, type: this.xComponentType, controller: this.xComponentController })
      .onLoad(() => {
        pip.initXComponentController(this.xComponentController);
        console.debug(TAG, 'XComponent onLoad done');
      })
      .size({ width: '100%', height: '100%' })
      .backgroundColor(Color.Transparent);
  }

  @Builder
  buildNode() {
    NodeContainer(this.mXCNodeController)
      .size({ width: '100%', height: '100%' })
  }
}