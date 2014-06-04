$(function(){
	var table = $("table.tables");
	table.each(function(){
		var t = $(this);
		var th = t.children("thead").children("tr").children("th").eq(0);
		th.attr("width","30%");
	});
});
