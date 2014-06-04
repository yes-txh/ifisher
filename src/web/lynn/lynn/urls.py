from django.conf.urls import patterns, include, url
from django.conf.urls.defaults import *
from django.conf import settings

# Uncomment the next two lines to enable the admin:
# from django.contrib import admin
# admin.autodiscover()

#from service.views import server

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'lxc.views.home', name='home'),
    # url(r'^lxc/', include('lxc.foo.urls')),
    #url(r'^login$', 'accounts.views.userlogin',name="userlogin"),
    url(r'^$', 'service.views.task_state_info_list',name="task_state_info_list"),
    url(r'^job_add/$', 'service.views.job_add',name="job_add"),
    url(r'^task_state_info_list$', 'service.views.task_state_info_list',name="task_state_info_list"),
  

    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    # url(r'^admin/', include(admin.site.urls)),
    (r'^media/(?P<path>.*)$','django.views.static.serve',{'document_root':settings.STATIC_PATH}),
)

