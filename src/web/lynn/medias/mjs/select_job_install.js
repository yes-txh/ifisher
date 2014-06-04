$(function(){
	$('#id_apptype').bind('change',function(){
		val = $("#id_apptype").val()
		if (val == "")
			val = "-1"
		way = $("#id_way").val()
		window.location.href= '/select/job_install_add/?way=' + way+'&app_id='+val;
	});
	$('#id_way').bind('change',function(){
               val = $("#id_apptype").val()
               if (val == "")
			val = "-1"
                way = $("#id_way").val()
                /*if (way == "")
                        way = "-1"*/
                window.location.href= '/select/job_install_add/?way=' + way+'&app_id='+val;
        });
});
