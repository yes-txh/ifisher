$(function(){
	function getJob_run_state(){
		$.getJSON('/ajax/job_run_state',function(data){
			$.each(data,function(entryIndex,entry){
				var state_id = "#state_" + entry.id
				$(state_id).html(entry.state)
				var ftime_id = "#ftime_" + entry.id
                                $(ftime_id).html(entry.finish_time)
			});
		});
	}
	setInterval(getJob_run_state,10000 );
});
