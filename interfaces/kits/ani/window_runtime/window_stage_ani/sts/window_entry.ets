import window from '@ohos.window'
import { AsyncCallback } from '@ohos.window'

loadLibrary("windowstageani_kit.z")

function main()
{
    let ws = window.CreateWindowStage(0);
    let callback:AsyncCallback = (err) => {
      if (err.code) {
        console.error(`Failed to load the content. Cause code: ${err.code}, message: ${err.message}`);
        return;
      }
      console.info('Succeeded in loading the content.');
    }
    ws.loadContent("ohos.test", callback);

    let w = window.CreateWindow(0);
    w.getWindowDecorHeight();
    w.setWindowBackgroundColorSync("0xFFFFFF");
    console.println("ok");
    //window.setImmersiveModeEnabledState(false);
    //window.setWindowDecorVisible(false);
    //window.setWindowDecorHeight(100);
    //window.getWindowProperties();
}

