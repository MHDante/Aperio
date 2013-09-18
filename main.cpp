// ***********************************************************************
// Assembly         : additive
// Author           : David Tran
// Created          : 03-10-2013
//
// Last Modified By : David Tran
// Last Modified On : 02-19-2013
// ***********************************************************************
// <copyright file="main.cpp" company="">
//     Copyright (c) . All rights reserved.
// </copyright>
// <summary>Main runner file (Just creates a window, very little code) </summary>
// ***********************************************************************

/** 
\page Todo
\tableofcontents

\section list Checklist
<hr />

\subsection things Things to do
+ Shaders for drawing widget elements as contour lines 
+ vtkPlaneSource for widget elements
+ Undo system?
+ click and drag widget elements
+ cut element must put back in same index on list
+ unchecking list item should make opacity 0, check 1.0
+ Depth peeling is kind of broken
+ rid of memory leaks
+ select picked object before placing markers on
+ cell picker is slow, so everytime doing picking, check if selected obj is full opacity, if not, increase to 100% (or 0 is fine)
\subsection bugs Current bugs
myOBJReader expects extra dummy group at end as delimiter (just put g nothing at the end of file)

\subsection solved Solved (tasks/bugs)

+ SOLVED, Speed up boolean operations/loading (recompile carve statically linked with boost)
+ SOLVED, Carve CSG not implemented
<hr />
+ SOLVED, Sometimes crashes on access violation (check myOBJReader for pointer problems)


\mainpage notitle
\tableofcontents

\section about About
<hr />

\subsection intro Introduction

Additive Widgets is a 3D anatomical or other mesh browser
allowing widget elements to be stretched and placed onto surfaces as a way
of marking up areas to be modified; the marked areas can then be cut, peeled, split, exploded,
or these widget elements can act as hinges, directional constraints for exploders etc. Additionally, widget elements can 
be placed acting as handles on meshes for moving them out of the way; parts can be transformed in a free-form fashion,  
constrained like beads on a string, or exploded along a constrained axis, etc. The goal is to develop an intuitive 
and immediate exploration system using widget elements that lends itself to visual affordance (i.e. users
can easily discover ways to combine the elements such that they can perform various actions). It is
primarily created for users who have little to no prior 3D modeling experience and just 
want to break apart a complicated system for the goal of better understanding relationships 
between parts. It can be used by medical professionals on anatomical data
or even be extended for use as an immediate rigging tool on other polygonal
meshes.

<p>These pages document the source code of Additive Widgets.

\subsection links Important Links (dependencies)

Visualization Toolkit 6.0.0 (algorithms and rendering), http://www.vtk.org/
<br>Qt 4.85 (GUI), http://qt-project.org/
<br>Carve CSG 1.40 (Boolean operations), http://carve-csg.com/

*/
#include "stdafx.h"		// Precompiled header files 
#include "additive.h"  

//#include <vld.h>					// Visual Leak Detector
#include "CheckForMemoryLeaks.h"	// MUST be Last include
 
/// ---------------------------------------------------------------------------------------------
/// <summary> The main method itself, creates Additive window class and starts it
/// </summary>
/// <param name="argc">argc.</param>
/// <param name="argv">argv.</param>
/// <returns>success value (0) </returns>
int main(int argc, char *argv[])
{
	// For debugging memory leaks (also must uncomment code in CheckForMemoryLeaks.h)
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );  

	// NOTE TO SELF: if .obj file doesn't load, it's because there's only one group; myOBJReader supports multiple groups only
	// add another "g nothing" line at the end (if more than one, add extra g line)
	 
	QApplication a(argc, argv);
	QApplication::setStyle("cleanlooks");

	//Qt::FramelessWindowHint 
	//Qt::WindowFlags flags = windowFlags();
	//flags &= ~Qt::FramelessWindowHint;
	//setWindowFlags(flags);

	additive w;
	w.show();
	return a.exec();
}
