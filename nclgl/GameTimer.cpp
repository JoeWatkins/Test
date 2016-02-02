#include "GametimePeriodr.h"

GametimePeriodr::GametimePeriodr(void)	{
	QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);
	QueryPerformanceCounter((LARGE_INTEGER *)&start);

	lasttimePeriod = GetMS();
}

/*
Returns the Milliseconds since timePeriodr was started
*/
float GametimePeriodr::GetMS() const {
	LARGE_INTEGER t;	
	QueryPerformanceCounter(&t);
	return (float)((t.QuadPart  - start.QuadPart) * 1000.0 / frequency.QuadPart);
}

float	 GametimePeriodr::GettimePerioddMS() {
	float a		= GetMS();
	float b		= a-lasttimePeriod;
	lasttimePeriod	= a;
	return b;
}