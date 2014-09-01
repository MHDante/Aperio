/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkMyAdvancedPass.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMyAdvancedPass - Implement a flexible post-processing pass
//
// This pass expects an initialized depth buffer and color buffer.
// Initialized buffers means they have been cleared with farest z-value and
// background color/gradient/transparent color.
// An opaque pass may have been performed right after the initialization.
//
// The delegate is used once.
//
// Its delegate is usually set to a vtkCameraPass or to a post-processing pass.
// 
// This pass requires a OpenGL context that supports texture objects (TO),
// framebuffer objects (FBO) and GLSL. If not, it will emit an error message
// and will render its delegate and return.
//
// .SECTION Implementation
// .SECTION See Also
// vtkRenderPass

#ifndef __vtkMyAdvancedPass_h
#define __vtkMyAdvancedPass_h

#include "vtkImageProcessingPass.h"

class vtkOpenGLRenderWindow;
class vtkDepthPeelingPassLayerList; // Pimpl
class vtkShaderProgram2;
class vtkShader2;
class vtkFrameBufferObject;
class vtkTextureObject;

//class VTK_RENDERING_EXPORT vtkMyAdvancedPass : public vtkImageProcessingPass
class vtkMyAdvancedPass : public vtkImageProcessingPass
{
public:
	static vtkMyAdvancedPass *New();
	vtkTypeMacro(vtkMyAdvancedPass, vtkImageProcessingPass);
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

protected:
	// Description:
	// Default constructor. DelegatePass is set to NULL.
	vtkMyAdvancedPass();

	// Description:
	// Destructor.
	virtual ~vtkMyAdvancedPass();

	// Description:
	// Graphics resources.
	vtkFrameBufferObject *FrameBufferObject;
	vtkTextureObject *Pass1; // render target for the scene
	vtkTextureObject *Gx1; // render target 0 for the first shader
	vtkTextureObject *Gy1; // render target 1 for the first shader
	vtkShaderProgram2 *Program1; // shader to compute Gx1 and Gy1
	vtkShaderProgram2 *Program2; // shader to compute |G| from Gx1 and Gy1

private:
	vtkMyAdvancedPass(const vtkMyAdvancedPass&);  // Not implemented.
	void operator=(const vtkMyAdvancedPass&);  // Not implemented.
};

#endif