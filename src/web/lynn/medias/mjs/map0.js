$(function () {
	//数据可以动态生成，格式自己定义，id对应china-zh.js中省份的简称
	var dataStatus = [{ id: 'HKG', name: '香港', num: 3, url: '' },
                             { id: 'HAI', name: '海南', num: 3, url: '' },
                             { id: 'YUN', name: '云南', num: 3, url: '' },
                             { id: 'BEJ', name: '北京', num: 2, url: '' },
                             { id: 'TAJ', name: '天津', num: 3, url: '' },
                             { id: 'XIN', name: '新疆', num: 3, url: '' },
                             //{ id: 'TIB', name: '西藏', num: 0, url: '' },
                             { id: 'QIH', name: '青海', num: 3, url: ''},
                             //{ id: 'GAN', name: '甘肃', num: 0, url: '' },
                             { id: 'NMG', name: '内蒙古', num: 3, url: '' },
                             { id: 'NXA', name: '宁夏', num: 1, url: '' },
                             { id: 'SHX', name: '山西', num: 3, url: '' },
                             { id: 'LIA', name: '辽宁', num: 3, url: '' },
                             { id: 'JIL', name: '吉林', num: 3, url: '' },
                             { id: 'HLJ', name: '黑龙江', num: 0, url: '' },
                             { id: 'HEB', name: '河北', num: 3, url: '' },
                             //{ id: 'SHD', name: '山东', num: 0, url: '' },
                             { id: 'HEN', name: '河南', num: 2, url: '' },
                             { id: 'SHA', name: '陕西', num: 1, url: '' },
                             { id: 'SCH', name: '四川', num: 2, url: '' },
                             { id: 'CHQ', name: '重庆', num: 3, url: '' },
                             { id: 'HUB', name: '湖北', num: 0, url: '' },
                             { id: 'ANH', name: '安徽', num: 1, url: '' },
                             { id: 'JSU', name: '江苏', num: 1, url: '' },
                             //{ id: 'SHH', name: '上海', num: 0, url: '' },
                             { id: 'ZHJ', name: '浙江', num: 3, url: '' },
                             { id: 'FUJ', name: '福建', num: 3, url: '' },
                             //{ id: 'TAI', name: '台湾', num: 0, url: '' },
                             { id: 'JXI', name: '江西', num: 3, url: '' },
                             { id: 'HUN', name: '湖南', num: 3, url: '' },
                             { id: 'GUI', name: '贵州', num: 3, url: '' },
                             { id: 'GXI', name: '广西', num: 3, url: '' }, 
                             { id: 'GUD', name: '广东', num: 3, url: ''}];
	
	$('#dataMap').vectorMap({ map: 'china_zh',
		color: "#B4B4B4", //地图颜色
		hoverColor: false,
		hoverOpacity: 1,
		onLabelShow: function (event, label, code) {
			$.each(dataStatus, function (i, item) {
				if (code == item.id) {
					label.html();
					label.hied();
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
	// set color
	$.each(dataStatus, function (i, item) {
		if (item.num == 3) {
		var josnStr = "{" + item.id + ":'#00FF00'}";
			$('#dataMap').vectorMap('set', 'colors', eval('(' + josnStr + ')'));
		}
		else if(item.num == 2 ) {
			var josnStr = "{" + item.id + ":'#FFFF00'}";
			$('#dataMap').vectorMap('set', 'colors', eval('(' + josnStr + ')'));
		}
		else if(item.num == 1 ) {
			var josnStr = "{" + item.id + ":'#FF7F00'}";
			$('#dataMap').vectorMap('set', 'colors', eval('(' + josnStr + ')'));
		}
		else if(item.num == 0 ) {
                        var josnStr = "{" + item.id + ":'#FF0000'}";
                        $('#dataMap').vectorMap('set', 'colors', eval('(' + josnStr + ')'));
                }

	});
	$('.jvectormap-zoomin').hide();
	$('.jvectormap-zoomout').hide();
	$("#dataMap a").mousemove(function(e){$("#dataTip").html($(this).html() + "<br/>" + $(this).next().html()+"个活跃节点" +"<br/>点击查看详情").css({"left":e.pageX+10+"px","top":e.pageY-15+"px"}).show()}).mouseout(function(){$("#dataTip").hide();});
});
