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

const TAG = "PiPMeeting";
const TIMEOUT = 3e3;

export class PiPMeeting extends ViewPU {
    constructor(e, t, o, i = -1, n = void 0) {
        super(e, o, i);
        "function" == typeof n && (this.paramsGenerator_ = n);
        this.xComponentId = "pip";
        this.windowType = PiPWindow.PiPTemplateType.VIDEO_MEETING;
        this.hideEventId = -1;
        this.__showControl = new ObservedPropertySimplePU(!1, this, "showControl");
        this.xComponentController = new XComponentController;
        this.surfaceId = "";
        this.controlTransEffect = TransitionEffect.OPACITY;
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
        this.hideControlNow && this.switchToHideWithoutAnime();
        this.hideControlNow = !1
    }

    onHideControlDelay() {
        this.hideControlDelay && this.delayHide();
        this.hideControlDelay = !1
    }

    switchToShow() {
        Context.animateTo({ curve: curves.responsiveSpringMotion(0.25, 1) }, (() => {
            this.showControl = !0
        }));
        this.delayHide()
    }

    switchToHide() {
        -1 !== this.hideEventId && clearTimeout(this.hideEventId);
        Context.animateTo({ curve: curves.responsiveSpringMotion(0.25, 1) }, (() => {
            this.showControl = !1
        }))
    }

    switchToHideWithoutAnime() {
        -1 !== this.hideEventId && clearTimeout(this.hideEventId);
        this.showControl = !1
    }

    delayHide() {
        -1 !== this.hideEventId && clearTimeout(this.hideEventId);
        this.hideEventId = this.showControl ? setTimeout((() => {
            Context.animateTo({ curve: curves.responsiveSpringMotion(0.25, 1) }, (() => {
                this.showControl = !1
            }))
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
                this.switchToHideWithoutAnime();
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
            Stack.create();
            Stack.size({ width: "100%", height: "100%" });
            Stack.id("fill_stack")
        }), Stack);
        Stack.pop();
        this.observeComponentCreation2(((e, t) => {
            If.create();
            this.showControl ? this.ifElseBranchUpdateFunction(0, (() => {
                if (!If.canRetake("control_inner")) {
                    this.observeComponentCreation2(((e, t) => {
                        RelativeContainer.create();
                        RelativeContainer.size({ width: "100%", height: "100%" });
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
                            ViewPU.create(new MeetingControl(this, {}, void 0, e, t))
                        } else this.updateStateVarsOfChildByElmtId(e, {})
                    }), null);
                    RelativeContainer.pop()
                }
            })) : this.ifElseBranchUpdateFunction(1, (() => {
            }))
        }), If);
        If.pop();
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

const sizeArray = [1, 1.5];

class MeetingControl extends ViewPU {
    constructor(e, t, o, i = -1, n = void 0) {
        super(e, o, i);
        "function" == typeof n && (this.paramsGenerator_ = n);
        this.__isMute = new ObservedPropertySimplePU(!0, this, "isMute");
        this.__isRecord = new ObservedPropertySimplePU(!0, this, "isRecord");
        this.__defaultMargin = new ObservedPropertySimplePU(8, this, "defaultMargin");
        this.__defaultSize = new ObservedPropertySimplePU(12, this, "defaultSize");
        this.__defaultBigSize = new ObservedPropertySimplePU(24, this, "defaultBigSize");
        this.__sizeIndex = new ObservedPropertySimplePU(0, this, "sizeIndex");
        this.__hideControlDelay = this.initializeConsume("hideControlDelay", "hideControlDelay");
        this.setInitiallyProvidedValue(t)
    }

    setInitiallyProvidedValue(e) {
        void 0 !== e.isMute && (this.isMute = e.isMute);
        void 0 !== e.isRecord && (this.isRecord = e.isRecord);
        void 0 !== e.defaultMargin && (this.defaultMargin = e.defaultMargin);
        void 0 !== e.defaultSize && (this.defaultSize = e.defaultSize);
        void 0 !== e.defaultBigSize && (this.defaultBigSize = e.defaultBigSize);
        void 0 !== e.sizeIndex && (this.sizeIndex = e.sizeIndex)
    }

    updateStateVars(e) {
    }

    purgeVariableDependenciesOnElmtId(e) {
        this.__isMute.purgeDependencyOnElmtId(e);
        this.__isRecord.purgeDependencyOnElmtId(e);
        this.__defaultMargin.purgeDependencyOnElmtId(e);
        this.__defaultSize.purgeDependencyOnElmtId(e);
        this.__defaultBigSize.purgeDependencyOnElmtId(e);
        this.__sizeIndex.purgeDependencyOnElmtId(e);
        this.__hideControlDelay.purgeDependencyOnElmtId(e)
    }

    aboutToBeDeleted() {
        this.__isMute.aboutToBeDeleted();
        this.__isRecord.aboutToBeDeleted();
        this.__defaultMargin.aboutToBeDeleted();
        this.__defaultSize.aboutToBeDeleted();
        this.__defaultBigSize.aboutToBeDeleted();
        this.__sizeIndex.aboutToBeDeleted();
        this.__hideControlDelay.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal()
    }

    get isMute() {
        return this.__isMute.get()
    }

    set isMute(e) {
        this.__isMute.set(e)
    }

    get isRecord() {
        return this.__isRecord.get()
    }

    set isRecord(e) {
        this.__isRecord.set(e)
    }

    get defaultMargin() {
        return this.__defaultMargin.get()
    }

    set defaultMargin(e) {
        this.__defaultMargin.set(e)
    }

