#pragma once

#include "Looper.h"


class TimeMonitor :
public Looper
{
public:
	TimeMonitor( std::shared_ptr< ServiceDirectory > svc, std::shared_ptr< ConfigDetails > conf );
	virtual ~TimeMonitor();

	virtual void loop();
	virtual void setup();

private:
    std::shared_ptr< ServiceDirectory > _svc;

    int _lastMinute;
};
