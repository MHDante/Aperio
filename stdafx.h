// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

// ------- Can also go to project settings > c/c++ > advanced  have this file "Force included" in all files (or done manually)
#pragma once

//------------  Must include Carve CSG before Windows.h (since it modifies standard types) ---------------------
#include <carve/csg.hpp>
#include <carve/csg_triangulator.hpp>

//------------- Windows includes ---------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//------------ QT Includes ---------------------------------

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QFileDialog> 
#include <QtGui/QStatusBar>
#include <QtGui/QLabel>
#include <QtGui/QResizeEvent>

#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtCore/QString>


//------------ VTK Includes -----------------------

#define vtkRenderingCore_AUTOINIT 2(vtkInteractionStyle,vtkRenderingOpenGL)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "vtkCamera.h"

#include <vtkOBJReader.h>
#include <vtkJPEGReader.h>
//#include <vtkShaderProgram.h>

#include <vtkCellArray.h>
#include <vtkCellData.h>

#include <vtkFloatArray.h> 
#include <vtkMath.h>

#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPolyDataCollection.h> 

#include <vtkPolygon.h>
#include <vtkProperty.h>

#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
