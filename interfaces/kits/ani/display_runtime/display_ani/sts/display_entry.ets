import display from '@ohos.display'

loadLibrary('displayani_kit.z');

function main() {
    let testout = display.isFoldable();
    console.log(testout);
}