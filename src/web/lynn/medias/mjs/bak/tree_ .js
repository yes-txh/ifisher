	//apptree
	apptree = new dTree('apptree');   //div id para:apptree
	apptree.add(0,-1,'应用视图','../appmgt','','content','','','','menu_phy');//apptree root
	$("#workDiv").html(apptree.toString());

	phytree = new dTree('phytree');   //div id para:phytree
	phytree.add(0,-1,'物理视图','../physical','','content','','','','menu_phy');//phytree root
	$("#phyDiv").html(phytree.toString());

	//	ajax function
	function getTree(){
		//var stime = new Date();
		$.getJSON('/ajax/apptree',function(data){
			alert('apptree')
			apptree = new dTree('apptree');   //div id para:apptree
			apptree.add(0,-1,'应用视图','../appmgt','','content','','','','menu_phy');     
			$.each(data,function(entryIndex,entry){
				apptree.add(entry['id'], entry['pid'], entry['name'],'../'+entry['url'],'','content','/media/dtree/img/base.gif','/media/dtree/img/base.gif','',entry['menu']);
			});
			$("#workDiv").html(apptree.toString());

			//ajax request in another request

			function getTree(){ 
            //var stime = new Date();
            $.getJSON('/ajax/phytree',function(data){
alert('phytree')
                  phytree = new dTree('phytree');   //参数tree，表示生成的html代码中id的标记，不影响功能
                  phytree.add(0,-1,'物理视图','../physical','','content','','','','menu_phy');     
                  $.each(data,function(entryIndex,entry){
                       phytree.add(entry['id'], entry['pid'], entry['name'],'../'+entry['url'],'','content','/media/dtree/img/base.gif','/media/dtree/img/base.gif','',entry['menu']);
                                                 });
                  $("#phyDiv").html(phytree.toString());
                                });
                    }
		 getTree();
		 setInterval("getTree()",3000 );
                                });
                    }
		 getTree();
		 setInterval("getTree()",3000 );
