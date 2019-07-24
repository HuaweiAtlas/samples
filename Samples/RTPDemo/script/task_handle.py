# encoding: utf-8
import json
from create import mss_config
from client_common import HttpClient as client
import constants as CONSTANTS

class OpenVideoInfo:
    def __init__(self):
        self.inProtocol = 'admin'
        self.rtsp_url = 'rtsp'
        self.remoteIP = '-1'
        self.remotePort = '-1'
        self.serverIP = '-1'
        self.serverPort = '-1'
        self.h264Payload = '-1'
        self.h265Payload = '-1'

class TaskHander:
    def __init__(self):
        self.get_openvideo = {
            'svc': 'mts', 'method': 'POST', 'url_postfix': '/mts/v1/transmit'
        }
        self.get_ipcinfo = {
            'svc': 'mgs', 'method': 'GET', 'url_postfix': '/mgs/v1/ipc/info/query/?channelid='
        }
        self.get_streamQuery = {
            'svc': 'mgs', 'method': 'GET', 'url_postfix': '/mgs/v1/ipc/stream/query/?channelid='
        }
        self.get_mtspullinfo = {
            'svc': 'mts', 'method': 'GET', 'url_postfix': '/mts/v1/pullinfo/query/?channelid='
        }
        self.get_streamStart = {
            'svc': 'mgs', 'method': 'POST', 'url_postfix': '/mgs/v1/ipc/stream/start'
        }
        self.get_streamStatus = {
            'svc': 'mts', 'method': 'GET', 'url_postfix': '/mts/v1/streamstatus/query/?channelid='
        }
        self.hosts = mss_config.hosts

    def handle_task(self, **kwargs):
        if kwargs.get('task_id') == CONSTANTS.TASK_ID_GET_IPCINFO:
            method = self.get_ipcinfo.get('method')
            url_prefix = CONSTANTS.HTTP
            host_addr = self.hosts.get('mgs')
            url_postfix = self.get_ipcinfo.get('url_postfix')
            url = url_prefix + host_addr + url_postfix + kwargs.get('channelID')

            task_kw = {}
        elif kwargs.get('task_id') == CONSTANTS.TASK_ID_GET_STREAMQUERY:
            method = self.get_streamQuery.get('method')
            url_prefix = CONSTANTS.HTTP
            host_addr = self.hosts.get('mgs')
            url_postfix = self.get_streamQuery.get('url_postfix')
            url = url_prefix + host_addr + url_postfix+ kwargs.get('channelID')

            task_kw = {}
        elif kwargs.get('task_id') == CONSTANTS.TASK_ID_GET_MTSPULLINFO:
            method = self.get_mtspullinfo.get('method')
            url_prefix = CONSTANTS.HTTP
            host_addr = self.hosts.get('mts')
            url_postfix = self.get_mtspullinfo.get('url_postfix')
            url = (url_prefix + host_addr + url_postfix + kwargs.get('channelID')
                   + ("&inprotocol=GB28181") + ("&outprotocol=RTP") + ("&ip=")
                   + kwargs.get('serverIp') + ("&port=") + kwargs.get('serverPort')
                   + ("&h264=") + kwargs.get('h264Payload')
                   + ("&h265=") + kwargs.get('h265Payload'))

            task_kw = {}
        elif kwargs.get('task_id') == CONSTANTS.TASK_ID_GET_STREAMSTART:
            method = self.get_streamStart.get('method')
            url_prefix = CONSTANTS.HTTP
            host_addr = self.hosts.get('mgs')
            url_postfix = self.get_streamStart.get('url_postfix')
            url = url_prefix + host_addr + url_postfix

            task_kw = {
                "userInfo": kwargs.get('userInfo'),
                "channelID": kwargs.get('channelID'),
                "IP": kwargs.get('ip'),
                "port": kwargs.get('port'),
                "frameInfo": kwargs.get('frameInfo'),
                "protocol": kwargs.get('protocol')
            }
        elif kwargs.get('task_id') == CONSTANTS.TASK_ID_GET_OPENVIDEO:
            method = self.get_openvideo.get('method')
            url_prefix = CONSTANTS.HTTP
            host_addr = self.hosts.get('mts')
            url_postfix = self.get_openvideo.get('url_postfix')
            url = url_prefix + host_addr + url_postfix

            task_kw = {
                "userInfo": kwargs.get('userInfo'),
                "channelID": kwargs.get('channelID'),
                "inProtocol": kwargs.get('inProtocol'),
                "inInfo": kwargs.get('inInfo'),
                "outProtocol": kwargs.get('outProtocol'),
                "outInfo": kwargs.get('outInfo'),
            }
        elif kwargs.get('task_id') == CONSTANTS.TASK_ID_GET_STREAMSTATUS:
            method = self.get_streamStatus.get('method')
            url_prefix = CONSTANTS.HTTP
            host_addr = self.hosts.get('mts')
            url_postfix = self.get_streamStatus.get('url_postfix')
            url = url_prefix + host_addr + url_postfix + kwargs.get('channelID')

            task_kw = {}

        body = json.dumps(task_kw)
        resp = client.call(method=method, url=url, body=body)

        if not resp:
            return None, None

        status_code = resp[0]
        resp_json = resp[1]
        return status_code, resp_json

TaskHande = TaskHander()
