// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

// ------- Can also go to project settings > c/c++ > advanced  have this file "Force included" in all files (or done manually)
#pragma once

//------------  Must include Carve CSG before Windows.h (since it modifies standard types) ---------------------
#include <carve/csg.hpp>
#include <carve/csg_triangulator.hpp>
#include <carve/input.hpp>

 
//------------- Windows includes ---------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h> 

// Include QT classes
#include <QtGui>
#include <QtGui/QApplication> 

// Include OSG classes 
#include "osgViewer/Viewer"
#include <osgViewer/CompositeViewer>
#include <osgQt/GraphicsWindowQt>
#include <osgviewer/ViewerEventHandlers>

#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgUtil/SmoothingVisitor>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/Fog>
#include <osg/ShapeDrawable>
#include <osg/Depth>
#include <osg/PolygonMode>
#include <osg/Shader>
#include <osg/ShadeModel>
#include <osg/CullFace>
#include <osgUtil/TriStripVisitor>
#include <osgUtil/Tessellator>
#include <osg/Shader>
#include <osg/Program>




