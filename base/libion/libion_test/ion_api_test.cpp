/*
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, SystemInterface 2.0 (the "License");
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
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/exynos/ion.h>
#include <ion/ion.h>
#include <linux/dma-buf.h>
#include <linux/dma-heap.h>
#include <linux/ion.h>
#include <linux/ion_4.19.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../ion.h"
#include "../ion_uapi.h"

using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::DoAll;
using ::testing::_;

#define kb(nr) ((nr) * 1024)
#define mb(nr) ((nr) * 1024 * 1024)
#define mkb(mnr, knr) ((mnr) * 1024 * 1024 + (knr) * 1024)

ACTION_P2(ReturnWithErrno, result, error) {
    errno = error;
    return result;
}

ACTION_P(SetIonSharedFd, fd) {
    static_cast<struct ion_fd_data *>(arg2)->fd = fd;
    return 0;
}

class MockSystemInterface : public SystemInterface {
public:
    ~MockSystemInterface() { }

    MOCK_METHOD(int, Open, (const char *path), (override));
    MOCK_METHOD(int, Ioctl, (int fd, unsigned int cmd, void *data), (override));
    MOCK_METHOD(int, Ioctl, (int fd, unsigned int cmd), (override));
    MOCK_METHOD(int, Close, (int fd), (override));
};

class IonAPI : public ::testing::Test {
protected:
    off_t checkZero(int fd, size_t size, unsigned long *val) {
        unsigned long *p = reinterpret_cast<unsigned long *>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
        if (p == MAP_FAILED)
            return -1;

        off_t idx;
        for (idx = 0; idx < static_cast<off_t>(size / sizeof(*p)); idx++) {
            if (p[idx] != 0) {
                if (val)
                    *val = p[idx];
                break;
            }
        }

        munmap(p, size);

        return idx * sizeof(*p);
    }
};

TEST_F(IonAPI, Unknown)
{
    MockSystemInterface mockSystemInterface;
    EXPECT_CALL(mockSystemInterface, Open(_))
        .Times(2)
        .WillOnce(Return(-1))
        .WillOnce(Return(-1));

    DmabufExporter UnknownExporter(mockSystemInterface);
}

TEST_F(IonAPI, LegacyIon)
{
    MockSystemInterface mockSystemInterface;
    EXPECT_CALL(mockSystemInterface, Open(_))
        .Times(4)
        .WillOnce(Return(-1))
        .WillOnce(Return(1))
        .WillOnce(Return(-1))
        .WillOnce(Return(1));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_FREE, _))
        .Times(3)
        .WillOnce(Return(-1))
        .WillOnce(Return(0))
        .WillOnce(Return(-1));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_ALLOC, _))
        .Times(3)
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(-1));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_SHARE, _))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(Return(-1));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_SYNC, _))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(Return(-1));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_SYNC_PARTIAL, _))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(Return(-1));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, DMA_BUF_IOCTL_SYNC, _))
        .Times(0);

    EXPECT_CALL(mockSystemInterface, Close(_))
        .Times(3)
        .WillOnce(Return(0))
        .WillOnce(Return(-1))
        .WillOnce(Return(0));

    /* Determine Modern and Legacy ION by errno of IOC_FREE */
    errno = EINVAL;
    DmabufExporter LegacyExporter(mockSystemInterface);

    /* Open */
    EXPECT_LE(-1, LegacyExporter.open());
    EXPECT_EQ(1, LegacyExporter.open());

    /* Close */
    EXPECT_EQ(-1, LegacyExporter.close(1));
    EXPECT_EQ(0, LegacyExporter.close(1));

    /* Allocate */
    EXPECT_EQ(0, LegacyExporter.alloc(1, 4096, EXYNOS_ION_HEAP_SYSTEM_MASK, ION_FLAG_CACHED));
    EXPECT_EQ(-1, LegacyExporter.alloc(1, 4096, EXYNOS_ION_HEAP_SYSTEM_MASK, ION_FLAG_CACHED));
    EXPECT_EQ(-1, LegacyExporter.alloc(1, 4096, EXYNOS_ION_HEAP_SYSTEM_MASK, ION_FLAG_CACHED));

    /* Sync */
    EXPECT_EQ(0, LegacyExporter.sync(1, 1, 0, 0));
    EXPECT_EQ(-1, LegacyExporter.sync(1, 1, 0, 0));

    /* Sync partial */
    EXPECT_EQ(0, LegacyExporter.sync_fd_partial(1, 1, 0, 0));
    EXPECT_EQ(-1, LegacyExporter.sync_fd_partial(1, 1, 0, 0));
}

