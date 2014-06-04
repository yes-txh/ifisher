		var chart_cpu;
		var chart_mem;
		var chart_rx; 
		var chart_tx; 
		var maxmem;
		var mem;
		var pre_cpu = 0
		var pre_cpu_total = 0
		var pre_rb = 0
		var pre_tb = 0
		var flag = 1
		var timeout =5000
		var para = window.location.search;
		function getQueryStringRegExp(name)
		{
    			var reg = new RegExp("(^|\\?|&)"+ name +"=([^&]*)(\\s|&|$)", "i");
    			if (reg.test(location.href)) return unescape(RegExp.$2.replace(/\+/g, " ")); return "";
		};
		var ip = getQueryStringRegExp('ip');
		
 		
		//更新
		function requestData() {
			var stime = new Date();
			$.ajax({
				//async: false,
				url:'/ajax/jsonserver/'+ para, 
				success: function(point) {
              
				// add 4 points
				var x = (new Date()).getTime() + 8*3600*1000; // 当前时间 
               			var out = eval("(" + point + ")")
               			//var out = eval( point )
				maxmem = out[0]
				mem = out[1]
				cpu_use = out[2]
				cpu_total = out[3]
				rb = out[4]
 				tb = out[5]

				memrate = mem/maxmem * 100
				//cpu
				d_cpu = cpu_use - pre_cpu
				d_cpu_total = cpu_total - pre_cpu_total
				pre_cpu = cpu_use
				pre_cpu_total = cpu_total
	    			cpurate = d_cpu/d_cpu_total *100
				if(cpurate == 100)
					cpurate = 99.9
				//rb tb
				d_rb = rb - pre_rb
				d_tb = tb - pre_tb
				pre_rb = rb
				pre_tb = tb
	    			rkb = d_rb/1024
				tkb = d_tb/1024
				if(flag == 1){
					rkb = 0
	         			tkb = 0
        	 			cpurate = 0
					memrate = 0
        	 			flag =0
				}
				//add points
        			chart_cpu.series[0].addPoint([x,cpurate], true, true);
				chart_mem.series[0].addPoint([x,memrate], true, true);
				chart_rx.series[0].addPoint([x,rkb], true, true);
				chart_tx.series[0].addPoint([x,tkb], true, true);

				var etime = new Date();
				d_date=etime.getTime()-stime.getTime();
				setTimeout(requestData, timeout - d_date);	
			},
			cache: false
		});
	}
		


//cpu
$(document).ready(function(){ 
    chart_cpu = new Highcharts.Chart({ 
        chart: { 
            renderTo: 'cpu', //图表放置的容器，DIV 
            defaultSeriesType: 'spline', //图表类型为曲线图 
            backgroundColor:'#DFFDFD',
            events: { 
                //load: requestData
                               } 
                      }, 
        title: { 
            text: 'CPU负载'      //图表标题 
                      }, 
        xAxis: { //设置X轴 
            title: { text: '时间'   },  
            type: 'datetime',       //X轴为日期时间类型 
            tickPixelInterval: 150  //X轴标签间隔 
                      }, 
        yAxis: { //设置Y轴 
            title: { text: 'CPU使用率'   },
            labels: {
			   	  formatter: function() {
				    		return this.value +'%';
				              }
                                 },
            max: 100, //Y轴最大值 
            min: 0  //Y轴最小值 
                      }, 
        tooltip: {//当鼠标悬置数据点时的提示框 
            formatter: function() { //格式化提示信息 
                return Highcharts.dateFormat('%H:%M:%S', this.x) +' '+  
                Highcharts.numberFormat(this.y, 2)+'%'; 
                                } 
                      }, 
        legend: { 
            enabled: false  //设置图例不可见 
                      }, 
        exporting: { 
            enabled: false  //设置导出按钮不可用 
                      }, 
       
        series: [{ 
            data: (function() { //设置默认数据， 
                var data = [], 
                time = (new Date()).getTime() + 8*3600*1000, 
                i; 
 
                for (i = -19; i <= 0; i++) { 
                    data.push({ 
                        x: time + i * timeout,  
                        y: 0 * 100 
                                                      }); 
                                           } 
                return data; 
                                 })() 
                       }] 
           }); 
}); 
//mem
 
