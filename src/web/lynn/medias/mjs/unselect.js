/* virt */
$(function(){
	obj = $("#id_app").parent();
	obj.append("<br/><input type='button' class='submit' value='取消所有应用' onclick='UnSelectApp()'>");
});
function UnSelectApp(){
	obj = $("#id_app")
	len = obj.find("option").length
	for(i=0; i<len; i++){
		obj.find("option")[i].selected = false;
	}
}

$(function(){
	obj = $("#id_perm").parent();
	obj.append("<br/><input type='button' class='submit' value='取消所有权限' onclick='UnSelectPerm()'>");
});
function UnSelectPerm(){
	obj = $("#id_perm")
	len = obj.find("option").length
	for(i=0; i<len; i++){
		obj.find("option")[i].selected = false;
	}
}
$(function(){
	obj = $("#id_cgroup").parent();
	obj.append("<br/><input type='button' class='submit' value='取消所有用户组' onclick='UnSelectCGroup()'>");
});
function UnSelectCGroup(){
	obj = $("#id_cgroup")
	len = obj.find("option").length
	for(i=0; i<len; i++){
		obj.find("option")[i].selected = false;
	}
}
