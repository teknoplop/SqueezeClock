#include "ButtonMonitor.h"


ButtonMonitor::ButtonMonitor( std::shared_ptr< ServiceDirectory > svc, std::shared_ptr< ConfigDetails > conf ) 
: _svc( svc )
{
}

ButtonMonitor::~ButtonMonitor() 
{

}

void 
ButtonMonitor::setup()
{
	Serial.println( "ButtonMonitor::setup" );
	_buttonMux.begin();	
	_buttonTouched = false;
}

void 
ButtonMonitor::loop()
{
	// detect a toggle
	if ( _buttonMux.touched() != _buttonTouched )
	{
		_buttonTouched = _buttonMux.touched();
		Serial.printlnf( "ButtonMonitor::loop toggle->%s", _buttonTouched ? "On" : "Off" );

		for( auto const& onButtonChanged : _svc->_onButtonChanged ) 
        {
            onButtonChanged( _buttonTouched );
        }	
	}
}