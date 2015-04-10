// Main.cpp

#include "stdafx.h"
#include "WebsocketServer.h"

#include "EchoService.h"
#include "RoutedCommunicationService.h"
#include "GroupCommunicationService.h"
#include "StreamedCommunicationService.h"
#include "IdentificationService.h"

#include "WebsocketClientFactory.h"
#include "WebsocketGenClientFactory.h"

#include <string>
#include <map>
#include <stdarg.h>
#include "Tools/CmdForwarder.h"
#include <list>

#define EXIT_CODE -99

float version = 1;
bool terminated = false;

unsigned __stdcall threadProc(LPVOID WorkContext)
{
    while (!terminated)
    {
        WebsocketDataMessage* message = new WebsocketDataMessage(StreamedCommunication);
        message->SetArguments("stream message");
        broadcastManager.PushPacket(message, "streamingQueue");
        Sleep(1000);
    }
    return 0;
}

// == Action handlers ==

bool safeStart(WebsocketServer &server)
{
	cout << "Starting server... ";
	if(!server.isStarted()) {
		try {
			server.start(true);
			cout << "DONE" << endl;
			return true;
		}
		catch (std::exception& e) {
			cout << "FAILED!" << endl;
			cout << "Exception : " << e.what() << std::endl;
			return false;
		}
	} else {
		cout << "FAILED!" << endl;
		cout << "Server is already started, cannot start it." << endl;
		return false;
	}
}
int safeStartHandler(CmdForwarder &source, WebsocketServer &server)
{
	return safeStart(server);
}

bool safeStop(WebsocketServer &server)
{
	cout << "Stopping server... ";
	if(server.isStarted()) {
		terminated = true;
		Sleep(1000);

		server.stop();
		cout << "DONE" << endl;
		return true;
	} else
	{
		cout << "FAIL!" << endl;
		cout << "Server hasn't started, cannot stop it." << endl;
		return false;
	}
}
int safeStopHandler(CmdForwarder &source, WebsocketServer &server)
{
	return safeStop(server);
}

void printPort(WebsocketServer &server)
{
	printf("Port used: %d\n\r", server.getConfigParams().port);
}
int printPortHandler(CmdForwarder &source, WebsocketServer &server)
{
	printPort(server);
	return true;
}

int printHelpHandler(CmdForwarder &source, WebsocketServer &server)
{
	cout << "List of commands" << endl;
	map<string, list<string>> descList = source.getCmdDesc();
	for(pair<string, list<string>> entry : descList) {
		list<string> *sKeys = &entry.second;
		list<string>::iterator oIt = sKeys->begin();

		cout << *(oIt++);
		while(oIt != sKeys->end())
			cout << ", " << *(oIt++);

		cout << "\t: " << entry.first << endl;
	}
	return true;
}

int printStatusHandler(CmdForwarder &source, WebsocketServer &server)
{
	cout << "Server is " << (server.isStarted() ? "RUNNING" : "STOPPED") << endl;
	return true;
}

int quitHandler(CmdForwarder &source, WebsocketServer &server)
{
	if(server.isStarted())
		safeStop(server);
	
	cout << "Terminating program...";
	return EXIT_CODE;
}

/**
 * Display command options and takes user input to control the program
 * Blocking until the user selects exit option.
 */
void cmdInterfaceLoop(CmdForwarder &cmdFwd, WebsocketServer &server)
{
	string ui;
	int code;
	printf("Type '?' for a list of commands\n\r");
	while (true)
	{
		cout << "> ";
		cin >> ui;

		if(cmdFwd.isCmd(ui)) {
			code = cmdFwd.retrieveAndCallCmd<int, CmdForwarder&, WebsocketServer&>(ui, cmdFwd, server);
			if(code == EXIT_CODE)
				break;
		} else {
			cout << "Command not found" << endl;
		}

		cout << endl;
	}
}

void setupServer(WebsocketServer &server, ServerConfigParams cfg)
{
	cout << "Setupping server... ";
	server.registerService(EchoCommunication, new EchoService, "echo");
	server.registerService(Routedcommunication, new RoutedCommunicationService, "routed");
	server.registerService(GroupCommunication, new GroupCommunicationService, "grouped");
	server.registerService(StreamedCommunication, new StreamedCommunicationService, "streamed");
	server.registerService(Identification, new IdentificationService, "identification");

	server.setClientFactory(new WebsocketGenClientFactory);
	server.setProtocol(new WebsocketProtocol);
	
	server.setAndApplyConfigParams(cfg);

	//broadcastManager.CreateQueue("groupQueue", 100, true, 10, 10);
	//broadcastManager.CreateQueue("streamingQueue", 100, true, 10, 10);

	//Start streaming thread :
    //UINT  nThreadID;

    //_beginthreadex(NULL, 0, threadProc, NULL, 0, &nThreadID);

	cout << "DONE" << endl;
}

void createCommands(CmdForwarder &cmdFwd)
{
	cmdFwd.registerCmd(safeStartHandler,
		"Start the server if not already started",
		1, "start");
	cmdFwd.registerCmd(safeStopHandler,
		"Stop the server if it has been started",
		1, "stop");
	cmdFwd.registerCmd(printPortHandler, 
		"Print the port being currently used",
		2, "p", "port");
	cmdFwd.registerCmd(printStatusHandler,
		"Acquire status of server",
		1, "status");
	cmdFwd.registerCmd(printHelpHandler,
		"Print the help menu",
		2, "?", "help");
	cmdFwd.registerCmd(quitHandler,
		"Exit the program",
		2, "e", "exit");
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Telemetry Websocket Server v%.1f\n\r", version);
	CmdForwarder cmdFwd;
	createCommands(cmdFwd);
	
	ServerConfigParams cfg;
	if(argc > 1) {
		cfg.port = _ttoi(argv[1]);
	}

	WebsocketServer server;
	setupServer(server, cfg);

	safeStart(server);
	cmdInterfaceLoop(cmdFwd, server);
	
	return 0;
}