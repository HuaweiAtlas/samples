/* mbox_test.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <iostream>
#include "mbox.h"
#include "mbox_extern.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

bool g_bStopRecv = false;
char g_acWatchFile[MBOX_PATH_MAX_SIZE] = MBOX_UP_PATH;

#define MBOX_FAILOVER_REQUEST_STR   "failover request"
#define MBOX_QUERY_STATUS_STR       "query ha status"

void *RecvMsgThread(void *pvArg)
{
    printf("==========begin receive msg=============\n");

    int iRet = 0;
    unsigned int uiOffset = 0;
    unsigned int uiEnd = 0;

    struct inotify_event *pstEvt = NULL;
    struct inotify_event astEvent[MBOX_READ_EVENT_MAX] = {0};

    char acBuf[MBOX_MSG_MAX_SIZE] = {0};
    while (!g_bStopRecv)
    {
        pstEvt = mbox_get_event(astEvent, &uiOffset, &uiEnd);
        if (NULL == pstEvt)
        {
            usleep(1*1000);
            continue;
        }

        iRet = mbox_read_msg(g_acWatchFile, acBuf, sizeof(acBuf));
        if (0 != iRet)
        {
            printf("mbox_read_msg fail, file=%s, ret=%d\n", g_acWatchFile, iRet);
            continue;
        }

        printf("Recv message===>>>[%s] \n", acBuf);
    }
    
    printf("==========exit  receive msg=============\n");
    return NULL;
}

/* 开始接收mbox发送过来的消息 */
int start_recv()
{
    pthread_t thread;
    
    int iRet = 0;
    iRet = pthread_create(&thread, NULL, RecvMsgThread, 0);
    if (0 != iRet) 
    {
        printf("==========create recv thread failed=============\n");
    } 

    return iRet;
}

/* 向mbox发送消息 */
int send_mbox_message(const char* request, int nLength)
{
    int iRet = 0;

    iRet = mbox_client_request(request, nLength);
    if (0 != iRet)
    {
        printf("mbox_send fail, ret=%d\n", iRet);
        return -1;
    }

    printf("====>>>>send message success! [%s]\n", request);
    return 0;
    
}

/* 查询主备状态 */
int query_ha_status()
{
    return send_mbox_message(MBOX_QUERY_STATUS_STR, sizeof(MBOX_QUERY_STATUS_STR));
}

/* 强制倒换 */
int forceswap()
{
    return send_mbox_message(MBOX_FAILOVER_REQUEST_STR, sizeof(MBOX_FAILOVER_REQUEST_STR));
}


/* 停止接收mbox发送过来的消息 */
int stop_recv()
{
    g_bStopRecv = true;
    return query_ha_status();
}


/*  初始化 */
int init_mbox()
{
    int iRet = 0;
    iRet = mbox_client_watch(g_acWatchFile);
    if (0 != iRet)
    {
        printf("init mbox fail, ret=%d\n", iRet);
        return -1;
    }

    printf("init mbox success!\n");
    return 0;
}

/*
1.初始化
2.接收消息
3.查询 主备状态
4.发送
5.请求倒换
 */

int main(int argc, char *argv[])
{
    printf("===>>>mbox client start!\n");
    

    while(1)
    {
        printf("1-init mbox\n"
               "2-start receive message\n"
               "3-stop receive\n"
               "4-query ha status\n"
               "5-force swap\n");
        printf("please input operation\n");
        int nOpt = 0;
        cin>>nOpt;
        switch(nOpt)
        {
          case 1:
            init_mbox();
            break;
          case 2:
            start_recv();
            break;
          case 3:
            {
                stop_recv();
                return 0;
            }
            break;
          case 4:
            query_ha_status();
            break;
          case 5:
            forceswap();
            break;
          default:
            break;
        }

    }
    
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


