/******************************************************************************

                  版权所有 (c) 华为技术有限公司 2012-2018

 ******************************************************************************
  文 件 名   : mbox.c
  版 本 号   : 1.0
  作    者   :
  生成日期   : 2019.08.06
  最近修改   : create file
  功能描述   : 容器邮箱基础功能
  函数列表   :

  修改历史   :
  2019.08.06 create file
******************************************************************************/
#include <iostream>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <pthread.h>
#include <sys/file.h>
#include <fcntl.h>
#include <linux/limits.h>
#include "mbox.h"
#include "mbox_extern.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int g_iMboxInotifyFd = -1;

std::unordered_map<int, string> g_MboxWd2Docker;

std::unordered_map<string, int> g_MboxDocker2Wd;

#define SECUREC_MEM_MAX_LEN 0x7fffffffUL

/*
 * mbox_get_inotify_fd: fetch inotify file descriptor
 * no paramter required
 * this function shall return positive value on success, negative value on failure.
 * by no one, on 2019-08-15
 */
int mbox_get_inotify_fd(void)
{
    int iRet = -1;

    if (unlikely(0 >= g_iMboxInotifyFd)) {
        iRet = inotify_init();
        if (0 > iRet) {
            return iRet;
        }
        g_iMboxInotifyFd = iRet;
    }

    return g_iMboxInotifyFd;
}

/*
 * mbox_find_watch_docker : find docker name according watch descriptor
 * @wd: watch descriptor, which is returned by inotify_add_watch()
 * return a string on success, NULL on failure.
 * by no one, on 2019-08-15.
 */
char *mbox_find_watch_docker(int wd)
{
    if (unlikely(0 > wd)) {
        printf("Invalid WD[%d]", wd);
        return NULL;
    }

    auto got = g_MboxWd2Docker.find(wd);
    if (unlikely(got == g_MboxWd2Docker.end())) {
        return NULL;
    }

    return (char *)got->second.c_str();
}

/*
 * mbox_try_lock : try lock a file
 * @fd: file descriptor.
 * @timeout_us: timeout in usecond
 * return 0 success, negative value on failure.
 * by no one, on 2019-08-15.
 */
inline int mbox_try_lock(int fd, int timeout_us)
{
    do {
        if (0 == flock(fd, LOCK_EX | LOCK_NB)) {
            return 0;
        }
        timeout_us -= MBOX_LOCK_WAIT_INTERVAL;
        usleep(MBOX_LOCK_WAIT_INTERVAL);
    } while (timeout_us > 0);

    return -ETIMEDOUT;
}

/*
 * mbox_try_unlock : try unlock a file
 * @fd: file descriptor.
 * @timeout_us: timeout in usecond
 * return 0 success, negative value on failure.
 * by no one, on 2019-08-15.
 */
inline int mbox_try_unlock(int fd, int timeout_us)
{
    do {
        if (0 == flock(fd, LOCK_UN | LOCK_NB)) {
            return 0;
        }
        timeout_us -= MBOX_LOCK_WAIT_INTERVAL;
        usleep(MBOX_LOCK_WAIT_INTERVAL);
    } while (timeout_us > 0);

    return -ETIMEDOUT;
}

/*
 * mbox_write_msg : write message to mailbox file
 * @msg_file: message file in mailbox.
 * @msg: message string
 * @size: message size
 * return 0 success, negative value on failure.
 * by no one, on 2019-08-15.
 */
int mbox_write_msg(const char *msg_file, const char *msg, unsigned int size)
{
    int iRet;
    int iFd = -1;

    if (unlikely(!msg_file || !msg)) {
        printf("Null pointer for msg_file or msg detected!");
        return -EINVAL;
    }

    if (MBOX_MSG_MAX_SIZE < size) {
        printf("Too larger msg size[%d], should <= %d", size, MBOX_MSG_MAX_SIZE);
        return -EINVAL;
    }

    char szFilePath[PATH_MAX] = {0};
    if (realpath(msg_file, szFilePath) == 0) {
        printf("Get msg_file path %s \n", msg_file);
        return -EINVAL;
    }

    iFd = open(szFilePath, O_RDWR | O_TRUNC);
    if (unlikely(0 > iFd)) {
        printf("fail to open file %s, errno=%d", msg_file, errno);
        return -EIO;
    }

    iRet = mbox_try_lock(iFd, MBOX_LOCK_WAIT_TMOUT);
    if (0 != iRet) {
        close(iFd);
        printf("Cannot lock file %s, errno=%d, ret = %d", msg_file, errno, iRet);
        return -EAGAIN;
    }

    /* write msg to mailbox */
    iRet = write(iFd, msg, size);
    if (0 > iRet) {
        printf("write msg to file %s fail, size=%d,  ret=%d, errno=%d", msg_file, iRet, size, errno);
        iRet = mbox_try_unlock(iFd, MBOX_LOCK_WAIT_TMOUT);
        if (0 != iRet) {
            printf("unlock file[%s] fail, ret=%d, errno=%d", msg_file, iRet, errno);
        }
        close(iFd);
        return -EIO;
    }

    /* unlock the mailbox file */
    iRet = mbox_try_unlock(iFd, MBOX_LOCK_WAIT_TMOUT);
    if (0 != iRet) {
        printf("unlock file[%s] fail, ret=%d, errno=%d", msg_file, iRet, errno);
    }

    /* TODO: shall we concern about the ACK? */
    close(iFd);
    return 0;
}

