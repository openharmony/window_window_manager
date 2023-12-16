/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import curves from '@ohos.curves';
import PiPWindow from '@ohos.PiPWindow';
import pip from '@ohos.pip';

const TAG: string = 'PiPCall';
const TIMEOUT: number = 3000;

@Styles
function fillSize() {
  .size({ width: '100%', height: '100%' })
}

@Entry
@Component
export struct PiPCall {
  xComponentId: string = 'pip';
  windowType: PiPWindow.PiPTemplateType = PiPWindow.PiPTemplateType.VIDEO_CALL;
  private hideEventId: number = -1;
  @State private showControl: boolean = false;
  private xComponentController: XComponentController = new XComponentController();
  private surfaceId: string = '';
  private controlTransEffect: TransitionEffect = TransitionEffect.OPACITY
    .animation({ curve: curves.responsiveSpringMotion(0.25, 1) });
  @Provide @Watch('onHideControlNow') hideControlNow: boolean = false;
  @Provide @Watch('onHideControlDelay') hideControlDelay: boolean = false;

  onHideControlNow() {
    if (this.hideControlNow) {
      this.switchToHide();
    }
    this.hideControlNow = false;
  }

  onHideControlDelay() {
    if (this.hideControlDelay) {
      this.delayHide();
    }
    this.hideControlDelay = false;
  }

  switchToShow() {
    this.showControl = true;
    this.delayHide();
  }

  switchToHide() {
    if (this.hideEventId !== -1) {
      clearTimeout(this.hideEventId);
    }
    this.showControl = false;
  }

  delayHide() {
    if (this.hideEventId !== -1) {
      clearTimeout(this.hideEventId);
    }
    this.hideEventId = this.showControl ?
    setTimeout(() => {
      this.showControl = false;
    }, TIMEOUT) :
      -1;
  }

  build() {
    Stack() {
      XComponent({ id: this.xComponentId, type: 'surface', controller: this.xComponentController })
        .onLoad(() => {
          pip.initXComponentController(this.xComponentController);
          console.debug(TAG, 'XComponent onLoad done');
        })
        .fillSize();

      RelativeContainer() {
        RelativeContainer() {
          DefaultControl();
          CallControl();
        }
        .fillSize()
        .visibility(this.showControl ? Visibility.Visible : Visibility.None)
        .transition(this.controlTransEffect)
        .alignRules({
          top: { anchor: '__container__', align: VerticalAlign.Top },
          right: { anchor: '__container__', align: HorizontalAlign.End }
        })
        .id('control_inner')
      }
      .fillSize()
      .id('control')
      .gesture(
        GestureGroup(GestureMode.Exclusive,
          TapGesture({ count: 2 })
            .onAction((event: GestureEvent) => {
              this.switchToHide();
              pip.processScale();
            }),
          TapGesture({ count: 1 })
            .onAction((event: GestureEvent) => {
              if (this.showControl) {
                this.switchToHide();
              } else {
                this.switchToShow();
              }
            }),
          PanGesture()
            .onActionStart((event: GestureEvent) => {
              this.switchToHide();
              pip.startMove();
            })
        )
      )
    }
    .fillSize()
  }
}

@Component
struct DefaultControl {
  @Consume hideControlNow: boolean;

  build() {
    RelativeContainer() {
      Button({ type: ButtonType.Circle }) {
        Image($r('sys.media.ohos_ic_public_cancel'))
          .fillColor($r('sys.color.ohos_id_color_primary_contrary'))
          .objectFit(ImageFit.Contain)
      }
      .backgroundColor('#00FFFFFF')
      .size({ width: 24, height: 24 })
      .margin(12)
      .alignRules({
        center: { anchor: '__container__', align: VerticalAlign.Center },
        left: { anchor: '__container__', align: HorizontalAlign.Start }
      })
      .id('control_exit')
      .responseRegion({ x: '-50%', y: '-50%', width: '200%', height: '200%' })
      .onClick(() => {
        this.hideControlNow = true;
        pip.close();
        console.debug(TAG, 'action: exit');
      })

      Button({ type: ButtonType.Circle }) {
        Image($r('sys.media.ohos_ic_public_restore'))
          .fillColor($r('sys.color.ohos_id_color_primary_contrary'))
          .objectFit(ImageFit.Contain)
      }
      .backgroundColor('#00FFFFFF')
      .size({ width: 24, height: 24 })
      .margin(12)
      .alignRules({
        center: { anchor: '__container__', align: VerticalAlign.Center },
        right: { anchor: '__container__', align: HorizontalAlign.End }
      })
      .id('control_restore')
      .responseRegion({ x: '-50%', y: '-50%', width: '200%', height: '200%' })
      .onClick(() => {
        this.hideControlNow = true;
        pip.restore();
        console.debug(TAG, 'action: restore');
      })
    }
    .width('100%')
    .height(48)
    .linearGradient({
      angle: 180,
      colors: [['#30000000', 0.0], ['#00000000', 1.0]]
    })
    .alignRules({
      top: { anchor: '__container__', align: VerticalAlign.Top },
      left: { anchor: '__container__', align: HorizontalAlign.Start }
    })
    .id('default_control')
  }
}

