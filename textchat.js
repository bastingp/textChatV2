var XMLHttp;
var timeCode;

function pageInit() 
{
	console.log("Page load");
	timeCode = Date.now();
	
	//setup XMLHttp
	if(navigator.appName == "Microsoft Internet Explorer") 
	{
		XMLHttp = new ActiveXObject("Microsoft.XMLHTTP");
	} else 
	{
		XMLHttp = new XMLHttpRequest();
	}
	
	//send load message to CGI
	callCGI("$" + timeCode "|LOAD|*");
}

function callCGI(CGIMessage)
{
	console.log("Sending to CGI: " + CGIMessage);
}

