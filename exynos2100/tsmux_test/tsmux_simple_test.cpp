/*
 * Copyright (C) 2020 The Android Open Source Project
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <BufferAllocator/BufferAllocatorWrapper.h>

#include "tsmux.h"
#include "repeater.h"

#define TSMUX_M2M 0
#define TSMUX_OTF 1
#define ARRAY_SIZE(x)   (sizeof(x) / sizeof((x)[0]))

#include "test_desc.h"

#define DEVNAME_REPEATER    "/dev/repeater"
#define DEVNAME_TSMUX       "/dev/tsmux"

#define READ_BUFFER_SIZE    (32)

#define TSMUX_INFO          (1 << 1)
#define TSMUX_ERR           (1)

#define M2M_PATH            (0)
#define OTF_PATH            (1)

#define M2M_TESTCASE_NUM    (10)
#define OTF_TESTCASE_NUM    (2)

#ifndef __ALIGN_UP
#define __ALIGN_UP(x, a)        (((x) + ((a) - 1)) & ~((a) - 1))
#endif
#define NV12N_Y_SIZE(w, h)      (__ALIGN_UP((w), 16) * __ALIGN_UP((h), 16) + 256)
#define NV12N_CBCR_SIZE(w, h)   (__ALIGN_UP((__ALIGN_UP((w), 16) * (__ALIGN_UP((h), 16) / 2) + 256), 16))
#define v4l2_fourcc(a,b,c,d) ((__u32) (a) | ((__u32) (b) << 8) | ((__u32) (c) << 16) | ((__u32) (d) << 24))
#define V4L2_PIX_FMT_NV12N v4l2_fourcc('N', 'N', '1', '2')


BufferAllocator* gBufferAllocator;
int g_tsmux_debug_level = 3;

#define print_tsmux(level, fmt, args...)    \
    do {                                    \
        if ((g_tsmux_debug_level & level))  \
            printf(fmt, ##args);            \
    } while (0)

void alloc_buffer(struct tsmux_buffer *in_buf, struct tsmux_buffer *out_buf, int size, int type) {
    int ret;
    if (type == M2M_PATH) {
        in_buf->buffer_size = size;
        in_buf->offset = 0;
        ret = DmabufHeapAlloc(gBufferAllocator, "system-uncached", size, 0);
        if (ret < 0) {
            printf("failed to inbuf dmabufheap alloc\n");
            return;
        }
        in_buf->ion_buf_fd = ret;
        print_tsmux(TSMUX_INFO, "in_buf ion_buf_fd %d, buffer_size %d\n",
        in_buf->ion_buf_fd, in_buf->buffer_size);
    }

    out_buf->buffer_size = size;
    out_buf->offset = 0;
    ret = DmabufHeapAlloc(gBufferAllocator, "system-uncached", size, 0);
    if (ret < 0) {
        printf("failed to outbuf dmabufheap alloc\n");
        return;
    }
    out_buf->ion_buf_fd = ret;
    print_tsmux(TSMUX_INFO, "out_buf ion_buf_fd %d, buffer_size %d\n",
        out_buf->ion_buf_fd, out_buf->buffer_size);
}

void free_buffer(struct tsmux_buffer *in_buf, struct tsmux_buffer *out_buf, int type) {
    if (type == M2M_PATH && in_buf->ion_buf_fd > 0) {
        close(in_buf->ion_buf_fd);
        in_buf->ion_buf_fd = -1;
    }

    if (out_buf->ion_buf_fd > 0) {
        print_tsmux(TSMUX_INFO, "free_buffer() ion_buf_fd %d\n", out_buf->ion_buf_fd);
        close(out_buf->ion_buf_fd);
        out_buf->ion_buf_fd = -1;
    }
}

int load_sfr(
    struct tsmux_asb_job *job,
    int type, int test_count) {
    int i, j;
    int job_id = (type == M2M_PATH) ? 1 : 0;

    /* TODO: change job id */
    for (i = 0; i < tsmux_testcases[test_count].nonsecure_sfr_count; i++) {
        job->normal_sfr_cmd[i].addr = tsmux_testcases[test_count].nonsecure_sfr_dump[i].addr;
        job->normal_sfr_cmd[i].value = tsmux_testcases[test_count].nonsecure_sfr_dump[i].value;
        print_tsmux(TSMUX_INFO, "nonsecure sfr addr: 0x%.8x, value: 0x%0.8x\n",
            job->normal_sfr_cmd[i].addr, job->normal_sfr_cmd[i].value);
        if (job->normal_sfr_cmd[i].addr == 0x00000000) {
            if (type == M2M_PATH) {
                /* m2m job id will be 1 */
                job->normal_sfr_cmd[i].value = job->normal_sfr_cmd[i].value & 0xFFFFFFFB;
                print_tsmux(TSMUX_INFO, "1. value: 0x%0.8x\n", job->normal_sfr_cmd[i].value);
                job->normal_sfr_cmd[i].value = job->normal_sfr_cmd[i].value | 0x00000002;
                print_tsmux(TSMUX_INFO, "2. value: 0x%0.8x\n", job->normal_sfr_cmd[i].value);
            }
            else {
                /* otf job id will be 0 */
                job->normal_sfr_cmd[i].value = job->normal_sfr_cmd[i].value & 0xFFFFFFF9;
            }
        }
    }
    job->normal_sfr_cmd_count = tsmux_testcases[test_count].nonsecure_sfr_count;

    for (i = 0; i < tsmux_testcases[test_count].secure_sfr_count; i++) {
        job->secure_sfr_cmd[i].addr = tsmux_testcases[test_count].secure_sfr_dump[i].addr;
        job->secure_sfr_cmd[i].value = tsmux_testcases[test_count].secure_sfr_dump[i].value;
        print_tsmux(TSMUX_INFO, "secure sfr addr: 0x%.8x, value: 0x%0.8x\n",
            job->secure_sfr_cmd[i].addr, job->secure_sfr_cmd[i].value);
    }
    job->secure_sfr_cmd_count = tsmux_testcases[test_count].secure_sfr_count;

    return job_id;
}

