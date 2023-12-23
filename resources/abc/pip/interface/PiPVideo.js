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

const curves = requireNativeModule('ohos.curves');
const PiPWindow = requireNapi('PiPWindow');
const pip = requireNapi('pip');

const TAG = "PiPVideo";
const TIMEOUT = 3e3;

export class PiPVideo extends ViewPU {
    constructor(e, t, o, i = -1, n = void 0) {
        super(e, o, i);
        "function" == typeof n && (this.paramsGenerator_ = n);
        this.xComponentId = "pip";
        this.windowType = PiPWindow.PiPTemplateType.VIDEO_PLAY;
        this.hideEventId = -1;
        this.__showControl = new ObservedPropertySimplePU(!1, this, "showControl");
        this.xComponentController = new XComponentController;
        this.surfaceId = "";
        this.controlTransEffect = TransitionEffect.OPACITY.animation({ curve: curves.responsiveSpringMotion(.25, 1) });
        this.__hideControlNow = new ObservedPropertySimplePU(!1, this, "hideControlNow");
        this.addProvidedVar("hideControlNow", this.__hideControlNow);
        this.__hideControlDelay = new ObservedPropertySimplePU(!1, this, "hideControlDelay");
        this.addProvidedVar("hideControlDelay", this.__hideControlDelay);
        this.setInitiallyProvidedValue(t);
        this.declareWatch("hideControlNow", this.onHideControlNow);
        this.declareWatch("hideControlDelay", this.onHideControlDelay)
    }

    setInitiallyProvidedValue(e) {
        void 0 !== e.xComponentId && (this.xComponentId = e.xComponentId);
        void 0 !== e.windowType && (this.windowType = e.windowType);
        void 0 !== e.hideEventId && (this.hideEventId = e.hideEventId);
        void 0 !== e.showControl && (this.showControl = e.showControl);
        void 0 !== e.xComponentController && (this.xComponentController = e.xComponentController);
        void 0 !== e.surfaceId && (this.surfaceId = e.surfaceId);
        void 0 !== e.controlTransEffect && (this.controlTransEffect = e.controlTransEffect);
        void 0 !== e.hideControlNow && (this.hideControlNow = e.hideControlNow);
        void 0 !== e.hideControlDelay && (this.hideControlDelay = e.hideControlDelay)
    }

    updateStateVars(e) {
    }

    purgeVariableDependenciesOnElmtId(e) {
        this.__showControl.purgeDependencyOnElmtId(e);
        this.__hideControlNow.purgeDependencyOnElmtId(e);
        this.__hideControlDelay.purgeDependencyOnElmtId(e)
    }

    aboutToBeDeleted() {
        this.__showControl.aboutToBeDeleted();
        this.__hideControlNow.aboutToBeDeleted();
        this.__hideControlDelay.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal()
    }

    get showControl() {
        return this.__showControl.get()
    }

    set showControl(e) {
        this.__showControl.set(e)
    }

    get hideControlNow() {
        return this.__hideControlNow.get()
    }

    set hideControlNow(e) {
        this.__hideControlNow.set(e)
    }

    get hideControlDelay() {
        return this.__hideControlDelay.get()
    }

    set hideControlDelay(e) {
        this.__hideControlDelay.set(e)
    }

    onHideControlNow() {
        this.hideControlNow && this.switchToHide();
        this.hideControlNow = !1
    }

    onHideControlDelay() {
        this.hideControlDelay && this.delayHide();
        this.hideControlDelay = !1
    }

    switchToShow() {
        this.showControl = !0;
        this.delayHide()
    }

    switchToHide() {
        -1 !== this.hideEventId && clearTimeout(this.hideEventId);
        this.showControl = !1
    }

    delayHide() {
        -1 !== this.hideEventId && clearTimeout(this.hideEventId);
        this.hideEventId = this.showControl ? setTimeout((() => {
            this.showControl = !1
        }), 3e3) : -1
    }

