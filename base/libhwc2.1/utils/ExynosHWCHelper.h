/*
 * Copyright (C) 2012 The Android Open Source Project
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
#ifndef _EXYNOSHWCHELPER_H
#define _EXYNOSHWCHELPER_H

#include <array>
#include <utils/String8.h>
#include <hardware/hwcomposer2.h>
#include <map>
#include "DeconCommonHeader.h"
#include "VendorVideoAPI.h"
#include "exynos_sync.h"
#include "ExynosHWCFormat.h"
#include "ExynosHWCTypes.h"

using namespace android;

#define DISPLAYID_MASK_LEN 8

const String8 kUnKnownStr("Unknown");
template <typename T>
inline const char *getString(T map, uint32_t id) {
    if (map.find(id) != map.end())
        return map.at(id).c_str();
    return kUnKnownStr.c_str();
};

template <typename T>
inline T max(T a, T b) { return (a > b) ? a : b; }
template <typename T>
inline T min(T a, T b) { return (a < b) ? a : b; }

static constexpr uint32_t TRANSFORM_MAT_WIDTH = 4;
static constexpr uint32_t TRANSFORM_MAT_SIZE = TRANSFORM_MAT_WIDTH * TRANSFORM_MAT_WIDTH;

enum {
    /* AFBC_NO_RESTRICTION means the case that resource set includes two DPPs
       sharing AFBC resource. If AFBC_NO_RESTRICTION of resource set
       is true, resource set supports AFBC without size restriction. */
    ATTRIBUTE_AFBC_NO_RESTRICTION = 0,
    ATTRIBUTE_HDR10PLUS,
    ATTRIBUTE_HDR10,
    ATTRIBUTE_DRM,
    ATTRIBUTE_AFBC,
    MAX_ATTRIBUTE_NUM,
};

enum {
    EXYNOS_HWC_DIM_LAYER = 0x00000001,
    EXYNOS_HWC_FORCE_CLIENT_WFD = 0x00000002,
    EXYNOS_HWC_FORCE_CLIENT_HDR_META_ERROR = 0x00000004,
};

enum {
    INTERFACE_TYPE_NONE = 0,
    INTERFACE_TYPE_FB = 1,
    INTERFACE_TYPE_DRM = 2,
};

enum {
    EXYNOS_ERROR_NONE = 0,
    EXYNOS_ERROR_CHANGED = 1
};

enum {
    eSkipLayer = 0x00000001,
    eInvalidHandle = 0x00000002,
    eHasFloatSrcCrop = 0x00000004,
    eUpdateExynosComposition = 0x00000008,
    eDynamicRecomposition = 0x00000010,
    eForceFbEnabled = 0x00000020,
    eSandwitchedBetweenGLES = 0x00000040,
    eSandwitchedBetweenEXYNOS = 0x00000080,
    eInsufficientWindow = 0x00000100,
    eInsufficientMPP = 0x00000200,
    eSkipStaticLayer = 0x00000400,
    eUnSupportedUseCase = 0x00000800,
    eDimLayer = 0x00001000,
    eResourcePendingWork = 0x00002000,
    eSkipRotateAnim = 0x00004000,
    eUnSupportedColorTransform = 0x00008000,
    eInvalidVideoMetaData = 0x00010000,
    eInvalidDispFrame = 0x00020000,
    eExceedMaxLayerNum = 0x00040000,
    eFroceClientLayer = 0x00080000,
    eRemoveDynamicMetadata = 0x00100000,
    eResourceAssignFail = 0x20000000,
    eMPPUnsupported = 0x40000000,
    eUnknown = 0x80000000,
};

enum regionType {
    eTransparentRegion = 0,
    eCoveredOpaqueRegion = 1,
    eDamageRegionByDamage = 2,
    eDamageRegionByLayer = 3,
};

enum {
    eDamageRegionFull = 0,
    eDamageRegionPartial,
    eDamageRegionSkip,
    eDamageRegionError,
};

typedef struct CompressionInfo {
    uint32_t type = COMP_TYPE_NONE;
    uint32_t SAJCMaxBlockSize = 0;
    uint32_t SAJCHeaderOffset = 0;
} compressionInfo_t;

/*
 * bufferHandle can be NULL if it is not allocated yet
 * or size or format information can be different between other field values and
 * member of bufferHandle. This means bufferHandle should be reallocated.
 * */

struct exynos_image {
    uint32_t fullWidth = 0;
    uint32_t fullHeight = 0;
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t w = 0;
    uint32_t h = 0;
    hwc_color_t color = {};

