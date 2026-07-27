 /*
 * Copyright (C) 2013 The Android Open Source Project
 * Copyright@ Samsung Electronics Co. LTD
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

/*!
 * \file      libscaler.cpp
 * \brief     source file for Scaler HAL
 * \author    Sunyoung Kang (sy0816.kang@samsung.com)
 * \date      2013/02/01
 *
 * <b>Revision History: </b>
 * - 2013.02.01 : Sunyoung Kang (sy0816.kang@samsung.com) \n
 *   Create
 * - 2013.04.10 : Cho KyongHo (pullip.cho@samsung.com) \n
 *   Refactoring
 *
 */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <system/graphics.h>

#define LOG_TAG "libexynosscaler"
#include <log/log.h>
#include <cerrno>
#include <cstring>

#include <hardware/exynos/acryl.h>
#include "exynos_scaler.h"

#define ARRSIZE(arr) (sizeof(arr)/sizeof(arr[0]))

class CScalerAcryl {
    Acrylic *mAcrylicHandle = nullptr;
    AcrylicLayer *mLayer = nullptr;

    bool mDRM;
    int mColorSpace;
    int mTransform;
    int mFilter;
    int mFramerate;

    struct frameInfo {
        hwc_rect_t rect;
        size_t len[SC_NUM_OF_PLANES];
        int num_buffers;
    } mSrcInfo, mDstInfo;

    bool SetFormat(frameInfo &info, unsigned int width, unsigned int height, unsigned int v4l2_fmt, bool isSrc);

    inline void SetCrop(hwc_rect_t &rect, unsigned int l, unsigned int t, unsigned int w, unsigned int h) {
        rect.left = l;
        rect.top = t;
        rect.right = l + w;
        rect.bottom = t + h;
    }

    inline size_t GetPlaneSize(frameInfo &info, unsigned int plane_num) {
        return info.len[plane_num];
    }

public:
    CScalerAcryl();
    ~CScalerAcryl();

    Acrylic *getHandle() { return mAcrylicHandle; }
    AcrylicLayer *getLayer() { return mLayer; }

    inline bool Valid() { return mAcrylicHandle != nullptr; }

    bool SetRotate(int rot, int hflip, int vflip);

    inline void SetFilter(unsigned int filter) {
        /* TODO */
        mFilter = filter;
    }

    inline void SetFrameRate(int framerate) {
        /* TODO */
        mFramerate = framerate;
    }

    inline void SetCSCEq(unsigned int colorspace) {
        mColorSpace = colorspace;
    }

    inline int GetCSCEq(void) {
        return mColorSpace;
    }

    inline void SetDRM(bool drm) {
        mDRM = drm;
    }

    inline bool GetDRM(void) {
        return mDRM;
    }

    inline void SetSrcCrop(unsigned int l, unsigned int t, unsigned int w, unsigned int h) {
        SetCrop(mSrcInfo.rect, l, t, w, h);
    }

    inline void SetDstCrop(unsigned int l, unsigned int t, unsigned int w, unsigned int h) {
        SetCrop(mDstInfo.rect, l, t, w, h);
    }

    inline hwc_rect_t& GetSrcCrop(void) { return mSrcInfo.rect; }
    inline hwc_rect_t& GetDstCrop(void) { return mDstInfo.rect; }

    inline bool SetSrcInfo(unsigned int width, unsigned int height, unsigned int v4l2_fmt) {
        return SetFormat(mSrcInfo, width, height, v4l2_fmt, true);
    }

    inline bool SetDstInfo(unsigned int width, unsigned int height, unsigned int v4l2_fmt) {
        return SetFormat(mDstInfo, width, height, v4l2_fmt, false);
    }

    inline size_t GetSrcPlaneSize(unsigned int plane_num) {
        return GetPlaneSize(mSrcInfo, plane_num);
    }

    inline size_t GetDstPlaneSize(unsigned int plane_num) {
        return GetPlaneSize(mDstInfo, plane_num);
    }

    inline int GetSrcPlaneCount(void) {
        return mSrcInfo.num_buffers;
    }

    inline int GetDstPlaneCount(void) {
        return mDstInfo.num_buffers;
    }

    inline int GetTransform(void) {
        return mTransform;
    }
};

#define V4L2_PIX_FMT_ABGR2101010       v4l2_fourcc('A', 'R', '1', '0')
#define V4L2_PIX_FMT_NV12N              v4l2_fourcc('N', 'N', '1', '2')
#define V4L2_PIX_FMT_NV12NT             v4l2_fourcc('T', 'N', '1', '2')
#define V4L2_PIX_FMT_YUV420N            v4l2_fourcc('Y', 'N', '1', '2')
#define V4L2_PIX_FMT_NV12N_10B          v4l2_fourcc('B', 'N', '1', '2')

