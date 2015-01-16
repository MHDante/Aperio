/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkMyBasePass.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMyBasePass - ProcessingPass, ShaderPass and PrePass all inherit from this
// .SECTION Description
//
// .SECTION See Also
// vtkRenderPass

#ifndef __vtkMyBasePass_h
#define __vtkMyBasePass_h

#include "vtkRenderingOpenGLModule.h" // For export macro
#include "vtkRenderPass.h"

#include "vtkInformationIntegerKey.h"

class vtkOpenGLRenderWindow;
class vtkDefaultPassLayerList; // Pimpl
class vtkProp;
class aperio;
class MyElem;

class vtkTextureObject;

// Override vtkOpenGLProperty to show front/back faces
class vtkMyOpenGLProperty : public vtkOpenGLProperty
{
public:
	void show_front()
	{
		this->BackfaceCulling = true;
		this->FrontfaceCulling = false;
	}
	void show_back()
	{
		this->BackfaceCulling = false;
		this->FrontfaceCulling = true;
	}
	void show_all()
	{
		this->BackfaceCulling = false;
		this->FrontfaceCulling = false;
	}
};

// --- Hold texture objects (vtk object, name and OpenGL id)
struct vtkMyTextureObject
{
	vtkSmartPointer<vtkTextureObject> texture;
	string name;
	int id;
};

// ---- Base Shader Pass
class VTK_EXPORT vtkMyBasePass : public vtkRenderPass
{
public:
	static vtkMyBasePass *New();

	vtkTypeMacro(vtkMyBasePass, vtkRenderPass);
	void PrintSelf(ostream& os, vtkIndent indent);

	// --- Custom Property Keys
	static vtkInformationIntegerKey *OUTLINEKEY();

	//BTX
	// Description:
	// Perform rendering according to a render state \p s.
	// Call RenderOpaqueGeometry(), RenderTranslucentPolygonalGeometry(),
	// RenderVolumetricGeometry(), RenderOverlay()
	// \pre s_exists: s!=0
	virtual void Render(const vtkRenderState *s);
	//ETX

	// Virtual methods to override in subclasses!!!
	virtual void setGlobalUniforms();
	virtual void setPropUniforms(vtkProp *p);
	virtual int RenderProp(vtkProp *p, const vtkRenderState *s, bool translucent);

	// Custom methods
	void initialize(aperio *a);
	void setShaderFile(string filename, bool frag);	// Must have a shader file set!

	void setElemUniforms(weak_ptr<MyElem> elem_wk);

	vtkSmartPointer<vtkShaderProgram2> Program1;
	vtkSmartPointer<vtkUniformVariables> uniforms;
	stringstream bufferV;	// Vertex  shader stringstream
	stringstream bufferF;	// Fragment shader stringstream

	aperio *a;

protected:
	// Description:
	// Default constructor.
	vtkMyBasePass();

	// Description:
	// Destructor.
	virtual ~vtkMyBasePass();

	// Description:
	// Opaque pass with key checking (transparent/opaque pass)
	// \pre s_exists: s!=0
	virtual void RenderGeometry(const vtkRenderState *s, bool translucent);

private:
	vtkMyBasePass(const vtkMyBasePass&);  // Not implemented.
	void operator=(const vtkMyBasePass&);  // Not implemented.
};

#endif