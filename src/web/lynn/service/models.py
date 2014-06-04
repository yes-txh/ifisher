#coding=utf-8
from django.db import models
from django.forms import ModelForm

class Virt(models.Model):
	name = models.CharField(u"名称", max_length=20, unique=True)
	vtype = models.CharField(u"类型", max_length=50, null=True, blank=True)
	cpunum = models.IntegerField(u"CPU核数")
	mem = models.IntegerField(u"内存")
	size = models.IntegerField(u"虚拟机大小/G", null=True, blank=True)
	img_location = models.CharField(u"镜像路径", max_length=100, null=True, blank=True)
	# iso_location = models.CharField(u"光盘路径", max_length=100, null=True, blank=True)
#	iso = models.ForeignKey(Iso, verbose_name=u"安装光盘", null=True, blank=True)
	os = models.CharField(u"操作系统", max_length=20)
	vip = models.IPAddressField(u"虚拟机IP", max_length=20, null=True, blank=True)
	port = models.IntegerField(u"通信端口", null=True, blank=True)
	vnc_port = models.IntegerField(u"VNC端口", null=True, blank=True)
	state = models.CharField(u"状态", max_length=20, null=True, blank=True)
#	server = models.ForeignKey(Server,verbose_name=u"宿主服务器")
	#server_port = models.IntegerField(u"NAT服务器端口",max_length=20,null=True ,blank =True)
	# workerjob_id = models.ForeignKey(Workerjob,verbose_name=u"虚拟机任务",null=True, blank=True)
	app_lasttime = models.CharField(u"上次应用运行时间", max_length=20, null=True, blank=True)
	app_state = models.CharField(u"应用运行状态", max_length=20, null=True, blank=True)
	# run_app = models.ManyToManyField(App, blank=True, verbose_name=u'运行应用', related_name='virt_run_set')

	def __unicode__(self):
		return self.server.name+"\t"+self.name+"\t"+self.server.line
	class Meta:
		ordering = ['name']
		unique_together = (("vip", "server"),)