    initialRender() {
        this.observeComponentCreation2(((e, t) => {
            Stack.create();
            Stack.size({ width: "100%", height: "100%" })
        }), Stack);
        this.observeComponentCreation2(((e, t) => {
            XComponent.create({
                id: this.xComponentId,
                type: "surface",
                controller: this.xComponentController
            }, "pipXComponent");
            XComponent.onLoad((() => {
                pip.initXComponentController(this.xComponentController);
                console.debug(TAG, "XComponent onLoad done")
            }));
            XComponent.size({ width: "100%", height: "100%" })
        }), XComponent);
        this.observeComponentCreation2(((e, t) => {
            RelativeContainer.create();
            RelativeContainer.size({ width: "100%", height: "100%" });
            RelativeContainer.id("control");
            Gesture.create(GesturePriority.Low);
            GestureGroup.create(GestureMode.Exclusive);
            TapGesture.create({ count: 2 });
            TapGesture.onAction((e => {
                this.switchToHide();
                pip.processScale()
            }));
            TapGesture.pop();
            TapGesture.create({ count: 1 });
            TapGesture.onAction((e => {
                this.showControl ? this.switchToHide() : this.switchToShow()
            }));
            TapGesture.pop();
            PanGesture.create();
            PanGesture.onActionStart((e => {
                this.switchToHide();
                pip.startMove()
            }));
            PanGesture.pop();
            GestureGroup.pop();
            Gesture.pop()
        }), RelativeContainer);
        this.observeComponentCreation2(((e, t) => {
            RelativeContainer.create();
            RelativeContainer.size({ width: "100%", height: "100%" });
            RelativeContainer.visibility(this.showControl ? Visibility.Visible : Visibility.None);
            RelativeContainer.transition(this.controlTransEffect);
            RelativeContainer.alignRules({
                top: { anchor: "__container__", align: VerticalAlign.Top },
                right: { anchor: "__container__", align: HorizontalAlign.End }
            });
            RelativeContainer.id("control_inner")
        }), RelativeContainer);
        this.observeComponentCreation2(((e, t) => {
            if (t) {
                let t = () => ({});
                ViewPU.create(new DefaultControl(this, {}, void 0, e, t))
            } else this.updateStateVarsOfChildByElmtId(e, {})
        }), null);
        this.observeComponentCreation2(((e, t) => {
            if (t) {
                let t = () => ({});
                ViewPU.create(new VideoControl(this, {}, void 0, e, t))
            } else this.updateStateVarsOfChildByElmtId(e, {})
        }), null);
        RelativeContainer.pop();
        RelativeContainer.pop();
        Stack.pop()
    }

    rerender() {
        this.updateDirtyElements()
    }
}

class DefaultControl extends ViewPU {
    constructor(e, t, o, i = -1, n = void 0) {
        super(e, o, i);
        "function" == typeof n && (this.paramsGenerator_ = n);
        this.__hideControlNow = this.initializeConsume("hideControlNow", "hideControlNow");
        this.setInitiallyProvidedValue(t)
    }

    setInitiallyProvidedValue(e) {
    }

    updateStateVars(e) {
    }

    purgeVariableDependenciesOnElmtId(e) {
        this.__hideControlNow.purgeDependencyOnElmtId(e)
    }

    aboutToBeDeleted() {
        this.__hideControlNow.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal()
    }

    get hideControlNow() {
        return this.__hideControlNow.get()
    }

    set hideControlNow(e) {
        this.__hideControlNow.set(e)
    }

    initialRender() {
        this.observeComponentCreation2(((e, t) => {
            RelativeContainer.create();
            RelativeContainer.width("100%");
            RelativeContainer.height(48);
            RelativeContainer.linearGradient({ angle: 180, colors: [["#30000000", 0], ["#00000000", 1]] });
            RelativeContainer.alignRules({
                top: { anchor: "__container__", align: VerticalAlign.Top },
                left: { anchor: "__container__", align: HorizontalAlign.Start }
            });
            RelativeContainer.id("default_control")
        }), RelativeContainer);
        this.observeComponentCreation2(((e, t) => {
            Button.createWithChild({ type: ButtonType.Circle });
            Button.backgroundColor("#00FFFFFF");
            Button.size({ width: 24, height: 24 });
            Button.margin(12);
            Button.alignRules({
                center: { anchor: "__container__", align: VerticalAlign.Center },
                left: { anchor: "__container__", align: HorizontalAlign.Start }
            });
            Button.id("control_exit");
            Button.responseRegion({ x: "-50%", y: "-50%", width: "200%", height: "200%" });
            Button.onClick((() => {
                this.hideControlNow = !0;
                pip.close();
                console.debug(TAG, "action: exit")
            }))
        }), Button);
        this.observeComponentCreation2(((e, t) => {
            Image.create({
                id: -1,
                type: 2e4,
                params: ["sys.media.ohos_ic_public_close"],
                bundleName: "",
                moduleName: ""
            });
            Image.size({ width: 24, height: 24 });
            Image.fillColor({
                id: -1,
                type: 10001,
                params: ["sys.color.ohos_id_color_primary_contrary"],
                bundleName: "",
                moduleName: ""
            });
            Image.objectFit(ImageFit.Contain)
        }), Image);
        Button.pop();
        this.observeComponentCreation2(((e, t) => {
            Button.createWithChild({ type: ButtonType.Circle });
            Button.backgroundColor("#00FFFFFF");
            Button.size({ width: 24, height: 24 });
            Button.margin(12);
            Button.alignRules({
                center: { anchor: "__container__", align: VerticalAlign.Center },
                right: { anchor: "__container__", align: HorizontalAlign.End }
            });
            Button.id("control_restore");
            Button.responseRegion({ x: "-50%", y: "-50%", width: "200%", height: "200%" });
            Button.onClick((() => {
                this.hideControlNow = !0;
                pip.restore();
                console.debug(TAG, "action: restore")
            }))
        }), Button);
        this.observeComponentCreation2(((e, t) => {
            Image.create({
                id: -1,
                type: 2e4,
                params: ["sys.media.ohos_ic_public_restore"],
                bundleName: "",
                moduleName: ""
            });
            Image.fillColor({
                id: -1,
                type: 10001,
                params: ["sys.color.ohos_id_color_primary_contrary"],
                bundleName: "",
                moduleName: ""
            });
            Image.objectFit(ImageFit.Contain)
        }), Image);
        Button.pop();
        RelativeContainer.pop()
    }

