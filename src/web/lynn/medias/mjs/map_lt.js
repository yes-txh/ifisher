$(function () {
	$("#id_line_1").attr("checked",true);
	//数据可以动态生成，格式自己定义，id对应china-zh.js中省份的简称
	function getMapData(){
		$.getJSON('/ajax/jsonmap_lt', function(data){
			$('#dataMap').vectorMap({map: 'china_zh',
				color: "#B4B4B4", //地图颜色
				hoverColor: false,
				//hoverOpacity: 1,
				onLabelShow: function (event, label, code) {
					$.each(data, function (i, item) {
						if (code == item.code) {
							//label.html('');
							label.hide();
							//label.html(item.name + "<br/>" + item.num + "个活跃节点");
						}
					});
				},
		                /*onRegionClick: function(element, code){
        		            $.each(dataStatus, function (i, item) {
                		        if (code == item.id) {
                        		    window.location.href= item.url + '/B/index.html';
		                        }
        		            });
                		},*/
			})
			// set color & add data
			$.each(data, function (i, item) {
				$('.'+item.code+' a').attr('href','/node_p?p=' + item.name)	
				$('.'+item.code+' num').html(item.num)
				if (item.num == 3) {
					var jsonStr = "{" + item.code + ":'#00FF00'}";
					$('#dataMap').vectorMap('set', 'colors', eval('(' + jsonStr + ')'));
				}
				else if(item.num == 2 ) {
					var jsonStr = "{" + item.code + ":'#FFFF00'}";
					$('#dataMap').vectorMap('set', 'colors', eval('(' + jsonStr + ')'));
				}
				else if(item.num == 1 ) {
					var jsonStr = "{" + item.code + ":'#FF7F00'}";
					$('#dataMap').vectorMap('set', 'colors', eval('(' + jsonStr + ')'));
				}
				else if(item.num == 0 ) {
        	        	        var jsonStr = "{" + item.code + ":'#FF0000'}";
                	        	$('#dataMap').vectorMap('set', 'colors', eval('(' + jsonStr + ')'));
	                	}
			});
			$('.jvectormap-zoomin').hide();
			$('.jvectormap-zoomout').hide();
		});
		$("#dataMap a").mousemove(function(e){
			if  ($(this).next().html()!="")
				$("#dataTip").html($(this).html() + "<br/>" + $(this).next().html()+"个活跃节点" +"<br/>点击查看详情").css({"left":e.pageX+10+"px","top":e.pageY-15+"px"}).show()
			else 
				$("#dataTip").html($(this).html() + "<br/>" + "尚未部署").css({"left":e.pageX+10+"px","top":e.pageY-15+"px"}).show()
		});
		$("#dataMap a").mouseout(function(){
			$("#dataTip").hide();
		});
	}
	getMapData();
	setInterval("getMapData()",180 );

	
	$(".line").change(function() {
		line = $("input[name='line']:checked").val();
		window.location.href= '/net_' + line + '/'
	});
});
