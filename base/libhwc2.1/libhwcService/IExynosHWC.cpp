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

#include <stdint.h>
#include <sys/types.h>

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <utils/Timers.h>

#include <binder/Parcel.h>
#include <binder/IInterface.h>

#include "ExynosVirtualDisplay.h"
#include "IExynosHWC.h"

namespace android {

class BpExynosHWCService : public BpInterface<IExynosHWCService> {
  public:
    BpExynosHWCService(const sp<IBinder> &impl)
        : BpInterface<IExynosHWCService>(impl) {
    }

    virtual int setWFDMode(unsigned int mode) {
        Parcel data, reply;
        data.writeInterfaceToken(IExynosHWCService::getInterfaceDescriptor());
        data.writeInt32(mode);
        int result = remote()->transact(SET_WFD_MODE, data, &reply);
        if (result == NO_ERROR)
            result = reply.readInt32();
        else
            ALOGE("SET_WFD_MODE transact error(%d)", result);
        return result;
    }

    virtual int getWFDMode() {
        Parcel data, reply;
        data.writeInterfaceToken(IExynosHWCService::getInterfaceDescriptor());
        int result = remote()->transact(GET_WFD_MODE, data, &reply);
        if (result == NO_ERROR)
            result = reply.readInt32();
        else
            ALOGE("GET_WFD_MODE transact error(%d)", result);
        return result;
    }

    virtual int getWFDInfo(int32_t *state, int32_t *compositionType, int32_t *format,
                           int64_t *usage, int32_t *width, int32_t *height) {
        Parcel data, reply;
        data.writeInterfaceToken(IExynosHWCService::getInterfaceDescriptor());
        int result = remote()->transact(GET_WFD_INFO, data, &reply);
        if (result == NO_ERROR) {
            *state = reply.readInt32();
            *compositionType = reply.readInt32();
            *format = reply.readInt32();
            *usage = reply.readInt64();
            *width = reply.readInt32();
            *height = reply.readInt32();
        } else {
            ALOGE("GET_WFD_INFO transact error(%d)", result);
        }
        return result;
    }

    virtual int sendWFDCommand(int32_t cmd, int32_t ext1, int32_t ext2) {
        Parcel data, reply;
        data.writeInterfaceToken(IExynosHWCService::getInterfaceDescriptor());
        data.writeInt32(cmd);
        data.writeInt32(ext1);
        data.writeInt32(ext2);
        int result = remote()->transact(SEND_WFD_COMMAND, data, &reply);
        if (result == NO_ERROR)
            result = reply.readInt32();
        else
            ALOGE("SEND_WFD_COMMAND transact error(%d)", result);
        return result;
    }

    virtual int setWFDOutputResolution(unsigned int width, unsigned int height) {
        Parcel data, reply;
        data.writeInterfaceToken(IExynosHWCService::getInterfaceDescriptor());
        data.writeInt32(width);
        data.writeInt32(height);
        int result = remote()->transact(SET_WFD_OUTPUT_RESOLUTION, data, &reply);
        if (result == NO_ERROR)
            result = reply.readInt32();
        else
            ALOGE("SET_WFD_OUTPUT_RESOLUTION transact error(%d)", result);
        return result;
    }

    virtual int setVDSGlesFormat(int format) {
        Parcel data, reply;
        data.writeInterfaceToken(IExynosHWCService::getInterfaceDescriptor());
        data.writeInt32(format);
        int result = remote()->transact(SET_VDS_GLES_FORMAT, data, &reply);
        if (result == NO_ERROR)
            result = reply.readInt32();
        else
            ALOGE("SET_VDS_GLES_FORMAT transact error(%d)", result);
        return result;
    }

    virtual int getExternalHdrCapabilities() {
        Parcel data, reply;
        data.writeInterfaceToken(IExynosHWCService::getInterfaceDescriptor());
        int result = remote()->transact(GET_EXTERNAL_HDR_CAPA, data, &reply);
        if (result == NO_ERROR)
            result = reply.readInt32();
        else
            ALOGE("GET_EXTERNAL_HDR_CAPA transact error(%d)", result);

        return result;
    }

    virtual void setBootFinished() {
        Parcel data, reply;
        data.writeInterfaceToken(IExynosHWCService::getInterfaceDescriptor());
        int result = remote()->transact(SET_BOOT_FINISHED, data, &reply);
        if (result != NO_ERROR)
            ALOGE("SET_BOOT_FINISHED transact error(%d)", result);
    }

    virtual uint32_t getHWCDebug() {
        Parcel data, reply;
        data.writeInterfaceToken(IExynosHWCService::getInterfaceDescriptor());
        int result = remote()->transact(GET_HWC_DEBUG, data, &reply);
        if (result == NO_ERROR)
            result = reply.readInt32();
        else {
            ALOGE("GET_HWC_DEBUG transact error(%d)", result);
        }
        return result;
    }

    virtual int getCPUPerfInfo(int display, int config, int32_t *cpuIDs, int32_t *min_clock) {
        Parcel data, reply;
        data.writeInterfaceToken(IExynosHWCService::getInterfaceDescriptor());
        data.writeInt32(display);
        data.writeInt32(config);
        int result = remote()->transact(GET_CPU_PERF_INFO, data, &reply);
        if (result == NO_ERROR) {
            *cpuIDs = reply.readInt32();
            *min_clock = reply.readInt32();
        } else {
            ALOGE("GET_DUMP_LAYER transact error(%d", result);
        }
        return result;
    }
    virtual int32_t setDisplayMultiThreadedPresent(const int32_t& displayId,
                                                   const bool& enable) {
        Parcel data, reply;
        data.writeInterfaceToken(IExynosHWCService::getInterfaceDescriptor());
        data.writeInt32(displayId);
        data.writeBool(enable);
        int result = remote()->transact(SET_DISPLAY_MULTI_THREADED_PRESENT, data, &reply);
        if (result) ALOGE("SET_DISPLAY_MULTI_THREADED_PRESENT transact error(%d)", result);
        return result;
    }
};

IMPLEMENT_META_INTERFACE(ExynosHWCService, "android.hal.ExynosHWCService");

}  // namespace android
