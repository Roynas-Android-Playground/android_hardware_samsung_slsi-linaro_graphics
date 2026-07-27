/*
 * Copyright 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "ExynosDisplay.h"
#include "ExynosLayer.h"
#include "ExynosHWCHelper.h"
#include "ExynosResourceManagerModule.h"
#include "ExynosPrimaryDisplayModule.h"
#include "ExynosMPPModule.h"
#include "ExynosMPP.h"

#include <drm/drm_mode.h>
#include <drm_fourcc.h>
#include "ExynosDeviceDrmInterface.h"
#include "ExynosHWCDebug.h"
#include <hardware/hwcomposer_defs.h>
#include "DeconDrmHeader.h"
#include "DrmDataType.h"

#include "ExynosDisplayInterface.h"
#include "ExynosHWCService.h"

#include <sys/types.h>
#include <drm_fourcc.h>
#include <xf86drm.h>
#include <drm.h>
#include <drm/drm_mode.h>
#include <cutils/properties.h>
#include "ExynosDisplayDrmInterface.h"
#include "ExynosHWCDebug.h"
#include "ExynosGraphicBuffer.h"

#include "OneShotTimer.h"

#include "TraceUtils.h"

#include "drmdevice.h"
#include "resourcemanager.h"

#include "ui/GraphicBuffer.h"

class HwcUnitTest : public testing::Test {
public:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(HwcUnitTest, Destructor_ExynosDeviceDrmInterface) {
    ExynosDeviceDrmInterface* tmp = new ExynosDeviceDrmInterface();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_ExynosDeviceInterface) {
    ExynosDeviceInterface* tmp = new ExynosDeviceDrmInterface();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_ExynosResourceManager) {
    ExynosResourceManager* tmp = new ExynosResourceManagerModule();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_ExynosMPPVector) {
    ExynosMPPVector* tmp = new ExynosMPPVector();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_ExynosDisplay) {
    uint32_t id = getDisplayId(HWC_DISPLAY_PRIMARY, 0);
    DisplayIdentifier node = {id, HWC_DISPLAY_PRIMARY, 0,
                              String8("PrimaryDisplay"),
                              String8("fake_decon_fb")};
    ExynosDisplay* tmp = new ExynosDisplay(node);
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_LayerDumpManager) {
    uint32_t id = getDisplayId(HWC_DISPLAY_PRIMARY, 0);
    DisplayIdentifier node = {id, HWC_DISPLAY_PRIMARY, 0,
                              String8("PrimaryDisplay"),
                              String8("fake_decon_fb")};
    ExynosDisplay* display = new ExynosDisplay(node);
    LayerDumpManager* tmp = new LayerDumpManager(display);
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_ExynosSortedLayer) {
    ExynosSortedLayer* tmp = new ExynosSortedLayer();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_ExynosDisplayDrmInterface) {
    ExynosDisplayInterface* tmp = new ExynosDisplayDrmInterface();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_ExynosDisplayInterface) {
    ExynosDisplayInterface* tmp = new ExynosDisplayInterface();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_exynos_dpu_data) {
    exynos_dpu_data* tmp = new exynos_dpu_data();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_FramebufferManager) {
    FramebufferManager* tmp = new FramebufferManager();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_ExynosPrimaryDisplay) {
    uint32_t id = getDisplayId(HWC_DISPLAY_PRIMARY, 0);
    DisplayIdentifier node = {id, HWC_DISPLAY_PRIMARY, 0,
                              String8("PrimaryDisplay"),
                              String8("fake_decon_fb")};
    ExynosPrimaryDisplay* tmp = new ExynosPrimaryDisplay(node);
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_ExynosMPP) {
    ExynosMPP* tmp = new ExynosMPP(MPP_DPP_G, MPP_LOGICAL_DPP_G, "DPP_G0", 0, 0,
                                   HWC_DISPLAY_PRIMARY_BIT, MPP_TYPE_OTF);
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_ExynosFenceTracer) {
    ExynosFenceTracer* tmp = new ExynosFenceTracer();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_format_description) {
    format_description* tmp = new format_description();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_DevicePresentInfo) {
    DevicePresentInfo* tmp = new DevicePresentInfo();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_DeviceValidateInfo) {
    DeviceValidateInfo* tmp = new DeviceValidateInfo();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_funcReturnCallback) {
    funcReturnCallback* tmp = new funcReturnCallback([]() {printf("test func\n"); });
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_OneShotTimer) {
    OneShotTimer* tmp = new OneShotTimer(std::chrono::milliseconds(500), [] {}, [] {});
    delete tmp;
}

TEST_F(HwcUnitTest, OneShotTimer) {
    OneShotTimer* tmp = new OneShotTimer(std::chrono::milliseconds(500),
                                         [] {printf("timer reset\n");},
                                         [] {printf("timeout\n");});
    tmp->setInterval(std::chrono::milliseconds(1000));
    tmp->start();
    EXPECT_EQ (tmp->isTimerRunning(), false);
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_TraceEnder) {
    android::TraceUtils::TraceEnder* tmp = new android::TraceUtils::TraceEnder();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_DeviceInterfaceData) {
    DrmDevice *device = new DrmDevice();
    DeviceInterfaceData* tmp = new DeviceInterfaceData(device);
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_displayTDMInfo) {
    displayTDMInfo* tmp = new displayTDMInfo();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_captureReadbackClass) {
    ExynosDevice::captureReadbackClass* tmp = new ExynosDevice::captureReadbackClass();
    delete tmp;
}

TEST_F(HwcUnitTest, Destructor_ExynosCompositionInfo) {
    ExynosCompositionInfo* tmp = new ExynosCompositionInfo();
    delete tmp;
}

TEST_F(HwcUnitTest, printBufLength) {
    sp<GraphicBuffer> buffer = new GraphicBuffer(1080, 1080,
                                                 HAL_PIXEL_FORMAT_RGBA_8888,
                                                 0, 0, "buffer_libui");
    buffer_handle_t handle = buffer->getNativeBuffer()->handle;
    size_t bufLength[MAX_HW2D_PLANES];
    printBufLength(handle, MAX_HW2D_PLANES, bufLength,
                   HAL_PIXEL_FORMAT_RGBA_8888, 1080, 1080);
}

TEST_F(HwcUnitTest, isDrm) {
    DisplayIdentifier primary_node = {getDisplayId(HWC_DISPLAY_PRIMARY, 0), HWC_DISPLAY_PRIMARY, 0,
                                      String8("PrimaryDisplay"), String8("fake_decon_fb")};
    ExynosDisplay* primaryDisplay = (ExynosDisplay *)(new ExynosPrimaryDisplayModule(primary_node));
    primaryDisplay->mPlugState = true;
    primaryDisplay->mXres = 1080;
    primaryDisplay->mYres = 1920;

    DisplayInfo display_info;
    primaryDisplay->getDisplayInfo(display_info);

    ExynosLayer *layer = new ExynosLayer(display_info);
    sp<GraphicBuffer> buffer = new GraphicBuffer(1080, 1080,
                                                 HAL_PIXEL_FORMAT_RGBA_8888,
                                                 0, 0, "buffer_libui");
    buffer_handle_t handle = buffer->getNativeBuffer()->handle;
    layer->mLayerBuffer = handle;

    EXPECT_EQ(layer->isDrm(), false);
    delete primaryDisplay;
    delete layer;
}

TEST_F(HwcUnitTest, updateConfigRequestAppliedTime) {
    DisplayIdentifier primary_node = {getDisplayId(HWC_DISPLAY_PRIMARY, 0), HWC_DISPLAY_PRIMARY, 0,
                                      String8("PrimaryDisplay"), String8("fake_decon_fb")};
    ExynosDisplay* primaryDisplay = (ExynosDisplay *)(new ExynosPrimaryDisplayModule(primary_node));
    primaryDisplay->mPlugState = true;
    primaryDisplay->mXres = 1080;
    primaryDisplay->mYres = 1920;

    primaryDisplay->mConfigRequestState = hwc_request_state_t::SET_CONFIG_STATE_NONE;
    EXPECT_EQ(primaryDisplay->updateConfigRequestAppliedTime(), NO_ERROR);

    delete primaryDisplay;
}

TEST_F(HwcUnitTest, ExynosDisplayInterface) {
    std::unique_ptr<ExynosDisplayInterface> tmp;
    tmp = std::make_unique<ExynosDisplayInterface>();

    uint32_t id = getDisplayId(HWC_DISPLAY_PRIMARY, 0);
    DisplayIdentifier node = {id, HWC_DISPLAY_PRIMARY, 0,
                              String8("PrimaryDisplay"),
                              String8("fake_decon_fb")};
    ExynosDisplay* primaryDisplay = (ExynosDisplay *)(new ExynosPrimaryDisplayModule(node));
    primaryDisplay->mPlugState = true;
    primaryDisplay->mXres = 1080;
    primaryDisplay->mYres = 1920;

    DisplayInfo display_info;
    primaryDisplay->getDisplayInfo(display_info);

    tmp->init(node, nullptr, 0);
    tmp->updateDisplayInfo(display_info);
    tmp->setPowerMode(0);
    tmp->setVsyncEnabled(0);
    tmp->setLowPowerMode(0);
    tmp->isDozeModeAvailable();

    uint32_t temp1 = 0;
    std::map<uint32_t, displayConfigs_t> displayConfigs;
    tmp->getDisplayConfigs(&temp1, nullptr, displayConfigs);
    tmp->dumpDisplayConfigs();
    tmp->getColorModes(&temp1, nullptr);
    tmp->setColorMode(0, 0);

    hwc2_config_t temp2 = 0;
    displayConfigs_t temp3;
    tmp->setActiveConfig(temp2, temp3);
    tmp->getDisplayVsyncPeriod(nullptr);
    tmp->getDisplayVsyncTimestamp(nullptr);
    tmp->getDPUConfig(&temp2);
    tmp->setCursorPositionAsync(0, 0);

    std::vector<int32_t> outTypes;
    tmp->updateHdrCapabilities(outTypes, nullptr, nullptr, nullptr);

    exynos_dpu_data temp4;
    tmp->deliverWinConfigData(temp4);
    tmp->clearDisplay();
    tmp->disableSelfRefresh(0);
    tmp->setForcePanic();
    tmp->getDisplayFd();
    tmp->getMaxWindowNum();
    tmp->setColorTransform(nullptr, 0, 0);
    tmp->getRenderIntents(0, nullptr, nullptr);
    tmp->setColorModeWithRenderIntent(0, 0, 0);
    tmp->getReadbackBufferAttributes(nullptr, nullptr);
    tmp->setRepeaterBuffer(0);
    tmp->getDisplayIdentificationData(nullptr, nullptr, nullptr);
    tmp->getDeconDMAType(0, 0);

    displayConfigs_t temp33;
    tmp->getVsyncAppliedTime(temp2, temp33, nullptr);
    tmp->getConfigChangeDuration();
    tmp->setActiveConfigWithConstraints(temp2, temp3);

    int temp11;
    std::vector<ResolutionInfo> resolutionInfo;
    tmp->getDisplayHWInfo(temp1, temp1, temp11, resolutionInfo);
    tmp->registerVsyncHandler(nullptr);
    tmp->getVsyncFd();

    DeviceToDisplayInterface temp5;
    tmp->setDeviceToDisplayInterface(temp5);
    tmp->readHotplugStatus();

    String8 temp6;
    tmp->updateUeventNodeName(temp6);
    tmp->updateHdrSinkInfo();
    tmp->onDisplayRemoved();

    hwc2_layer_t temp7 = 0;
    tmp->onLayerDestroyed(temp7);
    tmp->onLayerCreated(temp7);;
    tmp->canDisableAllPlanes(0);
    tmp->getWorkingVsyncPeriod();
}


TEST_F(HwcUnitTest, ExynosDisplayDrmInterface) {
    ExynosDisplayInterface* tmp = new ExynosDisplayDrmInterface();
    tmp->setForcePanic();
    tmp->setLowPowerMode(0);
    tmp->readHotplugStatus();
    tmp->updateHdrSinkInfo();
    tmp->setCursorPositionAsync(0, 0);

    delete tmp;
}

TEST_F(HwcUnitTest, ExynosDevice) {
    ExynosDevice* tmp = new ExynosDevice();
    tmp->handleHotplug();
    tmp->getCPUPerfInfo(0, 0, nullptr, nullptr);
    tmp->canSkipValidate();
    tmp->handleHotplugAfterBooting();
    tmp->releaseCPUPerfPerCluster();
    tmp->setCPUClocksPerCluster(0);
}

TEST_F(HwcUnitTest, ExynosResourceManager) {
    ExynosResourceManager* tmp = new ExynosResourceManagerModule();
    tmp->getHDR10OtfMPP();
    tmp->updateRestrictions();
}

TEST_F(HwcUnitTest, ExynosDisplay) {
    uint32_t id = getDisplayId(HWC_DISPLAY_PRIMARY, 0);
    DisplayIdentifier node = {id, HWC_DISPLAY_PRIMARY, 0,
                              String8("PrimaryDisplay"),
                              String8("fake_decon_fb")};
    ExynosDisplay* tmp = new ExynosDisplay(node);
    tmp->needChangeConfig(0);
    tmp->checkLayersForSettingDR();
    delete tmp;
}

TEST_F(HwcUnitTest, getMetaParcel) {
    DisplayIdentifier primary_node = {getDisplayId(HWC_DISPLAY_PRIMARY, 0), HWC_DISPLAY_PRIMARY, 0,
                                      String8("PrimaryDisplay"), String8("fake_decon_fb")};
    ExynosDisplay* primaryDisplay = (ExynosDisplay *)(new ExynosPrimaryDisplayModule(primary_node));
    primaryDisplay->mPlugState = true;
    primaryDisplay->mXres = 1080;
    primaryDisplay->mYres = 1920;

    DisplayInfo display_info;
    primaryDisplay->getDisplayInfo(display_info);

    ExynosLayer *layer = new ExynosLayer(display_info);
    layer->getMetaParcel();
}


TEST_F(HwcUnitTest, ExynosMPP) {
    ExynosMPP* tmp = new ExynosMPP(MPP_DPP_G, MPP_LOGICAL_DPP_G, "DPP_G0", 0, 0,
                                   HWC_DISPLAY_PRIMARY_BIT, MPP_TYPE_OTF);
    exynos_image temp;
    buffer_handle_t outbuf;
    tmp->canUseVotf(temp);
    tmp->getSubMPPs(nullptr, nullptr);
    tmp->setVotfLayerData(nullptr);

    DisableType type = DisableType::DISABLE_NONE;
    tmp->setDisableByUserScenario(type);
}


TEST_F(HwcUnitTest, ExynosFenceTracer) {
    ExynosFenceTracer* tmp = new ExynosFenceTracer();
    timeval tv;
    tmp->getLocalTime(tv);
    tmp->printLeakFds();
    tmp->dumpFenceInfo(0);
    tmp->dumpNCheckLeak(0);
    tmp->saveFenceTrace();
    tmp->printLastFenceInfo(0);
    tmp->fenceWarn(0);

    delete tmp;
}

TEST_F(HwcUnitTest, ExynosHWCHelper) {
    min(1,1);

    const std::map<int32_t, String8> temp = {
    {FENCE_FROM, String8("From")},
    {FENCE_TO, String8("To")},
    {FENCE_DUP, String8("Dup")},
    {FENCE_CLOSE, String8("Close")},};
    getString(temp, 0);

    format_description* tmp = new format_description();
    tmp->isCompressionSupported(0);

    sp<GraphicBuffer> buffer = new GraphicBuffer(1080, 1080,
                                                 HAL_PIXEL_FORMAT_RGBA_8888,
                                                 0, 0, "buffer_libui");
    buffer_handle_t handle = buffer->getNativeBuffer()->handle;
    dumpHandle(0, handle);
    isFormat8Bit(0);
    getTypeOfFormat(0);
    hwc_print_stack();
    getPlaneNumOfFormat(0);
    DpuFormatToHalFormat(0);
    halFormatToDpuFormat(0);
    halTransformToDpuRot(0);
    halBlendingToDpuBlending(0);
}

TEST_F(HwcUnitTest, updateFeatureTableAndRestrictions) {
    ExynosResourceManager *resourceManager = new ExynosResourceManagerModule();
    struct dpp_restrictions_info_v2 restrictions;
    restrictions.dpp_cnt = 1;
    restrictions.dpp_ch[0].attr = 0xFF;
    restrictions.dpp_ch[0].restriction.src_f_w = {0, 100, 1};
    restrictions.dpp_ch[0].restriction.src_f_h = {0, 100, 1};
    restrictions.dpp_ch[0].restriction.src_w = {0, 100, 1};
    restrictions.dpp_ch[0].restriction.src_h = {0, 100, 1};
    restrictions.dpp_ch[0].restriction.src_x_align = 1;
    restrictions.dpp_ch[0].restriction.src_y_align = 1;
    restrictions.dpp_ch[0].restriction.dst_f_w = {0, 100, 1};
    restrictions.dpp_ch[0].restriction.dst_f_h = {0, 100, 1};
    restrictions.dpp_ch[0].restriction.dst_w = {0, 100, 1};
    restrictions.dpp_ch[0].restriction.dst_h = {0, 100, 1};
    restrictions.dpp_ch[0].restriction.dst_x_align = 1;
    restrictions.dpp_ch[0].restriction.dst_y_align = 1;
    restrictions.dpp_ch[0].restriction.blk_w = {0, 100, 1};
    restrictions.dpp_ch[0].restriction.blk_h = {0, 100, 1};
    restrictions.dpp_ch[0].restriction.blk_x_align = 1;
    restrictions.dpp_ch[0].restriction.blk_y_align = 1;
    restrictions.dpp_ch[0].restriction.src_h_rot_max = 0;
    restrictions.dpp_ch[0].restriction.format[0] = 1;
    restrictions.dpp_ch[0].restriction.format_cnt = 1;
    restrictions.dpp_ch[0].restriction.scale_down = 1;
    restrictions.dpp_ch[0].restriction.scale_up = 1;

    resourceManager->updateFeatureTable(&restrictions);
    resourceManager->updateRestrictions();
    delete resourceManager;
}

TEST_F(HwcUnitTest, ExynosDisplay_cpp) {
    uint32_t id = getDisplayId(HWC_DISPLAY_PRIMARY, 0);
    DisplayIdentifier node = {id, HWC_DISPLAY_PRIMARY, 0,
                              String8("PrimaryDisplay"),
                              String8("fake_decon_fb")};
    ExynosDisplay* tmp = new ExynosDisplay(node);

    android::String8 result;
    exynos_win_config_data data;
    tmp->dumpConfig(result, data);

    tmp->closeFences();

    tmp->printConfig(data);

    tmp->mDisplayInterface = std::make_unique<ExynosDisplayInterface>();
    uint64_t geometryFlag;
    tmp->setPowerMode(HWC2_POWER_MODE_DOZE, geometryFlag);

    tmp->setHWCControl(100, 0);

    tmp->canSkipValidate();

    DisplayInfo display_info;
    tmp->mPlugState = true;
    tmp->mXres = 1080;
    tmp->mYres = 1920;
    tmp->getDisplayInfo(display_info);
    ExynosLayer *layer = new ExynosLayer(display_info);
    sp<GraphicBuffer> buffer = new GraphicBuffer(1080, 1080,
                                                 HAL_PIXEL_FORMAT_RGBA_8888,
                                                 0, 0, "buffer_libui");
    buffer_handle_t handle = buffer->getNativeBuffer()->handle;
    layer->mLayerBuffer = handle;

    tmp->setForceClient();

    tmp->printDebugInfos(result);

    tmp->setOutputBuffer(handle, 0);

    uint32_t tempuint = 0;
    int32_t tempint = 0;
    uint32_t* outNum = &tempuint;
    int32_t* outIntent = &tempint;
    tmp->getRenderIntents(0, outNum, outIntent);

    tmp->handleSkipPresent(nullptr);

    tmp->handleHotplugEvent(true);

    exynos_dpu_data last, cur;
    tmp->checkConfigChanged(last, cur);

    tmp->setCursorPositionAsync(0, 0);

    exynos_image tmp_src;
    tmp->is2StepBlendingRequired(tmp_src, handle);

    uint64_t* outTime = &geometryFlag;
    tmp->getDisplayVsyncTimestamp(outTime);
#ifdef USE_DISPLAY_COLOR_INTERFACE
    tmp->searchDisplayRenderIntent(0, 0);
#endif

    bool hpdStatus = false;
    tmp->checkHotplugEventUpdated(hpdStatus);

    tmp->setColorModeWithRenderIntent(HAL_COLOR_MODE_SRGB, 0, false, geometryFlag);
    tmp->hotplug();

    tmp->mNeedSkipValidatePresent = true;
    tmp->getDisplayRequests(outIntent, outNum, nullptr, nullptr);

    delete tmp;
}
