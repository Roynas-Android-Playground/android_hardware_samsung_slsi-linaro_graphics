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

#### Test program for libgiantmscl ####

LOCAL_PATH:= $(call my-dir)

GIANTMSCL_CLFAGS := -DLOG_TAG=\"giantmscl-test\"
GIANTMSCL_SRC_FILES := main.cpp
GIANTMSCL_STATIC_LIBRARIES := libjsoncpp
GIANTMSCL_SHARED_LIBRARIES := liblog libutils libcutils libion_exynos libgiantmscl
GIANTMSCL_EXE_PREFIX := giantmscl

include $(CLEAR_VARS)
LOCAL_CFLAGS += $(GIANTMSCL_CLFAGS)
LOCAL_STATIC_LIBRARIES := $(GIANTMSCL_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES := $(GIANTMSCL_SHARED_LIBRARIES)
LOCAL_HEADER_LIBRARIES += libexynos_headers
LOCAL_SRC_FILES := $(GIANTMSCL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := $(GIANTMSCL_EXE_PREFIX)64
#LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_CFLAGS += $(GIANTMSCL_CLFAGS)
LOCAL_STATIC_LIBRARIES := $(GIANTMSCL_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES := $(GIANTMSCL_SHARED_LIBRARIES)
LOCAL_HEADER_LIBRARIES += libexynos_headers
LOCAL_SRC_FILES := $(GIANTMSCL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_32_BIT_ONLY := true
LOCAL_MODULE := $(GIANTMSCL_EXE_PREFIX)32
#LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
