$(function(){
	function getVirtState(){
		$.getJSON('/ajax/virt_state',function(data){
			$.each(data,function(entryIndex,entry){
				var state_id = "#state_" + entry.id
				$(state_id).html(entry.state)
			});
		});
	}
	setInterval(getVirtState,10000 );
});
