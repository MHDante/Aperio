// stdafx.h :   Include standard system/library files or includes that are used frequently, 
//				but changed infrequently.

// ------- Can also go to project settings > c/c++ > advanced & have this file "Force included" in all files (or done manually)
#pragma once

//------------  Must include Carve CSG before Windows.h (since it modifies standard types) ---------------------
#include <carve/csg.hpp>
#include <carve/csg_triangulator.hpp>

//------------- Windows includes ---------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <memory>
#include <sstream>
#include <iostream>

// ----- Include GLEW -------
#include <GL/glew.h>

//------------ QT Includes ---------------------------------
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QFileDialog> 
#include <QStatusBar>
#include <QLabel>
#include <QResizeEvent>

#include <QTimer>
#include <QDebug>
#include <QString>


//------------ VTK Includes -----------------------


//#define vtkRenderingCore_AUTOINIT 2(vtkInteractionStyle,vtkRenderingOpenGL)
//#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)

#include <vtkAutoInit.h> 
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingOpenGL)

#include <vtkInformation.h>
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkPolyData.h>

#include <vtkColor.h>

#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include "vtkCamera.h"

#include <vtkOBJReader.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkImageData.h>

#include <vtkCellArray.h>
#include <vtkCellData.h>

#include <vtkFloatArray.h> 
#include <vtkMath.h>
#include <vtkVector.h>

#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPolyDataCollection.h> 

#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkPolyDataNormals.h>

#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>

#include <vtkRendererCollection.h>

#include <vtkCellPicker.h>
#include <vtkCellLocator.h>

#include <vtkLightCollection.h>

// OpenGL-VTK
#include <vtkgl.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGLProperty.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkShaderProgram2.h>
#include <vtkShader2.h>
#include <vtkShader2Collection.h>

#include <vtkDepthPeelingPass.h>
#include <vtkOverlayPass.h>
#include <vtkRenderPassCollection.h>
#include <vtkSmoothPolyDataFilter.h>

#include <vtkTexturedActor2D.h>
#include <vtkImageActor.h>
#include <vtkImageMapper.h>
#include <vtkProperty2D.h>
#include <vtkImageData.h>

#include <vtkUniformVariables.h>
#include <vtkDoubleArray.h>