$(function() { 
    chart_mem = new Highcharts.Chart({ 
        chart: { 
            renderTo: 'mem', //图表放置的容器，DIV 
            defaultSeriesType: 'spline', //图表类型为曲线图 
            backgroundColor:'#DFFDFD',
            events: { 

                                 } 
                      }, 
        title: { 
            text: '内存负载'  //图表标题 
                      }, 
        xAxis: { //设置X轴 
            title: { text: '时间'   },  
            type: 'datetime',  //X轴为日期时间类型 
            tickPixelInterval: 150  //X轴标签间隔 
                     }, 
        yAxis: { //设置Y轴 
            title: { text: '内存使用记录'   },
            labels: {
			   	  formatter: function() {
				    		return this.value + '%';
				              }
                                 },
            max: 100, //Y轴最大值 
            min: 0  //Y轴最小值 

                      }, 
        tooltip: {//当鼠标悬置数据点时的提示框 
            formatter: function() { //格式化提示信息 
                return Highcharts.dateFormat('%H:%M:%S', this.x) +' '+  
                Highcharts.numberFormat(this.y, 2) + '%' + ' ' +
					 Highcharts.numberFormat(mem,0) + 'MB';
                                } 
                      }, 
        legend: { 
            enabled: false  //设置图例不可见 
                     }, 
        exporting: { 
            enabled: false  //设置导出按钮不可用 
                     }, 
       
        series: [{ 
            data: (function() { //设置默认数据， 
                var data = [], 
                time = (new Date()).getTime() + 8*3600*1000, 
                i; 
 
                for (i = -19; i <= 0; i++) { 
                    data.push({ 
                        x: time + i * timeout,  
                        y: 0 
                                                      }); 
                                            } 
                return data; 
                                 })() 
                      }] 
         }); 
}); 
//rx

$(function() { 
    chart_rx = new Highcharts.Chart({ 
        chart: { 
            renderTo: 'rx', //图表放置的容器，DIV 
            defaultSeriesType: 'spline', //图表类型为曲线图 
            backgroundColor:'#DFFDFD',   //背景颜色
            events: { 
             
                                 } 
                      }, 
        title: { 
            text: '网络接收流量'  //图表标题 
                     }, 
        xAxis: { //设置X轴 
            title: { text: '时间'   },  
            type: 'datetime',  //X轴为日期时间类型 
            tickPixelInterval: 150  //X轴标签间隔 
                      }, 
        yAxis: { //设置Y轴 
            title: { text: '接收网络流量'   },
            labels: {
			   	  formatter: function() {
				    		return this.value +'kb/s';
				              }
                                 },
            //max: 200, //Y轴最大值 
            min: 0  //Y轴最小值 
                      }, 
        tooltip: {//当鼠标悬置数据点时的提示框 
            formatter: function() { //格式化提示信息 
                return Highcharts.dateFormat('%H:%M:%S', this.x) +' '+  
                Highcharts.numberFormat(this.y, 2)+'kb/s'; 
                                } 
                      }, 
        legend: { 
            enabled: false  //设置图例不可见 
                      }, 
        exporting: { 
            enabled: false  //设置导出按钮不可用 
                     }, 
       
        series: [{ 
            data: (function() { //设置默认数据， 
                var data = [], 
                time = (new Date()).getTime() + 8*3600*1000, 
                i; 
 
                for (i = -19; i <= 0; i++) { 
                    data.push({ 
                        x: time + i * timeout,  
                        y: 0 
                                                      }); 
                                           } 
                return data; 
                                })() 
                     }] 
          }); 
}); 
//tx

$(function() { 
    chart_tx = new Highcharts.Chart({ 
        chart: { 
            renderTo: 'tx', //图表放置的容器，DIV 
            defaultSeriesType: 'spline', //图表类型为曲线图 
            backgroundColor:'#DFFDFD',
            events: { 
                 load: requestData
                                 } 
                      }, 
        title: { 
            text: '网络发送流量'  //图表标题 
                     }, 
        xAxis: { //设置X轴 
            title: { text: '时间'   },  
            type: 'datetime',  //X轴为日期时间类型 
            tickPixelInterval: 150  //X轴标签间隔 
                      }, 
        yAxis: { //设置Y轴 
            title: { text: '发送网络流量'   },
            labels: {
			   	  formatter: function() {
				    		return this.value +'kb/s';
				              }
                                 },
            //max: 200, //Y轴最大值 
            min: 0  //Y轴最小值 
                      }, 
        tooltip: {//当鼠标悬置数据点时的提示框 
            formatter: function() { //格式化提示信息 
                return Highcharts.dateFormat('%H:%M:%S', this.x) +' '+  
                Highcharts.numberFormat(this.y, 2)+'kb/s'; 
                                } 
                      }, 
        legend: { 
            enabled: false  //设置图例不可见 
                      }, 
        exporting: { 
            enabled: false  //设置导出按钮不可用 
                     }, 
       
        series: [{ 
            data: (function() { //设置默认数据， 
                var data = [], 
                time = (new Date()).getTime() + 8*3600*1000, 
                i; 
 
                for (i = -19; i <= 0; i++) { 
                    data.push({ 
                        x: time + i * timeout,  
                        y: 0
                                                      }); 
                                           } 
                return data; 
                                })() 
                     }] 
          }); 
}); 
