#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>

#include <stdbool.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <ion/ion.h>

#include "repeater.h"

#define DEVNAME_REPEATER		"/dev/repeater"

#define MAX_HEAP_NAME 32

struct ion_heap_data {
    char name[MAX_HEAP_NAME];
    __u32 type;
    __u32 heap_id;
    __u32 size;       /* reserved 0 */
    __u32 heap_flags; /* reserved 1 */
    __u32 reserved2;
};

/* FIX ME */
#define __ALIGN_UP(x,a) (((x) + ((a) - 1)) & ~((a) - 1))
#define NV12N_Y_SIZE(w,h) (__ALIGN_UP((w), 16) * __ALIGN_UP((h), 16) + 256)
#define NV12N_CBCR_SIZE(w,h) (__ALIGN_UP((__ALIGN_UP((w), 16) * (__ALIGN_UP((h), 16) / 2) + 256), 16))
#define v4l2_fourcc(a,b,c,d) ((__u32) (a) | ((__u32) (b) << 8) | ((__u32) (c) << 16) | ((__u32) (d) << 24))
#define V4L2_PIX_FMT_NV12N v4l2_fourcc('N', 'N', '1', '2')

int main(void)
{
    int ion_fd;
    unsigned int heap_id;
    int repeater_fd;
    int buffer_size = NV12N_Y_SIZE(352, 288) + NV12N_CBCR_SIZE(352, 288); // OTF vector content size
    struct repeater_info info;
    int i;
    FILE* pSrcImg = NULL;
    void *buf_addr[MAX_SHARED_BUFFER_NUM];

    ion_fd = ion_open();
    printf("repeater ion_fd %d\n", ion_fd);

    heap_id = 0;
    if (!ion_is_legacy(ion_fd)) {
        int heap_cnt = 0;

        if (ion_query_heap_cnt(ion_fd, &heap_cnt) < 0 || heap_cnt <= 0) {
            printf("fail to query the heap count. heap_cnt: %d", heap_cnt);
            return -1;
        }

        struct ion_heap_data heaps[heap_cnt];

        if (ion_query_get_heaps(ion_fd, heap_cnt, heaps) < 0) {
            printf("fail to query the heaps");
            return -1;
        }

        for (int i = 0; i < heap_cnt; i++) {
            if (strcmp(heaps[i].name, "ion_system_heap") == 0)
                heap_id = heaps[i].heap_id;
        }
    }

    printf("tsmux heap_id %d", heap_id);

    repeater_fd = open(DEVNAME_REPEATER, O_RDWR);
    if (repeater_fd < 0) {
        printf("failed to open repeater : %s, %d %d\n",
                DEVNAME_REPEATER, repeater_fd, errno);
        return -1;
    }
    printf("open repeater\n");

    for (i = 0; i < MAX_SHARED_BUFFER_NUM; i++) {
        ion_alloc_fd(ion_fd, buffer_size, 0, 1 << heap_id, 0, &info.buf_fd[i]);

        printf("info.buf_fd[%d] %d\n", i, info.buf_fd[i]);

        buf_addr[i] = mmap(0, buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED, info.buf_fd[i], 0);
    }

    pSrcImg = fopen("352x288_aligned_f000.out", "rb");
    if (pSrcImg == NULL) {
        printf("352x288_aligned_f000.out open failed\n");
    } else {
        fseek(pSrcImg, 0, SEEK_END);
        buffer_size = ftell(pSrcImg);
        fseek(pSrcImg, 0, SEEK_SET);

        for (i = 0; i < MAX_SHARED_BUFFER_NUM; i++) {
            fread(buf_addr[i], buffer_size, 1, pSrcImg);
        }

        printf("src image is loaded, size: %d\n", buffer_size);
    }

    for (i = 0; i < MAX_SHARED_BUFFER_NUM; i++) {
        if (buf_addr[i] > 0) {
            munmap(buf_addr[i], buffer_size);
        }
    }

    fclose(pSrcImg);

    info.fps = 60;
    info.width = 352;
    info.height = 288;
    info.pixel_format = V4L2_PIX_FMT_NV12N;
    info.buffer_count = MAX_SHARED_BUFFER_NUM;

    ioctl(repeater_fd, REPEATER_IOCTL_MAP_BUF, &info);
    printf("repeater: map buf\n");

    ioctl(repeater_fd, REPEATER_IOCTL_START);
    printf("repeater: start\n");

    scanf("%d", &i);


    ioctl(repeater_fd, REPEATER_IOCTL_STOP);
    printf("repeater: stop\n");

    ioctl(repeater_fd, REPEATER_IOCTL_UNMAP_BUF);
    printf("repeater: unmap buf\n");

    if (ion_fd > 0)
        ion_close(ion_fd);

    if (repeater_fd > 0)
        close(repeater_fd);

    return 0;
}
