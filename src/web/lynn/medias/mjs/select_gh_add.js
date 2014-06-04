$(function(){
	$('#id_ghapp').bind('change',function(){
		ghapp = $("#id_ghapp").val()
		window.location.href= '/select/gh_add/?ghapp=' + ghapp;
	});
});