    get defaultSize() {
        return this.__defaultSize.get()
    }

    set defaultSize(e) {
        this.__defaultSize.set(e)
    }

    get defaultBigSize() {
        return this.__defaultBigSize.get()
    }

    set defaultBigSize(e) {
        this.__defaultBigSize.set(e)
    }

    get sizeIndex() {
        return this.__sizeIndex.get()
    }

    set sizeIndex(e) {
        this.__sizeIndex.set(e)
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
            RelativeContainer.onAreaChange(((e, t) => {
                e.width != t.width && (this.sizeIndex = t.width >= 150 ? 1 : 0)
            }));
            RelativeContainer.alignRules({
                bottom: { anchor: "__container__", align: VerticalAlign.Bottom },
                left: { anchor: "__container__", align: HorizontalAlign.Start }
            });
            RelativeContainer.id("meeting_control")
        }), RelativeContainer);
        this.observeComponentCreation2(((e, t) => {
            Button.createWithChild({ type: ButtonType.Circle });
            Button.backgroundColor({
                id: -1,
                type: 10001,
                params: ["sys.color.ohos_id_color_handup"],
                bundleName: "",
                moduleName: ""
            });
            Button.size({ width: 24 * sizeArray[this.sizeIndex], height: 24 * sizeArray[this.sizeIndex] });
            Button.margin({
                top: 12,
                bottom: 12
            });
            Button.alignRules({
                bottom: { anchor: "__container__", align: VerticalAlign.Bottom },
                middle: { anchor: "__container__", align: HorizontalAlign.Center }
            });
            Button.id("control_hangup");
            Button.onClick((() => {
                this.hideControlDelay = !0;
                pip.triggerAction("hangUp");
                console.debug(TAG, "action: hangup")
            }))
        }), Button);
        this.observeComponentCreation2(((e, t) => {
            Image.create({
                id: -1,
                type: 2e4,
                params: ["sys.media.ohos_ic_public_hang_up"],
                bundleName: "",
                moduleName: ""
            });
            Image.size({ width: 12 * sizeArray[this.sizeIndex], height: 12 * sizeArray[this.sizeIndex] });
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
            Button.backgroundColor({
                id: -1,
                type: 10001,
                params: ["sys.color.ohos_id_color_floating_button_icon"],
                bundleName: "",
                moduleName: ""
            });
            Button.size({ width: 16 * sizeArray[this.sizeIndex], height: 16 * sizeArray[this.sizeIndex] });
            Button.margin({
                left: 8 * sizeArray[this.sizeIndex],
                right: 8 * sizeArray[this.sizeIndex]
            });
            Button.alignRules({
                center: { anchor: "control_hangup", align: VerticalAlign.Center },
                right: { anchor: "control_hangup", align: HorizontalAlign.Start }
            });
            Button.id("control_mute");
            Button.onClick((() => {
                this.hideControlDelay = !0;
                this.isMute = !this.isMute;
                pip.triggerAction("voiceStateChanged");
                console.debug(TAG, "action: voice enable or disable")
            }))
        }), Button);
        this.observeComponentCreation2(((e, t) => {
            Image.create(this.isMute ? {
                id: -1,
                type: 2e4,
                params: ["sys.media.ohos_ic_public_sound_off"],
                bundleName: "",
                moduleName: ""
            } : {
                id: -1,
                type: 2e4,
                params: ["sys.media.ohos_ic_public_sound"],
                bundleName: "",
                moduleName: ""
            });
            Image.size({ width: 8 * sizeArray[this.sizeIndex], height: 8 * sizeArray[this.sizeIndex] });
            Image.fillColor({
                id: -1,
                type: 10001,
                params: ["sys.color.ohos_id_color_primary"],
                bundleName: "",
                moduleName: ""
            });
            Image.objectFit(ImageFit.Contain)
        }), Image);
        Button.pop();
        this.observeComponentCreation2(((e, t) => {
            Button.createWithChild({ type: ButtonType.Circle });
            Button.backgroundColor({
                id: -1,
                type: 10001,
                params: ["sys.color.ohos_id_color_floating_button_icon"],
                bundleName: "",
                moduleName: ""
            });
            Button.size({ width: 16 * sizeArray[this.sizeIndex], height: 16 * sizeArray[this.sizeIndex] });
            Button.margin({
                left: 8 * sizeArray[this.sizeIndex],
                right: 8 * sizeArray[this.sizeIndex]
            });
            Button.alignRules({
                center: { anchor: "control_hangup", align: VerticalAlign.Center },
                left: { anchor: "control_hangup", align: HorizontalAlign.End }
            });
            Button.id("control_record");
            Button.onClick((() => {
                this.hideControlDelay = !0;
                this.isRecord = !this.isRecord;
                pip.triggerAction("videoStateChanged");
                console.debug(TAG, "action: video enable or disable")
            }))
        }), Button);
        this.observeComponentCreation2(((e, t) => {
            Image.create(this.isRecord ? {
                id: -1,
                type: 2e4,
                params: ["sys.media.ohos_ic_public_video"],
                bundleName: "",
                moduleName: ""
            } : {
                id: -1,
                type: 2e4,
                params: ["sys.media.ohos_ic_public_video_off"],
                bundleName: "",
                moduleName: ""
            });
            Image.size({ width: 8 * sizeArray[this.sizeIndex], height: 8 * sizeArray[this.sizeIndex] });
            Image.fillColor({
                id: -1,
                type: 10001,
                params: ["sys.color.ohos_id_color_primary"],
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
loadDocument(new PiPMeeting(void 0, {}));
ViewStackProcessor.StopGetAccessRecording();