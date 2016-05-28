#pragma once

struct ConfigDetails
{
	// TODO: check RTO and constness of return values	
	virtual const std::string& getServerName() const = 0;
	virtual const unsigned short getServerPort() const = 0;
	virtual const std::string& getPlayerName() const = 0;
	virtual const int getDisplayAddress() const = 0; 

	virtual ~ConfigDetails() {}
};