$(function(){
	$('#id_apptype').bind('change',function(){
		val = $("#id_apptype").val()
		if (val == "")
			val = "-1"
		window.location.href= '/select/job_run_add/?app_id=' + val;
	});
});
