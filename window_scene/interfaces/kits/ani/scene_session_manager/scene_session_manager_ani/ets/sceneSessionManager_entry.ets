import sceneSessionManager from './@ohos.sceneSessionManager.ets';
 
function main() {
    loadLibrary('scenesessionmanagerani_kit.z');
    console.println('[ANI] getRootSceneUIContext start');
    sceneSessionManager.getRootSceneUIContext();
    console.println('[ANI] getRootSceneUIContext ok');
}