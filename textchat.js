//comment

var username;
var XMLHttp;
var strangers_messages = [];
var cgiBusy = false;
var serverBusy;
var MAX_LAG_TIME = 30000;			//if time between updates is greater than this, user has been signed out
var lastMessageTime;
var timeCode;


// Things to do at page load
function pageInit() 
{
	console.log("Page load");
	timeCode = Date.now();
	//getTimeCode();
	document.getElementById("sendbutton").disabled = true;
	serverBusy = true;
	setXMLHttp();
	getUsername();
	lastMessageTime = timeCode;
}

function getTimeCode()
{
	fullTimeCode = Date.now().toString();
	timeCode = fullTimeCode.substr(fullTimeCode.length - 6, fullTimeCode.length - 1);
}

function exitRoom()
{
	var unloadMessage = "$" + timeCode + "|UNLOAD|" + username + "*";
	if(!cgiBusy)
	{
		callCGI(unloadMessage);
	}
}

function setXMLHttp()
{
	if(navigator.appName == "Microsoft Internet Explorer") 
	{
		XMLHttp = new ActiveXObject("Microsoft.XMLHTTP");
	} else 
	{
		XMLHttp = new XMLHttpRequest();
	}
}

function getUsername()
{
	var loadMessage = "$" + timeCode + "|LOAD|*";
	if(!cgiBusy)
	{
		callCGI(loadMessage);
	}
}


// Respond to send button
function sendMessage() 
{
    // Get the text from the text box
    var inText = document.textBox.textInput.value;
    
    // Make sure something is there
    if (inText.length < 1) return;
	
	// Clear the input text
    document.chatForm.textInput.value = "";    
	var cgiMessage = "$" + timeCode + "|MESSAGE|" + username + '|' + inText + '*';
	if(!cgiBusy)
	{
		callCGI(cgiMessage);
	}
	updateChatBox(inText, "You");
}

function updateChatBox(messages, user)
{		
	//Check if messages is an array, then add all messages to bottom of chatbox
	if(messages.constructor == Array)
	{
		for(var i = 0; i < messages.length; i++)
		{
			document.getElementById('chatBox').innerHTML += "<font color='red'><b><br>" + user + "</b>: </font>" + messages[i] +"</>";
		}
	}
	else
	{
		document.getElementById('chatBox').innerHTML += "<font color='red'><br><b>" + user + "</b>: </font>" + messages +"</>";
	}
    
    // force to bottom
    updateScroll();
}

function checkServerForUpdates()
{
	var updateMessage = "$" + timeCode + "|UPDATE|" + username + "|" + strangers_messages.length + '*';
	var loadMessage = "$" + timeCode + "|LOAD|*";
	if(!cgiBusy && !serverBusy)
	{
		var time = GetTimeBetweenButtonPresses();
		if(time > MAX_LAG_TIME)		//if server has timed the user out, tell server user has signed back in
		{
			console.log("time between button presses: " + time);
			console.log("Reloading page because of server time out");
			callCGI(loadMessage);
		}
		else			//otherwise, ask the server for an update to the messages
		{
			callCGI(updateMessage);
		}
	}
}
setInterval(function(){ checkServerForUpdates()}, 1000);

// Function to force scrollable window to bottom
function updateScroll()
{
    var element = document.getElementById("chatBox");
    element.scrollTop = element.scrollHeight;
}

function callCGI(CGIMessage)
{
	console.log("CGI message: " + CGIMessage);
	
	cgiBusy = true;
	XMLHttp.open("GET", "/cgi-bin/schutzj_textchat_ajax.cgi?"
						 + "&message=" + CGIMessage
						 ,true);
    XMLHttp.onreadystatechange=function() 
	{
		if(XMLHttp.readyState == 4)
		{
			var response = XMLHttp.responseText;
			parseResponse(response);
			cgiBusy = false;
		}
	}
	XMLHttp.send(null);
	
}

function parseResponse(response)
{
	console.log("Parsing response....");
	var user_command = "USER";
	var update_command = "UPDATE";
	
	//Get the command
	var index = 0;
	var command = "";
	
	//go past $
	while(index < response.length)
	{
		if(response[index] == '$')
		{
			index++;
			break;
		}
		index++;
	}
	//read the command
	while(index < response.length)
	{
		if(response[index] == '|')
		{
			index++;
			break;
		}
		command += response[index];
		index++;
	}
	
	console.log("Got command: " + command);
	
	//figure out what to do with message
	if(command == user_command)
	{
		assignUsername(response, index);
	}
	if(command == update_command)
	{
		updateStrangerMessages(response, index);
	}
}

function assignUsername(response, startingIndex)
{
	var validUser1 = "user1";
	var validUser2 = "user2";
	
	var i = startingIndex;
	username = '';
	while(i < response.length)
	{
		if(response[i] == '*')
		{
			break;
		}
		username += response[i];
		i++;
	}
	if(username == validUser1 || username == validUser2)
	{
		document.getElementById("sendbutton").disabled = false;
		serverBusy = false;
	}
	//otherwise, don't let a user type a message to the server
	else
	{
		document.getElementById('chatBox').innerHTML = "<font color='red'><b>CHAT ROOM IS FULL</>";
	}
}

function updateStrangerMessages(response, startingIndex)
{
	var i = startingIndex;
	newMessages = [];
	var temp = '';
	
	while(i < response.length)
	{
		if(response[i] == '$')
		{
			while(i < response.length)
			{
				if(response[i] == '*')
				{
					break;
				}
				temp += response[i];				
				i++;
			}
			if(temp == "UPTODATE")
			{
				return;
			}
		}
		
		if(response[i] == '|')
		{
			newMessages.push(temp);
			strangers_messages.push(temp);
			temp = "";
			i++;
		}
		
		if(response[i] == '*')
		{
			newMessages.push(temp);
			strangers_messages.push(temp);
			break;
		}
		
		temp += response[i];
		i++;
	}
	newMessages.push(temp);
	strangers_messages.push(temp);
	
	updateChatBox(newMessages, "Stranger");
}

function GetTimeBetweenButtonPresses()
{
	var currentTime = Date.now();
	var elapsedTime = currentTime - lastMessageTime;
	lastMessageTime = currentTime;
	
	return elapsedTime;
}