const sizeArray = [1.0, 1.5];

@Component
struct CallControl {
  @State isMute: boolean = true;
  @State isRecord: boolean = true;
  @State defaultMargin: number = 8;
  @State defaultSize: number = 12;
  @State defaultBigSize: number = 24;
  @State sizeIndex: number = 0;
  @Consume hideControlDelay: boolean;

  build() {
    RelativeContainer() {
      Button({ type: ButtonType.Circle }) {
        Image($r('sys.media.ohos_ic_public_hang_up'))
          .size({ width: 12 * sizeArray[this.sizeIndex], height: 12 * sizeArray[this.sizeIndex] })
          .fillColor($r('sys.color.ohos_id_color_primary_contrary'))
          .objectFit(ImageFit.Contain)
      }
      .backgroundColor($r('sys.color.ohos_id_color_handup'))
      .size({ width: 24 * sizeArray[this.sizeIndex], height: 24 * sizeArray[this.sizeIndex] })
      .margin({ left: 8 * sizeArray[this.sizeIndex], right: 8 * sizeArray[this.sizeIndex], top: 12, bottom: 12 })
      .alignRules({
        bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
        middle: { anchor: '__container__', align: HorizontalAlign.Center }
      })
      .id('control_hangup')
      .onClick(() => {
        this.hideControlDelay = true;
        pip.triggerAction('hangUp');
        console.debug(TAG, 'action: hangup');
      })

      Button({ type: ButtonType.Circle }) {
        Image(this.isMute ? $r('sys.media.ohos_ic_public_voice') : $r('sys.media.ohos_ic_public_voice_off'))
          .size({ width: 8 * sizeArray[this.sizeIndex], height: 8 * sizeArray[this.sizeIndex] })
          .fillColor($r('sys.color.ohos_id_color_primary'))
          .objectFit(ImageFit.Contain)
      }
      .backgroundColor($r('sys.color.ohos_id_color_floating_button_icon'))
      .size({ width: 16 * sizeArray[this.sizeIndex], height: 16 * sizeArray[this.sizeIndex] })
      .alignRules({
        center: { anchor: 'control_hangup', align: VerticalAlign.Center },
        right: { anchor: 'control_hangup', align: HorizontalAlign.Start }
      })
      .id('control_mute')
      .onClick(() => {
        this.hideControlDelay = true;
        this.isMute = !this.isMute;
        pip.triggerAction('micStateChanged');
        console.debug(TAG, 'action: mic enable or disable');
      })

      Button({ type: ButtonType.Circle }) {
        Image(this.isRecord ? $r('sys.media.ohos_ic_public_video') : $r('sys.media.ohos_ic_public_video_off'))
          .size({ width: 8 * sizeArray[this.sizeIndex], height: 8 * sizeArray[this.sizeIndex] })
          .fillColor($r('sys.color.ohos_id_color_primary'))
          .objectFit(ImageFit.Contain)
      }
      .backgroundColor($r('sys.color.ohos_id_color_floating_button_icon'))
      .size({ width: 16 * sizeArray[this.sizeIndex], height: 16 * sizeArray[this.sizeIndex] })
      .alignRules({
        center: { anchor: 'control_hangup', align: VerticalAlign.Center },
        left: { anchor: 'control_hangup', align: HorizontalAlign.End }
      })
      .id('control_record')
      .onClick(() => {
        this.hideControlDelay = true;
        this.isRecord = !this.isRecord;
        pip.triggerAction('videoStateChanged');
        console.debug(TAG, 'action: video enable or disable');
      })
    }
    .width('100%')
    .height(48)
    .onAreaChange((oldValue: Area, newValue: Area) => {
      if (oldValue.width == newValue.width) {
        return;
      }
      this.sizeIndex = newValue.width >= 150 ? 1 : 0;
    })
    .alignRules({
      bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
      left: { anchor: '__container__', align: HorizontalAlign.Start }
    })
    .id('call_control')
  }
}
