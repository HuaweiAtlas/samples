# encoding: utf-8
import requests
import sys
_TIMEOUT = 5

class HttpClient:
    def __init__(self):
        pass

    @classmethod
    def call(cls, method, url, body=None):
        headers = {'Content-Type': 'application/json'}
        try:
            response = requests.request(method, url,
                                        headers=headers,
                                        data=body,
                                        timeout=_TIMEOUT,
                                        verify=False)
            return response.status_code, response.json()

        except Exception as e:
            sys.stdout.write("Exception:" + str(e) + '\n')
            return False, None