ACTION(SetArgToHeapData) {
    struct ion_heap_query *query = static_cast<struct ion_heap_query *>(arg2);
    struct ion_exynos_heap_data *heaps = reinterpret_cast<struct ion_exynos_heap_data *>(query->heaps);

    query->cnt = 1;

    heaps[0].heap_id = 1;
    strcpy(heaps[0].name, "ion_system_heap");
}

ACTION(SetArgToHeapWrongData) {
    struct ion_heap_query *query = static_cast<struct ion_heap_query *>(arg2);

    query->cnt = 64;
}

TEST_F(IonAPI, ModernIon)
{
    MockSystemInterface mockSystemInterface;

    EXPECT_CALL(mockSystemInterface, Open(_))
        .Times(2)
        .WillOnce(Return(-1))
        .WillOnce(Return(1));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_NEW_ALLOC, _))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(ReturnWithErrno(-1, EMFILE));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_HEAP_QUERY, _))
        .Times(9)
        .WillOnce(DoAll(SetArgToHeapData(), Return(0)))
        .WillOnce(DoAll(SetArgToHeapData(), Return(0)))
        .WillOnce(DoAll(SetArgToHeapData(), Return(0)))
        .WillOnce(DoAll(SetArgToHeapData(), Return(0)))
        .WillOnce(Return(-1))
        .WillOnce(Return(-1))
        .WillOnce(DoAll(SetArgToHeapWrongData(), Return(0)))
        .WillOnce(DoAll(SetArgToHeapWrongData(), Return(0)))
        .WillOnce(DoAll(SetArgToHeapData(), Return(0)));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_FREE, _))
        .Times(1)
        .WillOnce(Return(-1));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_SYNC, _))
        .Times(0);

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_SYNC_PARTIAL, _))
        .Times(0);

    EXPECT_CALL(mockSystemInterface, Ioctl(_, DMA_BUF_IOCTL_SYNC, _))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(Return(-1));

    EXPECT_CALL(mockSystemInterface, Close(_))
        .Times(1)
        .WillOnce(Return(0));

    /* Determine Modern and Legacy ION by errno of IOC_FREE */
    errno = ENOTTY;
    DmabufExporter ModernExporter(mockSystemInterface);

    /* Allocate */
    errno = EIO;
    EXPECT_EQ(0, ModernExporter.alloc(1, 4096, EXYNOS_ION_HEAP_SYSTEM_MASK, ION_FLAG_CACHED));
    EXPECT_EQ(0, errno);
    EXPECT_EQ(-1, ModernExporter.alloc(1, 4096, EXYNOS_ION_HEAP_SYSTEM_MASK, ION_FLAG_CACHED));
    EXPECT_EQ(EMFILE, errno);
    errno = EIO;
    EXPECT_EQ(-1, ModernExporter.alloc(1, 4096, EXYNOS_ION_HEAP_SYSTEM_MASK, ION_FLAG_CACHED));
    EXPECT_EQ(EIO, errno);
    EXPECT_EQ(-1, ModernExporter.alloc(1, 4096, EXYNOS_ION_HEAP_SYSTEM_MASK, ION_FLAG_CACHED));
    EXPECT_EQ(ENODEV, errno);
    EXPECT_EQ(-1, ModernExporter.alloc(1, 4096, 1 << 24, ION_FLAG_CACHED));
    EXPECT_EQ(ENODEV, errno);

    /* Sync */
    EXPECT_EQ(0, ModernExporter.sync(1, 1, 0, 0));
    EXPECT_EQ(-1, ModernExporter.sync(1, 1, 0, 0));

    /* Sync partial */
    EXPECT_EQ(0, ModernExporter.sync_fd_partial(1, 1, 0, 0));
}