    rerender() {
        this.updateDirtyElements()
    }
}

class VideoControl extends ViewPU {
    constructor(e, t, o, i = -1, n = void 0) {
        super(e, o, i);
        "function" == typeof n && (this.paramsGenerator_ = n);
        this.__isPlaying = new ObservedPropertySimplePU(!0, this, "isPlaying");
        this.__shouldShowNextAndPrev = new ObservedPropertySimplePU(!0, this, "shouldShowNextAndPrev");
        this.__horMargin = new ObservedPropertySimplePU(12, this, "horMargin");
        this.__hideControlDelay = this.initializeConsume("hideControlDelay", "hideControlDelay");
        this.setInitiallyProvidedValue(t)
    }

    setInitiallyProvidedValue(e) {
        void 0 !== e.isPlaying && (this.isPlaying = e.isPlaying);
        void 0 !== e.shouldShowNextAndPrev && (this.shouldShowNextAndPrev = e.shouldShowNextAndPrev);
        void 0 !== e.horMargin && (this.horMargin = e.horMargin)
    }

    updateStateVars(e) {
    }

    purgeVariableDependenciesOnElmtId(e) {
        this.__isPlaying.purgeDependencyOnElmtId(e);
        this.__shouldShowNextAndPrev.purgeDependencyOnElmtId(e);
        this.__horMargin.purgeDependencyOnElmtId(e);
        this.__hideControlDelay.purgeDependencyOnElmtId(e)
    }

    aboutToBeDeleted() {
        this.__isPlaying.aboutToBeDeleted();
        this.__shouldShowNextAndPrev.aboutToBeDeleted();
        this.__horMargin.aboutToBeDeleted();
        this.__hideControlDelay.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal()
    }

    get isPlaying() {
        return this.__isPlaying.get()
    }

    set isPlaying(e) {
        this.__isPlaying.set(e)
    }

    get shouldShowNextAndPrev() {
        return this.__shouldShowNextAndPrev.get()
    }

    set shouldShowNextAndPrev(e) {
        this.__shouldShowNextAndPrev.set(e)
    }

    get horMargin() {
        return this.__horMargin.get()
    }

    set horMargin(e) {
        this.__horMargin.set(e)
    }

    get hideControlDelay() {
        return this.__hideControlDelay.get()
    }

    set hideControlDelay(e) {
        this.__hideControlDelay.set(e)
    }