const int v4l2_to_hal_format_table[][2] = {
    { V4L2_PIX_FMT_RGB32,           HAL_PIXEL_FORMAT_RGBA_8888                      },
    { V4L2_PIX_FMT_RGB24,           HAL_PIXEL_FORMAT_RGB_888                        },
    { V4L2_PIX_FMT_RGB565,          HAL_PIXEL_FORMAT_RGB_565                        },
    { V4L2_PIX_FMT_BGR32,           HAL_PIXEL_FORMAT_BGRA_8888                      },
    { V4L2_PIX_FMT_ABGR2101010,     HAL_PIXEL_FORMAT_RGBA_1010102,                  },
    { V4L2_PIX_FMT_YVU420M,         HAL_PIXEL_FORMAT_EXYNOS_YV12_M                  },
    { V4L2_PIX_FMT_YUV420M,         HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_P_M           },
    { V4L2_PIX_FMT_YVU420,          HAL_PIXEL_FORMAT_YV12                           },
    { V4L2_PIX_FMT_YUV420,          HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_P             },
    { V4L2_PIX_FMT_YUV420N,         HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_PN            },
    { V4L2_PIX_FMT_NV16,            HAL_PIXEL_FORMAT_YCbCr_422_SP                   },
    { V4L2_PIX_FMT_NV12,            HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP            },
    { V4L2_PIX_FMT_NV12N,           HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN           },
    { V4L2_PIX_FMT_NV12M,           HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M          },
    { V4L2_PIX_FMT_NV12N_10B,       HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_S10B      },
    { V4L2_PIX_FMT_YUYV,            HAL_PIXEL_FORMAT_YCbCr_422_I                    },
    { V4L2_PIX_FMT_UYVY,            HAL_PIXEL_FORMAT_EXYNOS_CbYCrY_422_I            },
    { V4L2_PIX_FMT_NV61,            HAL_PIXEL_FORMAT_EXYNOS_YCrCb_422_SP            },
    { V4L2_PIX_FMT_NV21M,           HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M          },
    { V4L2_PIX_FMT_NV21,            HAL_PIXEL_FORMAT_YCrCb_420_SP                   },
    { V4L2_PIX_FMT_NV12MT_16X16,    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_TILED    },
    { V4L2_PIX_FMT_NV12NT,          HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_TILED     },
    { V4L2_PIX_FMT_YVYU,            HAL_PIXEL_FORMAT_EXYNOS_YCrCb_422_I             },
    { V4L2_PIX_FMT_VYUY,            HAL_PIXEL_FORMAT_EXYNOS_CrYCbY_422_I            },
    { V4L2_PIX_FMT_NV12M_P010,      HAL_PIXEL_FORMAT_EXYNOS_YCbCr_P010_M            },
    { V4L2_PIX_FMT_NV12_P010,       HAL_PIXEL_FORMAT_YCBCR_P010                     },
    { V4L2_PIX_FMT_YUV422P,         HAL_PIXEL_FORMAT_EXYNOS_YCbCr_422_P             },
    { V4L2_PIX_FMT_NV12M_SBWC_8B,   HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_SBWC     },
    { V4L2_PIX_FMT_NV12N_SBWC_8B,   HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_SBWC      },
    { V4L2_PIX_FMT_NV12M_SBWC_10B,  HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC },
    { V4L2_PIX_FMT_NV12N_SBWC_10B,  HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_10B_SBWC  },
    { V4L2_PIX_FMT_NV21M_SBWC_8B,   HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_SBWC     },
    { V4L2_PIX_FMT_NV21M_SBWC_10B,  HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_10B_SBWC },
    { V4L2_PIX_FMT_NV12M_SBWCL_8B_L50,   HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_SBWC_L50     },
    { V4L2_PIX_FMT_NV12M_SBWCL_8B_L75,   HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_SBWC_L75     },
    { V4L2_PIX_FMT_NV12M_SBWCL_10B_L40,  HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC_L40 },
    { V4L2_PIX_FMT_NV12M_SBWCL_10B_L60,  HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC_L60 },
    { V4L2_PIX_FMT_NV12M_SBWCL_10B_L80,  HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC_L80 },
    { V4L2_PIX_FMT_NV12M_SBWCL_32_8B,   HAL_PIXEL_FORMAT_EXYNOS_420_SP_M_32_SBWC_L          },
    { V4L2_PIX_FMT_NV12M_SBWCL_64_8B,   HAL_PIXEL_FORMAT_EXYNOS_420_SP_M_64_SBWC_L          },
    { V4L2_PIX_FMT_NV12N_SBWCL_32_8B,   HAL_PIXEL_FORMAT_EXYNOS_420_SPN_32_SBWC_L           },
    { V4L2_PIX_FMT_NV12N_SBWCL_64_8B,   HAL_PIXEL_FORMAT_EXYNOS_420_SPN_64_SBWC_L           },
    { V4L2_PIX_FMT_NV12M_SBWCL_32_10B,  HAL_PIXEL_FORMAT_EXYNOS_420_SP_M_10B_32_SBWC_L      },
    { V4L2_PIX_FMT_NV12M_SBWCL_64_10B,  HAL_PIXEL_FORMAT_EXYNOS_420_SP_M_10B_64_SBWC_L      },
    { V4L2_PIX_FMT_NV12N_SBWCL_32_10B,  HAL_PIXEL_FORMAT_EXYNOS_420_SPN_10B_32_SBWC_L       },
    { V4L2_PIX_FMT_NV12N_SBWCL_64_10B,  HAL_PIXEL_FORMAT_EXYNOS_420_SPN_10B_64_SBWC_L       },
    { V4L2_PIX_FMT_NV12N_SBWC_256_8B,   HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_256_SBWC      },
    { V4L2_PIX_FMT_NV12N_SBWC_256_10B,  HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_10B_256_SBWC  },
};

