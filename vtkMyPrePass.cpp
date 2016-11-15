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

#include <vtk/vtkObjectFactory.h>
#include <cassert>
#include <vtk/vtkRenderState.h>
#include <vtk/vtkProp.h>
#include <vtk/vtkRenderer.h>

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
	bool isElem = false;
	bool isSelectedMesh = false;

	auto toolTip = a->toolTip.lock();

	// tool tip element?
	if (a->toolTipOn && toolTip && toolTip->toolType != KNIFE && toolTip->actor.GetPointer() == vtkActor::SafeDownCast(p))
	{
		isElem = true;
	}
	else
	{
		auto elem = a->getElemByActorRaw(vtkActor::SafeDownCast(p)).lock();
		if (elem != nullptr)
		{
			isElem = true;
		}
		else
		{
			auto mesh = a->getMeshByActorRaw(vtkActor::SafeDownCast(p)).lock();
			if (mesh != nullptr && mesh->selected)
				isSelectedMesh = true;
		}
	}
	
	if	( isSelectedMesh || isElem )
	{
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
	}
	return rendered;
}