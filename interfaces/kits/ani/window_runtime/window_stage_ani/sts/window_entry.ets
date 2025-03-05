import window from '@ohos.window';
import {
  AsyncCallback,
  BaseContext,
  LocalStorage,
  Callback,
  BusinessError,
  UIContext,
} from '@ohos.window';

function main() {
    loadLibrary('windowstageani_kit.z')
    let ws = window.CreateWindowStage(0);
    let callback:AsyncCallback<int> = (err, data: int) => {
      if (err.code) {
        console.error(`Failed to load the content. Cause code: ${err.code}, message: ${err.message}`);
        return;
      }
      console.info('Succeeded in loading the content.');
    }
    ws.loadContent('ohos.test', callback);

    ws.getMainWindowSync();

    ws.getMainWindow().then((res:window.Window)=>{
      console.println('[ANI]getMainWindow primise');
    }).catch((e:NullishType)=>{
      console.println('[ANI]getMainWindow catch');
    });

    ws.getMainWindow(()=>{
      console.println('[ANI]getMainWindow callback');
    });

    let w = window.CreateWindow(0);

    w.isWindowSupportWideGamut().then((res:boolean)=>{
      console.println('[ANI]isWindowSupportWideGamut primise' + res);
    }).catch((e:NullishType)=>{
      console.println('[ANI]isWindowSupportWideGamut catch');
    });

    w.isWindowSupportWideGamut(()=>{
      console.println('[ANI]wideGamut callback');
    });

    w.setWindowLayoutFullScreen(true).then((res:int)=>{
      console.println('[ANI]setWindowLayoutFullScree primise' + res);
    }).catch((e:NullishType)=>{
      console.println('[ANI]setWindowLayoutFullScreen catch');
    });

    w.setWindowLayoutFullScreen(false, ()=>{
      console.println('[ANI]setWindowLayoutFullScreen callback');
    });

    let systemBarProperties = new window.SystemBarPropertiesInternal();
    w.setSystemBarProperties(systemBarProperties).then((res:int)=>{
      console.println('[ANI]setSystemBarProperties primise' + res);
    }).catch((e:NullishType)=>{
      console.println('[ANI]setSystemBarProperties catch');
    });

    w.setSystemBarProperties(systemBarProperties, ()=>{
      console.println('[ANI]setSystemBarProperties callback');
    });

    w.setSpecificSystemBarEnabled('w', false, false).then((res:int)=>{
      console.println('[ANI]setSpecificSystemBarEnabled primise' + res);
    }).catch((e:NullishType)=>{
      console.println('[ANI]setSpecificSystemBarEnabled catch');
    });


    w.getWindowDecorHeight();
    w.setWindowBackgroundColorSync('0xFFFFFF');
    w.setColorSpace(window.ColorSpace.DEFAULT).then((ret: int) => {
      console.println('[ANI] in ret:' + ret);
    }).catch((e: NullishType) => {
      let err = e as BusinessError;
      console.println('[ANI] in code:' + err?.code + ' err:' + err?.message);
    });
    w.setColorSpace(window.ColorSpace.DEFAULT, (err, data) => {
      console.println('[ANI] setColorSpace data:' + data + ' err:' + err);
    });
    w.setPreferredOrientation(window.Orientation.UNSPECIFIED);
    w.setWindowPrivacyMode(true);
    w.recover();
    w.setUIContent('path');
    w.loadContent('path');
    w.loadContent('path', new LocalStorage());
    w.setWindowKeepScreenOn(true);
    let value: 'status' | 'navigation' = 'status';
    w.setWindowSystemBarEnable(Array.of(value));
    try {
      let ctx = w.getUIContext();
      console.println('[ANI] getUIContext ctx:'+ctx);
    } catch (e: Error) {
      console.println('[ANI] in e:' + e);
    }
    let avoidArea = w.getWindowAvoidArea(window.AvoidAreaType.TYPE_SYSTEM);
    console.println('[ANI] avoidArea:' + JSON.stringify(avoidArea));
    w.setWaterMarkFlag(true);
    let cb = new Callback<window.WindowEventType | window.Size | window.WindowStatusType>();
    w.on('windowEvent', cb);
    w.off('windowEvent', cb);
    w.on('windowSizeChange', cb);
    w.off('windowSizeChange', cb);
    w.on('windowStatusChange', cb);
    w.off('windowStatusChange', cb);

    w.setImmersiveModeEnabledState(false);
    w.setWindowDecorVisible(false);
    w.setWindowDecorHeight(100);
    w.getWindowProperties();
    w.setSystemBarProperties(systemBarProperties);
    w.setSpecificSystemBarEnabled('w', false, false);
    console.println('ok');
}

