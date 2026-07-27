#
# Copyright (C) 2016 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

ifeq ($(BOARD_USES_HWC_SERVICES),true)
PRODUCT_PROPERTY_OVERRIDES += \
	ro.vendor.hw.use_hwc_services=1
endif

ifeq ($(BOARD_USES_VIRTUAL_DISPLAY), true)
PRODUCT_PROPERTY_OVERRIDES += \
	ro.vendor.hw.use_virtual_display=1
endif

ifeq ($(BOARD_USES_DISABLE_COMPOSITIONTYPE_GLES),true)
PRODUCT_PROPERTY_OVERRIDES += \
	ro.vendor.hw.use_disable_composition_type_gles=1
endif

ifeq ($(BOARD_USES_SECURE_ENCODER_ONLY),true)
PRODUCT_PROPERTY_OVERRIDES += \
	ro.vendor.hw.use_secure_encoder_only=1
endif

ifeq ($(BOARD_USES_CONTIG_MEMORY_FOR_SCRATCH_BUF),true)
PRODUCT_PROPERTY_OVERRIDES += \
	ro.vendor.hw.use_contig_memory_for_scratch_buf=1
endif

ifeq ($(BOARD_USES_SURFACE_YUV),true)
PRODUCT_PROPERTY_OVERRIDES += \
	ro.vendor.hw.use_surface_yuv=1
endif

ifeq ($(BOARD_USES_HDRUI_GLES_CONVERSION),true)
PRODUCT_PROPERTY_OVERRIDES += \
	ro.vendor.hw.use_hdrui_gles_conversion=1
endif

ifeq ($(BOARD_USES_HWC_CPU_PERF_MODE),true)
PRODUCT_PROPERTY_OVERRIDES += \
	ro.vendor.hw.use_hwc_cpu_perf_mode=1
endif

ifeq ($(BOARD_USES_DP_VSYNC_FEATURE),true)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.hw.use_dp_vsync_feature=1
endif
