$(function(){
	$('#id_medium').bind('change',function(){
		medium = $("#id_medium").val()
		/*if (medium == "")
			medium = "-1"*/
		way = $("#id_way").val()
                /*if (way == "")
                        way = "-1"*/
		window.location.href= '/select/virt_add/?medium=' + medium + '&way=' + way;
	});
	$('#id_way').bind('change',function(){
                medium = $("#id_medium").val()
                /*if (medium == "")
                        medium = "-1"*/
                way = $("#id_way").val()
                /*if (way == "")
                        way = "-1"*/
               window.location.href= '/select/virt_add/?medium=' + medium + '&way=' + way;
        });
});
