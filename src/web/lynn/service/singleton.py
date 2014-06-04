# -*- coding: utf-8 -*-
#/********************************
# FileName: lxc/singleton.py
# Author:   TangXuehai
# Date:     2013-10-22
# Version:  0.1
# Description: lxc singleton
#*********************************/

import threading

class Singleton:  
    def __init__(self, T):
        self.T = T
        self.lock = threading.Lock()
        self.instance = None
    
    def Instance(self):
        self.lock.acquire()
        if not self.instance:
            self.instance = self.T()
        self.lock.release()
        return self.instance
        
