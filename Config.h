#pragma once

#include <string>

#include "ConfigDetails.h"


class Config :
public ConfigDetails
{
public:
	const std::string& getServerName() const { return _svrName; }
	const unsigned short getServerPort() const { return _svrPort; }
	const std::string& getPlayerName() const { return _playerName; }
	const int getDisplayAddress() const { return _displayAddress; }



	Config();
	virtual ~Config();
private:
	const std::string _svrName;
	const unsigned short _svrPort;
	const std::string _playerName;
	const int _displayAddress;
};