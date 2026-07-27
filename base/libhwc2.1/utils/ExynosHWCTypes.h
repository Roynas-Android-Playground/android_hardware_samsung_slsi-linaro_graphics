/*
 * Copyright (C) 2021 The Android Open Source Project
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
#ifndef _EXYNOSHWCTypes_H
#define _EXYNOSHWCTypes_H

#include <hardware/hwcomposer2.h>
#include <vector>
#include "ExynosMPPType.h"

using namespace android;

#define HWC_FPS_TH 5 /* valid range 1 to 60 */
#define VSYNC_INTERVAL (1000000000.0 / 60)

enum overlay_priority {
    ePriorityNone,
    /* Normal layer */
    ePriorityLow,
    /* Assign resource before normal layers */
    ePriorityMid,
    /*
     * Overlay is better than client composition,
     * Displayed screen can be abnormal if the layer is composited by client
     */
    ePriorityHigh,
    /*
     * Overlay is mandatory,
     * Black screen will be displayed if the layer is composited by client
     */
    ePriorityMax
};

enum {
    /*add after hwc2_composition_t, margin number here*/
    HWC2_COMPOSITION_EXYNOS = 32,
};

enum {
    HWC_HAL_ERROR_INVAL = HWC2_ERROR_BAD_PARAMETER,
} hwc_hal_error_t;

enum {
    DEFAULT_MODE = 0,
    LOWEST_MODE = 1,
    HIGHEST_MODE = 2,
};

enum {
    HWC_CTL_MAX_OVLY_CNT = 100,
    HWC_CTL_VIDEO_OVLY_CNT = 101,
    HWC_CTL_DYNAMIC_RECOMP = 102,
    HWC_CTL_SKIP_STATIC = 103,
    /* HWC_CTL_DMA_BW_BAL = 104, */
    HWC_CTL_SECURE_DMA = 105,
    HWC_CTL_WINDOW_UPDATE = 106,
    HWC_CTL_FORCE_PANIC = 107,
    HWC_CTL_FORCE_GPU = 108,
    HWC_CTL_SKIP_M2M_PROCESSING = 109,
    HWC_CTL_DISPLAY_MODE = 110,
    HWC_CTL_SKIP_RESOURCE_ASSIGN = 111,
    HWC_CTL_SKIP_VALIDATE = 112,
    HWC_CTL_ADJUST_DYNAMIC_RECOMP_TIMER = 113,
    HWC_CTL_DUMP_MID_BUF = 200,
    HWC_CTL_CAPTURE_READBACK = 201,
    HWC_CTL_ENABLE_EXYNOSCOMPOSITION_OPT = 301,
    HWC_CTL_USE_MAX_G2D_SRC = 303,
    HWC_CTL_ENABLE_EARLY_START_MPP = 305,
    HWC_CTL_ENABLE_FENCE_TRACER = 307,
    HWC_CTL_DO_FENCE_FILE_DUMP = 308,
    HWC_CTL_SYS_FENCE_LOGGING = 309,
    HWC_CTL_USE_PERF_FILE = 310,
};

enum {
    HDR_INTERFACE_DEBUG = 0,
    DQE_INTERFACE_DEBUG,
    DISPLAY_COLOR_INTERFACE_DEBUG,
};

enum {
    NO_DRM = 0,
    NORMAL_DRM,
    SECURE_DRM,
};

enum {
    PSR_NONE = 0,
    PSR_DP,
    PSR_MIPI,
    PSR_MAX,
};

enum {
    PANEL_LEGACY = 0,
    PANEL_DSC,
    PANEL_MIC,
};

enum WFDState {
    DISABLE_WFD,
    GOOGLEWFD,
    LLWFD,
    GOOGLEWFD_TO_LLWFD,
    LLWFD_TO_GOOGLEWFD,
};

enum dynamic_recomp_mode {
    NO_MODE_SWITCH,
    DEVICE_TO_CLIENT,
    CLIENT_TO_DEVICE
};

