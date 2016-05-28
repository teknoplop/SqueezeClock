#include "AlarmSettingsMonitor.h"
#include "Utils.h"

AlarmSettingsMonitor::AlarmSettingsMonitor( std::shared_ptr< ServiceDirectory > svc, std::shared_ptr< ConfigDetails > conf ) :
_svrHost( conf->getServerName() ),
_svrPort( conf->getServerPort() ),
_svc( svc )
{
}

AlarmSettingsMonitor::~AlarmSettingsMonitor() 
{

}

void 
AlarmSettingsMonitor::onPlayerIdChanged( const std::string& playerId )
{
	_playerId = playerId;

    Serial.printlnf( "AlarmSettingsMonitor::onPlayerIdChanged: %s", _playerId.c_str() );	

    // TODO: repeat after current run if already busy
    if ( !_fnProcessResp )
    {
	    // TODO: reconnect logic
		Serial.printlnf( "AlarmSettingsMonitor::setup: connecting to %s:%d", _svrHost.c_str(), _svrPort );

		if ( _svrCon.connect( _svrHost.c_str(), _svrPort ) )
	  	{
	    	Serial.printlnf( "AlarmSettingsMonitor Server IP: %s", _svrCon.remoteIP().toString().c_str() );
	    	_svrCon.println("listen 1");
	    	_fnProcessResp = std::bind(&AlarmSettingsMonitor::monitorPlayer, this, std::placeholders::_1);;
	    }
    }
}

 
// std::string::starts_with
// https://gist.github.com/liuml07/5672247
inline bool starts_with(const std::string &big_str, const std::string &small_str) {
  return big_str.compare(0, small_str.length(), small_str) == 0;
}

void 
AlarmSettingsMonitor::loop()
{
	// Serial.println( "AlarmSettingsMonitor::loop" );
    if ( _fnProcessResp )
    {
        while ( _svrCon.available() )
        {
            // TODO: use read( n )
            char c = _svrCon.read();

            if ( c == '\n' )
            {
    			_fnProcessResp( Utils::urldecode( _monitor.c_str() ) );
                _monitor.clear();
            }
            else
            {
                _monitor += c;   
            }
        }
    }
}

void
AlarmSettingsMonitor::monitorPlayer ( const std::string& rsp )
{
// TODO: reload alarm on notify changed

/*
b8:27:eb:65:42:7e prefset server alarms HASH(0x2aba5c8)
b8:27:eb:65:42:7e alarm _cmd
b8:27:eb:65:42:7e prefset server alarms HASH(0x2aba5c8)
b8:27:eb:65:42:7e alarm _cmd
*/

    Serial.println( rsp.c_str() );
    
    // TODO: locate std::string::starts_with
    if ( starts_with( rsp, _playerId + " prefset server alarms") )
    {
    	Serial.println( "AlarmSettingsMonitor::monitorPlayer: Bedroom alarm prefs changed" );
            
        // Notify all subscribers that a change has occured to the alarm settings
        for( auto const& onAlarmChanged : _svc->_onAlarmChanged ) 
        {
            onAlarmChanged();
        }	
    }
}

void 
AlarmSettingsMonitor::setup()
{
    _svc->_onPlayerIdChanged.push_back( 
        std::bind(&AlarmSettingsMonitor::onPlayerIdChanged, this, std::placeholders::_1) );
}

