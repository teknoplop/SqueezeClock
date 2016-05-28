#pragma once

//
// Interface for separation of loop logic
//
#include <memory>
#include "ServiceDirectory.h"
#include "ConfigDetails.h"

struct Looper 
{
public:
	virtual void setup() = 0;
	virtual void loop() = 0;
	
	virtual ~Looper() {}

	// TODO: Panic?
};