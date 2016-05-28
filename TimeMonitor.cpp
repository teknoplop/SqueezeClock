#include <application.h>

#include "TimeMonitor.h"


TimeMonitor::TimeMonitor( std::shared_ptr< ServiceDirectory > svc, std::shared_ptr< ConfigDetails > conf ) 
: _lastMinute( 0 )
, _svc( svc )
{
}

TimeMonitor::~TimeMonitor() 
{

}

void 
TimeMonitor::setup()
{
	Serial.printlnf( "TimeMonitor::setup" );
}

void 
TimeMonitor::loop()
{
	auto t = Time.now();
	int m = Time.minute( t );

	if ( m != _lastMinute )
	{
		_lastMinute = m;

     	for( auto const& onTimeChanged : _svc->_onTimeChanged ) 
        {
    	    onTimeChanged( Time.hour(t), m );
        } 
	}
}