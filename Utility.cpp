#include "stdafx.h"		// Precompiled header files
#include "Utility.h"

// Standard
#include <iostream>
#include <map>
#include <time.h>

//#include "CheckForMemoryLeaks.h"	// MUST be Last include

// Utility Variables -------------------------------------------------------------------------
namespace Utility
{
	/// <summary>
	/// Map of "clockname->clock_t" pairs used for profiling (for timing chunks of code)
	/// </summary>
	std::map<char, clock_t> clocks;
}

/// ---------------------------------------------------------------------------------------------
void Utility::start_clock(char clockname)
{
	clocks[clockname] = clock();
	std::cout << "--- Started Clock: " << clockname << " --- " << std::endl;
}

/// ---------------------------------------------------------------------------------------------
void Utility::end_clock(char clockname)
{
	std::cout << "--- Ended Clock: " << clockname << ", (elapsed: " << float(clock() - clocks[clockname]) / CLOCKS_PER_SEC << ") ---" << std::endl;
}

/// ---------------------------------------------------------------------------------------------
void Utility::messagebox(std::string text)
{
	MessageBoxA(nullptr, text.c_str(), "", MB_ICONINFORMATION);
}