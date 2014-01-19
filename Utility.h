// ***********************************************************************
// Assembly         : additive
// Author           : David Tran
// Created          : 03-10-2013
//
// Last Modified By : David Tran
// Last Modified On : 03-10-2013
// ***********************************************************************
// <copyright file="Utility.h" company="">
//     Copyright (c) . All rights reserved.
// </copyright>
// <summary>Contains general utility functions (conversions, etc.) </summary>
// ***********************************************************************

#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <stdarg.h>

/// <summary>
/// Utility namespace: contains general utility functions
/// </summary>
namespace Utility
{
	/// <summary>
	/// Struct representing a color (contains 3 floats: r, g, b) - red, green, blue
	/// </summary>
	struct myColor
	{
		/// <summary> a float representing the red component(the user can decide the range of values) </summary>
		float r;
		/// <summary> a float representing the green component(the user can decide the range of values) </summary>
		float g;
		/// <summary> a float representing the blue component(the user can decide the range of values) </summary>
		float b;
	};

	/// Functions ---------------------------------------------------------------------------------

	/// <summary>
	/// Adds a clock to the map of clocks, linking clockname->clock_t
	/// and prints the start time of the clock
	/// </summary>
	/// <param name="clockname">The clockname.</param>
	void start_clock(char clockname);

	/// <summary>
	/// Ends the specified clock.
	/// </summary>
	/// <param name="clockname">The clock's name</param>
	void end_clock(char clockname);

	/// <summary> Display a windows Information MessageBox </summary>
	/// <param name="text">The message</param>
	void messagebox(std::string text);
}

#endif
