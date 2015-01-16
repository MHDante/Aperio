// stdafx.h :   Include standard system/library files or includes that are used frequently,
//				but changed infrequently.

// ------- Can also go to project settings > c/c++ > advanced & have this file "Force included" in all files (or done manually)
#pragma once

//------------  Must include Carve CSG before Windows.h (since it modifies standard types) ---------------------
#include <carve/csg.hpp>
#include <carve/csg_triangulator.hpp>

//------------- Windows includes ---------------------------------------
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <memory>
#include <sstream>
#include <iostream>

using std::cout;
using std::string;
using std::stringstream;

using std::vector;
using std::map;
using std::unique_ptr;
using std::shared_ptr;
using std::weak_ptr;

using std::make_shared;

// ----- Include GLEW -------
#include <GL/glew.h>

//------------ QT Includes ---------------------------------
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QTreeView>
#include <QStatusBar>
#include <QLabel>
#include <QResizeEvent>
#include <QProgressDialog>

#include <QTimer>
#include <QDebug>
#include <QString>

//------------ VTK Includes -----------------------

//#define vtkRenderingCore_AUTOINIT 2(vtkInteractionStyle,vtkRenderingOpenGL)
//#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingOpenGL)

#include <vtkCallbackCommand.h>

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

#include <vtkFillHolesFilter.h>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPolyLine.h>

#include <vtkConnectivityFilter.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkClipPolyData.h>
#include <vtkPlane.h>

#include <vtkVectorOperators.h>
#include <vtkFloatArray.h>
#include <vtkMath.h>
#include <vtkVector.h>
#include <vtkLine.h>

#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPolyDataCollection.h>

#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkPolyDataNormals.h>

#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkStripper.h>

#include <vtkTubeFilter.h>
#include <vtkRibbonFilter.h>
#include <vtkRendererCollection.h>

#include <vtkCellPicker.h>
#include <vtkCellLocator.h>

#include <vtkLightCollection.h>

// OpenGL-VTK
#include <vtkgl.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGLProperty.h>
#include <vtkOpenGLTexture.h>
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

#include <vtkTextureMapToSphere.h>
#include <vtkTextureMapToCylinder.h>
#include <vtkTextureMapToPlane.h>
#include <vtkProjectedTexture.h>

#include <vtkTriangle.h>
#include <vtkSphereSource.h>
#include <vtkFeatureEdges.h>
#include <vtkIntersectionPolyDataFilter.h>
#include <vtkCenterOfMass.h>
#include <vtkOctreePointLocator.h>

#include <vtkFileOutputWindow.h>
#include <vtkPropPicker.h>

#include <vtkOBBTree.h>
#include <QVTKInteractorAdapter.h>

// Assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/DefaultLogger.hpp>

// VTK Debug leaks
//#include <vtkDebugLeaks.h>
