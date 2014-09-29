#include "stdafx.h"

/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkMyPrePass.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMyPrePass.h"

#include <vtkObjectFactory.h>
#include <cassert>
#include <vtkRenderState.h>
#include <vtkProp.h>
#include <vtkRenderer.h>

#include "aperio.h"

vtkStandardNewMacro(vtkMyPrePass);

// ----------------------------------------------------------------------------
vtkMyPrePass::vtkMyPrePass()
{
}
// ----------------------------------------------------------------------------
vtkMyPrePass::~vtkMyPrePass()
{
}

// ----------------------------------------------------------------------------
void vtkMyPrePass::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
// --------------------------------------------------------------------------------
int vtkMyPrePass::RenderProp(vtkProp *p, const vtkRenderState *s, bool translucent)
{
	int rendered = 0;

	//auto mesh = a->getMeshByActorRaw(vtkActor::SafeDownCast(p));
	//if (!mesh->selected)
//		return 0;

	if (translucent)
	{
		static_cast<vtkMyOpenGLProperty *>(vtkOpenGLProperty::SafeDownCast(vtkActor::SafeDownCast(p)->GetProperty()))->show_back();
		rendered = p->RenderFilteredTranslucentPolygonalGeometry(s->GetRenderer(), s->GetRequiredKeys());

		static_cast<vtkMyOpenGLProperty *>(vtkOpenGLProperty::SafeDownCast(vtkActor::SafeDownCast(p)->GetProperty()))->show_front();
		rendered = p->RenderFilteredTranslucentPolygonalGeometry(s->GetRenderer(), s->GetRequiredKeys());
	}
	else
	{
		static_cast<vtkMyOpenGLProperty *>(vtkOpenGLProperty::SafeDownCast(vtkActor::SafeDownCast(p)->GetProperty()))->show_back();
		rendered = p->RenderFilteredOpaqueGeometry(s->GetRenderer(), s->GetRequiredKeys());

		static_cast<vtkMyOpenGLProperty *>(vtkOpenGLProperty::SafeDownCast(vtkActor::SafeDownCast(p)->GetProperty()))->show_front();
		rendered = p->RenderFilteredOpaqueGeometry(s->GetRenderer(), s->GetRequiredKeys());
	}
	return rendered;
}