    ExynosFormat exynosFormat;

    uint64_t usageFlags = 0;
    uint32_t layerFlags = 0;
    int acquireFenceFd = -1;
    int releaseFenceFd = -1;
    buffer_handle_t bufferHandle = NULL;
    android_dataspace dataSpace = HAL_DATASPACE_UNKNOWN;
    uint32_t blending = 0;
    uint32_t transform = 0;
    compressionInfo_t compressionInfo;
    float planeAlpha = 0;
    uint32_t zOrder = 0;
    /* refer
     * frameworks/native/include/media/hardware/VideoAPI.h
     * frameworks/native/include/media/hardware/HardwareAPI.h */
    ExynosVideoMeta *metaParcel = nullptr;
    ExynosVideoInfoType metaType = VIDEO_INFO_TYPE_INVALID;
    bool needColorTransform = false;
    std::array<float, TRANSFORM_MAT_SIZE> colorTransformMatrix = {
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

    bool isDimLayer() {
        if (layerFlags & EXYNOS_HWC_DIM_LAYER)
            return true;
        return false;
    };
    void reset() {
        *this = {};
    };
};

uint32_t getDrmMode(uint64_t flags);
uint32_t getDrmMode(const buffer_handle_t handle);

uint32_t is8KVideo(const buffer_handle_t handle, hwc_frect_t srcCrop);

inline int WIDTH(const hwc_rect &rect) { return rect.right - rect.left; }
inline int HEIGHT(const hwc_rect &rect) { return rect.bottom - rect.top; }
inline int WIDTH(const hwc_frect_t &rect) { return (int)(rect.right - rect.left); }
inline int HEIGHT(const hwc_frect_t &rect) { return (int)(rect.bottom - rect.top); }

enum decon_blending halBlendingToDpuBlending(int32_t blending);
enum dpp_rotate halTransformToDpuRot(uint32_t halTransform);
uint64_t halTransformToDrmRot(uint32_t halTransform);

void convertSizeRestriction(const dpp_restriction &restriction, restriction_size &src,
                            restriction_size &dst);
void convertYUVSizeRestriction(const dpp_restriction &restriction, restriction_size &src,
                               restriction_size &dst);
bool isAFBCCompressed(const buffer_handle_t handle);
bool isSAJCCompressed(const buffer_handle_t handle);
bool isSrcCropFloat(hwc_frect &frect);
bool hasHdrInfo(exynos_image &img);
bool hasHdrInfo(android_dataspace dataSpace);
bool hasHdr10Plus(exynos_image &img);

void dumpExynosImage(uint32_t type, exynos_image &img);
void dumpExynosImage(String8 &result, exynos_image &img);
void dumpHandle(uint32_t type, buffer_handle_t h);
void adjustRect(hwc_rect_t &rect, int32_t width, int32_t height);

int hwcFdClose(int fd);
int hwc_print_stack();

inline hwc_rect expand(const hwc_rect &r1, const hwc_rect &r2) {
    hwc_rect i;
    i.top = min(r1.top, r2.top);
    i.bottom = max(r1.bottom, r2.bottom);
    i.left = min(r1.left, r2.left);
    i.right = max(r1.right, r2.right);
    return i;
}

int pixel_align_down(int x, int a);

inline int pixel_align(int x, int a) {
    if ((a != 0) && ((x % a) != 0))
        return ((x) - (x % a)) + a;
    return x;
}

int getBufLength(buffer_handle_t handle, uint32_t planer_num, size_t *length, int format, uint32_t width, uint32_t height);
void printBufLength(buffer_handle_t handle, uint32_t planer_num, size_t *length,
                    int format, uint32_t width, uint32_t height);

struct tm *getHwcFenceTime();

class funcReturnCallback {
  public:
    funcReturnCallback(const std::function<void(void)> cb) : mCb(cb) {}
    ~funcReturnCallback() { mCb(); }

  private:
    const std::function<void(void)> mCb;
};

android_dataspace colorModeToDataspace(android_color_mode_t mode);
bool hasPPC(uint32_t physicalType, uint32_t formatIndex, uint32_t rotIndex);

inline uint32_t getDisplayId(int32_t displayType, int32_t displayIndex = 0) {
    return (displayType << DISPLAYID_MASK_LEN) | displayIndex;
}
int allocParcelData(int *fd, size_t size);
compressionInfo_t getCompressionInfo(buffer_handle_t handle);

android_dataspace_t getRefinedDataspace(int halFormat, android_dataspace_t dataspace);

#endif