const int hal_to_v4l2_format_table[][2] = {
    { HAL_PIXEL_FORMAT_RGBA_8888,                      V4L2_PIX_FMT_RGB32           },
    { HAL_PIXEL_FORMAT_RGBX_8888,                      V4L2_PIX_FMT_RGB32           },
    { HAL_PIXEL_FORMAT_RGB_888,                        V4L2_PIX_FMT_RGB24           },
    { HAL_PIXEL_FORMAT_RGB_565,                        V4L2_PIX_FMT_RGB565          },
    { HAL_PIXEL_FORMAT_BGRA_8888,                      V4L2_PIX_FMT_BGR32           },
    { HAL_PIXEL_FORMAT_RGBA_1010102,                   V4L2_PIX_FMT_ABGR2101010     },
    { HAL_PIXEL_FORMAT_EXYNOS_YV12_M,                  V4L2_PIX_FMT_YVU420M         },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_P_M,           V4L2_PIX_FMT_YUV420M         },
    { HAL_PIXEL_FORMAT_YV12,                           V4L2_PIX_FMT_YVU420          },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_P,             V4L2_PIX_FMT_YUV420          },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_PN,            V4L2_PIX_FMT_YUV420N         },
    { HAL_PIXEL_FORMAT_YCbCr_422_SP,                   V4L2_PIX_FMT_NV16            },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP,            V4L2_PIX_FMT_NV12            },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN,           V4L2_PIX_FMT_NV12N           },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M,          V4L2_PIX_FMT_NV12M           },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_PRIV,     V4L2_PIX_FMT_NV12M           },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_S10B,     V4L2_PIX_FMT_NV12M           },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_S10B,      V4L2_PIX_FMT_NV12N_10B       },
    { HAL_PIXEL_FORMAT_YCbCr_422_I,                    V4L2_PIX_FMT_YUYV            },
    { HAL_PIXEL_FORMAT_EXYNOS_CbYCrY_422_I,            V4L2_PIX_FMT_UYVY            },
    { HAL_PIXEL_FORMAT_EXYNOS_YCrCb_422_SP,            V4L2_PIX_FMT_NV61            },
    { HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M,          V4L2_PIX_FMT_NV21M           },
    { HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL,     V4L2_PIX_FMT_NV21M           },
    { HAL_PIXEL_FORMAT_YCrCb_420_SP,                   V4L2_PIX_FMT_NV21            },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_TILED,    V4L2_PIX_FMT_NV12MT_16X16    },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_TILED,     V4L2_PIX_FMT_NV12NT          },
    { HAL_PIXEL_FORMAT_EXYNOS_YCrCb_422_I,             V4L2_PIX_FMT_YVYU            },
    { HAL_PIXEL_FORMAT_EXYNOS_CrYCbY_422_I,            V4L2_PIX_FMT_VYUY            },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_P010_M,            V4L2_PIX_FMT_NV12M_P010      },
    { HAL_PIXEL_FORMAT_YCBCR_P010,                     V4L2_PIX_FMT_NV12_P010       },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_422_P,             V4L2_PIX_FMT_YUV422P         },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_SBWC,     V4L2_PIX_FMT_NV12M_SBWC_8B   },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_SBWC,      V4L2_PIX_FMT_NV12N_SBWC_8B   },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC, V4L2_PIX_FMT_NV12M_SBWC_10B  },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_10B_SBWC,  V4L2_PIX_FMT_NV12N_SBWC_10B  },
    { HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_SBWC,     V4L2_PIX_FMT_NV21M_SBWC_8B   },
    { HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_10B_SBWC, V4L2_PIX_FMT_NV21M_SBWC_10B  },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_SBWC_L50, V4L2_PIX_FMT_NV12M_SBWCL_8B_L50      },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_SBWC_L75, V4L2_PIX_FMT_NV12M_SBWCL_8B_L75      },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC_L40, V4L2_PIX_FMT_NV12M_SBWCL_10B_L40 },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC_L60, V4L2_PIX_FMT_NV12M_SBWCL_10B_L60 },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC_L80, V4L2_PIX_FMT_NV12M_SBWCL_10B_L80 },
    { HAL_PIXEL_FORMAT_EXYNOS_420_SP_M_32_SBWC_L,      V4L2_PIX_FMT_NV12M_SBWCL_32_8B },
    { HAL_PIXEL_FORMAT_EXYNOS_420_SP_M_64_SBWC_L,      V4L2_PIX_FMT_NV12M_SBWCL_64_8B },
    { HAL_PIXEL_FORMAT_EXYNOS_420_SPN_32_SBWC_L,       V4L2_PIX_FMT_NV12N_SBWCL_32_8B },
    { HAL_PIXEL_FORMAT_EXYNOS_420_SPN_64_SBWC_L,       V4L2_PIX_FMT_NV12N_SBWCL_64_8B },
    { HAL_PIXEL_FORMAT_EXYNOS_420_SP_M_10B_32_SBWC_L,  V4L2_PIX_FMT_NV12M_SBWCL_32_10B },
    { HAL_PIXEL_FORMAT_EXYNOS_420_SP_M_10B_64_SBWC_L,  V4L2_PIX_FMT_NV12M_SBWCL_64_10B },
    { HAL_PIXEL_FORMAT_EXYNOS_420_SPN_10B_32_SBWC_L,   V4L2_PIX_FMT_NV12N_SBWCL_32_10B },
    { HAL_PIXEL_FORMAT_EXYNOS_420_SPN_10B_64_SBWC_L,   V4L2_PIX_FMT_NV12N_SBWCL_64_10B },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_256_SBWC,      V4L2_PIX_FMT_NV12N_SBWC_256_8B  },
    { HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_10B_256_SBWC,  V4L2_PIX_FMT_NV12N_SBWC_256_10B },
};

const uint32_t v4l2_to_hal_dataspace_table[][2] = {
    { V4L2_COLORSPACE_SRGB,       HAL_DATASPACE_STANDARD_BT709 | HAL_DATASPACE_RANGE_FULL        },
    { V4L2_COLORSPACE_REC709,     HAL_DATASPACE_STANDARD_BT709 | HAL_DATASPACE_RANGE_LIMITED     },
    { V4L2_COLORSPACE_JPEG,       HAL_DATASPACE_STANDARD_BT601_625 | HAL_DATASPACE_RANGE_FULL    },
    { V4L2_COLORSPACE_JPEG,       HAL_DATASPACE_STANDARD_BT601_525 | HAL_DATASPACE_RANGE_FULL    },
    { V4L2_COLORSPACE_SMPTE170M,  HAL_DATASPACE_STANDARD_BT601_625 | HAL_DATASPACE_RANGE_LIMITED },
    { V4L2_COLORSPACE_SMPTE170M,  HAL_DATASPACE_STANDARD_BT601_525 | HAL_DATASPACE_RANGE_LIMITED },
    { V4L2_COLORSPACE_BT2020,     HAL_DATASPACE_STANDARD_BT2020 | HAL_DATASPACE_RANGE_LIMITED    },
    { V4L2_COLORSPACE_SRGB,       HAL_DATASPACE_STANDARD_FILM | HAL_DATASPACE_RANGE_FULL         },
    { V4L2_COLORSPACE_REC709,     HAL_DATASPACE_STANDARD_FILM | HAL_DATASPACE_RANGE_LIMITED      },
    { V4L2_COLORSPACE_SRGB,       HAL_DATASPACE_SRGB                                             },
    { V4L2_COLORSPACE_JPEG,       HAL_DATASPACE_JFIF                                             },
    { V4L2_COLORSPACE_SMPTE170M,  HAL_DATASPACE_BT601_525                                        },
    { V4L2_COLORSPACE_SMPTE170M,  HAL_DATASPACE_BT601_625                                        },
    { V4L2_COLORSPACE_REC709,     HAL_DATASPACE_BT709                                            },
};

