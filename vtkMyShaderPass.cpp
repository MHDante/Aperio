#include "stdafx.h"		// Precompiled header files

///*=========================================================================
//
//  Program:   Visualization Toolkit
//  Module:    vtkMyShaderPass.cxx
//
//  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
//  All rights reserved.
//  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
//
//     This software is distributed WITHOUT ANY WARRANTY; without even
//     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//     PURPOSE.  See the above copyright notice for more information.
//
//=========================================================================*/
//
#include <vtkObjectFactory.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkRenderState.h>
#include <vtkObject.h>

#include <vtkUniformVariables.h>
#include <assert.h>

// Custom
#include "aperio.h"
#include "vtkMyShaderPass.h"

// VTK
#include <vtkShader2Collection.h>

vtkStandardNewMacro(vtkMyShaderPass);

// Set up property key accessors using macro
//vtkInformationKeyMacro(vtkMyShaderPass,UNIFORMS, ObjectBase);

// ----------------------------------------------------------------------------
vtkMyShaderPass::vtkMyShaderPass()
{
	this->uniforms = vtkSmartPointer<vtkUniformVariables>::New();

	time = 0;
}

// ----------------------------------------------------------------------------
vtkMyShaderPass::~vtkMyShaderPass()
{
}

//// ----------------------------------------------------------------------------
void vtkMyShaderPass::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}

// ----------------------------------------------------------------------------
// Description:
// Perform rendering according to a render state \p s.
// \pre s_exists: s!=0
void vtkMyShaderPass::Render(const vtkRenderState *s)
{
	assert("pre: s_exists" && s != 0);

	this->RenderGeometry(s);
}

// ----------------------------------------------------------------------------
// Description:
// Opaque/Translucent pass with key checking.
// \pre s_exists: s!=0
void vtkMyShaderPass::RenderGeometry(const vtkRenderState *s)
{
	assert("pre: s_exists" && s != 0);

	this->NumberOfRenderedProps = 0;

	int c = s->GetPropArrayCount();
	int i = 0;

	// Set global uniforms
	float mousepos[3];
	mousepos[0] = a->mouse[0];
	mousepos[1] = a->mouse[1];
	mousepos[2] = a->mouse[2];

	int source = 0;	// potential source texture

	uniforms->SetUniformit("wiggle", 1, &a->wiggle);
	uniforms->SetUniformf("mouse", 3, mousepos);
	uniforms->SetUniformf("mouseSize", 1, &a->mouseSize);
	uniforms->SetUniformit("peerInside", 1, &a->peerInside);
	uniforms->SetUniformf("myexp", 1, &a->myexp);
	uniforms->SetUniformi("shadingnum", 1, &a->shadingnum);
	uniforms->SetUniformi("source", 1, &source);

	uniforms->SetUniformf("pos1", 3, a->pos1);
	uniforms->SetUniformf("pos2", 3, a->pos2);
	uniforms->SetUniformit("difftrans", 1, &a->difftrans);

	uniforms->SetUniformi("shininess", 1, &a->shininess);
	uniforms->SetUniformf("darkness", 1, &a->darkness);

	uniforms->SetUniformf("time", 1, &time);

	time += 0.01;
	if (time > 500)
		time = 0;
	//time = std::clock() / 1250.0;

	while (i < c)
	{
		vtkProp *p = s->GetPropArray()[i];

		// Find actor inside CustomMesh vector (using lambda to compare CustomMesh's actor pointer with vtkActor's pointer)
		auto it = a->getMeshByActorRaw(vtkActor::SafeDownCast(p));

		if (it != a->meshes.end())
		{
			// Found the CustomMesh object mapped to this actor (actor is a subclass of prop)
			uniforms->SetUniformit("selected", 1, &it->selected);

			bool iselem = false;
			uniforms->SetUniformit("iselem", 1, &iselem);
		}
		else
		{
			// Do another find here to see if mesh is part of widget elements
			auto it2 = a->getElemByActorRaw(vtkActor::SafeDownCast(p));

			if (it2 != a->myelems.end())
			{
				// Actor belongs to our Elements array (myelems)
				bool iselem = true;
				uniforms->SetUniformit("iselem", 1, &iselem);
			}
			else
			{
				// Else,
				// Not found the CustomMesh object, must be extra objects
			}
		}

		if (p->HasKeys(s->GetRequiredKeys()))
		{
			int rendered;
			
			//if (vtkActor::SafeDownCast(p) != nullptr)
				vtkOpenGLProperty::SafeDownCast(vtkActor::SafeDownCast(p)->GetProperty())->GetPropProgram()->SetUniformVariables(uniforms);

			//a->shaderProgram->SetUniformVariables(uniforms);
			//vtkOpenGLRenderer::SafeDownCast(s->GetRenderer())->SetShaderProgram(a->shaderProgram); // Dangerous, constantly allocs
			//a->shaderProgram->Use();

			if (passType == ShaderPassType::PASS_TRANSLUCENT)
			{
				//glEnable(GL_BLEND);
				//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				rendered = p->RenderFilteredTranslucentPolygonalGeometry(s->GetRenderer(), s->GetRequiredKeys());

				this->NumberOfRenderedProps += rendered;
			}
			else
			{
				rendered = p->RenderFilteredOpaqueGeometry(s->GetRenderer(), s->GetRequiredKeys());

				this->NumberOfRenderedProps += rendered;
			}
			//a->shaderProgram->Restore();

		}
		++i;
	}
}
//----------------------------------------------------------------------------------------------
void vtkMyShaderPass::initialize(aperio * window, ShaderPassType::T passType)
{
	this->a = window;
	this->passType = passType;
}