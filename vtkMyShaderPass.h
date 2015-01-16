/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkMyShaderPass.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMyShaderPass - Render the opaque/translucent geometry with property key
// filtering.
// .SECTION Description
// vtkMyShaderPass renders the opaque/translucent geometry of all props that have the
// keys contained in vtkRenderState.
//
// This pass expects an initialized depth buffer and color buffer.
// Initialized buffers means they have been cleared with farest z-value and
// background color/gradient/transparent color. (Also expects Pre-pass as delegate to draw on FBO)
//
// .SECTION See Also
// vtkRenderPass

#ifndef __vtkMyShaderPass_h
#define __vtkMyShaderPass_h

#include "vtkMyBasePass.h"

#include <vtkRenderingOpenGLModule.h> // For export macro
#include "vtkRenderPass.h"

class vtkTextureObject;			// pimpl
class vtkFrameBufferObject;

class VTK_EXPORT vtkMyShaderPass : public vtkMyBasePass
{
public:
	static vtkMyShaderPass *New();
	vtkTypeMacro(vtkMyShaderPass, vtkMyBasePass);
	void PrintSelf(ostream& os, vtkIndent indent);

	//BTX
	// Description:
	// Perform rendering according to a render state \p s.
	// \pre s_exists: s!=0
	virtual void Render(const vtkRenderState *s);

	// Set/Get Delegate pass (called while rendering to FBO, then texture sent to new shader for processing)
	//vtkGetObjectMacro(DelegatePass, vtkRenderPass);
	virtual void SetDelegatePass(vtkRenderPass *delegatePass);

	///<summary>My Custom delegate method that renders to FBO/Texture </summary>
	void MyRenderDelegate(const vtkRenderState *s, int width, int height, int newWidth, int newHeight);
	
protected:
	// Description:
	// Default constructor.
	vtkMyShaderPass();

	// Description:
	// Destructor.
	virtual ~vtkMyShaderPass();

	// Description:
	// Graphics resources.
	vector<vtkMyTextureObject> textures;

	vtkSmartPointer<vtkFrameBufferObject> FrameBufferObject;
	vtkSmartPointer<vtkTextureObject> DepthTexture; // Dummy Depth Texture

	vtkSmartPointer<vtkRenderPass> DelegatePass;	// Delegate pass
private:
	vtkMyShaderPass(const vtkMyShaderPass&);  // Not implemented.
	void operator=(const vtkMyShaderPass&);  // Not implemented.
};
#endif