    initialRender() {
        this.observeComponentCreation2(((e, t) => {
            RelativeContainer.create();
            RelativeContainer.width("100%");
            RelativeContainer.height(48);
            RelativeContainer.linearGradient({ angle: 0, colors: [["#30000000", 0], ["#00000000", 1]] });
            RelativeContainer.onAreaChange(((e, t) => {
                if (t.width < 104) this.shouldShowNextAndPrev = !1; else if (t.width < 152) {
                    this.horMargin = (t.width - 104) / 4;
                    this.shouldShowNextAndPrev = !0
                } else {
                    this.horMargin = 12;
                    this.shouldShowNextAndPrev = !0
                }
            }));
            RelativeContainer.alignRules({
                bottom: { anchor: "__container__", align: VerticalAlign.Bottom },
                left: { anchor: "__container__", align: HorizontalAlign.Start }
            });
            RelativeContainer.id("video_control")
        }), RelativeContainer);
        this.observeComponentCreation2(((e, t) => {
            Button.createWithChild({ type: ButtonType.Circle });
            Button.backgroundColor("#00FFFFFF");
            Button.size({ width: 24, height: 24 });
            Button.margin({ left: this.horMargin, right: this.horMargin, top: 12, bottom: 12 });
            Button.alignRules({
                center: { anchor: "__container__", align: VerticalAlign.Center },
                middle: { anchor: "__container__", align: HorizontalAlign.Center }
            });
            Button.id("control_play");
            Button.responseRegion({ x: "-50%", y: "-50%", width: "200%", height: "200%" });
            Button.onClick((() => {
                this.isPlaying = !this.isPlaying;
                this.hideControlDelay = !0;
                pip.triggerAction("playbackStateChanged");
                console.debug(TAG, "action: play or pause")
            }))
        }), Button);
        this.observeComponentCreation2(((e, t) => {
            Image.create(this.isPlaying ? {
                id: -1,
                type: 2e4,
                params: ["sys.media.ohos_ic_public_pause"],
                bundleName: "",
                moduleName: ""
            } : {
                id: -1,
                type: 2e4,
                params: ["sys.media.ohos_ic_public_play"],
                bundleName: "",
                moduleName: ""
            });
            Image.fillColor({
                id: -1,
                type: 10001,
                params: ["sys.color.ohos_id_color_primary_contrary"],
                bundleName: "",
                moduleName: ""
            });
            Image.objectFit(ImageFit.Contain)
        }), Image);
        Button.pop();
        this.observeComponentCreation2(((e, t) => {
            Button.createWithChild({ type: ButtonType.Circle });
            Button.backgroundColor("#00FFFFFF");
            Button.size({ width: 24, height: 24 });
            Button.margin({ left: this.horMargin, right: this.horMargin, top: 12, bottom: 12 });
            Button.visibility(this.shouldShowNextAndPrev ? Visibility.Visible : Visibility.None);
            Button.alignRules({
                center: { anchor: "__container__", align: VerticalAlign.Center },
                right: { anchor: "control_play", align: HorizontalAlign.Start }
            });
            Button.id("control_play_last");
            Button.responseRegion({ x: "-50%", y: "-50%", width: "200%", height: "200%" });
            Button.onClick((() => {
                this.hideControlDelay = !0;
                pip.triggerAction("previousVideo");
                console.debug(TAG, "action: play last")
            }))
        }), Button);
        this.observeComponentCreation2(((e, t) => {
            Image.create({
                id: -1,
                type: 2e4,
                params: ["sys.media.ohos_ic_public_play_last"],
                bundleName: "",
                moduleName: ""
            });
            Image.fillColor({
                id: -1,
                type: 10001,
                params: ["sys.color.ohos_id_color_primary_contrary"],
                bundleName: "",
                moduleName: ""
            });
            Image.objectFit(ImageFit.Contain)
        }), Image);
        Button.pop();
        this.observeComponentCreation2(((e, t) => {
            Button.createWithChild({ type: ButtonType.Circle });
            Button.backgroundColor("#00FFFFFF");
            Button.size({ width: 24, height: 24 });
            Button.margin({ left: this.horMargin, right: this.horMargin, top: 12, bottom: 12 });
            Button.visibility(this.shouldShowNextAndPrev ? Visibility.Visible : Visibility.None);
            Button.alignRules({
                center: { anchor: "__container__", align: VerticalAlign.Center },
                left: { anchor: "control_play", align: HorizontalAlign.End }
            });
            Button.id("control_play_next");
            Button.responseRegion({ x: "-50%", y: "-50%", width: "200%", height: "200%" });
            Button.onClick((() => {
                this.hideControlDelay = !0;
                pip.triggerAction("nextVideo");
                console.debug(TAG, "action: play next")
            }))
        }), Button);
        this.observeComponentCreation2(((e, t) => {
            Image.create({
                id: -1,
                type: 2e4,
                params: ["sys.media.ohos_ic_public_play_next"],
                bundleName: "",
                moduleName: ""
            });
            Image.fillColor({
                id: -1,
                type: 10001,
                params: ["sys.color.ohos_id_color_primary_contrary"],
                bundleName: "",
                moduleName: ""
            });
            Image.objectFit(ImageFit.Contain)
        }), Image);
        Button.pop();
        RelativeContainer.pop()
    }

    rerender() {
        this.updateDirtyElements()
    }
}

ViewStackProcessor.StartGetAccessRecordingFor(ViewStackProcessor.AllocateNewElmetIdForNextComponent());
loadDocument(new PiPVideo(void 0, {}));
ViewStackProcessor.StopGetAccessRecording();