int v4l2_pixfmt_to_hal(int v4l2_pixel_format)
{
    for (uint32_t i = 0; i < ARRSIZE(v4l2_to_hal_format_table); i++) {
        if (v4l2_to_hal_format_table[i][0] == v4l2_pixel_format)
            return v4l2_to_hal_format_table[i][1];
    }

    ALOGE("Unsupported v4l2 pixel format(%#x)", v4l2_pixel_format);

    return 0;
}

int hal_pixfmt_to_v4l2(int hal_pixel_format)
{
    for (uint32_t i = 0; i < ARRSIZE(hal_to_v4l2_format_table); i++) {
        if (hal_to_v4l2_format_table[i][0] == hal_pixel_format)
            return hal_to_v4l2_format_table[i][1];
    }

    ALOGE("Unsupported hal pixel format(%#x)", hal_pixel_format);

    return 0;
}

int v4l2_dataspace_to_hal(unsigned int v4l2_dataspace, unsigned int *hal_colorspace)
{
    for (uint32_t i = 0; i < ARRSIZE(v4l2_to_hal_dataspace_table); i++) {
        if (v4l2_to_hal_dataspace_table[i][0] == v4l2_dataspace) {
            *hal_colorspace = v4l2_to_hal_dataspace_table[i][1];
            return 0;
        }
    }

    ALOGE("Unsupported v4l2 dataspace(%#x)", v4l2_dataspace);

    return -1;
}

void *exynos_sc_create(int dev_num)
{
    CScalerAcryl *sc = new CScalerAcryl();
    if (!sc) {
        ALOGE("Failed to allocate a Scaler handle for instance %d", dev_num);
        return nullptr;
    }

    if (!sc->Valid()) {
        ALOGE("Failed to create a Scaler handle for instance %d", dev_num);
        delete sc;
        return nullptr;
    }

    return reinterpret_cast<void *>(sc);
}

int exynos_sc_destroy(void *handle)
{
    CScalerAcryl *sc = reinterpret_cast<CScalerAcryl *>(handle);
    if (!sc)
        return -1;

    delete sc;

    return 0;
}

int exynos_sc_set_csc_property(
        void        *handle,
        unsigned int __unused csc_range,
        unsigned int v4l2_colorspace,
        unsigned int filter)

{
    CScalerAcryl *sc = reinterpret_cast<CScalerAcryl *>(handle);
    if (!sc)
        return -1;

    unsigned int hal_colorspace;
    if (v4l2_dataspace_to_hal(v4l2_colorspace, &hal_colorspace) < 0)
        return -1;

    sc->SetCSCEq(hal_colorspace);
    sc->SetFilter(filter);

    return 0;
}

int exynos_sc_set_src_format(
        void        *handle,
        unsigned int width,
        unsigned int height,
        unsigned int crop_left,
        unsigned int crop_top,
        unsigned int crop_width,
        unsigned int crop_height,
        unsigned int v4l2_colorformat,
        unsigned int __unused cacheable,
        unsigned int mode_drm,
        unsigned int __unused premultiplied)
{
    CScalerAcryl *sc = reinterpret_cast<CScalerAcryl *>(handle);
    if (!sc)
        return -1;

    sc->SetDRM(mode_drm != 0);
    sc->SetSrcCrop(crop_left, crop_top, crop_width, crop_height);
    sc->SetSrcInfo(width, height, v4l2_colorformat);

    AcrylicLayer *layer = sc->getLayer();

    if (!layer->setImageDimension(width, height))
        return -1;

    if (!layer->setImageType(v4l2_pixfmt_to_hal(v4l2_colorformat), sc->GetCSCEq()))
        return -1;

    return 0;
}

int exynos_sc_set_dst_format(
        void        *handle,
        unsigned int width,
        unsigned int height,
        unsigned int crop_left,
        unsigned int crop_top,
        unsigned int crop_width,
        unsigned int crop_height,
        unsigned int v4l2_colorformat,
        unsigned int __unused cacheable,
        unsigned int mode_drm,
        unsigned int __unused premultiplied)
{
    CScalerAcryl *sc = reinterpret_cast<CScalerAcryl *>(handle);
    if (!sc)
        return -1;

    sc->SetDRM(mode_drm != 0);
    sc->SetDstCrop(crop_left, crop_top, crop_width, crop_height);
    sc->SetDstInfo(width, height, v4l2_colorformat);

    Acrylic *acrylHandle = sc->getHandle();

    if (!acrylHandle->setCanvasDimension(width, height))
        return -1;

    if (!acrylHandle->setCanvasImageType(v4l2_pixfmt_to_hal(v4l2_colorformat), sc->GetCSCEq()))
        return -1;

    return 0;
}

int exynos_sc_set_rotation(
        void *handle,
        int rot,
        int flip_h,
        int flip_v)
{
    CScalerAcryl *sc = reinterpret_cast<CScalerAcryl *>(handle);
    if (!sc)
        return -1;

    return sc->SetRotate(rot, flip_h, flip_v) ? 0 : -1;
}