enum {
    GEOMETRY_LAYER_TYPE_CHANGED = 1ULL << 0,
    GEOMETRY_LAYER_DATASPACE_CHANGED = 1ULL << 1,
    GEOMETRY_LAYER_DISPLAYFRAME_CHANGED = 1ULL << 2,
    GEOMETRY_LAYER_SOURCECROP_CHANGED = 1ULL << 3,
    GEOMETRY_LAYER_TRANSFORM_CHANGED = 1ULL << 4,
    GEOMETRY_LAYER_ZORDER_CHANGED = 1ULL << 5,
    GEOMETRY_LAYER_FPS_CHANGED = 1ULL << 6,
    GEOMETRY_LAYER_FLAG_CHANGED = 1ULL << 7,
    GEOMETRY_LAYER_PRIORITY_CHANGED = 1ULL << 8,
    GEOMETRY_LAYER_COMPRESSED_CHANGED = 1ULL << 9,
    GEOMETRY_LAYER_BLEND_CHANGED = 1ULL << 10,
    GEOMETRY_LAYER_FORMAT_CHANGED = 1ULL << 11,
    GEOMETRY_LAYER_DRM_CHANGED = 1ULL << 12,
    GEOMETRY_LAYER_HDR_META_CHANGED = 1ULL << 13,
    GEOMETRY_LAYER_UNKNOWN_CHANGED = 1ULL << 14,
    /* 1ULL << 15 */
    /* 1ULL << 16 */
    /* 1ULL << 17 */
    /* 1ULL << 18 */
    /* 1ULL << 19 */
    GEOMETRY_DISPLAY_LAYER_ADDED = 1ULL << 20,
    GEOMETRY_DISPLAY_LAYER_REMOVED = 1ULL << 21,
    GEOMETRY_DISPLAY_CONFIG_CHANGED = 1ULL << 22,
    GEOMETRY_DISPLAY_SINGLEBUF_CHANGED = 1ULL << 24,
    GEOMETRY_DISPLAY_FORCE_VALIDATE = 1ULL << 25,
    GEOMETRY_DISPLAY_COLOR_MODE_CHANGED = 1ULL << 26,
    GEOMETRY_DISPLAY_DYNAMIC_RECOMPOSITION = 1ULL << 27,
    GEOMETRY_DISPLAY_POWER_ON = 1ULL << 28,
    GEOMETRY_DISPLAY_POWER_OFF = 1ULL << 29,
    GEOMETRY_DISPLAY_COLOR_TRANSFORM_CHANGED = 1ULL << 30,
    GEOMETRY_DISPLAY_DATASPACE_CHANGED = 1ULL << 31,
    GEOMETRY_DISPLAY_FRAME_SKIPPED = 1ULL << 32,
    GEOMETRY_DISPLAY_ADJUST_SIZE_CHANGED = 1ULL << 33,
    GEOMETRY_DISPLAY_WORKING_VSYNC_CHANGED = 1ULL << 34,
    /* 1ULL << 35 */
    GEOMETRY_DEVICE_DISPLAY_ADDED = 1ULL << 36,
    GEOMETRY_DEVICE_DISPLAY_REMOVED = 1ULL << 37,
    GEOMETRY_DEVICE_CONFIG_CHANGED = 1ULL << 38,
    GEOMETRY_DEVICE_DISP_MODE_CHAGED = 1ULL << 39,
    GEOMETRY_DEVICE_SCENARIO_CHANGED = 1ULL << 40,
    GEOMETRY_DEVICE_FPS_CHANGED = 1ULL << 41,
    /* 1ULL << 42 */
    /* 1ULL << 43 */
    /* 1ULL << 44 */
    /* 1ULL << 45 */
    GEOMETRY_MPP_CONFIG_CHANGED = 1ULL << 46,

    GEOMETRY_ERROR_CASE = 1ULL << 63,
};

namespace MSCvOTFInfo {
enum MSCvOTFInfo {
    ENABLE,
    DPU_DMA_IDX,
    TRS_IDX,
    BUF_IDX,
    COUNT = 4,
};
}

