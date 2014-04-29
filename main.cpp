#include "stdafx.h"

// ***********************************************************************
// Assembly         : Mesh Illustrator
// Author           : David Tran
// Created          : 03-10-2013
//
// Last Modified By : David Tran
// Last Modified On : 03-24-2014
// ***********************************************************************
// <copyright file="main.cpp" company="">
//     Copyright (c) . All rights reserved.
// </copyright>
// <summary>Main runner file (Just creates a window; little code) </summary>
// ***********************************************************************

/**
\page Todo
\tableofcontents

\section list Checklist
<hr />

\subsection things Things to do
+ Shaders for drawing widget elements as contour lines (outline)
+ Undo system?
+ click and drag elements
+ if cutting, elements must be put back in same index on list
+ unchecking list item should set its opacity to 0, check for 1.0
+ Depth peeling is kind of broken
+ rid of memory leaks
+ select picked object before placing markers on
+ cell picker is slow, so everytime doing picking, check if selected obj is full opacity, if not, increase to 100% (or 0 is fine)

\subsection bugs Current bugs
carve CSG errors in double shell meshes
null reference exceptions when accessing non-existing objects

\subsection solved Solved (tasks/bugs)

+ SOLVED [created new loader], myOBJReader expects extra dummy group at end as delimiter (requires g at the end of obj file)
+ SOLVED, Speed up boolean operations/loading (recompile carve statically linked with boost)
+ SOLVED [implemented], Carve CSG not implemented
<hr />
+ SOLVED, [smart pointers and c++11] crashes and access violation (check myOBJReader for pointer problems)

\mainpage notitle
\tableofcontents

\section about About
<hr />

\subsection intro Introduction

Mesh Illustrator is a 3D anatomical or other mesh browser allowing slidable superquadrics to be stretched
and placed onto surfaces as a way of marking up areas to be illustratively manipulated
(transformed or deformed in a manner for the purpose of illustration and understanding). The marked areas can
then be cut, peeled, split, deformed or act as hinges or constraints for exploders etc. Additionally, elements can
act as handles on meshes for moving them out of the way; parts can also be transformed in a free-form fashion,
constrained like beads on a string, or exploded along a constrained axis, etc. The goal is to develop an
intuitive and immediate exploration system using slidable elements (in the shape of superquadrics) that
lends itself to visual affordance (i.e. users can easily and intuitively combine the elements such that
they can perform various actions). It is primarily created for users who have little to no prior 3D modeling
experience and just want to break apart a complicated system for the goal of better understanding relationships
between parts in the 3D system. It can be used by medical professionals on anatomical data or even be extended
for use as an immediate rigging tool on other polygonal meshes.

<p>These pages document the source code of Mesh Illustrator.

\subsection links Important Links (dependencies)

Written using C++11 (Requires Visual Studio 2010 or higher) <br />
Visualization Toolkit 6.1.0 (algorithms and rendering), [http://www.vtk.org/](http://www.vtk.org/) <br />
Qt 4.85 (GUI), [http://qt-project.org/](http://qt-project.org/) <br />
Carve CSG (Latest Repo)(Boolean operations), [http://carve-csg.com/](http://carve-csg.com/) <br />
Graphics card with GLSL Shader Support (OpenGL 3.0 or higher)

*/

#include "illustrator.h"
#include <QtWidgets/QApplication>

#include <QSplashScreen>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setStyle("Fusion");

	QPixmap pixmap("splash.png");
	QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
	splash.setWindowOpacity(0.8);
	splash.show();
	a.processEvents();

	illustrator w;
	w.show();
	//splash.finish(&w);

	return a.exec();
}
