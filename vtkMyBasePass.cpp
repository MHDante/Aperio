#include "stdafx.h"

/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkMyBasePass.cxx


Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMyBasePass.h"

#include <vtkObjectFactory.h>
#include <cassert>
#include <vtkRenderState.h>
#include <vtkProp.h>
#include <vtkRenderer.h>

#include "aperio.h"

vtkStandardNewMacro(vtkMyBasePass);

// Set up Property Keys (globally accessible from this class)
vtkInformationKeyMacro(vtkMyBasePass, OUTLINEKEY, Integer);

// ----------------------------------------------------------------------------
vtkMyBasePass::vtkMyBasePass()
{
	this->uniforms = vtkSmartPointer<vtkUniformVariables>::New();
}

// ----------------------------------------------------------------------------
vtkMyBasePass::~vtkMyBasePass()
{
}
// ----------------------------------------------------------------------------
void vtkMyBasePass::initialize(aperio *a)
{
	this->a = a;
}
// ----------------------------------------------------------------------------
void vtkMyBasePass::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}

// ----------------------------------------------------------------------------
// Description:
// Perform rendering according to a render state \p s.
// \pre s_exists: s!=0
void vtkMyBasePass::Render(const vtkRenderState *s)
{
	assert("pre: s_exists" && s != 0);

	this->NumberOfRenderedProps = 0;
	this->RenderGeometry(s, false);	// Opaque pass first
	this->RenderGeometry(s, true);	// Transparent pass
}