void exynos_sc_set_framerate(
        void *handle,
        int framerate)
{
    CScalerAcryl *sc = reinterpret_cast<CScalerAcryl *>(handle);
    if (!sc)
        return;

    sc->SetFrameRate(framerate);
}

int exynos_sc_set_src_addr(
        void *handle,
        void *addr[SC_NUM_OF_PLANES],
        int mem_type,
        int __unused acquireFenceFd)
{
    CScalerAcryl *sc = reinterpret_cast<CScalerAcryl *>(handle);
    if (!sc)
        return -1;

    AcrylicLayer *layer = sc->getLayer();
    int buf_count = sc->GetSrcPlaneCount();

    if (mem_type == V4L2_MEMORY_DMABUF) {
        int fd[SC_NUM_OF_PLANES];
        size_t len[SC_NUM_OF_PLANES];
        off_t offset[SC_NUM_OF_PLANES];

        for (int i = 0; i < buf_count; i++) {
            fd[i] = reinterpret_cast<intptr_t>(addr[i]);
            len[i] = sc->GetSrcPlaneSize(i);
            offset[i] = 0;
        }

        if (!layer->setImageBuffer(fd, len, offset, buf_count, -1, 0))
            return -1;
    } else if (mem_type == V4L2_MEMORY_USERPTR) {
        size_t len[SC_NUM_OF_PLANES];

        for (int i = 0; i < buf_count; i++)
            len[i] = sc->GetSrcPlaneSize(i);

        if (!layer->setImageBuffer(addr, len, buf_count, 0))
            return -1;
    } else
        return -1;

    return 0;
}

int exynos_sc_set_dst_addr(
        void *handle,
        void *addr[SC_NUM_OF_PLANES],
        int mem_type,
        int __unused acquireFenceFd)
{
    CScalerAcryl *sc = reinterpret_cast<CScalerAcryl *>(handle);
    if (!sc)
        return -1;

    Acrylic *acrylHandle = sc->getHandle();
    int buf_count = sc->GetDstPlaneCount();
    uint32_t attr = sc->GetDRM() ? AcrylicCanvas::ATTR_PROTECTED : AcrylicCanvas::ATTR_NONE;

    if (mem_type == V4L2_MEMORY_DMABUF)
    {
        int fd[SC_NUM_OF_PLANES];
        size_t len[SC_NUM_OF_PLANES];
        off_t offset[SC_NUM_OF_PLANES];

        for (int i = 0; i < buf_count; i++) {
            fd[i] = reinterpret_cast<intptr_t>(addr[i]);
            len[i] = sc->GetDstPlaneSize(i);
            offset[i] = 0;
        }

        if (!acrylHandle->setCanvasBuffer(fd, len, offset, buf_count, -1, attr))
            return -1;
    } else if (mem_type == V4L2_MEMORY_USERPTR) {
        size_t len[SC_NUM_OF_PLANES];

        for (int i = 0; i < buf_count; i++)
            len[i] = sc->GetDstPlaneSize(i);

        if (!acrylHandle->setCanvasBuffer(addr, len, buf_count, attr))
            return -1;
    } else
        return -1;

    return 0;
}

int exynos_sc_convert(void *handle)
{
    CScalerAcryl *sc = reinterpret_cast<CScalerAcryl *>(handle);
    if (!sc)
        return -1;

    AcrylicLayer *layer = sc->getLayer();

    layer->setCompositMode(HWC_BLENDING_NONE, 255, 0);
    layer->setCompositArea(sc->GetSrcCrop(), sc->GetDstCrop(), sc->GetTransform(), 0);

    Acrylic *acrylHandle = sc->getHandle();
    if (!acrylHandle->execute(nullptr))
        return -1;

    return 0;
}

/* CScalerAcryl */
struct PixFormat {
    unsigned int pixfmt;
    char planes;
    unsigned short bit_pp[3];
};

