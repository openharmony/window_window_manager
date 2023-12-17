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

const TAG = "PiPLive";
const TIMEOUT = 3e3;

export class PiPLive extends ViewPU {
    constructor(e, t, o, i = -1, n = void 0) {
        super(e, o, i);
        "function" == typeof n && (this.paramsGenerator_ = n);
        this.xComponentId = "pip";
        this.windowType = PiPWindow.PiPTemplateType.VIDEO_LIVE;
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
                ViewPU.create(new LiveControl(this, {}, void 0, e, t))
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

class LiveControl extends ViewPU {
    constructor(e, t, o, i = -1, n = void 0) {
        super(e, o, i);
        "function" == typeof n && (this.paramsGenerator_ = n);
        this.__isPlaying = new ObservedPropertySimplePU(!0, this, "isPlaying");
        this.__hideControlDelay = this.initializeConsume("hideControlDelay", "hideControlDelay");
        this.setInitiallyProvidedValue(t)
    }

    setInitiallyProvidedValue(e) {
        void 0 !== e.isPlaying && (this.isPlaying = e.isPlaying)
    }

    updateStateVars(e) {
    }

    purgeVariableDependenciesOnElmtId(e) {
        this.__isPlaying.purgeDependencyOnElmtId(e);
        this.__hideControlDelay.purgeDependencyOnElmtId(e)
    }

    aboutToBeDeleted() {
        this.__isPlaying.aboutToBeDeleted();
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
            RelativeContainer.height(56);
            RelativeContainer.linearGradient({ angle: 0, colors: [["#30000000", 0], ["#00000000", 1]] });
            RelativeContainer.alignRules({
                bottom: { anchor: "__container__", align: VerticalAlign.Bottom },
                left: { anchor: "__container__", align: HorizontalAlign.Start }
            });
            RelativeContainer.id("live_control")
        }), RelativeContainer);
        this.observeComponentCreation2(((e, t) => {
            Button.createWithChild({ type: ButtonType.Circle });
            Button.backgroundColor("#00FFFFFF");
            Button.size({ width: 24, height: 24 });
            Button.margin(12);
            Button.alignRules({
                top: { anchor: "__container__", align: VerticalAlign.Top },
                middle: { anchor: "__container__", align: HorizontalAlign.Center }
            });
            Button.id("control_play");
            Button.responseRegion({ x: "-50%", y: "-50%", width: "200%", height: "200%" });
            Button.onClick((() => {
                this.isPlaying = !this.isPlaying;
                this.hideControlDelay = !0;
                pip.triggerAction("playbackStateChanged");
                console.debug(TAG, "action: live play")
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
            Progress.create({ value: 0, total: 100, type: ProgressType.Linear });
            Progress.size({ width: "100%", height: 4 });
            Progress.color("#33EEEEEE");
            Progress.margin({ left: 16, right: 16, top: -4, bottom: 8 });
            Progress.alignRules({
                bottom: { anchor: "__container__", align: VerticalAlign.Bottom },
                middle: { anchor: "__container__", align: HorizontalAlign.Center }
            });
            Progress.id("control_progress")
        }), Progress);
        RelativeContainer.pop()
    }

    rerender() {
        this.updateDirtyElements()
    }
}

ViewStackProcessor.StartGetAccessRecordingFor(ViewStackProcessor.AllocateNewElmetIdForNextComponent());
loadDocument(new PiPLive(void 0, {}));
ViewStackProcessor.StopGetAccessRecording();