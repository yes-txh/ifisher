#-*- coding:utf8 -*-
#/********************************
# FileName: lxc/server.py
# Author:   TangXuehai
# Date:     2013-10-22
# Version:  0.1
# Description: lxc server
#*********************************/

# 导入thrift定义的模块
from gen.jobs_manager.JobsManager import Client
from gen.jobs_manager.ttypes import * # task struct

from thrift.transport.TTransport import TTransportException

from rpc import Rpc
from singleton import Singleton
import logging

# 日志信息存入vm_worker.log
logger = logging.getLogger()
handler=logging.FileHandler("service.log")
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
handler.setFormatter(formatter)
logger.addHandler(handler)
logger.setLevel(logging.NOTSET)

#worker endpoint
global jobs_manager_endpoint 
#jobs_manager_endpoint = "10.128.85.10:9999"
jobs_manager_endpoint = "127.0.0.1:9999"
class Server():
	def __init__(self):
		pass	
	def Task_test(self):
		global task_state_info_list
		task_state_info_list = []
		task1 = TaskStateInfo()	
		task1.job_id = 0
		task1.task_id = 0
		task1.state = "测试"
		task1.machine_ip = "0.0.0.0"
		task1.restart_times = 0
		task1.timeout_times = 0
		task_state_info_list.append(task1)

		task2 = TaskStateInfo()
                task2.job_id = 0
                task2.task_id = 1
                task2.state = "测试"
                task2.machine_ip = "0.0.0.0"
                task2.restart_times = 0
		task2.timeout_times = 0
                task_state_info_list.append(task2)
		return True

	# task state info update 
	def UpdateTaskStateInfo(self):
		job_id_list = []
		try :
                        jobs_manager_client = Rpc(T = Client).GetProxy(jobs_manager_endpoint)
                        job_id_list = jobs_manager_client.GetJobId()
                except  TTransportException, e:
                        return False
		global task_state_info_list 
		task_state_info_list = []
	        for job_id in job_id_list:
			try :
        	                jobs_manager_client = Rpc(T = Client).GetProxy(jobs_manager_endpoint)
                	        task_state_info = jobs_manager_client.GetTaskStateInfo(job_id)
	        	except  TTransportException, e:
                		return False
			for task in task_state_info:
				if task.state == 0:
					str(task.state)
					task.state = "排队"
				elif task.state == 1:
					str(task.state)
                                        task.state = "启动中"
				elif task.state == 2:
                                        str(task.state)
                                        task.state = "运行"
				elif task.state == 3:
                                        str(task.state)
                                        task.state = "完成"
				elif task.state == 4:
                                        str(task.state)
                                        task.state = "失败"
				elif task.state == 5:
                                        str(task.state)
                                        task.state = "失联"
				else :
                                        str(task.state)
                                        task.state = "未发现"
			
				task_state_info_list.append(task)
		return True	
	
	# get server information
	def GetTaskStateInfo(self):
		return task_state_info_list
	

ServerI = Singleton(T=Server)

if __name__ == '__main__':
	ret = ServerI.Instance().Task_test()
	print ret
	
