#include "StdAfx.h"
#include "GroupCommunicationService.h"
#include "WebsocketGenClient.h"
#include "WebsocketDataMessage.h"

GroupCommunicationService::GroupCommunicationService(void)
{
}


GroupCommunicationService::~GroupCommunicationService(void)
{
}

void GroupCommunicationService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{
	WebsocketDataMessage& request = (WebsocketDataMessage&)(*pRequest);
	WebsocketGenClient& client = (WebsocketGenClient&) (*pClient);

	string opType = request.GetArg1();
	string group = request.GetArg2();

	if (opType == "subscribe")
	{
		broadcastManager.SubscribeConnectionToQueue(
			client.getKey(), group.c_str());
	}
	else if (opType == "unsubscribe")
	{
		broadcastManager.UnsubscribeConnectionFromQueue(
			client.getKey(), group.c_str());
	}
	else if (opType == "publish")
	{
		if(client.isProducer())
		{
			//cout << endl << "Producer [" << client.getKey() << "] has produced : [" << request.GetArg2() << "]" << endl;
			WebsocketDataMessage* message = new WebsocketDataMessage(GroupCommunication);
			message->SetArguments(client.getKeyStr(), request.GetArg2());
			broadcastManager.PushPacket(message, client.getKey());
		}
	}
}
