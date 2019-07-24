# encoding: utf-8
import configparser
import os
import socket
import sys
import constants as CONSTANTS

class MSSConfig:
    def __init__(self):
        self.mss_version = ""
        self.mts_host = ""
        self.mgs_host = ""
        self.hosts = {
            'mts': "",
            'mgs': ""
        }

    def read_ini_config(self):
        try:
            cf = configparser.ConfigParser()
            cf.read(CONSTANTS.CONFIG_FILENAME)
            self.mss_version = str(cf.get("v1", "mss_version"))
            self.mts_host = str(cf.get("v1", "mts_host"))
            self.mgs_host = str(cf.get("v1", "mgs_host"))
            self.hosts = {
                'mts': self.mts_host,
                'mgs': self.mgs_host
            }

        except Exception as e:
            self.mss_version = 'x.x.x'
            sys.stdout.write("read config.ini error: " + str(e) + '\n')

if __name__ == '__main__':
    MSSConfig = MSSConfig()
