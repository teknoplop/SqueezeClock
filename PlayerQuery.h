#pragma once

#include <application.h>

#include "Looper.h"

class PlayerQuery :
public Looper
{
public:
	PlayerQuery( std::shared_ptr< ServiceDirectory > svc, std::shared_ptr< ConfigDetails > conf );
	virtual ~PlayerQuery();

	virtual void loop();
	virtual void setup();

	void onAlarmChanged();

private:
    std::shared_ptr< ServiceDirectory > _svc;

	std::string     _svrHost;
	unsigned short  _svrPort;
	TCPClient       _svrCon;

	std::string     _response;
    std::string		_playerId;
    std::string 	_playerName;

	std::function<void( const std::string& )> _fnProcessResp;

	int _playerCount = 0;
	int _thisPlayerInfoId = 0;

	void cbPlayerInfo( const std::string& playerInfoRsp );
	void cbPlayerCount( const std::string& playerCountRsp );
	void cbSnooze( const std::string& snoozeRsp );
	void cbPlayerAlarm( const std::string& alarmInfoRsp );

	void requestNextPlayerInfo();
	void requestAlarmTime();

};
