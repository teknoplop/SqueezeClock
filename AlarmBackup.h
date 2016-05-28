#pragma once

#include <string>
#include <application.h>

#include "Looper.h"

class AlarmBackup :
public Looper
{
public:
	AlarmBackup( std::shared_ptr< ServiceDirectory > svc, std::shared_ptr< ConfigDetails > conf );
	virtual ~AlarmBackup();

	virtual void loop();
	virtual void setup();

	void onAlarmTimeChanged( bool e, int h, int m );	
	void onTimeChanged( int h, int m );
	void onDstChanged( const bool isDst );

private:
	int _h;
	int _m;
	int _ah;
	int _am;
	bool _dst;
	bool _alarmEnabled;


    std::shared_ptr< ServiceDirectory > _svc;

    std::function< void() > _loopAction; 
    void checkAlarm(); 
};