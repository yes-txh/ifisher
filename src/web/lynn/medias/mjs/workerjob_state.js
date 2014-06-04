$(function(){
	var pre_states = []
	var report = []
	function init_Workerjob_state(){
		$.getJSON('/ajax/workerjob_state',function(data){
			var i = 0
			$.each(data,function(entryIndex,entry){
				var state_id = "#state_" + entry.id
				$(state_id).html(entry.state)
				//var ftime_id = "#ftime_" + entry.id
                                //$(ftime_id).html(entry.finish_time)
				var progress_id = "#progressBar_" + entry.id
				pre_states.push(entry.progress)
				if (entry.progress == 100)
					report.push(false)
				else
					report.push(true)
				$(progress_id).progressBar(entry.progress,{steps:1, stepDuration:10, running_value:entry.progress, barImage: '/media/progressbar/images/progressbg_orange.gif',callback: 
					function(data) { 
						if (report[entryIndex] == true) {
							if (data.running_value == 100) {
								//var totalId = bar.attr("id");
								//var id = totalId.split("_")[1];
								alert("任务" + entry.job_id + "完成")
							}
						}
					}
				});
			});
			//getWorkerjob_state();
			//setInterval(getWorkerjob_state,10000 );
		});
	}
	function getWorkerjob_state(){
		$.getJSON('/ajax/workerjob_state',function(data){
			$.each(data,function(entryIndex,entry){
				var state_id = "#state_" + entry.id
				$(state_id).html(entry.state)
				var ftime_id = "#ftime_" + entry.id
                                $(ftime_id).html(entry.finish_time)
				var progress_id = "#progressBar_" + entry.id
				/*alert("前状态" + entry.id)
				alert(pre_states[i])*/
				$(progress_id).progressBar(entry.progress,{running_value:pre_states[entryIndex], steps:50, stepDuration:200, barImage: '/media/progressbar/images/progressbg_orange.gif',callback: 
					function(data) {
						if (report[entryIndex] == true) {
							if (data.running_value == 100) {
								//var totalId = bar.attr("id");
								//var id = totalId.split("_")[1];
								alert("任务" + entry.job_id + "完成")
								report[entryIndex] = false;
							}
						}
					}
				});
				pre_states[entryIndex] = entry.progress
			});
			//setTimeout(getWorkerjob_state, 5000);
		});
	}
	init_Workerjob_state();
	//getWorkerjob_state();
	//setTimeout(getWorkerjob_state, 5000);
	setInterval(getWorkerjob_state,10005 );
});
