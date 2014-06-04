$(function(){
	m1 = document.getElementById('m1');
	m1.style.visibility = 'hidden';
	m2 = document.getElementById('m2');
	m2.style.visibility = 'hidden';
	m3 = document.getElementById('m3');
	m3.style.visibility = 'hidden';
});
var timeout         = 500;
var closetimer;
var ddmenuitem;
// open hidden layer
function mopen(id)
{	
	// cancel close timer
	mcancelclosetime();

	// close old layer
	if(ddmenuitem) ddmenuitem.style.visibility = 'hidden';

	// get new layer and show it
	ddmenuitem = document.getElementById(id);
	ddmenuitem.style.visibility = 'visible';
}
// close showed layer
function mclose()
{
	if(ddmenuitem) ddmenuitem.style.visibility = 'hidden';
}
// go close timer
function mclosetime()
{
	closetimer = window.setTimeout(mclose, timeout);
}
// cancel close timer
function mcancelclosetime()
{
	if(closetimer!=null)
	{
		window.clearTimeout(closetimer);
		closetimer = null;
	}
}
