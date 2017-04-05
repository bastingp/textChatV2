var XMLHttp;
var timeCode;
var username;
var cgiBusy = false;
var numMessages;

function pageInit() 
{
	console.log("Page load");
	timeCode = Date.now();
	numMessages = 0;
	
	//setup XMLHttp
	if(navigator.appName == "Microsoft Internet Explorer") 
	{
		XMLHttp = new ActiveXObject("Microsoft.XMLHTTP");
	} else 
	{
		XMLHttp = new XMLHttpRequest();
	}
	
	//send load message to CGI
	if(!cgiBusy)
	{
		callCGI("$" + timeCode + "|LOAD|*");
	}
}

function callCGI(CGIMessage)
{
	cgiBusy = true;
	console.log("Sending to CGI: " + CGIMessage);
	cgiBusy = false;
}

function sendMessage()
{
	// Get the text from the text box
    var inText = document.textBox.textInput.value;
    
    // Make sure something is there
    if (inText.length < 1) return;
	
	// Clear the input text
    document.textBox.textInput.value = "";    
	var cgiMessage = "$" + timeCode + "|MESSAGE|" + username + '|' + inText + '|' + numMessages + '*';
	if(!cgiBusy)
	{
		callCGI(cgiMessage);
	}
}

function checkServerForUpdates()
{
	if(!cgiBusy)
	{
		callCGI("$" + timeCode + "|UPDATE|" + username + "|" + numMessages + "*");
	}
}
setInterval(function(){ checkServerForUpdates()}, 1000);