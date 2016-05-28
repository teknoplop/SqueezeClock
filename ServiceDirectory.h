#pragma once

#include <list>
#include <string>

// TODO: make more classy
struct ServiceDirectory
{
	typedef std::function<void()> 						OnAlarmChanged;
	typedef std::function<void( bool, int, int )> 			OnAlarmTimeChanged;
	typedef std::function<void()> 						OnAlarmStart;
	typedef std::function<void( const std::string& )> 	OnPlayerIdChanged;

	typedef std::function<void( int, int )> 			OnTimeChanged;
	typedef std::function<void()> 						OnDstChanged;

	typedef std::function<void( bool ) >				OnButtonChanged;

	// TODO: make more classy too - a templated notify
	std::list< OnPlayerIdChanged >  _onPlayerIdChanged;
	std::list< OnAlarmChanged >     _onAlarmChanged;
	std::list< OnTimeChanged > 		_onTimeChanged;	
	std::list< OnAlarmTimeChanged> 	_onAlarmTimeChanged;
	std::list< OnAlarmStart >       _onAlarmStart;
	std::list< OnButtonChanged >	_onButtonChanged;
};