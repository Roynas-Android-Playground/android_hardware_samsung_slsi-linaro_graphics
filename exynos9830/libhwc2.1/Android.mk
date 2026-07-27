# Copyright (C) 2008 The Android Open Source Project
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

ifndef TARGET_SOC_BASE
	TARGET_SOC_BASE := $(TARGET_SOC)
endif

LOCAL_SHARED_LIBRARIES += libhardware libhidlbase android.hardware.power@1.0

LOCAL_SRC_FILES += \
	../../$(TARGET_SOC_BASE)/libhwc2.1/device/ExynosDeviceModule.cpp \
	../../$(TARGET_SOC_BASE)/libhwc2.1/device/ExynosResourceManagerModule.cpp \
	../../$(TARGET_SOC_BASE)/libhwc2.1/display/ExynosDisplayFbInterfaceModule.cpp \
	../../$(TARGET_SOC_BASE)/libhwc2.1/primarydisplay/ExynosPrimaryDisplayModule.cpp \
	../../$(TARGET_SOC_BASE)/libhwc2.1/primarydisplay/ExynosPrimaryDisplayFbInterfaceModule.cpp \
	../../$(TARGET_SOC_BASE)/libhwc2.1/externaldisplay/ExynosExternalDisplayModule.cpp \
	../../$(TARGET_SOC_BASE)/libhwc2.1/externaldisplay/ExynosExternalDisplayFbInterfaceModule.cpp \
	../../$(TARGET_SOC_BASE)/libhwc2.1/virtualdisplay/ExynosVirtualDisplayModule.cpp \
	../../$(TARGET_SOC_BASE)/libhwc2.1/resources/ExynosMPPModule.cpp
