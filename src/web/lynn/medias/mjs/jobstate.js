function getJobState(){
	//alert("a")
	$.get('/ajax/jobstate',function(data){
		//alert("b")
	});
}
getJobState();
setInterval("getJobState()",10000 );