int start_repeater()
{
    int repeater_fd;
    int buffer_size = NV12N_Y_SIZE(352, 288) + NV12N_CBCR_SIZE(352, 288); // OTF vector content size
    struct repeater_info info;
    int i;
    FILE* pSrcImg = NULL;
    void *buf_addr[MAX_SHARED_BUFFER_NUM];
    int ret;

    repeater_fd = open(DEVNAME_REPEATER, O_RDWR);
    if (repeater_fd < 0) {
        print_tsmux(TSMUX_ERR, "failed to open repeater : %s, %d %d\n",
                DEVNAME_REPEATER, repeater_fd, errno);
        return -1;
    }
    print_tsmux(TSMUX_INFO, "open repeater\n");

    for (i = 0; i < MAX_SHARED_BUFFER_NUM; i++) {
        /* dmabufheap alloc */
        ret = DmabufHeapAlloc(gBufferAllocator, "system-uncached", buffer_size, 0);

        if (ret < 0) {
            printf("failed to dmabufheap alloc");
            return ret;
        }

        info.buf_fd[i] = ret;
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
        if (buf_addr[i] != NULL) {
            munmap(buf_addr[i], buffer_size);
        }
    }

    if (pSrcImg != NULL)
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

    return repeater_fd;
}

int stop_repeater(int repeater_fd)
{
    if (repeater_fd < 0)
        return -1;

    ioctl(repeater_fd, REPEATER_IOCTL_STOP);
    printf("repeater: stop\n");

    ioctl(repeater_fd, REPEATER_IOCTL_UNMAP_BUF);
    printf("repeater: unmap buf\n");

    close(repeater_fd);
    return 0;
}

