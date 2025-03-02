import uiExtensionHost from '@ohos.uiExtensionHost';
import window from '@ohos.window';

loadLibrary("embeddablewindowstageani_kit.z")

function main()
{
    let win = uiExtensionHost.createExtentionWindow(0, 0);
    console.println(win.getWindowAvoidArea(window.AvoidAreaType.TYPE_SYSTEM));
}
