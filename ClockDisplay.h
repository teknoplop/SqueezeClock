#pragma once

#include <string>
#include <application.h>

#include "Looper.h"
#include "adafruit-led-backpack.h"

class ClockDisplay :
public Looper
{
public:
	ClockDisplay( std::shared_ptr< ServiceDirectory > svc, std::shared_ptr< ConfigDetails > conf );
	virtual ~ClockDisplay();

	virtual void loop();
	virtual void setup();

	void onAlarmTimeChanged( bool e, int h, int m );	
	void onTimeChanged( int h, int m );
	void onDstChanged( const bool isDst );
	void onButtonChanged( const bool buttonTouched );
	void onAlarmStart();

private:
	int _h;
	int _m;
	int _ah;
	int _am;
	int _dim;

	bool _dst;
	bool _colonOn;
	int _lastTicks;
	int _lastAlarmTicks;
	bool _dirty;
	bool _alarmDirty;
	bool _alarmEnabled;
	int _displayAddress;
	bool _buttonTouched;
	bool _alarmOn;

	int _searchingCount;

	Adafruit_7segment _clockDisplay = Adafruit_7segment();

    std::shared_ptr< ServiceDirectory > _svc;

    std::function< void() > _fnDisplayMode;    
    std::function< void() > _fnDisplayAlarm;

    void showNow();
    void showAlarm();
    void showAlarmNext();
    void showSnooze();
    void showWaitingForSync();
};