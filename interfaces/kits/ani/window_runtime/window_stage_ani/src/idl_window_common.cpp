#include "idl_window_common.h"

OH_WINDOW_Rect IdlWindowCommon::ConvertToIdlWindowRect(OHOS::Rosen::Rect rect) {
    OH_WINDOW_Rect idlRect;
    idlRect.left.i32 = rect.posX_;
    idlRect.top.i32 = rect.posY_;
    idlRect.width.i32 = rect.width_;
    idlRect.height.i32 = rect.height_;
    return idlRect;
}