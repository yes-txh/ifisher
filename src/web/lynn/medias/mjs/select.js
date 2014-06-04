/* virt app & run_app */
$(function(){
	obj = $("#id_app").parent();
	obj.append("<br/><input type='button' class='submit' value='全选' onclick='SelectApp()'>");
        obj.append("<input type='button' class='submit' value='反选' onclick='InvertSelectApp()'>");
        obj.append("<input type='button' class='submit' value='全部取消' onclick='UnSelectApp()'>");
});
function SelectApp(){
        obj = $("#id_app")
        len = obj.find("option").length
        for(i=0; i<len; i++){
                obj.find("option")[i].selected = true;
        }
}
function InvertSelectApp(){
        obj = $("#id_app")
        len = obj.find("option").length
        for(i=0; i<len; i++){
                if(obj.find("option")[i].selected == true)
                        obj.find("option")[i].selected = false;
                else
                        obj.find("option")[i].selected = true;
        }
}
function UnSelectApp(){
	obj = $("#id_app")
	len = obj.find("option").length
	for(i=0; i<len; i++){
		obj.find("option")[i].selected = false;
	}
}
/*
$(function(){
        obj = $("#id_run_app").parent();
        obj.append("<br/><input type='button' class='submit' value='全选' onclick='SelectRunApp()'>");
        obj.append("<input type='button' class='submit' value='反选' onclick='InvertSelectRunApp()'>");
        obj.append("<input type='button' class='submit' value='全部取消' onclick='UnSelectRunApp()'>");
});
function SelectRunApp(){
        obj = $("#id_run_app")
        len = obj.find("option").length
        for(i=0; i<len; i++){
                obj.find("option")[i].selected = true;
        }
}
function InvertSelectRunApp(){
        obj = $("#id_run_app")
        len = obj.find("option").length
        for(i=0; i<len; i++){
                if(obj.find("option")[i].selected == true)
                        obj.find("option")[i].selected = false;
                else
                        obj.find("option")[i].selected = true;
        }
}
function UnSelectRunApp(){
        obj = $("#id_run_app")
        len = obj.find("option").length
        for(i=0; i<len; i++){
                obj.find("option")[i].selected = false;
        }
}
*/
/* group perm  */
$(function(){
	obj = $("#id_perm").parent();
	obj.append("<br/><input type='button' class='submit' value='全选' onclick='SelectPerm()'>");
	obj.append("<input type='button' class='submit' value='反选' onclick='InvertSelectPerm()'>");
	obj.append("<input type='button' class='submit' value='全部取消' onclick='UnSelectPerm()'>");
});
function SelectPerm(){
        obj = $("#id_perm")
        len = obj.find("option").length
        for(i=0; i<len; i++){
                obj.find("option")[i].selected = true;
        }
}
function InvertSelectPerm(){
        obj = $("#id_perm")
        len = obj.find("option").length
        for(i=0; i<len; i++){
		if(obj.find("option")[i].selected == true)
	                obj.find("option")[i].selected = false;
		else
			obj.find("option")[i].selected = true;
        }
}
function UnSelectPerm(){
	obj = $("#id_perm")
	len = obj.find("option").length
	for(i=0; i<len; i++){
		obj.find("option")[i].selected = false;
	}
}
/* job_install virts */
$(function(){
        obj = $("input[name='virts']").parent().parent().parent().parent();
        obj.append("<br/><input type='button' class='submit' value='全选' onclick='SelectVirt()'>");
        obj.append("<input type='button' class='submit' value='反选' onclick='InvertSelectVirt()'>");
        obj.append("<input type='button' class='submit' value='全部取消' onclick='UnSelectVirt()'>");
});
function SelectVirt(){
	$("input[name='virts']").each(function() {
        	$(this).attr("checked", true);
        });
	/*
	if(this.checked){
		$("input[name='virts']").each(function(){this.checked=true;});
	}else{
		$("input[name='virts']").each(function(){this.checked=false;});
	} */
}
function InvertSelectVirt(){
        $("input[name='virts']").each(function() {
		if(this.checked)
	                $(this).attr("checked", false);
		else
			$(this).attr("checked", true);
        });
}
function UnSelectVirt(){
        $("input[name='virts']").each(function() {
                $(this).attr("checked", false);
        });
}


/*$(function(){
	obj = $("#id_cgroup").parent();
	obj.append("<br/><input type='button' class='submit' value='取消所有用户组' onclick='UnSelectCGroup()'>");
});
function UnSelectCGroup(){
	obj = $("#id_cgroup")
	len = obj.find("option").length
	for(i=0; i<len; i++){
		obj.find("option")[i].selected = false;
	}
}*/


