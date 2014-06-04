from django.shortcuts import render_to_response,get_object_or_404
from django.http import HttpResponseRedirect
from django.core.urlresolvers import reverse
from django.template import Template, Context, RequestContext
from django.template.loader import get_template

from django.utils import simplejson
from django.core import serializers

from django.core.paginator import Paginator, InvalidPage, EmptyPage

from form import *
from server import ServerI

import string,os
from filereplace import replace

#thrift
from gen.jobs_manager.JobsManager import Client
from gen.jobs_manager.ttypes import * # task struct

from thrift.transport.TTransport import TTransportException

from rpc import Rpc

import os, string, shutil
global jobs_manager_endpoint
jobs_manager_endpoint = "10.128.85.10:9999"

# server
def task_state_info_list(request):
        #if not request.user.is_authenticated():
        #        return HttpResponseRedirect(reverse("loginerror"))
        #username = request.user.usernamie
        #user = get_object_or_404(User,username=username)
        task_state_info_list = ServerI.Instance().GetTaskStateInfo()
	tasklistform = TaskListForm()
	#print task_state_info_list
	paginator = Paginator(task_state_info_list, 15)
        # Make sure page request is an int. If not, deliver first page.
        try:
                page = int(request.GET.get('page', '1'))
        except ValueError:
                page = 1

        # If page request (9999) is out of range, deliver last page of results.
        try:
                tasks = paginator.page(page)
        except (EmptyPage, InvalidPage):
                tasks = paginator.page(paginator.num_pages)

        dic = {'task_state_info_list':tasks, 'tasklistform':tasklistform}
        #dic = {'server': server, 'lxc_list':lxc_list, 'lxclistform':lxclistform, 'cpu_list':cpu_list, 'cpuform':cpuform}
        return render_to_response('task_state_info_list.html',dic)


def job_add(request):
        form = JobSubmit()
        dic = {'form':form}
        if request.method=="POST":
                form = JobSubmit(request.POST,request.FILES)
                if form.is_valid():
                        cpunum = form.cleaned_data["cpunum"]
                        mem = form.cleaned_data["mem"]
                        priority = form.cleaned_data["medium"]
                        way = form.cleaned_data["way"]
                        appname = form.cleaned_data["appname"]
                        imgsize = form.cleaned_data["imglar"]
                        system = form.cleaned_data["system"]
			timeout = form.cleaned_data["timeout"]

                        repla_file = "./xml_file/submit_job_model.xml"
			temp = os.path.splitext(repla_file)[0] + ".tmp"
			#return HttpResponse('hello')
			fi = open(repla_file, "r")
			fo = open(temp, "w")
			s = fi.read()
		        s = string.replace(s, "T_CPU", cpunum)
			s = string.replace(s, "T_MEMORY", mem)
			s = string.replace(s, "APP_NAME", appname)
			s = string.replace(s, "T_PRIORITY", priority)
			s = string.replace(s, "T_SCHED_MODEL",way)
			img_name = system + ".qcow2" 
			s = string.replace(s, "T_IMG", img_name)
			s = string.replace(s, "T_TIME", timeout)
			fo.write(s)
			fi.close()
                        fo.close()

			f_tmp = open(temp,"a")
			f = request.FILES['content']
                        de_name = f.name.encode('UTF-8')
                        file_name = str(de_name)	
			lines = f.readlines()
			task_str = "<tasks>\n"
                        for line in lines:
				if line == '\n':
					continue
				line = string.replace(line, "\n", "")
                                task_str = task_str + "<task>\n"
				task_str = task_str + "<exe_path>" + line + "</exe_path>\n"
				task_str = task_str + "<task_num>1</task_num>\n"
				task_str = task_str + "</task>\n"
			task_str = task_str + "</tasks>\n</job>"
			f_tmp.write(task_str)
			f_tmp.close()

                        file_obj = open(temp,"r")
                        file_string = file_obj.read()
                        file_obj.close()
			result = SubmitJobResult()
			try :
                                jobs_manager_client = Rpc(T = Client).GetProxy(jobs_manager_endpoint)
                                result = jobs_manager_client.SubmitJob(file_string)
                        except  TTransportException, e:
                                logger.error(e)	
				return HttpResponse('submit job error')
                        return HttpResponseRedirect(reverse("service.views.task_state_info_list"))
        return render_to_response('job_submit.html', dic, context_instance=RequestContext(request))

