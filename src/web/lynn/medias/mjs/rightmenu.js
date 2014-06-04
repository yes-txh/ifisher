/*** 
DreamCore - JsLib/Menu 
Date : Dec 03, 2006 
Copyright: DreamSoft Co.,Ltd. 
Mail : Dream@Dreamsoft.Ca 
Author : Egmax 
Browser : IE5.0&+,Firefox1.5&+,Netscape7.0&+ 
Update: 
***/ 

$(function(){
	if(!document.all) document.captureEvents(Event.MOUSEDOWN); 
	var _Beforemenu = 0; 
	var _Nowmenu = 0; 
	var _Type = 'A'; 
	var _html = 0;
	var _a = 0;
	var _herf;
	var dbody = document.getElementById("dbody");
	//alert(dbody);
	dbody.onclick = _Hidden;

	//document.onclick = _Hidden; 
	function _Hidden() 
	{ 
		//alert("hidden");
		if(_Beforemenu==0) return; 
		document.getElementById(_Beforemenu).style.visibility='hidden'; 
		_Beforemenu=0; 
	} 
	
	dbody.oncontextmenu = function (e) 
	{ 
		_Hidden(); 
	   //alert('1');
		//get oncontext object
		var _Obj = document.all ? event.srcElement : e.target; 
	   if(_Type.indexOf(_Obj.tagName) == -1) return; 
		_Nowmenu = _Obj.getAttribute('menu'); 
	   _html = _Obj.innerHTML;
		//alert(_Nowmenu)
	   //alert(_html);
		if(_Nowmenu == 'null') return; 
		if(document.all) {e = event;} 
	   //alert(_Nowmenu);
	   change_menu_herf(_Nowmenu, _html);
	
		_ShowMenu(_Nowmenu, e); 
		return false; 
	} 

	function change_menu_herf(_Nowmenu, _html)
	{
	   if(_Nowmenu == 'menu_server') {
			_a = document.getElementById('editserver');
	      _herf = '../server_update/?key=' + _html;
			_a.setAttribute("href", _herf);
	      //_a = document.getElementById('editserver');
			//alert(_a);
      	_a = document.getElementById('addvirt');
      	_herf = '../server_virt_add/?ip=' + _html;
			_a.setAttribute("href", _herf);
	
      	_a = document.getElementById('deleteserver');
      	_herf = '../server_delete/?key=' + _html;
      	_onclick = 'return confirm("你确认要删除服务器' + _html + '吗?")'
			_a.setAttribute("href", _herf);
			_a.setAttribute("onclick", _onclick);
	          }   
   	 else if(_Nowmenu == 'menu_virt') {
			_a = document.getElementById('editvirt');
   	   //alert(_html);
   	   _herf = '../virt_update/?key=' + _html;
   	   _a.setAttribute("href", _herf);
	
		   _a = document.getElementById('deletevirt');
   	   _herf = '../virt_delete/?key=' + _html;
   	   _onclick = 'return confirm("你确认要删除虚拟机' + _html + '吗?")'
			_a.setAttribute("href", _herf);
			_a.setAttribute("onclick", _onclick);
        	  }   
	
	}	
	function _ShowMenu(Eid, event) 
	{ 	
		var _Menu = document.getElementById(Eid); 
		var _Left = event.clientX + document.body.scrollLeft; 
		var _Top = event.clientY + document.body.scrollTop; 
		_Menu.style.left = _Left.toString() + 'px'; 
		_Menu.style.top = _Top.toString() + 'px'; 
		_Menu.style.visibility = 'visible'; 
		//alert(_Menu);
		//alert(_Menu.style.visibility);
		_Beforemenu = Eid; 
	} 	
	/*** 
	可以支持其他标签INPUT,IMG 
	***/ 
	_Type='INPUT,A,DIV,BODY,IMG';
});
