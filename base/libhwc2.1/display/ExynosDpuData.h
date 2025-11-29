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
#ifndef _EXYNOSDPUDATA_H
#define _EXYNOSDPUDATA_H

#include "ExynosMPP.h"
#include "ExynosHWCDebug.h"
#include "ExynosHWCTypes.h"

constexpr uint32_t kIdmaFdNum = 3;

struct exynos_win_config_data {
    enum {
        WIN_STATE_DISABLED = 0,
        WIN_STATE_COLOR,
        WIN_STATE_BUFFER,
        WIN_STATE_UPDATE,
        WIN_STATE_CURSOR,
	WIN_STATE_FINGERPRINT = 0x20000,
    } state = WIN_STATE_DISABLED;

    uint32_t color = 0;
    int fd_idma[kIdmaFdNum] = {-1, -1, -1};
    uint64_t buffer_id = UINT64_MAX;
    void *owner = nullptr;
    int fd_lut = -1;
    ExynosVideoMeta *metaParcel = nullptr;
    int acq_fence = -1;
    int rel_fence = -1;
    float plane_alpha = 1;
    int32_t blending = HWC2_BLEND_MODE_NONE;
    ExynosMPP *assignedMPP = NULL;
    ExynosFormat format;
    uint32_t transform = 0;
    android_dataspace dataspace = HAL_DATASPACE_UNKNOWN;
    bool hdr_enable = false;
    enum dpp_comp_src comp_src = DPP_COMP_SRC_NONE;
    uint32_t min_luminance = 0;
    uint32_t max_luminance = 0;
    struct decon_win_rect block_area = {0, 0, 0, 0};
    struct decon_win_rect transparent_area = {0, 0, 0, 0};
    struct decon_win_rect opaque_area = {0, 0, 0, 0};
    struct decon_frame src = {0, 0, 0, 0, 0, 0};
    struct decon_frame dst = {0, 0, 0, 0, 0, 0};
    bool protection = false;
    compressionInfo_t compressionInfo;
    bool needColorTransform = false;
    android_dataspace hdrLayerDataspace = HAL_DATASPACE_UNKNOWN;

    /* vOTF */
    bool vOtfEnable = false;
    uint32_t vOtfBufIndex = 0;

    /* virtual8K */
    uint32_t split = 0;

    void reset() {
        *this = {};
    };
};

struct exynos_dpu_data {
    int present_fence = -1;
    std::vector<exynos_win_config_data> configs;
    bool enable_win_update = false;
    std::atomic<bool> enable_readback = false;
    bool enable_standalone_writeback = false;
    struct decon_frame win_update_region = {0, 0, 0, 0, 0, 0};
    struct exynos_writeback_info readback_info;
    struct exynos_writeback_info standalone_writeback_info;
#ifdef USE_DQE_INTERFACE
    int fd_dqe = -1;
#endif
    void init(uint32_t configNum) {
        for (uint32_t i = 0; i < configNum; i++) {
            exynos_win_config_data config_data;
            configs.push_back(config_data);
        }
    };

    void reset() {
        present_fence = -1;
        for (uint32_t i = 0; i < configs.size(); i++)
            configs[i].reset();
        /*
         * Should not initialize readback_info
         * readback_info should be initialized after present
         */
    };
    exynos_dpu_data &operator=(const exynos_dpu_data &configs_data) {
        present_fence = configs_data.present_fence;
        if (configs.size() != configs_data.configs.size()) {
            HWC_LOGE_NODISP("invalid config, it has different configs size");
            return *this;
        }
        configs = configs_data.configs;
#ifdef USE_DQE_INTERFACE
        fd_dqe = configs_data.fd_dqe;
#endif
        return *this;
    };
};

#endif
