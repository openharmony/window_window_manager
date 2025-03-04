import uiExtensionHost from '@ohos.uiExtensionHost';
import window from '@ohos.window';

function main() {
    loadLibrary('embeddablewindowstageani_kit.z')
    let win = uiExtensionHost.createExtentionWindow(0, 0);
    console.println('test <set> uiExtensionHostWindowProxyRect');

    win.properties.uiExtensionHostWindowProxyRect.left = 99;
    console.println('test <get> uiExtensionHostWindowProxyRect' + win.properties);

    console.println('test getWindowAvoidArea CUT ' + win.getWindowAvoidArea(window.AvoidAreaType.TYPE_CUTOUT).visible);
    console.println('test getWindowAvoidArea SYS ' + win.getWindowAvoidArea(window.AvoidAreaType.TYPE_SYSTEM).visible);
}
