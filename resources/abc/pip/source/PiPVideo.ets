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

const TAG: string = 'PiPVideo';
const TIMEOUT: number = 3000;

@Styles
function fillSize() {
  .size({ width: '100%', height: '100%' })
}

@Entry
@Component
export struct PiPVideo {
  xComponentId: string = 'pip';
  windowType: PiPWindow.PiPTemplateType = PiPWindow.PiPTemplateType.VIDEO_PLAY;
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
          VideoControl();
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
        Image($r('sys.media.ohos_ic_public_close'))
          .size({ width: 24, height: 24 })
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


@Component
struct VideoControl {
  @State isPlaying: boolean = true;
  @State shouldShowNextAndPrev: boolean = true;
  @State horMargin: number = 12;
  @Consume hideControlDelay: boolean;

  build() {
    RelativeContainer() {
      Button({ type: ButtonType.Circle }) {
        Image(this.isPlaying ? $r('sys.media.ohos_ic_public_pause') : $r('sys.media.ohos_ic_public_play'))
          .fillColor($r('sys.color.ohos_id_color_primary_contrary'))
          .objectFit(ImageFit.Contain)
      }
      .backgroundColor('#00FFFFFF')
      .size({ width: 24, height: 24 })
      .margin({ left: this.horMargin, right: this.horMargin, top: 12, bottom: 12 })
      .alignRules({
        center: { anchor: '__container__', align: VerticalAlign.Center },
        middle: { anchor: '__container__', align: HorizontalAlign.Center }
      })
      .id('control_play')
      .responseRegion({ x: '-50%', y: '-50%', width: '200%', height: '200%' })
      .onClick(() => {
        this.isPlaying = !this.isPlaying;
        this.hideControlDelay = true;
        pip.triggerAction('playbackStateChanged');
        console.debug(TAG, 'action: play or pause');
      })

      Button({ type: ButtonType.Circle }) {
        Image($r('sys.media.ohos_ic_public_play_last'))
          .fillColor($r('sys.color.ohos_id_color_primary_contrary'))
          .objectFit(ImageFit.Contain)
      }
      .backgroundColor('#00FFFFFF')
      .size({ width: 24, height: 24 })
      .margin({ left: this.horMargin, right: this.horMargin, top: 12, bottom: 12 })
      .visibility(this.shouldShowNextAndPrev ? Visibility.Visible : Visibility.None)
      .alignRules({
        center: { anchor: '__container__', align: VerticalAlign.Center },
        right: { anchor: 'control_play', align: HorizontalAlign.Start }
      })
      .id('control_play_last')
      .responseRegion({ x: '-50%', y: '-50%', width: '200%', height: '200%' })
      .onClick(() => {
        this.hideControlDelay = true;
        pip.triggerAction('previousVideo');
        console.debug(TAG, 'action: play last');
      })

      Button({ type: ButtonType.Circle }) {
        Image($r('sys.media.ohos_ic_public_play_next'))
          .fillColor($r('sys.color.ohos_id_color_primary_contrary'))
          .objectFit(ImageFit.Contain)
      }
      .backgroundColor('#00FFFFFF')
      .size({ width: 24, height: 24 })
      .margin({ left: this.horMargin, right: this.horMargin, top: 12, bottom: 12 })
      .visibility(this.shouldShowNextAndPrev ? Visibility.Visible : Visibility.None)
      .alignRules({
        center: { anchor: '__container__', align: VerticalAlign.Center },
        left: { anchor: 'control_play', align: HorizontalAlign.End }
      })
      .id('control_play_next')
      .responseRegion({ x: '-50%', y: '-50%', width: '200%', height: '200%' })
      .onClick(() => {
        this.hideControlDelay = true;
        pip.triggerAction('nextVideo');
        console.debug(TAG, 'action: play next');
      })
    }
    .width('100%')
    .height(48)
    .linearGradient({
      angle: 0,
      colors: [['#30000000', 0.0], ['#00000000', 1.0]]
    })
    .onAreaChange((oldValue, newValue) => {
      if (newValue.width < 104) {
        this.shouldShowNextAndPrev = false;
      } else if (newValue.width < 152) {
        this.horMargin = (newValue.width as number - 104) / 4;
        this.shouldShowNextAndPrev = true;
      } else {
        this.horMargin = 12;
        this.shouldShowNextAndPrev = true;
      }
    })
    .alignRules({
      bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
      left: { anchor: '__container__', align: HorizontalAlign.Start }
    })
    .id('video_control')
  }
}
