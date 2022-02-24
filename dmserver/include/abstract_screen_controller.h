/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DMSERVER_ABSTRACT_SCREEN_CONTROLLER_H
#define FOUNDATION_DMSERVER_ABSTRACT_SCREEN_CONTROLLER_H

#include <map>
#include <vector>

#include <refbase.h>
#include <surface.h>
#include <transaction/rs_interfaces.h>

#include "abstract_screen.h"
#include "dm_common.h"
#include "screen.h"

namespace OHOS::Rosen {
class AbstractScreenController : public RefBase {
using OnAbstractScreenConnectCb = std::function<void(sptr<AbstractScreen>)>;
using OnAbstractScreenChangeCb = std::function<void(sptr<AbstractScreen>, DisplayChangeEvent event)>;
public:
    struct AbstractScreenCallback : public RefBase {
        OnAbstractScreenConnectCb onConnect_;
        OnAbstractScreenConnectCb onDisconnect_;
        OnAbstractScreenChangeCb onChange_;
    };

    AbstractScreenController(std::recursive_mutex& mutex);
    ~AbstractScreenController();

    void Init();
    void ScreenConnectionInDisplayInit(sptr<AbstractScreenCallback> abstractScreenCallback);
    std::vector<ScreenId> GetAllScreenIds();
    sptr<AbstractScreen> GetAbstractScreen(ScreenId dmsScreenId) const;
    std::vector<ScreenId> GetShotScreenIds(std::vector<ScreenId>) const;
    std::vector<ScreenId> GetAllExpandOrMirrorScreenIds(std::vector<ScreenId>) const;
    sptr<AbstractScreenGroup> GetAbstractScreenGroup(ScreenId dmsScreenId);
    ScreenId GetDefaultAbstractScreenId();
    ScreenId ConvertToRsScreenId(ScreenId dmsScreenId);
    ScreenId ConvertToDmsScreenId(ScreenId rsScreenId);
    void RegisterAbstractScreenCallback(sptr<AbstractScreenCallback> cb);
    ScreenId CreateVirtualScreen(VirtualScreenOption option);
    DMError DestroyVirtualScreen(ScreenId screenId);
    DMError SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface);
    bool RequestRotation(ScreenId screenId, Rotation rotation);

    void OnScreenRotate(ScreenId dmsScreenId, Rotation before, Rotation after);
    bool IsScreenGroup(ScreenId screenId) const;
    bool SetScreenActiveMode(ScreenId screenId, uint32_t modeId);
    std::shared_ptr<RSDisplayNode> GetRSDisplayNodeByScreenId(ScreenId dmsScreenId) const;
    void UpdateRSTree(ScreenId dmsScreenId, std::shared_ptr<RSSurfaceNode>& surfaceNode, bool isAdd);
    bool MakeMirror(ScreenId, std::vector<ScreenId> screens);
    bool MakeExpand(std::vector<ScreenId> screenIds, std::vector<Point> startPoints);
    void DumpScreenInfo() const;
    void DumpScreenGroupInfo() const;

    // colorspace, gamut
    DMError GetScreenSupportedColorGamuts(ScreenId screenId, std::vector<ScreenColorGamut>& colorGamuts);
    DMError GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut);
    DMError SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx);
    DMError GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap);
    DMError SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap);
    DMError SetScreenColorTransform(ScreenId screenId);

private:
    void OnRsScreenConnectionChange(ScreenId rsScreenId, ScreenEvent screenEvent);
    void ProcessScreenDisconnected(ScreenId rsScreenId);
    bool FillAbstractScreen(sptr<AbstractScreen>& absScreen, ScreenId rsScreenId);
    sptr<AbstractScreenGroup> AddToGroupLocked(sptr<AbstractScreen> newScreen);
    sptr<AbstractScreenGroup> RemoveFromGroupLocked(sptr<AbstractScreen> newScreen);
    bool CheckScreenInScreenGroup(sptr<AbstractScreen> newScreen) const;
    sptr<AbstractScreenGroup> AddAsFirstScreenLocked(sptr<AbstractScreen> newScreen);
    sptr<AbstractScreenGroup> AddAsSuccedentScreenLocked(sptr<AbstractScreen> newScreen);
    void ProcessScreenModeChanged(ScreenId dmsScreenId);

    std::recursive_mutex& mutex_;
    OHOS::Rosen::RSInterfaces& rsInterface_;
    std::atomic<ScreenId> dmsScreenCount_;
    // No AbstractScreenGroup
    std::map<ScreenId, ScreenId> rs2DmsScreenIdMap_;
    std::map<ScreenId, ScreenId> dms2RsScreenIdMap_;
    std::map<ScreenId, sptr<AbstractScreen>> dmsScreenMap_;
    std::map<ScreenId, sptr<AbstractScreenGroup>> dmsScreenGroupMap_;
    sptr<AbstractScreenCallback> abstractScreenCallback_;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_SCREEN_CONTROLLER_H