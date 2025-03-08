import display from '@ohos.display'

loadLibrary('displayani_kit.z');

function main()
{
    let isFoldable = display.isFoldable();
    console.log("[ANI] isfoldable test, result = " + isFoldable);
    display.getFoldDisplayMode();
    let disret : display.Display = display.getDefaultDisplaySync();
    let cbk : display.Callback = (err: display.BusinessError, i : number) => {
    }
    display.on('add', cbk);
    display.getDisplayByIdSync(0);
    let asyncCbk : display.AsyncCallBackByArray = (err: display.BusinessError, arr : Array<display.Display>) => {
        console.log("AsyncCallBackByArray");
    }
    display.getAllDisplays(asyncCbk);
    display.getCurrentFoldCreaseRegion();
    let dis : display.Display = new display.DisplayImpl();
    let asyncbk : display.AsyncCallback = (err: display.BusinessError, arr : display.CutoutInfo) => {
        console.log("AsyncCallBack");
    }
    dis.getCutoutInfo(asyncbk);
}