#-*- coding:utf8 -*-
#/********************************
# FileName: lxc/demo.py
# Author:   TangXuehai
# Date:     2013-10-22
# Version:  0.1
# Description: lxc main
#*********************************/
##system 
import time
import logging
import threading

# 日志信息存入vm_worker.log
logger = logging.getLogger()
handler=logging.FileHandler("service.log")
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
handler.setFormatter(formatter)
logger.addHandler(handler)
logger.setLevel(logging.NOTSET)

#import server class
from server import ServerI

#interval
global interval 
interval = 15

# update server and lxc info 
def UpdateThread():
    #init server info
    #server = Server()
    #ret = ServerI.Instance().ServerInit()
    ret = ServerI.Instance().Task_test()
    if ret == False:
	return False
    while(True):
	#ret = ServerI.Instance().SetAllLxcInfo()
	ret = ServerI.Instance().UpdateTaskStateInfo()
	if ret == False:
		time.sleep(interval)
		continue
	# for test
	time.sleep(interval)

def DemoThread():
     #create thread
    update_thread = threading.Thread(target=UpdateThread)
    update_thread.start()

#for test	
if __name__ == '__main__':

    #create thread
    update_thread = threading.Thread(target=UpdateThread)
    update_thread.start()
    count_num = 0
    #daemon thread
    while True:
        if not update_thread.is_alive():
                 update_thread = threading.Thread(target=UpdateThread)
                 update_thread.start()
        print count_num
        count_num = count_num + 1
        time.sleep(100)