TEST_F(IonAPI, DmaHeap)
{
    MockSystemInterface mockSystemInterface;

    EXPECT_CALL(mockSystemInterface, Open(_))
        .Times(5)
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(ReturnWithErrno(-1, EACCES));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, DMA_HEAP_IOCTL_ALLOC, _))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(ReturnWithErrno(-1, EINVAL));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_HEAP_QUERY, _))
        .Times(0);

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_FREE, _))
        .Times(0);

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_SYNC, _))
        .Times(0);

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_SYNC_PARTIAL, _))
        .Times(0);

    EXPECT_CALL(mockSystemInterface, Ioctl(_, DMA_BUF_IOCTL_SYNC, _))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(Return(-1));

    EXPECT_CALL(mockSystemInterface, Close(_))
        .Times(4)
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(0));

    DmabufExporter DmaHeapExporter(mockSystemInterface);

    /* Open */
    EXPECT_EQ(1, DmaHeapExporter.open());

    /* Close */
    EXPECT_EQ(0, DmaHeapExporter.close(1));

    /* Allocate */
    EXPECT_EQ(-EINVAL, DmaHeapExporter.alloc(1, 4096, 0, ION_FLAG_CACHED));
    EXPECT_EQ(0, DmaHeapExporter.alloc(1, 4096, EXYNOS_ION_HEAP_SYSTEM_MASK, ION_FLAG_CACHED));
    EXPECT_EQ(-1, DmaHeapExporter.alloc(1, 4096, EXYNOS_ION_HEAP_SYSTEM_MASK, ION_FLAG_CACHED));
    EXPECT_EQ(-1, DmaHeapExporter.alloc(1, 4096, EXYNOS_ION_HEAP_SYSTEM_MASK, ION_FLAG_CACHED));

    /* Sync */
    EXPECT_EQ(0, DmaHeapExporter.sync(1, 1, 0, 0));
    EXPECT_EQ(-1, DmaHeapExporter.sync(1, 1, 0, 0));

    /* Sync partial */
    EXPECT_EQ(0, DmaHeapExporter.sync_fd_partial(1, 1, 0, 0));
}

TEST_F(IonAPI, DmaHeapMissingSpecializedHeapFallsBackToLegacyIon)
{
    MockSystemInterface mockSystemInterface;
    ::testing::InSequence sequence;

    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/system")))
        .WillOnce(Return(10));
    EXPECT_CALL(mockSystemInterface, Close(10))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/vscaler")))
        .WillOnce(ReturnWithErrno(-1, ENOENT));
    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/ion")))
        .WillOnce(Return(11));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_FREE, _))
        .WillOnce(ReturnWithErrno(-1, EINVAL));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_ALLOC, _))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_SHARE, _))
        .WillOnce(SetIonSharedFd(42));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_FREE, _))
        .WillOnce(ReturnWithErrno(-1, EIO));
    EXPECT_CALL(mockSystemInterface, Close(11))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Close(42))
        .WillOnce(Return(0));

    DmabufExporter DmaHeapExporter(mockSystemInterface);

    int fd = DmaHeapExporter.alloc(10, 4096, EXYNOS_ION_HEAP_VIDEO_SCALER_MASK,
                                   ION_FLAG_CACHED);
    EXPECT_EQ(42, fd);
    EXPECT_EQ(0, errno);
    EXPECT_EQ(0, DmaHeapExporter.close(fd));
}

TEST_F(IonAPI, DmaHeapResourceFailureFallsBackToLegacyIon)
{
    MockSystemInterface mockSystemInterface;
    ::testing::InSequence sequence;

    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/system")))
        .WillOnce(Return(10));
    EXPECT_CALL(mockSystemInterface, Close(10))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/vframe")))
        .WillOnce(Return(12));
    EXPECT_CALL(mockSystemInterface, Ioctl(12, DMA_HEAP_IOCTL_ALLOC, _))
        .WillOnce(ReturnWithErrno(-1, ENOMEM));
    EXPECT_CALL(mockSystemInterface, Close(12))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/ion")))
        .WillOnce(Return(11));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_FREE, _))
        .WillOnce(ReturnWithErrno(-1, EINVAL));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_ALLOC, _))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_SHARE, _))
        .WillOnce(SetIonSharedFd(43));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_FREE, _))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Close(11))
        .WillOnce(Return(0));

    DmabufExporter DmaHeapExporter(mockSystemInterface);

    EXPECT_EQ(43, DmaHeapExporter.alloc(10, 4096, EXYNOS_ION_HEAP_VIDEO_FRAME_MASK,
                                        ION_FLAG_CACHED));
    EXPECT_EQ(0, errno);
}

TEST_F(IonAPI, DmaHeapLegacyShareFailurePreservesErrno)
{
    MockSystemInterface mockSystemInterface;
    ::testing::InSequence sequence;

    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/system")))
        .WillOnce(Return(10));
    EXPECT_CALL(mockSystemInterface, Close(10))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/vscaler")))
        .WillOnce(ReturnWithErrno(-1, ENOENT));
    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/ion")))
        .WillOnce(Return(11));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_FREE, _))
        .WillOnce(ReturnWithErrno(-1, EINVAL));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_ALLOC, _))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_SHARE, _))
        .WillOnce(ReturnWithErrno(-1, EMFILE));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_FREE, _))
        .WillOnce(ReturnWithErrno(-1, EIO));
    EXPECT_CALL(mockSystemInterface, Close(11))
        .WillOnce(ReturnWithErrno(-1, EBADF));

    DmabufExporter DmaHeapExporter(mockSystemInterface);

    EXPECT_EQ(-1, DmaHeapExporter.alloc(10, 4096,
                                        EXYNOS_ION_HEAP_VIDEO_SCALER_MASK,
                                        ION_FLAG_CACHED));
    EXPECT_EQ(EMFILE, errno);
}

