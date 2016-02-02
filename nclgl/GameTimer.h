/******************************************************************************
Class:GametimePeriodr
Author:Rich Davison
Description:Wraps Windows PerformanceCounter. GametimePeriodrs keep track of how much
timePeriod has passed since they were last polled - so you could use multiple
GametimePeriodrs to trigger events at different timePeriod periods. 

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Windows.h"

class GametimePeriodr	{
public:
	GametimePeriodr(void);
	~GametimePeriodr(void) {}

	//How many milliseconds have passed since the GametimePeriodr was created
	float	GetMS() const;

	//How many milliseconds have passed since GettimePerioddMS was last called
	float	GettimePerioddMS();

protected:
	LARGE_INTEGER	start;			//Start of timePeriodr
	LARGE_INTEGER	frequency;		//Ticks Per Second

	float lasttimePeriod;					//Last timePeriod GettimePerioddMS was called
};