/*
 * mbox_read_msg : read message from mailbox file
 * @msg_file: message file in mailbox.
 * @buf: message buffer
 * @size: buffer size
 * return 0 success, negative value on failure.
 * by no one, on 2019-08-15.
 */
int mbox_read_msg(const char *msg_file, char *buf, unsigned int size)
{
    int iRet;
    int iFd = -1;

    if (unlikely(!msg_file || !buf)) {
        printf("Null pointer detected for msg_file or buf");
        return -EINVAL;
    }

    char szFilePath[PATH_MAX] = {0};
    if (realpath(msg_file, szFilePath) == 0) {
        printf("Get msg_file path %s \n", msg_file);
        return -EINVAL;
    }

    iFd = open(szFilePath, O_RDONLY);
    if (unlikely(0 > iFd)) {
        printf("fail to open file %s, errno=%d", msg_file, errno);
        return -EIO;
    }

    iRet = mbox_try_lock(iFd, MBOX_LOCK_WAIT_TMOUT);
    if (0 != iRet) {
        close(iFd);
        printf("Cannot lock file %s, errno=%d, ret = %d", msg_file, errno, iRet);
        return -EAGAIN;
    }

    iRet = read(iFd, buf, size);
    if (0 > iRet) {
        printf("read msg from file %s fail, size=%d,  ret=%d, errno=%d", msg_file, iRet, size, errno);
        iRet = mbox_try_unlock(iFd, MBOX_LOCK_WAIT_TMOUT);
        if (0 != iRet) {
            printf("unlock file[%s] fail, ret=%d, errno=%d", msg_file, iRet, errno);
        }
        close(iFd);
        return -EIO;
    }

    /* unlock the mailbox file */
    iRet = mbox_try_unlock(iFd, MBOX_LOCK_WAIT_TMOUT);
    if (0 != iRet) {
        printf("unlock file[%s] fail, ret=%d, errno=%d", msg_file, iRet, errno);
    }

    close(iFd);
    return 0;
}

/*
 * mbox_get_event : get inotfiy event
 * @event: pointer of inotify event structure.
 * return 0 success, negative value on failure.
 * on 2019-08-15.
 */
struct inotify_event *mbox_get_event(struct inotify_event *event, unsigned int *offset, unsigned int *end)
{
    int iRet;

    if (!event || !offset || !end) {
        return NULL;
    }

    if (*offset < *end) {
        event += *offset;
        *offset += 1;
        return event;
    }

    *offset = 0;
    *end = 0;

    iRet = read(g_iMboxInotifyFd, event, MBOX_READ_EVENT_MAX * sizeof(struct inotify_event));
    if (iRet <= 0) {
        printf("cannnot read from inotify[%d], ret=%d, errno=%d, %s", g_iMboxInotifyFd, iRet, errno, strerror(errno));
        return NULL;
    }

    *end = iRet / sizeof(struct inotify_event);

    if (unlikely(iRet % sizeof(struct inotify_event))) {
        printf("Strange, read bytes[%d] not multiple of inotify_event size[%d]", iRet,
               (int)sizeof(struct inotify_event));
    }

    *offset += 1;
    return event;
}

int mbox_client_request(const char *request, unsigned int size)
{
    if (!request || !size) {
        printf("Invalid parameter!\n");
        return -1;
    }

    return mbox_write_msg(MBOX_DOWN_PATH, request, size);
}

int mbox_client_watch(const char *watch_file)
{
    int iWd = 0;
    int iFd = -1;

    if (!watch_file) {
        printf("Null pointer for file!\n");
        return -1;
    }

    iFd = mbox_get_inotify_fd();
    if (0 >= iFd) {
        printf("mbox_get_inotify_fd fail, ret=%d", iFd);
        return -EIO;
    }

    iWd = inotify_add_watch(iFd, watch_file, MBOX_WATCH_EVENT_MASK);
    if (0 > iWd) {
        printf("inotify_add_watch fail, file=%s, mask=0x%08x, ret=%d", watch_file, MBOX_WATCH_EVENT_MASK, iWd);
        return -EIO;
    }

    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
