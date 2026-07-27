/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef _REPEATER_H_
#define _REPEATER_H_
#define MAX_SHARED_BUFFER_NUM 3
struct repeater_info {
  int pixel_format;
  int width;
  int height;
  int buffer_count;
  int fps;
  int buf_fd[MAX_SHARED_BUFFER_NUM];
};
#define REPEATER_IOCTL_MAGIC 'R'
#define REPEATER_IOCTL_MAP_BUF _IOWR(REPEATER_IOCTL_MAGIC, 0x10, struct repeater_info)
#define REPEATER_IOCTL_UNMAP_BUF _IO(REPEATER_IOCTL_MAGIC, 0x11)
#define REPEATER_IOCTL_START _IO(REPEATER_IOCTL_MAGIC, 0x20)
#define REPEATER_IOCTL_STOP _IO(REPEATER_IOCTL_MAGIC, 0x21)
#define REPEATER_IOCTL_PAUSE _IO(REPEATER_IOCTL_MAGIC, 0x22)
#define REPEATER_IOCTL_RESUME _IO(REPEATER_IOCTL_MAGIC, 0x23)
#define REPEATER_IOCTL_DUMP _IOR(REPEATER_IOCTL_MAGIC, 0x31, int)
#define REPEATER_IOCTL_SET_MAX_SKIPPED_FRAME _IOW(REPEATER_IOCTL_MAGIC, 0x40, int)
#define REPEATER_IOCTL_GET_IDLE_INFO _IOR(REPEATER_IOCTL_MAGIC, 0x41, int)
#endif
