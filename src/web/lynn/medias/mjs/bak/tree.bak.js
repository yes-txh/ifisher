                    //生成新的树，ajax方式获取最新tree，每个json item表示一个节点
		tree = new dTree('tree');   //参数tree，表示生成的html代码中id的标记，不影响功能
		//tree.add(0,-1,'物理视图','../physical','','content','','','','menu_phy');//tree root
		tree.add(0,-1,'云管理平台','','','content','','','','menu_phy');//tree root
		$("#treeDiv").html(tree.toString());
		function getTree(){ 
            //var stime = new Date();
            $.getJSON('/ajax/tree',function(data){
alert('tree')
                  tree = new dTree('tree');   //参数tree，表示生成的html代码中id的标记，不影响功能
                  tree.add(0,-1,'云管理平台','','','content','','','','menu_phy');     
                  $.each(data,function(entryIndex,entry){
                       tree.add(entry['id'], entry['pid'], entry['name'],'../'+entry['url'],'','content','/media/dtree/img/base.gif','/media/dtree/img/base.gif','',entry['menu']);
                                                 });
                  $("#treeDiv").html(tree.toString());
                                });
                    }
		 getTree();
		 setInterval("getTree()",3000 );
