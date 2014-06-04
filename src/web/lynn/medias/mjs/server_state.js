$(function(){
	function getServerState(){
		$.getJSON('/ajax/server_state',function(data){
			$.each(data,function(entryIndex,entry){
				var state_id = "#state_" + entry.id
				$(state_id).html(entry.state)
			});
		});
	}
	setInterval(getServerState,10000 );
});
