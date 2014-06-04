	//phytree
	phytree = new dTree('phytree');   //tree div id para: phytree
	phytree.add(0,-1,'物理视图','../physical','','content','','','','menu_phy');//tree root
	$("#phyDiv").html(phytree.toString());
	function getPhyTree(){ 
		//var stime = new Date();
		$.getJSON('/ajax/phytree',function(data){
			//alert('phytree')
			phytree = new dTree('phytree');   //参数tree，表示生成的html代码中id的标记，不影响功能
			phytree.add(0,-1,'物理视图','../physical','','content','','','','menu_phy');     
			$.each(data,function(entryIndex,entry){
				if(entry['pid']==0)
					phytree.add(entry['id'], entry['pid'], entry['name'],'../'+entry['url'],'','content','/media/dtree/img/base.gif','/media/dtree/img/base.gif','',entry['menu']);
				else
					phytree.add(entry['id'], entry['pid'], entry['name'],'../'+entry['url'],'','content','/media/dtree/img/cd.gif','/media/dtree/img/cd.gif','',entry['menu']);
			});
			$("#phyDiv").html(phytree.toString());
		});
	}
	getPhyTree();
	setInterval("getPhyTree()", 5000);
