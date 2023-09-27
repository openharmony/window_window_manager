/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JS_WINDOW_EXTENSION_H
#define JS_WINDOW_EXTENSION_H

#include <js_runtime.h>
#include <js_runtime_utils.h>

#include "js_window_extension_context.h"
#include "window.h"
#include "window_extension.h"
#include "window_extension_stub.h"
#include "window_extension_stub_impl.h"

namespace OHOS {
namespace Rosen {
class JsWindowExtension : public WindowExtension {
public:
    explicit JsWindowExtension(AbilityRuntime::JsRuntime& jsRuntime);
    virtual ~JsWindowExtension() override;

    /**
     * @brief Create JsAccessibilityExtension.
     *
     * @param runtime The runtime.
     * @return The JsAccessibilityExtension instance.
     */
    static JsWindowExtension* Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime);

    /**
     * @brief Init the extension.
     *
     * @param record the extension record.
     * @param application the application info.
     * @param handler the extension handler.
     * @param token the remote token.
     */
    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord>& record,
        const std::shared_ptr<AppExecFwk::OHOSApplication>& application,
        std::shared_ptr<AppExecFwk::AbilityHandler>& handler,
        const sptr<IRemoteObject>& token) override;

    /**
     * @brief Called when this Accessibility extension is connected for the first time.
     *
     * You can override this function to implement your own processing logic.
     *
     * @param want Indicates the {@link Want} structure containing connection information
     * about the Accessibility extension.
     * @return Returns a pointer to the <b>sid</b> of the connected Accessibility extension.
     */
    sptr<IRemoteObject> OnConnect(const AAFwk::Want& want) override;

    /**
     * @brief Called when all abilities connected to this Service extension are disconnected.
     *
     * You can override this function to implement your own processing logic.
     *
     */
    void OnDisconnect(const AAFwk::Want& want) override;

    /**
     * @brief Called when this extension is started. You must override this function if you want to perform some
     *        initialization operations during extension startup.
     *
     * This function can be called only once in the entire lifecycle of an extension.
     * @param Want Indicates the {@link Want} structure containing startup information about the extension.
     * @param sessionInfo Indicates the {@link SessionInfo} structure containing session info about the extension.
     */
    virtual void OnStart(const AAFwk::Want& want, sptr<AAFwk::SessionInfo> sessionInfo) override;
private:
    void GetSrcPath(std::string& srcPath) const;
    napi_value CallJsMethod(const char* name, napi_value const* argv = nullptr, size_t argc = 0) const;
    void OnWindowCreated() const;
    void BindContext(napi_env env, napi_value obj);

    AbilityRuntime::JsRuntime& jsRuntime_;
    std::unique_ptr<NativeReference> jsObj_;
    sptr<WindowExtensionStubImpl> stub_;
    static int extensionCnt_;
    sptr<AAFwk::SessionInfo> sessionInfo_;
};
} // namespace Rosen
} // namespace OHOS
#endif // JS_WINDOW_EXTENSION_H
