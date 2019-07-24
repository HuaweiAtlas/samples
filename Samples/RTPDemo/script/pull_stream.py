# encoding: utf-8
import sys
import json
import constants as CONSTANTS
from task_handle import TaskHander, OpenVideoInfo
import error_code as ERR_CODE

TaskHander = TaskHander()

# get Ipc Info
def getIpcInfo(channelID):
    ipcinfo_code, ipcinfo_json = \
        TaskHander.handle_task(task_id=CONSTANTS.TASK_ID_GET_IPCINFO,
                               channelID=channelID)
    if ipcinfo_code != ERR_CODE.HTTP_STATUS_200:
        sys.stdout.write("get ip info failed." + '\n')
        return False, None
    return True, ipcinfo_json

# get stream status
def getStreamStatus(channelID):
    streamstatus_code, streamstatus_json = \
        TaskHander.handle_task(task_id=CONSTANTS.TASK_ID_GET_STREAMSTATUS,
                               channelID=channelID)
    if streamstatus_code != ERR_CODE.HTTP_STATUS_200:
        sys.stdout.write("get stream status failed." + '\n')
        return False, None
    return True, streamstatus_json

# get stream query
def getStreamQuery(channelID):
    streamQuery_code, streamQuery_json = \
        TaskHander.handle_task(task_id=CONSTANTS.TASK_ID_GET_STREAMQUERY,
                               channelID=channelID)
    if streamQuery_code != ERR_CODE.HTTP_STATUS_200:
        sys.stdout.write("get stream query failed." + '\n')
        return False, None
    return True, streamQuery_json

# get mts pullinfo
def getMtsPullinfo(channelID, serverIp, serverPort):
    mtspullinfo_code, mtspullinfo_json = \
        TaskHander.handle_task(task_id=CONSTANTS.TASK_ID_GET_MTSPULLINFO,
                               channelID=channelID,
                               serverIp=serverIp,
                               serverPort=serverPort,
                               h264Payload=CONSTANTS.H264_PAYLOAD,
                               h265Payload=CONSTANTS.H265_PAYLOAD)
    if mtspullinfo_code != ERR_CODE.HTTP_STATUS_200:
        sys.stdout.write("get MTS pull info failed." + '\n')
        return False, None
    return True, mtspullinfo_json

# get stream start
def getStreamStart(channelID, userInfo, ip, port):
    streamStart_code, streamStart_json = \
        TaskHander.handle_task(task_id=CONSTANTS.TASK_ID_GET_STREAMSTART,
                               userInfo=userInfo,
                               channelID=channelID,
                               ip=ip,
                               port=port,
                               frameInfo="H264",
                               protocol="UDP")
    if streamStart_code != ERR_CODE.HTTP_STATUS_200:
        sys.stdout.write("get stream start failed." + '\n')
        return False, None
    return True, streamStart_json

# get open video
def getOpenVideo(channelID, userInfo, openVideoInfo):
    if openVideoInfo.inProtocol in ("RTSP", "ONVIF"):
        in_Info = {
            "rtspUrl": openVideoInfo.rtsp_url
        }
    elif openVideoInfo.inProtocol == "GB28181":
        in_Info = {
            "remoteIP": openVideoInfo.remoteIP,
            "remotePort": openVideoInfo.remotePort
        }

    out_Info = {
        "IP":openVideoInfo.serverIP,
        "port": openVideoInfo.serverPort,
        "mediaInfo":{
            "video":{
                "h264":openVideoInfo.h264Payload,
                "h265":openVideoInfo.h265Payload
                }
        }
    }
    openvideo_code, openvideo_json = \
        TaskHander.handle_task(task_id=CONSTANTS.TASK_ID_GET_OPENVIDEO,
                               userInfo=userInfo,
                               channelID=channelID,
                               inProtocol=openVideoInfo.inProtocol,
                               inInfo=in_Info,
                               outProtocol="RTP",
                               outInfo=out_Info)
    if openvideo_code != ERR_CODE.HTTP_STATUS_200:
        sys.stdout.write(in_Info + '\n')
        sys.stdout.write(out_Info + '\n')
        sys.stdout.write("get open video failed." + '\n')
        return False, None
    return True, openvideo_json


def UriInfoCallback(channelID, serverIp, serverPort):
    ipcinfo_code, ipcinfo_json = getIpcInfo(channelID)
    if not ipcinfo_code:
        return False, None

    ipcInfo = ipcinfo_json.get("ipcInfo")
    inProtocol = ""
    for items in ipcInfo:
        if items.get("channelID") == channelID:
            ipc_status = items.get("ipcOnlineStatus")
            if ipc_status == "":
                return False, None
            if ipc_status == "connecting":
                return False, None
            inProtocol = items.get("protocol")

    streamstatus_code, streamstatus_json = getStreamStatus(channelID)
    if not streamstatus_code:
        return False, None
    streamStatus = int(streamstatus_json.get("streamStatus"))
    openVideoInfo = OpenVideoInfo()
    openVideoInfo.inProtocol = inProtocol
    userInfo = {
        "userName": "mss",
        "userAddr": "localhost"
    }
    if streamStatus == 0:
        if inProtocol == "GB28181":
            mtspullinfo_code, mts_pullinfo_json = getMtsPullinfo(channelID, serverIp, serverPort)
            if not mtspullinfo_code:
                return False, None
            ipAddr = mts_pullinfo_json.get("ipAddr")
            ipPort = mts_pullinfo_json.get("ipPort")
            streamStart_code, streamStart_json = getStreamStart(channelID, userInfo, ipAddr, ipPort)
            if not streamStart_code:
                return False, None
            info = streamStart_json.get("info")
            openVideoInfo.remoteIP = info.get("remoteIP")
            openVideoInfo.remotePort = info.get("remotePort")
        elif inProtocol in ("RTSP", "ONVIF"):
            streamQuery_code, streamQuery_json = getStreamQuery(channelID)
            if not streamQuery_code:
                return False, None
            streamQuery = streamQuery_json.get("ipcInfo")
            rtspUrl = ""
            for streamitems in streamQuery:
                if streamitems.get("channelID ") == channelID:
                    rtspUrl = streamitems.get("uriInfo")
            openVideoInfo.rtsp_url = rtspUrl

    openVideoInfo.serverIP = serverIp
    openVideoInfo.serverPort = serverPort
    openVideoInfo.h264Payload = CONSTANTS.H264_PAYLOAD
    openVideoInfo.h265Payload = CONSTANTS.H265_PAYLOAD
    openvideo_code, openvideo_json = getOpenVideo(channelID, userInfo, openVideoInfo)
    if not openvideo_code:
        return False, None
    ret_dict = {
        "remoteIp": openvideo_json.get("ipAddr"),
        "remotePort": int(openvideo_json.get("ipPort"))
    }
    return True, json.dumps(ret_dict)

def pull_stream(parameter):
    try:
        if len(parameter) != 4:
            sys.stdout.write("pull stream parameter num error" + '\n')
            exit(1)

        channel_id = parameter[1]
        serverIp = parameter[2]
        serverPort = parameter[3]

        if not isinstance(channel_id, str):
            sys.stdout.write("channel id type error" + '\n')
            exit(1)

    except Exception as e:
        sys.stdout.write(e + '\n')
        exit(1)

    status_code, ret_dict = UriInfoCallback(channel_id, serverIp, serverPort)
    if status_code:
        sys.stdout.write(ret_dict + '\n')
        exit(0)
    else:
        sys.stdout.write("uri info callback failed." + '\n')
        exit(1)


pull_stream(sys.argv)

