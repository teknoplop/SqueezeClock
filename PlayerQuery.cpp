#include "PlayerQuery.h"

#include <algorithm>

#include "Utils.h"


PlayerQuery::PlayerQuery( std::shared_ptr< ServiceDirectory > svc, std::shared_ptr< ConfigDetails > conf ) :
_svrHost( conf->getServerName() ),
_svrPort( conf->getServerPort() ),
_playerName( conf->getPlayerName() ),
_svc( svc )
{

}

PlayerQuery::~PlayerQuery() 
{

}

void 
PlayerQuery::setup()
{
    // bind to be notified of alarm changes
    _svc->_onAlarmChanged.push_back( std::bind(&PlayerQuery::onAlarmChanged, this ) );

    // TODO: reconnect logic
    Serial.printlnf( "PlayerQuery::setup: connecting to %s:%d", _svrHost.c_str(), _svrPort );

    // kick off a query to indentify our player id
    if ( _svrCon.connect( _svrHost.c_str(), _svrPort ) )
    {
        Serial.printlnf( "PlayerQuery Server IP: %s", _svrCon.remoteIP().toString().c_str() );
        _svrCon.println("players");
        _fnProcessResp = std::bind(&PlayerQuery::cbPlayerCount, this, std::placeholders::_1);
    }



}
 
// std::string::starts_with
// https://gist.github.com/liuml07/5672247
inline bool starts_with(const std::string &big_str, const std::string &small_str) {
  return big_str.compare(0, small_str.length(), small_str) == 0;
}

void 
PlayerQuery::loop()
{
    // Serial.println( "AlarmSettingsMonitor::loop" );
    if ( _fnProcessResp )
    {
        // TODO: use read(n)
        while ( _svrCon.available() )
        {
            char c = _svrCon.read();
            if ( c == '\n' )
            {
                _fnProcessResp( Utils::urldecode( _response.c_str() ) );
                _response.clear();
            }
            else
            {
                _response += c;   
            }
        }
    }
}


void 
PlayerQuery::cbPlayerAlarm( const std::string& alarmInfoRsp )
{
    Serial.println( alarmInfoRsp.c_str() );
    bool enabled = false;
    int h = 0;
    int m = 0;

    std::size_t spos = alarmInfoRsp.find( "time:");
    if ( spos  != std::string::npos )
    {
        std::size_t epos = alarmInfoRsp.find( " " );

        int sm = atoi( alarmInfoRsp.substr( spos + 5, epos == std::string::npos ? std::string::npos : epos - spos ).c_str() );
        h = sm / ( 60 * 60 );
        m = ( sm - ( h * 60 * 60 ) ) / 60;

        enabled =true;
        Serial.printlnf( "Alarm Time: %d %02d:%02d:%02d", sm, h, m, sm % 60 ); 
    }
    else
    {
        Serial.printlnf( "Alarm Disabled" ); 
    }

    for( auto const& onAlarmTimeChanged : _svc->_onAlarmTimeChanged ) 
    {
        // TODO: use optional
        onAlarmTimeChanged( enabled, h, m );
    }   

    _fnProcessResp = 0;


        //sqControl.printlnf("%s button snooze", _playerId.c_str() );
        //infoCallBack = cbSnooze;


}


void 
PlayerQuery::cbPlayerInfo( const std::string& playerInfoRsp )
{
    Serial.printlnf( "PlayerQuery::cbPlayerInfo: %s", playerInfoRsp .c_str() );   

    if ( playerInfoRsp.find( "name:" + _playerName ) != std::string::npos )
    {
        _playerId = playerInfoRsp.substr( playerInfoRsp.find( "playerid:") + 9, 17); 
    }

    requestNextPlayerInfo();
}


void 
PlayerQuery::cbPlayerCount( const std::string& playerCountRsp )
{
    _playerCount = atoi( playerCountRsp.substr( playerCountRsp.find_last_of( ":" ) + 1 ).c_str() );
    _thisPlayerInfoId = 0;

    Serial.printlnf( "PlayerQuery:: Player Count: %d", _playerCount );   

    requestNextPlayerInfo();
}

void 
PlayerQuery::cbSnooze( const std::string& snoozeRsp )
{
 /*  
   Serial.println( snoozeRsp.c_str() );
    
        infoCallBack = 0;
        sqControl.stop();*/
}

void
PlayerQuery::onAlarmChanged()
{
    Serial.printlnf( "PlayerQuery::onAlarmChanged: %s", _playerId.c_str() );    

    // TODO: repeat if already busy
    requestAlarmTime();
}

void
PlayerQuery::requestAlarmTime()
{
    // TODO: no player id ?
    _svrCon.printlnf("%s alarms 1 1", _playerId.c_str() );
    _fnProcessResp = std::bind(&PlayerQuery::cbPlayerAlarm, this, std::placeholders::_1);
}

void 
PlayerQuery::requestNextPlayerInfo()
{
    if ( _thisPlayerInfoId < _playerCount )
    {
        _svrCon.printlnf("players %d 1", _thisPlayerInfoId++);

        _fnProcessResp = std::bind(&PlayerQuery::cbPlayerInfo, this, std::placeholders::_1);
    }
    else
    {       
        Serial.printlnf( "PlayerQuery::%s ID: %s", 
            _playerName.c_str(),
            _playerId.c_str() ); 
  
        // Notify all subscribers of the player ID
        for( auto const& onPlayerIdChanged : _svc->_onPlayerIdChanged ) 
        {
            onPlayerIdChanged( _playerId);
        }

        requestAlarmTime();

    }
}