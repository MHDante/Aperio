/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkMyProcessingPass.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMyProcessingPass - Implement a basic
// post-processing pass consisting of a fragment and vertex shader
//
// .SECTION See Also
// vtkRenderPass

#ifndef __vtkMyProcessingPass_h
#define __vtkMyProcessingPass_h

#include <vtkImageProcessingPass.h>

class vtkOpenGLRenderWindow;
class vtkDepthPeelingPassLayerList; // Pimpl
class vtkShaderProgram2;
class vtkShader2;
class vtkFrameBufferObject;
class vtkTextureObject;

class vtkMyProcessingPass : public vtkImageProcessingPass
{
public:
	static vtkMyProcessingPass *New();
	vtkTypeMacro(vtkMyProcessingPass, vtkImageProcessingPass);
	void PrintSelf(ostream& os, vtkIndent indent);

	///<summary> Set the shader file for this pass and whether or not it is a fragment shader/vertex (Must be called immediately)</summary>
	void setShaderFile(std::string filename, bool frag);

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

protected:
	// Description:
	// Default constructor. DelegatePass is set to NULL.
	vtkMyProcessingPass();

	// Description:
	// Destructor.
	virtual ~vtkMyProcessingPass();

	///<summary>My Custom delegate method that enables alpha blending in render to target </summary>
	void MyRenderDelegate(const vtkRenderState *s, int width, int height, int newWidth, int newHeight,
		vtkFrameBufferObject *fbo, vtkTextureObject *target, vtkTextureObject *targetDepth);

	// Description:
	// Graphics resources.
	vtkFrameBufferObject *FrameBufferObject;
	vtkTextureObject *Pass1; // render target for the scene
	vtkShaderProgram2 *Program1; // shader to compute final result

	vtkTextureObject *Pass1Depth;	// CUSTOM - Depth texture object

	std::stringstream bufferV;	// Vertex  shader stringstream
	std::stringstream bufferF;	// Fragment shader stringstream

private:
	vtkMyProcessingPass(const vtkMyProcessingPass&);  // Not implemented.
	void operator=(const vtkMyProcessingPass&);  // Not implemented.
};
#endif