int test_tsmux(int type)
{
    int test_count = (type == M2M_PATH) ? M2M_TESTCASE_NUM : OTF_TESTCASE_NUM;
    int i, j, k, l;
    struct tsmux_psi_info psi_info;
    struct tsmux_m2m_cmd_queue m2m_cmd_queue;
    struct tsmux_otf_cmd_queue otf_cmd_queue;
    struct tsmux_job *job;
    int tsmux_fd;
    int repeater_fd;
    int job_id;
    char *inbuf_addr[TSMUX_MAX_M2M_CMD_QUEUE_NUM];
    char *outbuf_addr[TSMUX_MAX_M2M_CMD_QUEUE_NUM];
    char *otf_outbuf_addr[TSMUX_OUT_BUF_CNT];
    char *golden_addr;
    char *temp_out_addr;
    char *temp_golden_addr;

    char read_buffer[READ_BUFFER_SIZE];

    int ret = 0;
    int mis_matched = 0;
    int buffer_size = NV12N_Y_SIZE(1920, 1080) + NV12N_CBCR_SIZE(1920, 1080);



    /*map dmabufheap name to ion heap*/
    gBufferAllocator = CreateDmabufHeapBufferAllocator();
    ret = MapDmabufHeapNameToIonHeap(gBufferAllocator, "system-uncached", "ion_system_heap", 0, 1<<0, 0);

    if (ret < 0) {
        printf("failed to dmabufheap MapNameToIonHeap\n");
        return -1;
    }

    print_tsmux(TSMUX_INFO, "tsmux heap_name %s\n", "system-uncached");

    if (type == OTF_PATH) {
        repeater_fd = start_repeater();
        if (repeater_fd < 0) {
            print_tsmux(TSMUX_ERR, "failed to start repeater\n");
            return -1;
        }
    } else
        repeater_fd = -1;

    /* open TS MUX driver */
    tsmux_fd = open(DEVNAME_TSMUX, O_RDWR);
    if (tsmux_fd < 0) {
        print_tsmux(TSMUX_ERR, "failed to open TS MUX : %s, %d %d\n",
        DEVNAME_TSMUX, tsmux_fd, errno);
        return -1;
    }
    print_tsmux(TSMUX_INFO, "tsmux_fd %d\n", tsmux_fd);

    golden_addr = (char *)malloc(NV12N_Y_SIZE(1920, 1080) + NV12N_CBCR_SIZE(1920, 1080));

    for (i = 0; i < test_count; i++) {
        print_tsmux(TSMUX_ERR, "test case %u\n", i);

        /* init */
        memset(&m2m_cmd_queue, 0x0, sizeof(struct tsmux_m2m_cmd_queue));
        memset(&otf_cmd_queue, 0x0, sizeof(struct tsmux_otf_cmd_queue));
        memset(&psi_info, 0x0, sizeof(struct tsmux_psi_info));
        for (j = 0; j < TSMUX_MAX_M2M_CMD_QUEUE_NUM; j++) {
            m2m_cmd_queue.m2m_job[j].pes_hdr.pts39_16 = -1;
            inbuf_addr[j] = NULL;
            outbuf_addr[j] = NULL;
        }
        for (j = 0; j < TSMUX_OUT_BUF_CNT; j++)
            otf_outbuf_addr[j] = NULL;

        mis_matched = 0;

        struct tsmux_asb_job asb_job;
        job_id = load_sfr(&asb_job, type, i /* test case num */);

        if (ioctl(tsmux_fd, TSMUX_IOCTL_ASB_TEST, &asb_job))
            print_tsmux(TSMUX_INFO, "ioctl(TSMUX_IOCTL_ASB_TEST) ret %d\n", ret);

        if (type == M2M_PATH) {
            job = &m2m_cmd_queue.m2m_job[job_id];

            alloc_buffer(&job->in_buf, &job->out_buf, buffer_size, type);

            inbuf_addr[job_id] = (char *)mmap(NULL, job->in_buf.buffer_size,
                PROT_READ | PROT_WRITE, MAP_SHARED, job->in_buf.ion_buf_fd, 0);
            memset(inbuf_addr[job_id], 0x0, job->in_buf.buffer_size);

            outbuf_addr[job_id] = (char *)mmap(NULL, job->out_buf.buffer_size,
                PROT_READ | PROT_WRITE, MAP_SHARED, job->out_buf.ion_buf_fd, 0);
            memset(outbuf_addr[job_id], 0x0, job->out_buf.buffer_size);

            job->in_buf.actual_size = tsmux_testcases[i].input_size;

            if (ioctl(tsmux_fd, TSMUX_IOCTL_M2M_MAP_BUF, &m2m_cmd_queue))
                print_tsmux(TSMUX_INFO, "ioctl(TSMUX_IOCTL_M2M_MAP_BUF) ret %d\n", ret);

            memcpy(inbuf_addr[job_id], tsmux_testcases[i].input, tsmux_testcases[i].input_size);

            if (ioctl(tsmux_fd, TSMUX_IOCTL_M2M_RUN, &m2m_cmd_queue))
                print_tsmux(TSMUX_INFO, "ioctl(TSMUX_IOCTL_M2M_RUN) ret %d\n", ret);
        } else {
            for (j = 0; j < TSMUX_OUT_BUF_CNT; j++) {
                struct tsmux_buffer *buf;
                buf = &otf_cmd_queue.out_buf[j];

                alloc_buffer(NULL, buf, buffer_size, type);

                otf_outbuf_addr[j] = (char *)mmap(NULL, buf->buffer_size,
                    PROT_READ | PROT_WRITE, MAP_SHARED, buf->ion_buf_fd, 0);
                memset(otf_outbuf_addr[j], 0x0, buf->buffer_size);
            }

            if (ioctl(tsmux_fd, TSMUX_IOCTL_OTF_MAP_BUF, &otf_cmd_queue))
                print_tsmux(TSMUX_INFO, "ioctl(TSMUX_IOCTL_OTF_MAP_BUF) ret %d\n", ret);

            while ((ret = ioctl(tsmux_fd, TSMUX_IOCTL_OTF_DQ_BUF, &otf_cmd_queue)) < 0);

            if (ret)
                print_tsmux(TSMUX_ERR, "ioctl(TSMUX_IOCTL_OTF_DQ_BUF) ret: %d\n", ret);
        }

        ret = DmabufHeapCpuSyncStart(gBufferAllocator, job->out_buf.ion_buf_fd, (SyncType)DMA_BUF_SYNC_RW, NULL, NULL);
        if (ret < 0) {
            printf("ERR: cpu sync start fail\n");
            return 0;
        }
        memcpy(golden_addr, tsmux_testcases[i].golden, tsmux_testcases[i].golden_size);

        if (type == M2M_PATH)
            temp_out_addr = outbuf_addr[job_id];
        else
            temp_out_addr = otf_outbuf_addr[otf_cmd_queue.cur_buf_num];
        temp_golden_addr = golden_addr;

        for (j = 0; j < tsmux_testcases[i].golden_size; j += 16) {
            for (k = 0; k < 16; k++) {
                if (j + k >= tsmux_testcases[i].golden_size)
                    break;
                if (temp_golden_addr[k] != temp_out_addr[k]) {
                    print_tsmux(TSMUX_ERR, "%.4d, golden:", j);
                    for (l = 0; l < 16; l++)
                        printf("%.2x ", temp_golden_addr[l]);
                    printf("\n");

                    usleep(10);

                    printf("%.4u, output:", j);
                    for (l = 0; l < 16; l++)
                        printf("%.2x ", temp_out_addr[l]);
                    printf("\n");

                    usleep(10);

                    mis_matched = 1;

                    break;
                }
            }

            usleep(10);

            temp_golden_addr += 16;
            temp_out_addr += 16;
        }

        ret = DmabufHeapCpuSyncEnd(gBufferAllocator, job->out_buf.ion_buf_fd, NULL, NULL);
        if (ret < 0) {
            printf("ERR: cpu sync end fail\n");
            return 0;
        }

        if (mis_matched)
            print_tsmux(TSMUX_ERR, "test_count %d, mis_matched\n", i);
        else
            print_tsmux(TSMUX_ERR, "test_count %d matched\n", i);

        if (type == M2M_PATH) {
            if (ioctl(tsmux_fd, TSMUX_IOCTL_M2M_UNMAP_BUF))
                print_tsmux(TSMUX_ERR, "ioctl(TSMUX_IOCTL_M2M_UNMAP_BUF) ret: %d\n", ret);
        } else {
            if (ioctl(tsmux_fd, TSMUX_IOCTL_OTF_Q_BUF, &otf_cmd_queue))
                print_tsmux(TSMUX_ERR, "ioctl(TSMUX_IOCTL_OTF_Q_BUF) ret: %d\n", ret);
            if (ioctl(tsmux_fd, TSMUX_IOCTL_OTF_UNMAP_BUF))
                print_tsmux(TSMUX_ERR, "ioctl(TSMUX_IOCTL_OTF_UNMAP_BUF) ret: %d\n", ret);
        }

        if (type == M2M_PATH) {
            munmap(inbuf_addr[job_id], job->in_buf.buffer_size);
            munmap(outbuf_addr[job_id], job->out_buf.buffer_size);
            free_buffer(&job->in_buf, &job->out_buf, type);
        } else {
            for (j = 0; j < TSMUX_OUT_BUF_CNT; j++) {
                munmap(otf_outbuf_addr[j], otf_cmd_queue.out_buf[j].buffer_size);
                free_buffer(NULL, &otf_cmd_queue.out_buf[j], type);
            }
        }
    }

    free(golden_addr);

    if (tsmux_fd > 0)
        close(tsmux_fd);

    stop_repeater(repeater_fd);
    FreeDmabufHeapBufferAllocator(gBufferAllocator);

    if (mis_matched)
        return -1;
    else
        return 0;
}

int main()
{
    int ret;

#if defined(M2M)
    print_tsmux(TSMUX_ERR, "m2m test\n");
    ret = test_tsmux(M2M_PATH);
#elif defined(OTF)
    print_tsmux(TSMUX_ERR, "otf test\n");
    ret = test_tsmux(OTF_PATH);
#endif

    return ret;
}
