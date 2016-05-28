#include "AlarmBackup.h"

AlarmBackup::AlarmBackup( std::shared_ptr< ServiceDirectory > svc, std::shared_ptr< ConfigDetails > conf )
: _svc( svc )
, _alarmEnabled( false )
, _h( 0 )
, _m( 0 )
, _ah( 0 )
, _am( 0 )

{
}

AlarmBackup::~AlarmBackup()
{

}

void 
AlarmBackup::onAlarmTimeChanged( bool e, int h, int m )
{
	_ah = h;
	_am = m;
	_alarmEnabled = e;
	_loopAction = std::bind( &AlarmBackup::checkAlarm, this );
}

void 
AlarmBackup::onTimeChanged( int h, int m )
{
	_h = h;
	_m = m;
	_loopAction = std::bind( &AlarmBackup::checkAlarm, this );
}

void
AlarmBackup::checkAlarm()
{
	_loopAction= 0;

	if ( _alarmEnabled && _ah == _h && _am == _m )
	{
  		Serial.println( "AlarmBackup::checkAlarm: started");  

		for( auto const& onAlarmStart : _svc->_onAlarmStart ) 
        {
            onAlarmStart();
        }

        // TODO: buzz if squeezebox doesn't play after 1 min
	}

}

void 
AlarmBackup::loop()
{
	if ( _loopAction )
	{
		_loopAction();
	}
}

void 
AlarmBackup::setup()
{
	_svc->_onAlarmTimeChanged.push_back( 
	    std::bind(&AlarmBackup::onAlarmTimeChanged, 
	    this, 
	    std::placeholders::_1,
	    std::placeholders::_2,
	    std::placeholders::_3 ) );

	_svc->_onTimeChanged.push_back( 
	    std::bind(&AlarmBackup::onTimeChanged, 
	    this, 
	    std::placeholders::_1,
	    std::placeholders::_2) );
}