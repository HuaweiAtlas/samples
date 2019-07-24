# encoding: utf-8
import os
import sys
import config

class MSSFactoryCreate:
    def __init__(self):
        self.mss_config = config.MSSConfig()
        self.mss_config.read_ini_config()
        self.map = {
            'config': self.mss_config
        }

    def factory_add(self, name, value):
        self.map[name] = value

    def factory_get(self, name):
        if self.map[name] is None:
            sys.stdout.write(str(name) + ' is not Init Now' + '\n')
        return self.map[name]

mss_factory = MSSFactoryCreate()
mss_config = mss_factory.factory_get('config')
