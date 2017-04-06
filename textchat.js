var XMLHttp;
var timeCode = "NULL";
var username;
var cgiBusy = false;
var numMessages;
var lastUpdateTime;
const MAX_UPDATE_INTERVAL = 10000;

function parseResponse(response)
{
	console.log("Got reponse from CGI: " + response);
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
		if(response[index] == '|' || response[index] == '*')
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
		assignUsername(response);
	}
	else if(command == update_command)
	{
		updateMessages(response);
	}
	else if(command == "CORRUPT")
	{
		console.log("********ERROR:Yikes! You're corrupt!*********");
		document.getElementById('chatBox').innerHTML = "<font color='red'><b>Whoops! Your data got corrupted. Please reload page.</>";
		document.getElementById("sendbutton").disabled = true;
		lastUpdateTime = 0;
	}
}

function assignUsername(response)
{	
	var i = 0;
	
	//go past $
	while(i < response.length)
	{
		if(response[i] == '$')
		{
			i++;
			break;
		}
		i++;
	}
	
	//go past command
	while(i < response.length)
	{
		if(response[i] == '|')
		{
			i++;
			break;
		}
		i++;
	}	
	
	//get username
	username = "";
	while(i < response.length)
	{
		if(response[i] == '*' || response[i] == '|')
		{
			break;
		}
		username += response[i];
		i++;
	}
	
	//if username is 'FULL', chat room is full and user can't get on
	if(username == "FULL")
	{
		document.getElementById('chatBox').innerHTML = "<font color='red'><b>CHAT ROOM IS FULL</>";
		document.getElementById("sendbutton").disabled = true;
	}
}

function updateMessages(response)
{
	var i = 0;
	
	//go past $
	while(i < response.length)
	{
		if(response[i] == '$')
		{
			i++;
			break;
		}
		i++;
	}
	
	//go past command
	while(i < response.length)
	{
		if(response[i] == '|')
		{
			i++;
			break;
		}
		i++;
	}
	
	//get messages
	var sendUser = '';
	var message = '';
	while(i < response.length && response[i] != '*')
	{
		sendUser = '';
		message = '';
		//get the username for the message sent
		while(i < response.length && response[i] != '*' && response[i] != '|')
		{
			sendUser += response[i];
			i++;
		}
		i++;
		
		//get the message that was sent
		while(i < response.length && response[i] != '*' && response[i] != '|')
		{
			message += response[i];
			i++;
		}
		//increment number of messages received
		numMessages++;
		
		//if sendUser == user, we want the chatbox to print 'You'
		if(sendUser == username)
		{
			sendUser = 'You';
		}
		updateChatBox(message, sendUser);
		i++;
	}
}

function updateChatBox(messages, user)
{		
	console.log("Total messages: " + numMessages);
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


function pageInit() 
{
	initializeVariables();
	
	console.log("Time between loads: " + (Date.now() - lastUpdateTime));
	
	//send load message to CGI
	if(!cgiBusy && ((Date.now() - lastUpdateTime) > MAX_UPDATE_INTERVAL))
	{
		timeCode = Date.now();
		callCGI("$" + timeCode + "|LOAD|*");
	}
}

function initializeVariables()
{
	//get time code from local storage
	timeCode = localStorage.getItem("timeCode");
	//if it's equal to NULL, then there's no local storage, and we need to set it now
	if(timeCode == "NULL")
	{
		timeCode = Date.now().toString();
		localStorage.setItem("timeCode", timeCode);
		newUser = true;
	}
	
	//get numMessages from local storage
	numMessages = localStorage.getItem("numMessages");
	//if it's equal to NULL, then there's no local storage, and we need to set it to 0
	if(numMessages == "NULL")
	{
		numMessages = 0;
		localStorage.setItem("numMessages", numMessages.toString());
	}
	
	//get lastUpdateTime from from local storage
	lastUpdateTime = localStorage.getItem("lastUpdateTime");
	//if unassigned, set it to 0
	if(lastUpdateTime == null)
	{
		lastUpdateTime = 0;
	}
	
	//get username from local storage--if unassigned, leave it unassigned
	username = localStorage.getItem("username");
	
	//setup XMLHttp
	if(navigator.appName == "Microsoft Internet Explorer") 
	{
		XMLHttp = new ActiveXObject("Microsoft.XMLHTTP");
	} else 
	{
		XMLHttp = new XMLHttpRequest();
	}
}

function callCGI(CGIMessage)
{
	cgiBusy = true;
	console.log("Sending to CGI: " + CGIMessage);
	
	//open CGI file
	XMLHttp.open("GET", "/cgi-bin/schutzj_textchat_ajax.cgi?"
						 + "&message=" + CGIMessage
						 ,true);
    XMLHttp.onreadystatechange=function() 
	{
		//once it's loaded, give it the message
		if(XMLHttp.readyState == 4)
		{
			var response = XMLHttp.responseText;
			parseResponse(response);
			cgiBusy = false;
		}
	}
	XMLHttp.send(null);
}

function sendMessage()
{
	// Get the text from the text box
    var inText = document.textBox.textInput.value;
    
    // Make sure something is there
    if (inText.length < 1) return;
	
	// Clear the input text
    document.textBox.textInput.value = "";    
	
	//Send CGI user's message
	var cgiMessage = "$" + timeCode + "|MESSAGE|" + username + '|' + inText + '|' + numMessages + '*';
	if(!cgiBusy)
	{
		callCGI(cgiMessage);
	}
}

function checkServerForUpdates()
{
	//used for checking time between page loads
	lastUpdateTime = Date.now();
	var updateMessage = "$" + timeCode + "|UPDATE|" + username + "|" + numMessages + "*";
	if(!cgiBusy)
	{
		callCGI(updateMessage);
	}
}
//call function every second
setInterval(function(){ checkServerForUpdates()}, 1000);

function exitRoom()
{
	var unloadMessage = "$" + timeCode + "|UNLOAD|" + username + "*";
	if(!cgiBusy)
	{
		callCGI(unloadMessage);
	}
	
	username = "NULL";
	timeCode = "NULL";
	numMessages = "NULL";
	
	//put all current data in local storage
	localStorage.setItem("timeCode", timeCode);
	localStorage.setItem("username", username);
	localStorage.setItem("numMessages", numMessages);
	localStorage.setItem("lastUpdateTime", lastUpdateTime);
}

// Function to force scrollable window to bottom
function updateScroll()
{
    var element = document.getElementById("chatBox");
    element.scrollTop = element.scrollHeight;
}
