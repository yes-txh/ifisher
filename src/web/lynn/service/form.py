#coding=utf-8
from django.forms import ModelForm
from django import forms
from django.utils.translation import ugettext_lazy as _
from models import Virt
from django.shortcuts import render_to_response,RequestContext


level_choice = (
        ('ext', u"特急"),
        ('ugt', u"紧急"),
        ('nor', u"普通"),
)
size_choice = (
        ('', u"---------"),
        (3, u"3G"),
        (5, u"5G"),
        (10, u"10G"),
        (15, u"15G"),
)

medium_choice = (
        ('', u"---------"),
        ('high', u"高"),
        ('ordinary', u"中"),
        ('low', u"低"),
)

cpunum_choice = (
        (1,u'1'),
        (2,u'2'),
        (3,u'3'),
        (4,u'4'),

)

way_choice = (
        ('', u"---------"),
        ('Default', u"默认"),
        ('WideDistribution', u"广分布"),
)
mem_choice = (
        (512,u'512'),
        (1024,u'1024'),
        (1536,u'1536'),
        (2048,u'2048'),
        (4096,u'4096'),

)

img_choice = (
        ('centos', u"centos"),
#       ('centos_10G', u"centos_10G"),
        ('ubuntu', u"ubuntu"),
#       ('ubuntu-11.10_10G', u"ubuntu-11.10_10G"),
        ('windows_xp', u"windows_xp"),
#       ('windows_xp_7G', u"windows_xp_7G"),
        ('windows_7',u"win7"),
)

img_large = (
        ('10G', u"10G"),
        ('20G',u"20G"),
)

class JobSubmit(ModelForm):
        medium = forms.ChoiceField(label=_(u"优先级"),required=True, choices=medium_choice)
        way = forms.ChoiceField(label=_(u"调度模式"),required=True, choices=way_choice)
        cpunum = forms.ChoiceField(label=_(u"CPU核数"),required=True, choices=cpunum_choice)
        mem = forms.ChoiceField(label=_(u"内存大小"),required=True, choices=mem_choice)
        system = forms.ChoiceField(label=_(u"运行环境"),required=True, choices=img_choice)
        appname = forms.CharField(label=_(u"应用名称"),required=True, max_length=30)
        imglar = forms.ChoiceField(label=_(u"镜像大小"),required=True, choices=img_large)
	timeout = forms.CharField(label=_(u"超时大小"), required=True)
        content = forms.FileField(label=u'配置文件路径')
        class Meta:
		model = Virt
                fields = ('medium','way' ,'appname','cpunum','mem','system','imglar','timeout','content')

	
class TaskListForm(forms.Form):
	job_id = forms.IntegerField(label=u'作业号')
	task_id = forms.IntegerField(label=u'任务号')
        state = forms.CharField(label=u'状态')
        machine_ip = forms.CharField(label=u'部署机器')
	timeout_times = forms.IntegerField(label=u'超时次数')
        restart_times = forms.IntegerField(label=u'重部署次数')
