#include "Config.h"

// Could be sourced from flash to protect private config values
Config::Config()
: _svrName( "squeezebox" )
, _svrPort( 9090 )
, _playerName( "Bedroom")
, _displayAddress( 0x70 )
{

}

Config::~Config()
{

}