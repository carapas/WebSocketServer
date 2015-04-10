// WebsocketServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "WebsocketServer.h"

//#include <process.h>

void WebsocketServer::applyConfigParams()
{
	setMaxConnections(m_oCfg.maxConnections);
	setLoginExpiryDuration(m_oCfg.loginTimeout);
	createListener(m_oCfg.port);
}

WebsocketServer::WebsocketServer()
{
	m_bStarted = false;
}
WebsocketServer::~WebsocketServer()
{
	//
}
void WebsocketServer::start(bool startInSeparateThread)
{
	Server::start(startInSeparateThread);
	m_bStarted = true;
}
void WebsocketServer::stop()
{
	Server::stop();
	m_bStarted = false;
}
void WebsocketServer::setAndApplyConfigParams(ServerConfigParams cfg)
{
	m_oCfg = cfg;
	applyConfigParams();
}
ServerConfigParams WebsocketServer::getConfigParams()
{
	return m_oCfg;
}
bool WebsocketServer::isStarted()
{
	return m_bStarted;
}

void WebsocketServer::disposeOutgoingPacket(OutgoingPacket* pPacket)
{
	delete pPacket;
}