#include "ClockDisplay.h"

ClockDisplay::ClockDisplay( std::shared_ptr< ServiceDirectory > svc, std::shared_ptr< ConfigDetails > conf )
: _svc( svc )
, _colonOn( false )
, _alarmEnabled( false )
, _displayAddress( conf->getDisplayAddress() )
, _h( 0 )
, _m( 0 )
, _searchingCount( 0 )
, _buttonTouched ( false )
, _dim( false )
, _alarmOn( false )
, _ah( 0 )
, _am( 0 )
{
	_lastTicks = millis();
	_dirty = true;
}

ClockDisplay::~ClockDisplay()
{

}


void 
ClockDisplay::loop()
{
	// TODO: make timer interrupt driven?
	// if 500ms has passed then toggle the colon and draw
	auto ticks = millis();
	if ( ticks - _lastTicks > 500 )
	{
		_lastTicks = ticks;
		_colonOn = !_colonOn;

		if (++ _searchingCount == 3 )
		{
			_searchingCount = 0;
		}
		_dirty = true;


		// TODO: make an event from the light monitor
		int a0 = analogRead(A0);
	   // sprintf( sz, "Lightness: %d", a0 );
	   // sendSyslogMessage( sz );

	    if ( !_dim  && a0 < 75 )
	    {
	    	_dim = true;
	    	_clockDisplay.setBrightness(0);

	    }
	    else if ( _dim && a0 > 200 )
	    {
	    	_dim = false;
	    	_clockDisplay.setBrightness(14);
	    }

	}

	if ( _fnDisplayAlarm )
	{
		_fnDisplayAlarm();	
	}
	else if ( _fnDisplayMode )
	{
		_fnDisplayMode();
	}
}

void
ClockDisplay::showAlarm()
{
	// go back to now 5 seconds after button released (or alarm time changed)
	auto ticks = millis();
	if ( _buttonTouched )
	{
		_lastAlarmTicks = ticks;
	}
	else if ( ticks - _lastAlarmTicks > 5000 )
	{
		_lastTicks = ticks;
		_fnDisplayAlarm = 0;
		return;
	}


	if ( _alarmDirty )
	{  
		_alarmDirty = false;
		if ( _alarmEnabled )
		{
			_clockDisplay.writeDigitNum( 0, _ah / 10, false );
			_clockDisplay.writeDigitNum( 1, _ah % 10, false );

			_clockDisplay.writeDigitNum( 3, _am / 10, false );
			_clockDisplay.writeDigitNum( 4, _am % 10, false );
		}
		else
		{
			_clockDisplay.writeDigitRaw( 0, 1 << 7 );
			_clockDisplay.writeDigitRaw( 1, 1 << 7 );

			_clockDisplay.writeDigitRaw( 3, 1 << 7 );
			_clockDisplay.writeDigitRaw( 4, 1 << 7 );		
		}
		_clockDisplay.drawColon( true );

		// commit the written values
		_clockDisplay.writeDisplay();	
	}
}

void
ClockDisplay::showNow()
{
	if ( _dirty)
	{
		_dirty = false;

		if ( _alarmOn &&  !_colonOn )
		{
			_clockDisplay.clear();
		}
		else
		{
			_clockDisplay.writeDigitNum( 0, _h / 10 );
			_clockDisplay.writeDigitNum( 1, _h % 10 );

			_clockDisplay.writeDigitNum( 3, _m / 10 );
			_clockDisplay.writeDigitNum( 4, _m % 10, _dst ); // show dst debug

			// tick the colon
			_clockDisplay.drawColon( _colonOn );
		}

		// commit the written values
		_clockDisplay.writeDisplay();	
	}
}

void
ClockDisplay::showWaitingForSync()
{
	if ( _dirty)
	{
		_dirty = false;

		_clockDisplay.writeDigitRaw( 0, (1 << 0 )+ (1 << 5) + (1 << 4) + (1 << 3) 
			+ ( _searchingCount == 0 ? (1 << 7 ) : 0 ));
		_clockDisplay.writeDigitRaw( 1, (1 << 0 )+ (1 << 3)
			+ ( _searchingCount == 1 ? (1 << 7 ) : 0 ));

		_clockDisplay.writeDigitRaw( 3, (1 << 0) + (1 << 3) 
			+ ( _searchingCount == 2 ? (1 << 7 ) : 0 ));
		_clockDisplay.writeDigitRaw( 4, (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) ); 

		// worry the colon
		_clockDisplay.drawColon( false );

		// commit the written values
		_clockDisplay.writeDisplay();	
	}
}

void 
ClockDisplay::onAlarmTimeChanged( bool e, int h, int m )
{
	_ah = h;
	_am = m;
	_alarmEnabled = e;

	_fnDisplayAlarm = std::bind( &ClockDisplay::showAlarm, this );
	_alarmDirty = true;
	_lastAlarmTicks = millis();
}

void 
ClockDisplay::onTimeChanged( int h, int m )
{
	_h = h;
	_m = m;
	_fnDisplayMode = std::bind( &ClockDisplay::showNow, this );
	_dirty = true;
}

void ClockDisplay::onButtonChanged( const bool buttonTouched )
{
	_buttonTouched = buttonTouched;

	if ( _buttonTouched )
	{
		if ( _alarmOn )
		{
			_alarmOn = false;
		}

		if ( !_fnDisplayAlarm )
		{
			_fnDisplayAlarm = std::bind( &ClockDisplay::showAlarm, this );
			_alarmDirty = true;
			_lastAlarmTicks = millis();	
		}	
	}
}

void
ClockDisplay::onAlarmStart()
{
	_alarmOn = true;
}

void 
ClockDisplay::setup()
{
  _clockDisplay.begin( _displayAddress );
  _clockDisplay.clear();
  _clockDisplay.writeDisplay();

  _clockDisplay.setBrightness(14);

	_svc->_onAlarmTimeChanged.push_back( 
	    std::bind(&ClockDisplay::onAlarmTimeChanged, 
	    this, 
	    std::placeholders::_1,
	    std::placeholders::_2,
	    std::placeholders::_3 ) );

	_svc->_onTimeChanged.push_back( 
	    std::bind(&ClockDisplay::onTimeChanged, 
	    this, 
	    std::placeholders::_1,
	    std::placeholders::_2) );

	_svc->_onButtonChanged.push_back( 
	    std::bind(&ClockDisplay::onButtonChanged, 
	    this, 
	    std::placeholders::_1 ) );

	_svc->_onAlarmStart.push_back( 
	    std::bind(&ClockDisplay::onAlarmStart, 
	    this ) );

	_dirty = true;
	_fnDisplayMode = std::bind( &ClockDisplay::showWaitingForSync, this );

}