typedef enum restriction_classification {
    RESTRICTION_RGB = 0,
    RESTRICTION_YUV,
    RESTRICTION_MAX
} restriction_classification_t;

struct exynos_callback_info_t {
    hwc2_callback_data_t callbackData = nullptr;
    hwc2_function_pointer_t funcPointer = nullptr;
};

typedef struct update_time_info {
    struct timeval lastUeventTime;
    struct timeval lastEnableVsyncTime;
    struct timeval lastDisableVsyncTime;
    struct timeval lastValidateTime;
    struct timeval lastPresentTime;
} update_time_info_t;

typedef struct restriction_key {
    mpp_phycal_type_t hwType; /* MPP_DPP_VG, MPP_DPP_VGFS, ... */
    uint32_t nodeType;        /* src or dst */
    uint32_t format;          /* HAL format */
    uint32_t reserved;
    bool operator==(const restriction_key &other) const {
        return (hwType == other.hwType) && (nodeType == other.nodeType) &&
               (format == other.format) && (reserved == other.reserved);
    };
} restriction_key_t;

typedef struct restriction_size {
    uint32_t maxDownScale = 1;
    uint32_t maxUpScale = 1;
    uint32_t maxFullWidth = 0;
    uint32_t maxFullHeight = 0;
    uint32_t minFullWidth = 0;
    uint32_t minFullHeight = 0;
    uint32_t fullWidthAlign = 1;
    uint32_t fullHeightAlign = 1;
    uint32_t maxCropWidth = 0;
    uint32_t maxCropHeight = 0;
    uint32_t minCropWidth = 0;
    uint32_t minCropHeight = 0;
    uint32_t cropXAlign = 1;
    uint32_t cropYAlign = 1;
    uint32_t cropWidthAlign = 1;
    uint32_t cropHeightAlign = 1;
} restriction_size_t;

typedef struct exynos_hwc_control {
    uint32_t forceGpu;
    uint32_t windowUpdate;
    uint32_t forcePanic;
    uint32_t skipResourceAssign;
    uint32_t multiResolution;
    uint32_t dumpMidBuf;
    uint32_t displayMode;
    uint32_t skipWinConfig;
    uint32_t skipValidate;
    uint32_t doFenceFileDump;
    uint32_t fenceTracer;
    uint32_t sysFenceLogging;
    uint32_t usePerfFile;
} exynos_hwc_control_t;

typedef struct restriction_size_element {
    restriction_key_t key;
    restriction_size_t sizeRestriction;
} restriction_size_element_t;

typedef struct restriction_table_element {
    uint32_t classfication_type;
    const restriction_size_element *table;
    uint32_t table_element_size;
} restriction_table_element_t;

struct exynos_writeback_info {
    buffer_handle_t handle = NULL;
    /* release sync fence file descriptor,
     * which will be signaled when it is safe to write to the output buffer.
     */
    int rel_fence = -1;
    /* acquire sync fence file descriptor which will signal when the
     * buffer provided to setReadbackBuffer has been filled by the device and is
     * safe for the client to read.
     */
    int acq_fence = -1;
};

struct DisplayIdentifier {
    uint32_t id = UINT32_MAX;
    uint32_t type = UINT32_MAX;
    uint32_t index = UINT32_MAX;
    String8 name;
    String8 deconNodeName;
    bool operator==(const DisplayIdentifier &rhs) const {
        return (id == rhs.id);
    };
};

struct DeviceValidateInfo {
    bool useCameraException = false;
    std::vector<DisplayIdentifier> nonPrimaryDisplays;
    restriction_size srcSizeRestriction;
    restriction_size dstSizeRestriction;
    bool hasUnstartedDisplay = false;
};

struct DevicePresentInfo {
    uint32_t vsyncMode = DEFAULT_MODE;
    bool isBootFinished = true;
    std::vector<DisplayIdentifier> nonPrimaryDisplays;
};

