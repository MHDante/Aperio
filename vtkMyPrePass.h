/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkMyPrePass.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMyPrePass - Implement the basic pre-render pass (Very simple. FBO setup is in vtkMyShaderPass)
// .SECTION Description
//
// .SECTION See Also
// vtkRenderPass

#ifndef __vtkMyPrePass_h
#define __vtkMyPrePass_h

#include "vtkMyBasePass.h"
#include <vtk/vtkRenderingOpenGLModule.h> // For export macro
#include <vtk/vtkRenderPass.h>

class vtkOpenGLRenderWindow;
class vtkDefaultPassLayerList; // Pimpl

class VTK_EXPORT vtkMyPrePass : public vtkMyBasePass
{
public:
	static vtkMyPrePass *New();
	vtkTypeMacro(vtkMyPrePass, vtkMyBasePass);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Override
	virtual int RenderProp(vtkProp *p, const vtkRenderState *s, bool translucent) override;

protected:
	// Description:
	// Default constructor.
	vtkMyPrePass();

	// Description:
	// Destructor.
	virtual ~vtkMyPrePass();

private:
	vtkMyPrePass(const vtkMyPrePass&);  // Not implemented.
	void operator=(const vtkMyPrePass&);  // Not implemented.
};

#endif