TEST_F(IonAPI, DmaHeapModernFallbackQueryFailurePreservesErrno)
{
    MockSystemInterface mockSystemInterface;
    ::testing::InSequence sequence;

    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/system")))
        .WillOnce(Return(10));
    EXPECT_CALL(mockSystemInterface, Close(10))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/vscaler")))
        .WillOnce(ReturnWithErrno(-1, ENOENT));
    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/ion")))
        .WillOnce(Return(11));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_FREE, _))
        .WillOnce(ReturnWithErrno(-1, ENOTTY));
    EXPECT_CALL(mockSystemInterface, Ioctl(11, ION_IOC_HEAP_QUERY, _))
        .WillOnce(ReturnWithErrno(-1, EIO));
    EXPECT_CALL(mockSystemInterface, Close(11))
        .WillOnce(ReturnWithErrno(-1, EBADF));

    DmabufExporter DmaHeapExporter(mockSystemInterface);

    EXPECT_EQ(-1, DmaHeapExporter.alloc(10, 4096,
                                        EXYNOS_ION_HEAP_VIDEO_SCALER_MASK,
                                        ION_FLAG_CACHED));
    EXPECT_EQ(EIO, errno);
}

TEST_F(IonAPI, DmaHeapFallbackOpenFailurePreservesErrno)
{
    MockSystemInterface mockSystemInterface;
    ::testing::InSequence sequence;

    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/system")))
        .WillOnce(Return(10));
    EXPECT_CALL(mockSystemInterface, Close(10))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/vscaler")))
        .WillOnce(ReturnWithErrno(-1, ENOENT));
    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/ion")))
        .WillOnce(ReturnWithErrno(-1, EMFILE));

    DmabufExporter DmaHeapExporter(mockSystemInterface);

    EXPECT_EQ(-1, DmaHeapExporter.alloc(10, 4096,
                                        EXYNOS_ION_HEAP_VIDEO_SCALER_MASK,
                                        ION_FLAG_CACHED));
    EXPECT_EQ(EMFILE, errno);
}

TEST_F(IonAPI, DmaHeapProtectedFailureDoesNotFallback)
{
    MockSystemInterface mockSystemInterface;
    ::testing::InSequence sequence;

    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/system")))
        .WillOnce(Return(10));
    EXPECT_CALL(mockSystemInterface, Close(10))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/vscaler-secure")))
        .WillOnce(ReturnWithErrno(-1, ENOENT));

    DmabufExporter DmaHeapExporter(mockSystemInterface);

    EXPECT_EQ(-1, DmaHeapExporter.alloc(10, 4096, EXYNOS_ION_HEAP_VIDEO_SCALER_MASK,
                                        ION_FLAG_PROTECTED));
    EXPECT_EQ(ENOENT, errno);
}

TEST_F(IonAPI, DmaHeapPermissionFailureDoesNotFallback)
{
    MockSystemInterface mockSystemInterface;
    ::testing::InSequence sequence;

    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/system")))
        .WillOnce(Return(10));
    EXPECT_CALL(mockSystemInterface, Close(10))
        .WillOnce(Return(0));
    EXPECT_CALL(mockSystemInterface, Open(::testing::StrEq("/dev/dma_heap/vscaler")))
        .WillOnce(ReturnWithErrno(-1, EACCES));

    DmabufExporter DmaHeapExporter(mockSystemInterface);

    EXPECT_EQ(-1, DmaHeapExporter.alloc(10, 4096, EXYNOS_ION_HEAP_VIDEO_SCALER_MASK,
                                        ION_FLAG_CACHED));
    EXPECT_EQ(EACCES, errno);
}

TEST_F(IonAPI, TraceNoLegacy)
{
    MockSystemInterface mockSystemInterface;

    EXPECT_CALL(mockSystemInterface, Open(_))
        .Times(1)
        .WillOnce(Return(1));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, DMA_BUF_IOCTL_TRACK))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(Return(-1));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, DMA_BUF_IOCTL_UNTRACK))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(Return(-1));

    EXPECT_CALL(mockSystemInterface, Close(_))
        .Times(1)
        .WillOnce(Return(0));

    DmabufExporter DmaHeapExporter(mockSystemInterface);

    int handle;

    EXPECT_EQ(0, DmaHeapExporter.import_handle(1, 1, &handle));
    EXPECT_EQ(1, handle);
    EXPECT_EQ(0, DmaHeapExporter.free_handle(1, handle));

    errno = EINVAL;
    EXPECT_EQ(-1, DmaHeapExporter.import_handle(1, 1, &handle));
    EXPECT_EQ(-1, DmaHeapExporter.free_handle(1, handle));
}

