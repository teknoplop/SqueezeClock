#pragma once

#include <string>
#include <application.h>

#include "Looper.h"

class AlarmSettingsMonitor :
public Looper
{
public:
	AlarmSettingsMonitor( std::shared_ptr< ServiceDirectory > svc, std::shared_ptr< ConfigDetails > conf );
	virtual ~AlarmSettingsMonitor();

	virtual void loop();
	virtual void setup();

	void onPlayerIdChanged( const std::string& playerId );

private:
	std::string     _svrHost;
	unsigned short  _svrPort;
	TCPClient       _svrCon;
    std::string     _monitor;
    std::string		_playerId;

    std::shared_ptr< ServiceDirectory > _svc;

	std::function<void( const std::string& )> _fnProcessResp;

	void monitorPlayer ( const std::string& rsp );
};