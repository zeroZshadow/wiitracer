/*
	#libprofile (Tiny code profiler for various game consoles)
 
	timer.h
 
	Copyright (c) 2010, Ian Callaghan <ian (at) unrom (dot) com>

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
	documentation files (the "Software"), to deal in the Software without restriction, including without
	limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
	the Software, and to permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial
	portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
	LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
	EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
	AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
	OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __TIMER_H__
#define __TIMER_H__

#ifndef WII
#define CPU_CLK		486000000u		// Gamecube CPU Clock Speed in Mhz
#define TIMER_CLK	40500000		// Gamecube Timer Clock Speed (CPU CLK / 12)
#else
#define CPU_CLK		729000000u		// Wii CPU Clock Speed in Mhz
#define TIMER_CLK	60750000		// Wii Timer Clock Speed (CPU CLK / 12)
#endif

u64 timer_gettime();

#endif // __TIMER_H__
