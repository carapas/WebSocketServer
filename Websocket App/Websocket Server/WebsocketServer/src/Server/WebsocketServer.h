#pragma once

const int DEF_MAXCON	 = 128;
const int DEF_LOGTIMEOUT = 120;
const int DEF_PORT		 = 81;

struct ServerConfigParams {
	ServerConfigParams() : maxConnections(DEF_MAXCON),
		                   loginTimeout(DEF_LOGTIMEOUT),
	                       port(DEF_PORT) {}
	int maxConnections;
	int loginTimeout;
	int port;
};

class WebsocketServer : public Server
{
private:
	bool m_bStarted;
	ServerConfigParams m_oCfg;
	
	void applyConfigParams();
public:
	WebsocketServer();
	~WebsocketServer();
	void start(bool startInSeparateThread);
	void stop();
	void setAndApplyConfigParams(ServerConfigParams cfg);
	ServerConfigParams getConfigParams();
	bool isStarted();

protected:
	virtual void disposeOutgoingPacket(OutgoingPacket* pPacket);
};
