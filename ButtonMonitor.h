#pragma once

#include "Looper.h"

#include "CAP1188.h"

class ButtonMonitor :
public Looper
{
public:
	ButtonMonitor( std::shared_ptr< ServiceDirectory > svc, std::shared_ptr< ConfigDetails > conf );
	virtual ~ButtonMonitor();

	virtual void loop();
	virtual void setup();

private:
	Adafruit_CAP1188 _buttonMux;
	std::shared_ptr< ServiceDirectory > _svc;
	bool _buttonTouched;
};