const static PixFormat g_pixfmt_table[] = {
    {V4L2_PIX_FMT_RGB32,        1, {32, 0, 0}, },
    {V4L2_PIX_FMT_BGR32,        1, {32, 0, 0}, },
    {V4L2_PIX_FMT_RGB565,       1, {16, 0, 0}, },
    {V4L2_PIX_FMT_RGB555X,      1, {16, 0, 0}, },
    {V4L2_PIX_FMT_RGB444,       1, {16, 0, 0}, },
    {V4L2_PIX_FMT_ABGR2101010,  1, {32, 0, 0}, },
    {V4L2_PIX_FMT_YUYV,         1, {16, 0, 0}, },
    {V4L2_PIX_FMT_YVYU,         1, {16, 0, 0}, },
    {V4L2_PIX_FMT_UYVY,         1, {16, 0, 0}, },
    {V4L2_PIX_FMT_NV16,         1, {16, 0, 0}, },
    {V4L2_PIX_FMT_NV61,         1, {16, 0, 0}, },
    {V4L2_PIX_FMT_YUV420,       1, {12, 0, 0}, },
    {V4L2_PIX_FMT_YVU420,       1, {12, 0, 0}, },
    {V4L2_PIX_FMT_NV12M,        2, {8, 4, 0}, },
    {V4L2_PIX_FMT_NV21M,        2, {8, 4, 0}, },
    {v4l2_fourcc('V', 'M', '1', '2'), 2, {8, 4, 0}, },
    {V4L2_PIX_FMT_NV12,         1, {12, 0, 0}, },
    {V4L2_PIX_FMT_NV21,         1, {12, 0, 0}, },
    {v4l2_fourcc('N', 'M', '2', '1'), 2, {8, 4, 0}, },
    {V4L2_PIX_FMT_YUV420M,      3, {8, 2, 2}, },
    {V4L2_PIX_FMT_YVU420M,      3, {8, 2, 2}, },
    {V4L2_PIX_FMT_NV12M_P010,   2, {16, 8, 0}, },
    {V4L2_PIX_FMT_NV12_P010,    1, {24, 0, 0}, },
    {V4L2_PIX_FMT_NV24,         1, {24, 0, 0}, },
    {V4L2_PIX_FMT_NV42,         1, {24, 0, 0}, },
    {V4L2_PIX_FMT_YUV422P,      1, {16, 0, 0}, },
    /*
     * In SBWC foramt, bit_pp is meaningless to calculate size.
     * So, in this, meaning of bit_pp is different.
     *      1. bit_pp[0] : if 0, this format is SBWC format.
     *                     if 27, this format is SBWC format of version 2.7.
     *      2. bit_pp[1] : this format is 8 or 10 bit format.
     *      3. bit_pp[2] : it means blocksize of SBWC lossy.
     *                     In SBWC version 2.7, this means alignment.
     */
    {V4L2_PIX_FMT_NV12M_SBWC_8B, 2, {0, 8, 0}, },
    {V4L2_PIX_FMT_NV21M_SBWC_8B, 2, {0, 8, 0}, },
    {V4L2_PIX_FMT_NV12N_SBWC_8B, 1, {0, 8, 0}, },
    {V4L2_PIX_FMT_NV12M_SBWC_10B, 2, {0, 10, 0}, },
    {V4L2_PIX_FMT_NV21M_SBWC_10B, 2, {0, 10, 0}, },
    {V4L2_PIX_FMT_NV12N_SBWC_10B, 1, {0, 10, 0}, },
    {V4L2_PIX_FMT_NV12M_SBWCL_8B_L50, 2, {0, 8, 64}, },
    {V4L2_PIX_FMT_NV12M_SBWCL_8B_L75, 2, {0, 8, 96}, },
    {V4L2_PIX_FMT_NV12M_SBWCL_10B_L40, 2, {0, 10, 64}, },
    {V4L2_PIX_FMT_NV12M_SBWCL_10B_L60, 2, {0, 10, 96}, },
    {V4L2_PIX_FMT_NV12M_SBWCL_10B_L80, 2, {0, 10, 128}, },
    {V4L2_PIX_FMT_NV12M_SBWCL_32_8B, 2, {27, 8, 32}, },
    {V4L2_PIX_FMT_NV12M_SBWCL_64_8B, 2, {27, 8, 64}, },
    {V4L2_PIX_FMT_NV12N_SBWCL_32_8B, 1, {27, 8, 32}, },
    {V4L2_PIX_FMT_NV12N_SBWCL_64_8B, 1, {27, 8, 64}, },
    {V4L2_PIX_FMT_NV12M_SBWCL_32_10B, 2, {27, 10, 32}, },
    {V4L2_PIX_FMT_NV12M_SBWCL_64_10B, 2, {27, 10, 64}, },
    {V4L2_PIX_FMT_NV12N_SBWCL_32_10B, 1, {27, 10, 32}, },
    {V4L2_PIX_FMT_NV12N_SBWCL_64_10B, 1, {27, 10, 64}, },
    {V4L2_PIX_FMT_NV12N_SBWC_256_8B,  1, {27, 8, 256}, },
    {V4L2_PIX_FMT_NV12N_SBWC_256_10B, 1, {27, 10, 256}, },
};


CScalerAcryl::CScalerAcryl() : mDRM(false), mColorSpace(0), mTransform(0), mFilter(0), mFramerate(0)
{
    mAcrylicHandle = Acrylic::createScaler();
    if (mAcrylicHandle == nullptr) {
        ALOGE("Failed to create default scaler");
    } else {
        mLayer = mAcrylicHandle->createLayer();
        if (mLayer == nullptr) {
            ALOGE("Failed to create layer");
            delete mAcrylicHandle;
            mAcrylicHandle = nullptr;
        }
    }

    memset(&mSrcInfo, 0, sizeof(frameInfo));
    memset(&mDstInfo, 0, sizeof(frameInfo));
}

CScalerAcryl::~CScalerAcryl()
{
    delete mLayer;
    delete mAcrylicHandle;
}

/* helper macros */
#ifndef __ALIGN_UP
#define __ALIGN_UP(x, a)		(((x) + ((a) - 1)) & ~((a) - 1))
#endif

/* SBWC */
/* w: width, h: height, b: block size, a: alignment, s: stride */
#define SBWC_STRIDE(w, b, a)		__ALIGN_UP(((b) * (((w) + 31) / 32)), a)
#define SBWC_HEADER_STRIDE(w)		((((((w) + 63) / 64) + 15) / 16) * 16)
#define SBWC_Y_SIZE(s, h)		(((s) * ((__ALIGN_UP((h), 16) + 3) / 4)) + 64)
#define SBWC_CBCR_SIZE(s, h)		(((s) * (((__ALIGN_UP((h), 16) / 2) + 3) / 4)) + 64)
#define SBWC_Y_HEADER_SIZE(w, h)	__ALIGN_UP(((SBWC_HEADER_STRIDE(w) * ((__ALIGN_UP((h), 16) + 3) / 4)) + 256), 32)
#define SBWC_CBCR_HEADER_SIZE(w, h)	((SBWC_HEADER_STRIDE(w) * (((__ALIGN_UP((h), 16) / 2) + 3) / 4)) + 128)

/* SBWC 32B align */
#define SBWC_8B_STRIDE(w)		SBWC_STRIDE(w, 128, 1)
#define SBWC_10B_STRIDE(w)		SBWC_STRIDE(w, 160, 1)

#define SBWC_8B_Y_SIZE(w, h)		SBWC_Y_SIZE(SBWC_8B_STRIDE(w), h)
#define SBWC_8B_Y_HEADER_SIZE(w, h)	SBWC_Y_HEADER_SIZE(w, h)
#define SBWC_8B_CBCR_SIZE(w, h)		SBWC_CBCR_SIZE(SBWC_8B_STRIDE(w), h)
#define SBWC_8B_CBCR_HEADER_SIZE(w, h)	SBWC_CBCR_HEADER_SIZE(w, h)

