import display from '@ohos.display'

loadLibrary('displayani_kit.z');

function main() {
    let isFoldable = display.isFoldable();
    console.log('[ANI] isfoldable test, result = ' + isFoldable);
    display.getFoldDisplayMode();
    let disret : display.Display = display.getDefaultDisplaySync();
    let cbk = (i : number): void => {
        console.log('[ANI] isfoldable test, result = ' + i);
    }
    display.on('add', cbk);
    display.getDisplayByIdSync(0);
    let asyncCbk = (err: display.BusinessError, arr : Array<display.Display>):void => {
        console.log('AsyncCallBack');
    }
    display.getAllDisplays(asyncCbk);
    display.getCurrentFoldCreaseRegion();
    let dis : display.Display = new display.DisplayImpl();
    let asyncbk = (err: display.BusinessError, arr : display.CutoutInfo):void => {
        console.log('AsyncCallBack');
    }
    dis.getCutoutInfo(asyncbk);
}