// ----------------------------------------------------------------------------
// Description:
// Opaque pass without key checking.
// \pre s_exists: s!=0
void vtkMyBasePass::RenderGeometry(const vtkRenderState *s, bool translucent)
{
	assert("pre: s_exists" && s != 0);

	int c = s->GetPropArrayCount();
	int i = 0;
	
	// Need this line!! (Enables alpha blending & depth testing)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	setGlobalUniforms();

	auto renderProp = [=](bool onlyelem, int i)
	{
		vtkProp *p = s->GetPropArray()[i];
		if (p->HasKeys(s->GetRequiredKeys()))
		{
			auto it = a->getElemByActorRaw(vtkActor::SafeDownCast(p));

			if (onlyelem)
			{
				if (it.lock())	// it is an element, continue
					;
				else			// Not an element, don't render
					return;
			}
			else	// Everything else
			{
				if (it.lock())	// it is an element, don't render
					return;
				else			// Not an element, go ahead
					;
			}
				
			// Use shaders to draw into FBO colour attachments

			if (this->Program1 == nullptr)
			{
				this->Program1 = vtkSmartPointer<vtkShaderProgram2>::New();
				this->Program1->SetContext(s->GetRenderer()->GetRenderWindow());

				if (bufferV.str().size() > 0)
				{
					vtkSmartPointer<vtkShader2> shader = vtkSmartPointer<vtkShader2>::New();
					shader->SetType(VTK_SHADER_TYPE_VERTEX);
					shader->SetSourceCode(bufferV.str().c_str());
					shader->SetContext(this->Program1->GetContext());

					this->Program1->GetShaders()->AddItem(shader);
				}

				if (bufferF.str().size() > 0)
				{
					vtkSmartPointer<vtkShader2> shader2 = vtkSmartPointer<vtkShader2>::New();
					shader2->SetType(VTK_SHADER_TYPE_FRAGMENT);
					shader2->SetSourceCode(bufferF.str().c_str());
					shader2->SetContext(this->Program1->GetContext());

					this->Program1->GetShaders()->AddItem(shader2);
				}
			}
			this->Program1->Build();

			if (this->Program1->GetLastBuildStatus() != VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
			{
				vtkErrorMacro("Couldn't build the shader program. At this point , it can be an error in a shader or a driver bug.");

				// restore some state.
				vtkgl::ActiveTexture(vtkgl::TEXTURE0);
				return;
			}

			setPropUniforms(p);

			this->Program1->SetUniformVariables(uniforms);
			this->Program1->Use();
			if (!this->Program1->IsValid())
				vtkErrorMacro(<< this->Program1->GetLastValidateLog());

			int rendered = RenderProp(p, s, translucent);

			this->Program1->Restore();

			this->NumberOfRenderedProps += rendered;
		}
	};

	while (i < c)
	{
		renderProp(true, i);
		++i;
	}
	i = 0;
	while (i < c)
	{
		renderProp(false, i);
		++i;
	}

	// Need this line!! (Enables alpha blending & depth testing)
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
}
//------------------------------------------------------------------
int vtkMyBasePass::RenderProp(vtkProp *p, const vtkRenderState *s, bool translucent)
{
	int rendered = 0;

	if (translucent)
	{
		glDepthMask(GL_FALSE);	// Disable/enable writing to depth buffer for translucent objects

		static_cast<vtkMyOpenGLProperty *>(vtkOpenGLProperty::SafeDownCast(vtkActor::SafeDownCast(p)->GetProperty()))->show_back();
		rendered = p->RenderFilteredTranslucentPolygonalGeometry(s->GetRenderer(), s->GetRequiredKeys());

		static_cast<vtkMyOpenGLProperty *>(vtkOpenGLProperty::SafeDownCast(vtkActor::SafeDownCast(p)->GetProperty()))->show_front();
		rendered = p->RenderFilteredTranslucentPolygonalGeometry(s->GetRenderer(), s->GetRequiredKeys());

		glDepthMask(GL_TRUE);	// Disable/enable writing to depth buffer for translucent objects
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
//--------------------------------------------------------------------------------------------------
void vtkMyBasePass::setShaderFile(string filename, bool frag)
{
	ifstream file1(filename);

	//bufferF.str("");
	//bufferV.str("");

	if (frag)
		bufferF << file1.rdbuf();		
	else
		bufferV << file1.rdbuf();
}
//-----------------------------------------------------------------------------
void vtkMyBasePass::setGlobalUniforms()
{
	float myalpha = 0;
	uniforms->SetUniformf("myalpha", 1, &myalpha);

	// Set global uniform variables
	float mousepos[3] = { a->mouse[0], a->mouse[1], a->mouse[2] };
	int source = 0;		// potential source texture
	int sourceBump = 1;		// potential source texture

	uniforms->SetUniformit("wiggle", 1, &a->wiggle);
	uniforms->SetUniformf("mouse", 3, mousepos);
	uniforms->SetUniformf("mouseSize", 1, &a->mouseSize);
	uniforms->SetUniformf("brushSize", 1, &a->brushSize);
	uniforms->SetUniformit("previewer", 1, &a->previewer);
	uniforms->SetUniformit("cap", 1, &a->cap);
	uniforms->SetUniformi("shadingnum", 1, &a->shadingnum);
	uniforms->SetUniformi("source", 1, &source);
	uniforms->SetUniformi("sourceBump", 1, &sourceBump);
	uniforms->SetUniformi("matcap", 1, &a->matcap.unit);
	uniforms->SetUniformit("toolTipOn", 1, &a->toolTipOn);

	uniforms->SetUniformf("selectedColor", 3, a->selectedColor);	// Access any time previous color

	int elemssize = a->myelems.size();

	auto toolTip = a->toolTip.lock();

	if (toolTip)
	{
		// Set to last element's properties (because that is the active one when planted/picked up)
		//auto elem = toolTip;
		//setElemUniforms(elem);

		/*if (a->toolTipOn && !(toolTip->toolType == KNIFE && a->myelems.size() > 0))
		{
			elemssize = 1;
			uniforms->SetUniformf("pos1", 3, toolTip->p1.point.GetData());
			uniforms->SetUniformf("pos2", 3, toolTip->p2.point.GetData());
			uniforms->SetUniformf("norm1", 3, toolTip->p1.normal.GetData());
			uniforms->SetUniformf("norm2", 3, toolTip->p1.normal.GetData());
			uniforms->SetUniformf("scale", 3, toolTip->scale.GetData());
			uniforms->SetUniformf("right", 3, toolTip->right.GetData());
			uniforms->SetUniformf("up", 3, toolTip->up.GetData());
			uniforms->SetUniformf("forward", 3, toolTip->forward.GetData());
			uniforms->SetUniformf("angle", 1, &toolTip->spinAngle);
			uniforms->SetUniformit("ribbons", 1, &toolTip->ribbons);
			uniforms->SetUniformit("frontRibbons", 1, &toolTip->frontRibbons);
			uniforms->SetUniformf("ribbonWidth", 1, &toolTip->ribbonWidth);
			uniforms->SetUniformi("ribbonFrequency", 1, &toolTip->ribbonFrequency);
		}
		else if (a->myelems.size() > 0)
		{
			auto elem = a->myelems.at(a->myelems.size() - 1);

			uniforms->SetUniformf("pos1", 3, elem->p1.point.GetData());
			uniforms->SetUniformf("pos2", 3, elem->p2.point.GetData());

			uniforms->SetUniformf("norm1", 3, elem->p1.normal.GetData());
			uniforms->SetUniformf("norm2", 3, elem->p2.normal.GetData());

			uniforms->SetUniformf("scale", 3, elem->scale.GetData());
			uniforms->SetUniformf("right", 3, elem->right.GetData());
			uniforms->SetUniformf("up", 3, elem->up.GetData());
			uniforms->SetUniformf("forward", 3, elem->forward.GetData());
			uniforms->SetUniformf("angle", 1, &toolTip->spinAngle);
		}*/

		//bool toroid = a->getUI().chkToroid->isChecked();
	}

	uniforms->SetUniformi("elemssize", 1, &elemssize);

	//float phi = a->getUI().phiSlider->value() / a->roundnessScale;
	//float theta = a->getUI().thetaSlider->value() / a->roundnessScale;
	//float thickness = a->getUI().thicknessSlider->value() / a->thicknessScale;

	//uniforms->SetUniformf("phi", 1, &phi);
	//uniforms->SetUniformf("theta", 1, &theta);
	//uniforms->SetUniformf("thickness", 1, &thickness);

	uniforms->SetUniformit("difftrans", 1, &a->difftrans);

	uniforms->SetUniformi("shininess", 1, &a->shininess);
	uniforms->SetUniformf("darkness", 1, &a->darkness);

	uniforms->SetUniformf("time", 1, &a->wavetime);
}
//-----------------------------------------------------------------------------
void vtkMyBasePass::setPropUniforms(vtkProp *p)
{
	// Find actor inside CustomMesh vector (using lambda to compare CustomMesh's actor pointer with vtkActor's pointer)
	auto it = a->getMeshByActorRaw(vtkActor::SafeDownCast(p)).lock();

	// Default uniforms
	bool outline = false;
	uniforms->SetUniformit("outline", 1, &outline);

	bool iselem = false;
	uniforms->SetUniformit("iselem", 1, &iselem);			// False default (not an element)

	bool selected = false;
	uniforms->SetUniformit("selected", 1, &selected);

	bool active_elem = false;
	uniforms->SetUniformit("active_elem", 1, &active_elem);			// False default (not an element)

	auto toolTip = a->toolTip.lock();

	// Mesh
	if (it != nullptr)
	{
		// Found the CustomMesh object mapped to this actor (actor is a subclass of prop)
		uniforms->SetUniformit("selected", 1, &it->selected);
	}
	/*else if (a->toolTipOn && toolTip && toolTip->actor.GetPointer() == vtkActor::SafeDownCast(p))
	{
		iselem = true;
		uniforms->SetUniformit("iselem", 1, &iselem);
	}*/
	else
	{
		// Do another find here to see if mesh is part of widget elements
		auto it2 = a->getElemByActorRaw(vtkActor::SafeDownCast(p)).lock();

		if (it2 != nullptr)
		{
			// Actor belongs to our Elements array (myelems)
			iselem = true;
			uniforms->SetUniformit("iselem", 1, &iselem);

			if (toolTip == it2)	// The Active element
			{
				bool active_elem = true;
				uniforms->SetUniformit("active_elem", 1, &active_elem);			// False default (not an element)
			}

			// It's an element! 
			auto elem = it2;
			setElemUniforms(elem);
		}
		else
		{
			// Else,
			// Not found the CustomMesh object, must be extra objects (The outliner, UI widgets, etc)

			if (p->GetPropertyKeys() && p->GetPropertyKeys()->Has(vtkMyBasePass::OUTLINEKEY()))
			{
				outline = true;
				uniforms->SetUniformit("outline", 1, &outline);
				//glPointSize(100);
				//glEnable(GL_LINE_SMOOTH);

				//glDisable(GL_DEPTH_TEST);
			}
		}
	}
}
//--------------------------------------------------------------------------
void vtkMyBasePass::setElemUniforms(weak_ptr<MyElem> elem_wk)
{
	auto elem = elem_wk.lock();

	uniforms->SetUniformf("pos1", 3, elem->p1.point.GetData());
	uniforms->SetUniformf("pos2", 3, elem->p2.point.GetData());
	uniforms->SetUniformf("norm1", 3, elem->p1.normal.GetData());
	uniforms->SetUniformf("norm2", 3, elem->p1.normal.GetData());
	uniforms->SetUniformf("scale", 3, elem->scale.GetData());
	uniforms->SetUniformf("right", 3, elem->right.GetData());
	uniforms->SetUniformf("up", 3, elem->up.GetData());
	uniforms->SetUniformf("forward", 3, elem->forward.GetData());
	uniforms->SetUniformf("angle", 1, &elem->spinAngle);
	uniforms->SetUniformit("ribbons", 1, &elem->ribbons);
	uniforms->SetUniformit("frontRibbons", 1, &elem->frontRibbons);
	uniforms->SetUniformf("ribbonWidth", 1, &elem->ribbonWidth);
	uniforms->SetUniformi("ribbonFrequency", 1, &elem->ribbonFrequency);
	uniforms->SetUniformf("ribbonTilt", 1, &elem->ribbonTilt);
	uniforms->SetUniformf("tilt", 1, &elem->tilt);

	bool toroid = (elem->toolType == RING);
	bool cutter = ((elem->toolType == CUTTER) || (elem->toolType == KNIFE));

	uniforms->SetUniformit("toroid", 1, &toroid);
	uniforms->SetUniformit("cutter", 1, &cutter);

	float phi = elem->source->GetPhiRoundness();//a->getUI().phiSlider->value() / a->roundnessScale;
	float theta = elem->source->GetThetaRoundness(); //a->getUI().thetaSlider->value() / a->roundnessScale;
	float thickness = elem->source->GetThickness(); //a->getUI().thicknessSlider->value() / a->thicknessScale;
	float taper = elem->source->GetTaper();

	uniforms->SetUniformf("phi", 1, &phi);
	uniforms->SetUniformf("theta", 1, &theta);
	uniforms->SetUniformf("thickness", 1, &thickness);
	uniforms->SetUniformf("taper", 1, &taper);
}
