/******************************************************************************

                  版权所有 (C),  华为技术有限公司

 ******************************************************************************
  文 件 名   : mbox.h
  版 本 号   : 1.0
  作    者   : 
  生成日期   : 2019.08.06
  最近修改   : create file
  功能描述   : 容器邮箱功能
  函数列表   :

  修改历史   :
  2019.08.06 create file
******************************************************************************/
#ifndef __HA_MBOX_H__
#define __HA_MBOX_H__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/inotify.h>
#include <string.h>

using namespace std;

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define MBOX_SERVER_UP_CHAN    "up"
#define MBOX_SERVER_DOWN_CHAN  "down"
#define MBOX_SERVER_PATH    "/var/log/docker/ha-mailbox"

#define MBOX_WATCH_EVENT_MASK   (IN_CLOSE_WRITE)
#define MBOX_PATH_MAX_SIZE  512

#define MBOX_LOCK_WAIT_INTERVAL 100             /* 100us */
#define MBOX_LOCK_WAIT_TMOUT    (10 * MBOX_LOCK_WAIT_INTERVAL)  /* 1ms */

#define MBOX_MSG_MAX_SIZE   512    /* mailbox message size */
#define MBOX_READ_EVENT_MAX      32

#ifndef unlikely
#define unlikely(x)       __builtin_expect((x), 0)
#endif


int mbox_get_inotify_fd(void);
char *mbox_find_watch_docker(int wd);

inline int mbox_try_lock(int fd, int timeout_us);
inline int mbox_try_unlock(int fd, int timeout_us);
int mbox_write_msg(const char *msg_file, const char *msg, unsigned int size);
int mbox_read_msg(const char *msg_file, char *buf, unsigned int size);
struct inotify_event *mbox_get_event(struct inotify_event *event, unsigned int *offset, unsigned int *end);
int mbox_client_request(const char *request, unsigned int size);
int mbox_client_watch(const char *watch_file);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


