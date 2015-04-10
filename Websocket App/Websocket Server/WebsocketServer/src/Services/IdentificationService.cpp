#include "StdAfx.h"
#include "IdentificationService.h"
#include "WebsocketGenClient.h"
#include "WebsocketDataMessage.h"

IdentificationService::IdentificationService(void)
{
}


IdentificationService::~IdentificationService(void)
{
}

/**
 * Get list of producer-only clients
 */
vector<WebsocketGenClient*> filterListProducer(vector<LogicalConnection*> list)
{
	vector<WebsocketGenClient*> prodList;
	for(vector<LogicalConnection*>::iterator it = list.begin();
		it != list.end(); ++it)
	{
		WebsocketGenClient* pCl = (WebsocketGenClient*)(*it);
		if(pCl != NULL && pCl->isProducer())
			prodList.push_back(pCl);
	}
	return prodList;
}

void IdentificationService::handle( LogicalConnection* pClient, IncomingPacket* pRequest )
{
	WebsocketDataMessage& request = (WebsocketDataMessage&)(*pRequest);
	WebsocketGenClient& client = (WebsocketGenClient&) (*pClient);
	WebsocketDataMessage response(Identification);

	std::string ret;
	vector<WebsocketGenClient*> clients = filterListProducer(ListClient());
	for(vector<WebsocketGenClient*>::iterator it = clients.begin();
		it != clients.end(); ++it)
	{
		if(it != clients.begin())
			ret.append(",");
		ret.append((*it)->getKeyStr());
	}
	response.SetArguments(ret);
	client.PushPacket(&response);
}