TEST_F(IonAPI, TraceLegacy)
{
    MockSystemInterface mockSystemInterface;

    EXPECT_CALL(mockSystemInterface, Open(_))
        .Times(2)
        .WillOnce(Return(-1))
        .WillOnce(Return(0));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_FREE, _))
        .Times(2)
        .WillOnce(Return(-1))
        .WillOnce(Return(0));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, ION_IOC_IMPORT, _))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(Return(-1));

    EXPECT_CALL(mockSystemInterface, Ioctl(_, DMA_BUF_IOCTL_TRACK))
        .Times(0);

    EXPECT_CALL(mockSystemInterface, Ioctl(_, DMA_BUF_IOCTL_UNTRACK))
        .Times(0);

    EXPECT_CALL(mockSystemInterface, Close(_))
        .Times(1)
        .WillOnce(Return(1));

    errno = EINVAL;
    DmabufExporter LegacyExporter(mockSystemInterface);

    int handle;

    errno = ENOTTY;
    EXPECT_EQ(0, LegacyExporter.import_handle(1, 1, &handle));
    EXPECT_EQ(0, LegacyExporter.free_handle(1, handle));

    EXPECT_EQ(-1, LegacyExporter.import_handle(1, 1, &handle));
}

TEST_F(IonAPI, GetHeapName)
{
    const char *name = exynos_ion_get_heap_name(ION_EXYNOS_HEAP_ID_SYSTEM);

    ASSERT_EQ(0, strcmp("ion_system_heap", name));
    ASSERT_EQ(NULL, exynos_ion_get_heap_name(32));
}

TEST_F(IonAPI, Allocate)
{
    static const size_t allocation_sizes[] = {
        mkb(16, 716), mkb(12, 4), mkb(8, 912), mkb(4, 60), mkb(2, 520), mkb(1, 92),
        mb(16), mb(12), mb(8), mb(4), mb(2), mb(1), kb(64), kb(4),
    };
    static unsigned int types[] = {
        0,
        ION_FLAG_CACHED,
    };
    static unsigned int heapmasks[] = {
        EXYNOS_ION_HEAP_SYSTEM_MASK,
        EXYNOS_ION_HEAP_CRYPTO_MASK,
    };

    int ion_fd = exynos_ion_open();

    for (size_t size: allocation_sizes) {
        for (unsigned int type: types) {
            for (unsigned int heapmask: heapmasks) {
                int fd;

                EXPECT_LE(0, fd = exynos_ion_alloc(ion_fd, size, heapmask, type)) << ": " << strerror(errno);
                EXPECT_LT(2, fd);
                EXPECT_GT(1024, fd);

                int handle;

                exynos_ion_import_handle(ion_fd, fd, &handle);
                exynos_ion_sync_start(ion_fd, fd, ION_SYNC_READ);

                off_t erridx;
                unsigned long val = 0;
                EXPECT_EQ(static_cast<off_t>(size), erridx = checkZero(fd, size, &val))
                          << "non-zero " << val << " found at " << erridx << " byte";

                exynos_ion_sync_fd(ion_fd, fd);
                exynos_ion_sync_fd_partial(ion_fd, fd, 0, size);

                exynos_ion_sync_end(ion_fd, fd, ION_SYNC_READ);
                exynos_ion_free_handle(ion_fd, handle);

                ASSERT_EQ(0, close(fd));
            }
        }
    }

    exynos_ion_close(ion_fd);
}

TEST_F(IonAPI, Protected)
{
    static const unsigned int secureHeaps[] = {
        EXYNOS_ION_HEAP_VIDEO_STREAM_MASK,
        EXYNOS_ION_HEAP_VIDEO_FRAME_MASK,
    };

    int ion_fd = exynos_ion_open();

    for (unsigned int heapMask : secureHeaps) {
        int map_fd = -1;
        ASSERT_GE(map_fd = exynos_ion_alloc(ion_fd, 4096, heapMask, ION_FLAG_PROTECTED), 0);

        void *ptr = NULL;
        ptr = mmap(NULL, 4096, PROT_READ, MAP_SHARED, map_fd, 0);
        ASSERT_TRUE(ptr != NULL);
        ASSERT_EQ(0, close(map_fd));
    }

    exynos_ion_close(ion_fd);
}
