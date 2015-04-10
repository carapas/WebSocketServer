#include "StdAfx.h"
#include "Rpc.h"
#include "Objbase.h"
#include "WebsocketGenClient.h"

WebsocketGenClient::WebsocketGenClient(std::string prodId /*= ""*/)
{
	m_bProducer = !prodId.empty();
	if(m_bProducer) // Producer
	{
		m_key = prodId;
		broadcastManager.CreateQueue(m_key.c_str(), 1, true, 10, 10);
	}
	else // Consumer
	{
		UUID uuid;
		char* strUUID;
		if(CoCreateGuid(&uuid) == S_OK)
			if(UuidToStringA(&uuid, (RPC_CSTR*)&strUUID) == RPC_S_OK)
			{
				m_key = std::string(strUUID);
				RpcStringFreeA((RPC_CSTR*)&strUUID);
			}
	}
}

WebsocketGenClient::~WebsocketGenClient(void)
{
	broadcastManager.RemoveQueue(getKey());
}

std::string WebsocketGenClient::getKeyStr()
{
	return m_key;
}

PushFramework::CLIENT_KEY WebsocketGenClient::getKey()
{
    return m_key.c_str();
}

bool WebsocketGenClient::isProducer()
{
	return m_bProducer;
}