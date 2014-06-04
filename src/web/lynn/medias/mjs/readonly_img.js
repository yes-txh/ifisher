$(function(){
	$("#id_name").attr("readonly","true")
	$("#id_server").attr("disabled","true")
	
	var select = $('#id_server');
	var name = select.attr('name');
	var val = select.val()
	
	var hidden = $('<input type = "hidden" name = "' + name + '" value = "' + val +  '"/>');	
	select.parents("form").append(hidden);
})
