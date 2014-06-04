	$(function(){
		//var stime = new Date();
		$.getJSON('/ajax/jsonappdata',function(data){
			appdata=""
			var flag = 1
			//apptree = new dTree('apptree');   //tree div id para: apptree   
			$.each(data,function(entryIndex,entry){
				if(entry['type']=='app')
				{	
					flag=0
					tmp = "<tr><td rowspan='1'>" + "<a href='/app-" + entry['id'] + "/'>" + entry['name'] +"</a></td>"					
				}	
				else
				{	
					if(flag==1)
					{
						tmp =	"<tr><td><a></a></td>"
					}
					else
					{
						tmp = ""
						flag = 1
					}	
					tmp = tmp + "<td><a href='/virt-" + entry['id'] + "/'>" + entry['name'] +"</a></td>"
					tmp = tmp + "<td><a>" + entry['vtype'] +"</a></td>"
					tmp = tmp + "<td><a>" + entry['state'] +"</a></td>"
					tmp = tmp + "<td><a>" + entry['os'] +"</a></td>"
					tmp = tmp + "<td><a>" + entry['vip'] +"</a></td>"
					tmp = tmp + "<td><a>" + entry['server'] +"</a></td>"
					tmp = tmp + "<td><a>" + entry['run_app'] +"</a></td>"
					tmp = tmp + "<td><a>" + entry['app_state'] +"</a></td>"
					tmp = tmp + "<td><a>" + entry['app_lasttime'] +"</a></td></tr>"
				}
				appdata = appdata+tmp
			});
			$("tbody").html(appdata.toString());
	 		$(".tables").each(function(i,t){
			 	var obj = $(t);
			 	obj.find("tr:even").addClass('tr1');
			    	obj.find("tr:odd").addClass("tr2");
			});
		});
	});


