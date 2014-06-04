/*  system  */
function sys_loadsubmit(){
	val = $("#select_load").val()
	text = $("#select_load").find("option:selected").text()
	$("#img_load").attr("src","/sys_load?time="+val)
}
function sys_cpusubmit(){
	val = $("#select_cpu").val()
	text = $("#select_cpu").find("option:selected").text()
	$("#img_cpu").attr("src","/sys_cpu?time="+val)
}
function sys_memsubmit(){
	val = $("#select_mem").val()
	text = $("#select_mem").find("option:selected").text()
	$("#img_mem").attr("src","/sys_mem?time="+val)
}
function sys_flowsubmit(){
	val = $("#select_flow").val()
	text = $("#select_flow").find("option:selected").text()
	$("#img_flow").attr("src","/sys_flow?time="+val)
}
function sys_disksubmit(){
	val = $("#select_disk").val()
	text = $("#select_disk").find("option:selected").text()
	$("#img_disk").attr("src","/sys_disk?time="+val)
}

/* sever node */
function cpusubmit(){
        val = $("#select_cpu").val()
      	server = $("td #server").text()
        $("#img_cpu").attr("src","/cpu?time="+val+"&s="+server)
}
function memsubmit(){
        val = $("#select_mem").val()
        server = $("td #server").text()
        $("#img_mem").attr("src","/mem?time="+val+"&s="+server)
}

function flowsubmit(){
        val = $("#select_flow").val()
        server = $("td #server").text()
        $("#img_flow").attr("src","/flow?time="+val+"&s="+server)
}

function w_flowsubmit(){
        val = $("#select_w_flow").val()
        server = $("td #server").text()
        $("#img_w_flow").attr("src","/w_flow?time="+val+"&s="+server)
}
/*
function flow_insubmit(){
        val = $("#select_flow_in").val()
        server = $("td #server").text()
        $("#img_flow_in").attr("src","/flow_in?time="+val+"&s="+server)
}

function flow_outsubmit(){
        val = $("#select_flow_out").val()
        server = $("td #server").text()
        $("#img_flow_out").attr("src","/flow_out?time="+val+"&s="+server)
}
*/
// virt
function virt_cpusubmit(){
	val = $("#select_cpu").val()
	server = $("td #server").text()
	virt = $("td #virt").text()
	$("#img_cpu").attr("src","/virt_cpu?time="+val+"&s="+server+"&v="+virt)
}
function virt_memsubmit(){
	val = $("#select_mem").val()
	server = $("td #server").text()
	virt = $("td #virt").text()
	$("#img_mem").attr("src","/virt_mem?time="+val+"&s="+server+"&v="+virt)
}
function virt_flow_insubmit(){
        val = $("#select_flow_in").val()
	server = $("td #server").text()
	virt = $("td #virt").text()
        $("#img_flow_in").attr("src","/virt_flow_in?time="+val+"&s="+server+"&v="+virt)
}

function virt_flow_outsubmit(){
        val = $("#select_flow_out").val()
	server = $("td #server").text()
	virt = $("td #virt").text()
        $("#img_flow_out").attr("src","/virt_flow_out?time="+val+"&s="+server+"&v="+virt)
}

/* over */

