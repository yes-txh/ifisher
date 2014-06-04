$(function(){
	var pre_states = []
	var report = []
	function init_Job_stop_state(){
		$.getJSON('/ajax/job_stop_state',function(data){
			$.each(data,function(entryIndex,entry){
				var state_id = "#state_" + entry.id
				$(state_id).html(entry.state)
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
                                                                alert("任务" + entry.job_id + "完成")
                                                        }
                                                }
                                        }
                                });
			});
		});
	}
	function getJob_stop_state(){
		$.getJSON('/ajax/job_stop_state',function(data){
			$.each(data,function(entryIndex,entry){
				var state_id = "#state_" + entry.id
				$(state_id).html(entry.state)
				var ftime_id = "#ftime_" + entry.id
                                $(ftime_id).html(entry.finish_time)
				var progress_id = "#progressBar_" + entry.id
				$(progress_id).progressBar(entry.progress,{running_value:pre_states[entryIndex], steps:50, stepDuration:200, barImage: '/media/progressbar/images/progressbg_orange.gif',callback: 
					function(data) {
						if (report[entryIndex] == true) {
							if (data.running_value == 100) {
								alert("任务" + entry.job_id + "完成")
								report[entryIndex] = false;
							}
						}
					}
				});
				pre_states[entryIndex] = entry.progress
			});
		});
	}
	init_Job_stop_state();
	setInterval(getJob_stop_state,10005 );
});
