/*function tableChange(){
	var tables = document.getElementsByTagName("table");
	alert ("This document contains " + tables.length + " tables");
	var trlist = document.getElementsByTagName('tr');
			alert(trlist.length)
			alert(trlist[0])
	for (var i=0; i< trlist.length; i++){
			alert(trlist.length)
			alert(trlist[i])
			trlist[i].className = (i%2==0)? 'tr1' : 'tr2';
	}
}
window.onload=tableChange;
*/
$(function(){
	$(".tables").each(function(i,t){
		var obj = $(t);
		//alert(obj)
		//obj.children('tbody').children("tr:even").addClass('tr1');
		//obj.children('tbody').children("tr:odd").addClass("tr2");
		obj.find("tr:even").addClass('tr1');
		obj.find("tr:odd").addClass("tr2");
	});
});
