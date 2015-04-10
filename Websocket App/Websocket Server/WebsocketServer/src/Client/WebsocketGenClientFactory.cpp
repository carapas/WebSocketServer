#include "StdAfx.h"
#include "WebsocketGenClientFactory.h"
#include "WebsocketGenClient.h"

WebsocketGenClientFactory::WebsocketGenClientFactory(void)
{
}

WebsocketGenClientFactory::~WebsocketGenClientFactory(void)
{
}

int WebsocketGenClientFactory::onFirstRequest( IncomingPacket& _request, ConnectionContext* pConnectionContext, LogicalConnection*& lpClient, OutgoingPacket*& lpPacket )
{
	//received messages belong to a physical connection that still did not transform into a logical connection :
	//understand in which stage we are :
	WebsocketConnectionContext* pCxt = (WebsocketConnectionContext*) pConnectionContext;

	switch(pCxt->GetStage())
	{
		case WebsocketConnectionContext::HandshakeStage:
		{
			WebsocketHandshakeMessage& request = (WebsocketHandshakeMessage&) _request;
			if (!request.Parse())
			{
				return ClientFactory::RefuseAndClose;
			}

			WebsocketHandshakeMessage *pResponse = new WebsocketHandshakeMessage();
			if (WebsocketProtocol::ProcessHandshake(request, *pResponse))
			{
				lpPacket = pResponse;
				pCxt->SetStage(WebsocketConnectionContext::LoginStage);
			}
			return ClientFactory::RefuseRequest; // Will not close the connection, but we still wait for login message to create a logical client.
		}
		case WebsocketConnectionContext::LoginStage:
		{
			WebsocketDataMessage& request = (WebsocketDataMessage&) _request;
			WebsocketGenClient* pClient = NULL;
			
			pClient = new WebsocketGenClient(request.GetArg1());

			// Handle invalid UUID
			if(pClient->getKeyStr() == "undefined")
			{
				cout << endl << "Invalid UUID, closing connection" << endl;
				return ClientFactory::RefuseAndClose;
			}

			lpClient = pClient;

			WebsocketDataMessage *pResponse = new WebsocketDataMessage(LoginCommunication);
			pResponse->SetArguments(pClient->isProducer() ?
				request.GetArg1() : // Send back Producer Id
				"Connected as : " + pClient->getKeyStr());
			lpPacket = pResponse;

			pCxt->SetStage(WebsocketConnectionContext::ConnectedStage);

			return ClientFactory::CreateClient;
		}
		default: // Undefined/unsupported stage
			break;
	}

	return -1;
}

void WebsocketGenClientFactory::onClientDisconnected( LogicalConnection* pClient )
{
	Disconnect(pClient, false);
}

void WebsocketGenClientFactory::disposeClient( LogicalConnection* pClient )
{
	delete pClient;
}

OutgoingPacket* WebsocketGenClientFactory::onNewConnection( ConnectionContext*& pConnectionContext )
{
	pConnectionContext = new WebsocketConnectionContext();
	return NULL;
}

