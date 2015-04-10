#pragma once

class WebsocketGenClient : public LogicalConnection
{
public:
    WebsocketGenClient(std::string prodId = "");
    ~WebsocketGenClient();
    CLIENT_KEY getKey();
	std::string getKeyStr();
	bool isProducer();
private:
	std::string m_key;
	bool m_bProducer;
};