struct DisplayInfo {
    struct DisplayIdentifier displayIdentifier;
    uint32_t xres = 0;
    uint32_t yres = 0;
    android_color_mode_t colorMode = HAL_COLOR_MODE_NATIVE;
    std::vector<uint32_t> hdrLayersIndex;
    std::vector<uint32_t> drmLayersIndex;
    /*
     * default is 16ms
     * This is display working vsync period.
     * It can be different with the vsync period that HWC has.
     * HWC reads this value from display driver and
     * uses it to check supported scaling performance.
     */
    uint32_t workingVsyncPeriod = 16666666;
    bool useDpu = true;
    float minLuminance = 0;
    float maxLuminance = 0;
    bool adjustDisplayFrame = false;
    bool cursorSupport = false;
    bool skipM2mProcessing = true;
    uint32_t baseWindowIndex = 0;
    decon_idma_type defaultDMA = MAX_DECON_DMA_TYPE;

    union {
        /* for virtual display */
        int32_t isWFDState = 0;
        /* for external display */
        bool sinkHdrSupported;
    };
    void reset() {
        *this = {};
    }
};

typedef struct displayConfigs {
    // HWC2_ATTRIBUTE_VSYNC_PERIOD
    uint32_t vsyncPeriod;
    // HWC2_ATTRIBUTE_WIDTH
    uint32_t width;
    // case HWC2_ATTRIBUTE_HEIGHT
    uint32_t height;
    // HWC2_ATTRIBUTE_DPI_X
    uint32_t Xdpi;
    // HWC2_ATTRIBUTE_DPI_Y
    uint32_t Ydpi;
    // HWC2_ATTRIBUTE_CONFIG_GROUP
    uint32_t groupId;
} displayConfigs_t;

struct VotfInfo {
    unsigned int data[MSCvOTFInfo::COUNT] = {0, UINT_MAX, UINT_MAX, UINT_MAX};
    unsigned int &enable = data[MSCvOTFInfo::ENABLE];
    unsigned int &dmaIndex = data[MSCvOTFInfo::DPU_DMA_IDX];
    unsigned int &trsIndex = data[MSCvOTFInfo::TRS_IDX];
    unsigned int &bufIndex = data[MSCvOTFInfo::BUF_IDX];

    VotfInfo &operator=(const VotfInfo &votfInfo) {
        enable = votfInfo.enable;
        dmaIndex = votfInfo.dmaIndex;
        trsIndex = votfInfo.trsIndex;
        bufIndex = votfInfo.bufIndex;
        return *this;
    };

    void reset() {
        /*
         * dmaIndex, trsIndex, bufIndex shouldn't be reset
         */
        enable = 0;
    };
};

struct DeviceResourceInfo {
    uint32_t displayMode = 0;
};

// Prepare multi-resolution
struct ResolutionSize {
    uint32_t w;
    uint32_t h;
    ResolutionSize(uint32_t _w, uint32_t _h) : w(_w), h(_h){};
};

struct ResolutionInfo {
    ResolutionSize nResolution;
    uint32_t nDSCXSliceSize;
    uint32_t nDSCYSliceSize;
    int nPanelType;
    ResolutionInfo(const ResolutionSize &r, uint32_t dx, uint32_t dy, int pt)
        : nResolution(r), nDSCXSliceSize(dx), nDSCYSliceSize(dy),
          nPanelType(pt){};
};

class ExynosHotplugHandler {
  public:
    virtual ~ExynosHotplugHandler(){};
    virtual void handleHotplug(){};
};

class ExynosPanelResetHandler {
  public:
    virtual ~ExynosPanelResetHandler(){};
    virtual void handlePanelReset(){};
};

class ExynosVsyncHandler {
  public:
    virtual ~ExynosVsyncHandler(){};
    virtual void handleVsync(uint64_t timestamp);
};

struct ExynosDisplayHandler {
    ExynosVsyncHandler *handle = nullptr;
    int32_t mVsyncFd = -1;
};

class ExynosFpsChangedCallback {
  public:
    virtual ~ExynosFpsChangedCallback(){};
    virtual void fpsChangedCallback() = 0;
};

#endif
