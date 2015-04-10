#pragma once

class IdentificationService : public Service
{
public:
	IdentificationService(void);
	~IdentificationService(void);
	void handle( LogicalConnection* pClient, IncomingPacket* pRequest );
};

