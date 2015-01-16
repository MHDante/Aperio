/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkMyImageProcessingPass.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMyImageProcessingPass - Implement a basic
// post-processing pass consisting of a fragment and vertex shader
//
// .SECTION See Also
// vtkRenderPass

#ifndef __vtkMyImageProcessingPass_h
#define __vtkMyImageProcessingPass_h

#include "vtkMyBasePass.h"

class vtkOpenGLRenderWindow;
class vtkDepthPeelingPassLayerList; // Pimpl
class vtkShaderProgram2;
class vtkShader2;
class vtkFrameBufferObject;
class vtkTextureObject;

class VTK_EXPORT vtkMyImageProcessingPass : public vtkMyBasePass
{
public:
	static vtkMyImageProcessingPass *New();
	vtkTypeMacro(vtkMyImageProcessingPass, vtkMyBasePass);
	void PrintSelf(ostream& os, vtkIndent indent);

	//BTX
	// Description:
	// Perform rendering according to a render state \p s.
	// \pre s_exists: s!=0
	virtual void Render(const vtkRenderState *s);
	//ETX

	// Description:
	// Release graphics resources and ask components to release their own
	// resources.
	// \pre w_exists: w!=0
	void ReleaseGraphicsResources(vtkWindow *w);

	// Set/Get Delegate pass (called when rendering to FBO, then the texture sent to new shader for processing)
	//vtkGetObjectMacro(DelegatePass, vtkRenderPass);
	virtual void SetDelegatePass(vtkRenderPass *delegatePass);

	///<summary>My Custom delegate method that enables alpha blending in render to target </summary>
	void MyRenderDelegate(const vtkRenderState *s, int width, int height, int newWidth, int newHeight);

protected:
	// Description:
	// Default constructor. DelegatePass is set to NULL.
	vtkMyImageProcessingPass();

	// Description:
	// Destructor.
	virtual ~vtkMyImageProcessingPass();

	vector<vtkMyTextureObject> textures;

	// Description:
	// Graphics resources.
	vtkSmartPointer<vtkFrameBufferObject> FrameBufferObject;

	vtkSmartPointer<vtkTextureObject> DepthTexture;

	vtkSmartPointer<vtkRenderPass> DelegatePass;	// Delegate pass

private:
	vtkMyImageProcessingPass(const vtkMyImageProcessingPass&);  // Not implemented.
	void operator=(const vtkMyImageProcessingPass&);  // Not implemented.
};
#endif