	//apptree
	apptree = new dTree('apptree');   //tree div id para: apptree
	apptree.add(0,-1,'应用视图','../appmgt','','content','','','','menu_app');//tree root
	$("#workDiv").html(apptree.toString());
	function getAppTree(){
		//var stime = new Date();
		$.getJSON('/ajax/apptree',function(data){
			//alert('apptree')
			apptree = new dTree('apptree');   //tree div id para: apptree
			apptree.add(0,-1,'应用视图','../appmgt','','content','','','','menu_app');     
			$.each(data,function(entryIndex,entry){
				if(entry['pid']==0)
					apptree.add(entry['id'], entry['pid'], entry['name'],'../'+entry['url'],'','content','/media/dtree/img/icon_success.gif','/media/dtree/img/icon_success.gif','',entry['menu']);
				else
					apptree.add(entry['id'], entry['pid'], entry['name'],'../'+entry['url'],'','content','/media/dtree/img/cd.gif','/media/dtree/img/cd.gif','',entry['menu']);
			});
			$("#workDiv").html(apptree.toString());
		});
	}
	getAppTree();
	setInterval("getAppTree()", 10000);