#define SBWC_10B_Y_SIZE(w, h)		SBWC_Y_SIZE(SBWC_10B_STRIDE(w), h)
#define SBWC_10B_Y_HEADER_SIZE(w, h)	__ALIGN_UP((((__ALIGN_UP((w), 32) * __ALIGN_UP((h), 16) * 2) + 256) - SBWC_10B_Y_SIZE(w, h)), 32)
#define SBWC_10B_CBCR_SIZE(w, h)	SBWC_CBCR_SIZE(SBWC_10B_STRIDE(w), h)
#define SBWC_10B_CBCR_HEADER_SIZE(w, h)	(((__ALIGN_UP((w), 32) * __ALIGN_UP((h), 16)) + 256) - SBWC_10B_CBCR_SIZE(w, h))

/* SBWC 256B align */
#define SBWC_256_8B_STRIDE(w)			SBWC_STRIDE(w, 128, 256)
#define SBWC_256_10B_STRIDE(w)			SBWC_STRIDE(w, 256, 1)

#define SBWC_256_8B_Y_SIZE(w, h)		SBWC_Y_SIZE(SBWC_256_8B_STRIDE(w), h)
#define SBWC_256_8B_Y_HEADER_SIZE(w, h)		SBWC_Y_HEADER_SIZE(w, h)
#define SBWC_256_8B_CBCR_SIZE(w, h)		SBWC_CBCR_SIZE(SBWC_256_8B_STRIDE(w), h)
#define SBWC_256_8B_CBCR_HEADER_SIZE(w, h)	SBWC_CBCR_HEADER_SIZE(w, h)

#define SBWC_256_10B_Y_SIZE(w, h)		SBWC_Y_SIZE(SBWC_256_10B_STRIDE(w), h)
#define SBWC_256_10B_Y_HEADER_SIZE(w, h)	SBWC_Y_HEADER_SIZE(w, h)
#define SBWC_256_10B_CBCR_SIZE(w, h)		SBWC_CBCR_SIZE(SBWC_256_10B_STRIDE(w), h)
#define SBWC_256_10B_CBCR_HEADER_SIZE(w, h)	SBWC_CBCR_HEADER_SIZE(w, h)

/* SBWC lossy buffer size */
#define SBWCL_BLOCK_COUNT(w, h)		(ALIGN(w, 32) * ALIGN(h, 4) / 128)
#define SBWCL_Y_SIZE(w, h, r)       (SBWCL_STRIDE(w, r) * ((ALIGN(h, 16) + 3) / 4) + 64)
#define SBWCL_CBCR_SIZE(w, h, r)    (SBWCL_STRIDE(w, r) * (((ALIGN(h, 16) / 2) + 3) / 4) + 64)
#define SBWCL_STRIDE(w, r)		(ALIGN(w, 32) * (r))

/* SBWC Lossy v2.7 32B/64B align */
#define SBWCL_32_STRIDE(w)      (96 * (((w) + 31) / 32))
#define SBWCL_64_STRIDE(w)      (128 * (((w) + 31) / 32))
#define SBWCL_HEADER_STRIDE(w)      ((((((w) + 63) / 64) + 15) / 16) * 16)

#define SBWCL_32_Y_SIZE(w, h)       (SBWCL_32_STRIDE(w) * (((h) + 3) / 4))
#define SBWCL_32_CBCR_SIZE(w, h)    (SBWCL_32_STRIDE(w) * ((((h) / 2) + 3) / 4))

#define SBWCL_64_Y_SIZE(w, h)       (SBWCL_64_STRIDE(w) * (((h) + 3) / 4))
#define SBWCL_64_CBCR_SIZE(w, h)    (SBWCL_64_STRIDE(w) * ((((h) / 2) + 3) / 4))

#define SBWCL_Y_HEADER_SIZE(w, h)   ((SBWCL_HEADER_STRIDE(w) * (((h) + 3) / 4)) + 64)
#define SBWCL_CBCR_HEADER_SIZE(w, h)    ((SBWCL_HEADER_STRIDE(w) * ((((h) / 2) + 3) / 4)) + 64)

#define pixIsSbwc(pixFmt)                  ((pixFmt)->bit_pp[0] == 0)
#define pixIsSbwc27(pixFmt)                ((pixFmt)->bit_pp[0] == 27)
#define pixGetBitOfSbwc(pixFmt)            ((pixFmt)->bit_pp[1])
#define pixIsSbwcLossless(pixFmt)          (pixIsSbwc(pixFmt) && (pixFmt)->bit_pp[2] == 0)
#define pixIsSbwcLossyWithComp(pixFmt)     (pixIsSbwc(pixFmt) && (pixFmt)->bit_pp[2] != 0)
#define pixGetBlockSizeOfSbwcLossy(pixFmt) ((pixFmt)->bit_pp[2])
#define pixGetByte32NumOfSbwcLossy(pixFmt) ((pixFmt)->bit_pp[2] / 32)
#define pixGetAlignmentOfSbwc(pixFmt)      ((pixFmt)->bit_pp[2])

#ifdef SCALER_ALIGN_RESTRICTION

#define SCALER_EXT_ALIGN       128
#define SCALER_EXT_SIZE        512

#endif

