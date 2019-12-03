/******************************************************************************

                  版权所有 (c) 华为技术有限公司 2012-2018

 ******************************************************************************

  文 件 名   : mbox_extern.h
  版 本 号   : 1.0
  作    者   :
  生成日期   : 2019.08.15
  最近修改   : create file
  功能描述   : 容器邮箱外部包含头文件
  函数列表   :

  修改历史   :
  2019.08.15 create file
******************************************************************************/
#ifndef __HA_MBOX_EXTERN_H__
#define __HA_MBOX_EXTERN_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/inotify.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MBOX_CLIENT_UP_CHAN "up"
#define MBOX_CLIENT_DOWN_CHAN "down"

#define MBOX_UP_PATH "/var/local/ha-mailbox/up"
#define MBOX_DOWN_PATH "/var/local/ha-mailbox/down"

/* mailbox request strings. Client to Server */
#define MBOX_FAILOVER_REQUEST_STR "failover request"
#define MBOX_QUERY_STATUS_STR "query ha status"

/* mailbox notify strings. Server to Client */
#define MBOX_ACTIVE_NOTIFY_STR "active notify"
#define MBOX_STANDBY_NOTIFY_STR "standby notify"
#define MBOX_UNKNOWN_NOTIFY_STR "unknown notify"

typedef enum { MBOX_NOTIFY_SWITCH_ACTIVE = 0, MBOX_NOTIFY_SWITCH_STANDBY, MBOX_NOTIFY_BUTT } MBOX_NOTIFY_EN;

/* external call functions */
extern int mbox_attach_docker(const char *docker);
extern int mbox_detach_docker(const char *docker);
extern int mbox_notify_docker(const char *docker, MBOX_NOTIFY_EN note);
extern int mbox_module_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
