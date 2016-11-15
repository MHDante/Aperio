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

#include <vtk/vtkAutoInit.h>
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingOpenGL)

#include <vtk/vtkCallbackCommand.h>

#include <vtk/vtkInformation.h>
#include <vtk/vtkSmartPointer.h>
#include <vtk/vtkNew.h>
#include <vtk/vtkPolyData.h>

#include <vtk/vtkColor.h>

#include <vtk/vtkPolyDataMapper.h>
#include <vtk/vtkRenderWindow.h>
#include "vtk/vtkCamera.h"

#include <vtk/vtkOBJReader.h>
#include <vtk/vtkJPEGReader.h>
#include <vtk/vtkPNGReader.h>
#include <vtk/vtkImageData.h>

#include <vtk/vtkFillHolesFilter.h>

#include <vtk/vtkCellArray.h>
#include <vtk/vtkCellData.h>
#include <vtk/vtkPolyLine.h>

#include <vtk/vtkConnectivityFilter.h>
#include <vtk/vtkPolyDataConnectivityFilter.h>
#include <vtk/vtkClipPolyData.h>
#include <vtk/vtkPlane.h>

#include <vtk/vtkVectorOperators.h>
#include <vtk/vtkFloatArray.h>
#include <vtk/vtkMath.h>
#include <vtk/vtkVector.h>
#include <vtk/vtkLine.h>

#include <vtk/vtkObjectFactory.h>
#include <vtk/vtkPointData.h>
#include <vtk/vtkPolyDataCollection.h>

#include <vtk/vtkPolygon.h>
#include <vtk/vtkProperty.h>
#include <vtk/vtkPolyDataNormals.h>

#include <vtk/vtkTransform.h>
#include <vtk/vtkTransformPolyDataFilter.h>
#include <vtk/vtkTriangleFilter.h>
#include <vtk/vtkStripper.h>

#include <vtk/vtkTubeFilter.h>
#include <vtk/vtkRibbonFilter.h>
#include <vtk/vtkRendererCollection.h>

#include <vtk/vtkCellPicker.h>
#include <vtk/vtkCellLocator.h>

#include <vtk/vtkLightCollection.h>

// OpenGL-VTK
#include <vtk/vtkgl.h>
#include <vtk/vtkOpenGLRenderWindow.h>
#include <vtk/vtkOpenGLRenderer.h>
#include <vtk/vtkOpenGLProperty.h>
#include <vtk/vtkOpenGLTexture.h>
#include <vtk/vtkOpenGLPolyDataMapper.h>
#include <vtk/vtkShaderProgram2.h>
#include <vtk/vtkShader2.h>
#include <vtk/vtkShader2Collection.h>

#include <vtk/vtkDepthPeelingPass.h>
#include <vtk/vtkOverlayPass.h>
#include <vtk/vtkRenderPassCollection.h>
#include <vtk/vtkSmoothPolyDataFilter.h>

#include <vtk/vtkTexturedActor2D.h>
#include <vtk/vtkImageActor.h>
#include <vtk/vtkImageMapper.h>
#include <vtk/vtkProperty2D.h>
#include <vtk/vtkImageData.h>

#include <vtk/vtkUniformVariables.h>
#include <vtk/vtkDoubleArray.h>

#include <vtk/vtkTextureMapToSphere.h>
#include <vtk/vtkTextureMapToCylinder.h>
#include <vtk/vtkTextureMapToPlane.h>
#include <vtk/vtkProjectedTexture.h>

#include <vtk/vtkTriangle.h>
#include <vtk/vtkSphereSource.h>
#include <vtk/vtkFeatureEdges.h>
#include <vtk/vtkIntersectionPolyDataFilter.h>
#include <vtk/vtkCenterOfMass.h>
#include <vtk/vtkOctreePointLocator.h>

#include <vtk/vtkFileOutputWindow.h>
#include <vtk/vtkPropPicker.h>

#include <vtk/vtkOBBTree.h>
#include <vtk/QVTKInteractorAdapter.h>

// Assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/DefaultLogger.hpp>

// VTK Debug leaks
//#include <vtk/vtkDebugLeaks.h>