bool CScalerAcryl::SetFormat(frameInfo &info, unsigned int width, unsigned int height,
                             unsigned int v4l2_fmt, bool isSrc) {
    (void)isSrc; // prevent unused warning instead of using [[maybe_unused]] of C++17
    const PixFormat *pixfmt = nullptr;

    for (size_t i = 0; i < ARRSIZE(g_pixfmt_table); i++) {
        if (g_pixfmt_table[i].pixfmt == v4l2_fmt) {
            pixfmt = &g_pixfmt_table[i];
            break;
        }
    }

    if (!pixfmt) {
        ALOGE("Format %#x is not supported", v4l2_fmt);
        return false;
    }

    if (pixIsSbwc(pixfmt) || pixIsSbwc27(pixfmt)) {
        if (pixIsSbwcLossyWithComp(pixfmt)) {
            info.len[0] = SBWCL_Y_SIZE(width, height, pixGetByte32NumOfSbwcLossy(pixfmt));
            info.len[1] = SBWCL_CBCR_SIZE(width, height, pixGetByte32NumOfSbwcLossy(pixfmt));
        } else if (pixIsSbwcLossless(pixfmt)) {
            if (pixGetBitOfSbwc(pixfmt) == 8) {
                info.len[0] = SBWC_8B_Y_SIZE(width, height) +
                              SBWC_8B_Y_HEADER_SIZE(width, height);
                info.len[1] = SBWC_8B_CBCR_SIZE(width, height) +
                              SBWC_8B_CBCR_HEADER_SIZE(width, height);
            } else {
                info.len[0] = SBWC_10B_Y_SIZE(width, height) +
                              SBWC_10B_Y_HEADER_SIZE(width, height);
                info.len[1] = SBWC_10B_CBCR_SIZE(width, height) +
                              SBWC_10B_CBCR_HEADER_SIZE(width, height);
            }
        } else {
            if (pixGetAlignmentOfSbwc(pixfmt) == 32) {
                info.len[0] = SBWCL_32_Y_SIZE(width, height) +
                              SBWCL_Y_HEADER_SIZE(width, height);
                info.len[1] = SBWCL_32_CBCR_SIZE(width, height) +
                              SBWCL_CBCR_HEADER_SIZE(width, height);
            } else if (pixGetAlignmentOfSbwc(pixfmt) == 64) {
                info.len[0] = SBWCL_64_Y_SIZE(width, height) +
                              SBWCL_Y_HEADER_SIZE(width, height);
                info.len[1] = SBWCL_64_CBCR_SIZE(width, height) +
                              SBWCL_CBCR_HEADER_SIZE(width, height);
            } else if (pixGetAlignmentOfSbwc(pixfmt) == 256) {
                if (pixGetBitOfSbwc(pixfmt) == 8) {
                    info.len[0] = SBWC_256_8B_Y_SIZE(width, height) +
                                  SBWC_256_8B_Y_HEADER_SIZE(width, height);
                    info.len[1] = SBWC_256_8B_CBCR_SIZE(width, height) +
                                  SBWC_256_8B_CBCR_HEADER_SIZE(width, height);
                } else {
                    info.len[0] = SBWC_256_10B_Y_SIZE(width, height) +
                                  SBWC_256_10B_Y_HEADER_SIZE(width, height);
                    info.len[1] = SBWC_256_10B_CBCR_SIZE(width, height) +
                                  SBWC_256_10B_CBCR_HEADER_SIZE(width, height);
                }
            }
        }
        if (pixfmt->planes == 1) {
            info.len[0] += info.len[1];
            info.len[1] = 0;
        }
    } else {
        for (int i = 0; i < pixfmt->planes; i++) {
            if (((pixfmt->bit_pp[i] * width) % 8) != 0) {
                ALOGE("Plane %d of format %#x must have even width", i, v4l2_fmt);
                return false;
            }
            info.len[i] = (pixfmt->bit_pp[i] * width * height) / 8;
        }
        if (pixfmt->pixfmt == V4L2_PIX_FMT_YVU420) {
            unsigned int y_size = width * height;
            unsigned int c_span = ALIGN(width / 2, 16);
            info.len[0] = y_size + (c_span * height / 2) * 2;
        }

#ifdef SCALER_ALIGN_RESTRICTION
    if (isSrc && (width % SCALER_EXT_ALIGN)) {
        for (int i = 0; i < pixfmt->planes; i++)
            info.len[i] += (i == 0) ? SCALER_EXT_SIZE : SCALER_EXT_SIZE / 2;
    }
#endif
    }

    info.num_buffers = pixfmt->planes;

    return true;
}

bool CScalerAcryl::SetRotate(int rot, int hflip, int vflip)
{
    if ((rot % 90) != 0) {
        ALOGE("Rotation degree %d must be multiple of 90", rot);
        return false;
    }

    switch (rot) {
    case 90:
        mTransform |= HAL_TRANSFORM_ROT_90;
        break;
    case 180:
        mTransform |= HAL_TRANSFORM_ROT_180;
        break;
    case 270:
        mTransform |= HAL_TRANSFORM_ROT_270;
        break;
    default:
        mTransform = 0;
        break;
    }

    if (hflip)
        mTransform |= HAL_TRANSFORM_FLIP_H;
    if (vflip)
        mTransform |= HAL_TRANSFORM_FLIP_V;

    return true;
}
/* Dummies to avoid build error */
void *exynos_sc_create_exclusive(
    int __unused dev_num,
    int __unused allow_drm)
{
    return nullptr;
}

int exynos_sc_csc_exclusive(void __unused *handle,
    unsigned int __unused range_full,
    unsigned int __unused v4l2_colorspace)
{
    return 0;
}

int exynos_sc_config_exclusive(
    void __unused *handle,
    exynos_sc_img __unused *src_img,
    exynos_sc_img __unused *dst_img)
{
    return 0;
}


int exynos_sc_run_exclusive(
    void __unused *handle,
    exynos_sc_img __unused *src_img,
    exynos_sc_img __unused *dst_img)
{
    return 0;
}

void *exynos_sc_create_blend_exclusive(
        int __unused dev_num,
        int __unused allow_drm)
{
    return 0;
}

int exynos_sc_config_blend_exclusive(
    void __unused *handle,
    exynos_sc_img __unused *src_img,
    exynos_sc_img __unused *dst_img,
    struct SrcBlendInfo __unused *srcblendinfo)
{
    return 0;
}

int exynos_sc_wait_frame_done_exclusive(void __unused *handle) {return 0; }
int exynos_sc_stop_exclusive(void __unused *handle) { return 0; }
int exynos_sc_free_and_close(void __unused *handle) { return 0; }
