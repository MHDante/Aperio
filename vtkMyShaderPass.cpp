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

// Set up Property Keys (globally accessible from this class)
vtkInformationKeyMacro(vtkMyShaderPass, OUTLINEKEY, Integer);


// Set up property key accessors using macro
//vtkInformationKeyMacro(vtkMyShaderPass,UNIFORMS, ObjectBase);

// ----------------------------------------------------------------------------
vtkMyShaderPass::vtkMyShaderPass()
{
	this->uniforms = vtkSmartPointer<vtkUniformVariables>::New();
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

	// Set global uniform variables
	float mousepos[3] = {a->mouse[0], a->mouse[1], a->mouse[2]};
	int source = 0;		// potential source texture
	int sourceBump = 1;		// potential source texture

	uniforms->SetUniformit("wiggle", 1, &a->wiggle);
	uniforms->SetUniformf("mouse", 3, mousepos);
	uniforms->SetUniformf("mouseSize", 1, &a->mouseSize);
	uniforms->SetUniformf("brushSize", 1, &a->brushSize);
	uniforms->SetUniformit("peerInside", 1, &a->peerInside);
	uniforms->SetUniformf("myexp", 1, &a->myexp);
	uniforms->SetUniformi("shadingnum", 1, &a->shadingnum);
	uniforms->SetUniformi("source", 1, &source);
	uniforms->SetUniformi("sourceBump", 1, &sourceBump);

	int elemssize = a->myelems.size();
	uniforms->SetUniformi("elemssize", 1, &elemssize);

	if (a->myelems.size() > 0)
	{
		MyElem & elem = a->myelems.at(a->myelems.size() - 1);

		uniforms->SetUniformf("pos1", 3, elem.p1.point.GetData());
		uniforms->SetUniformf("pos2", 3, elem.p2.point.GetData());

		uniforms->SetUniformf("norm1", 3, elem.p1.normal.GetData());
		uniforms->SetUniformf("norm2", 3, elem.p2.normal.GetData());

		uniforms->SetUniformf("scale", 3, elem.scale.GetData());
	}
	float phi = a->getUI().phiSlider->value() / a->roundnessScale;
	float theta = a->getUI().thetaSlider->value() / a->roundnessScale;

	uniforms->SetUniformf("phi", 1, &phi);
	uniforms->SetUniformf("theta", 1, &theta);

	uniforms->SetUniformit("difftrans", 1, &a->difftrans);

	uniforms->SetUniformi("shininess", 1, &a->shininess);
	uniforms->SetUniformf("darkness", 1, &a->darkness);

	uniforms->SetUniformf("time", 1, &a->wavetime);

	// Transform matrix
	float transMat[16];
	int z = 0;
	if (a->transform)
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)			
					transMat[z++] = a->transform->GetElement(i, j);

	uniforms->SetUniformMatrix("transMat", 4, 4, transMat);

	while (i < c)
	{
		// Manually set uniforms for each actor
		bool outline = false;
		uniforms->SetUniformit("outline", 1, &outline);

		vtkProp *p = s->GetPropArray()[i];

		if (p->HasKeys(s->GetRequiredKeys()))
		{
			// Find actor inside CustomMesh vector (using lambda to compare CustomMesh's actor pointer with vtkActor's pointer)
			auto it = a->getMeshByActorRaw(vtkActor::SafeDownCast(p));

			bool iselem = false;

			if (it != a->meshes.end())
			{
				// Found the CustomMesh object mapped to this actor (actor is a subclass of prop)
				uniforms->SetUniformit("selected", 1, &it->selected);

				iselem = false;
				uniforms->SetUniformit("iselem", 1, &iselem);
			}
			else
			{
				// Do another find here to see if mesh is part of widget elements
				auto it2 = a->getElemByActorRaw(vtkActor::SafeDownCast(p));

				if (it2 != a->myelems.end())
				{
					// Actor belongs to our Elements array (myelems)
					iselem = true;
					uniforms->SetUniformit("iselem", 1, &iselem);
				}
				else
				{
					// Else,
					// Not found the CustomMesh object, must be extra objects (The outliner, UI widgets, etc)
					
					if (p->GetPropertyKeys() && p->GetPropertyKeys()->Has(vtkMyShaderPass::OUTLINEKEY()))
					{
						bool outline = true;
						uniforms->SetUniformit("outline", 1, &outline);
						glEnable(GL_LINE_SMOOTH);

						//glDisable(GL_DEPTH_TEST);
					}
				}
			}
			int rendered;

			// Creating subclass of vtkShaderProgram2 to access protected Program Id variable			
			class MyShaderProgram2 : public vtkShaderProgram2
			{
			public:
				GLint getID() { return this->Id; }
			};					

			a->pgm->SetUniformVariables(uniforms);
		
			// Need this line!! (Enables alpha blending & depth testing)
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			glEnable(GL_DEPTH_TEST);

			//vtkOpenGLRenderer::SafeDownCast(s->GetRenderer())->SetShaderProgram(a->pgm); // Dangerous, constantly allocs
			a->pgm->Use();

			// Custom GL Code
			GLint progID = static_cast<MyShaderProgram2*>(a->pgm.Get())->getID();
			if (a->glew_available)
			{
				//GLuint loc = glGetUniformLocation(progID, "test");
				//glProgramUniform1f(progID, loc, 1.0);
			}

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

			if (passType == ShaderPassType::PASS_TRANSLUCENT)
			{
				//rendered = p->RenderFilteredTranslucentPolygonalGeometry(s->GetRenderer(), s->GetRequiredKeys());
				//this->NumberOfRenderedProps += rendered;

				glDepthMask(GL_FALSE);	// Disable/enable writing to depth buffer for translucent objects

				if (iselem)
				{
					vtkActor::SafeDownCast(p)->GetProperty()->SetOpacity(0.35);
				}
				static_cast<vtkMyOpenGLProperty *>(vtkOpenGLProperty::SafeDownCast(vtkActor::SafeDownCast(p)->GetProperty()))->show_back();
				rendered = p->RenderFilteredTranslucentPolygonalGeometry(s->GetRenderer(), s->GetRequiredKeys());

				if (iselem)
				{
					vtkActor::SafeDownCast(p)->GetProperty()->SetOpacity(0.01);
				}
				static_cast<vtkMyOpenGLProperty *>(vtkOpenGLProperty::SafeDownCast(vtkActor::SafeDownCast(p)->GetProperty()))->show_front();
				rendered = p->RenderFilteredTranslucentPolygonalGeometry(s->GetRenderer(), s->GetRequiredKeys());
				
				glDepthMask(GL_TRUE);	// Disable/enable writing to depth buffer for translucent objects

				this->NumberOfRenderedProps += rendered;
			}
			else
			{
				//rendered = p->RenderFilteredOpaqueGeometry(s->GetRenderer(), s->GetRequiredKeys());
				//this->NumberOfRenderedProps += rendered;

				static_cast<vtkMyOpenGLProperty *>(vtkOpenGLProperty::SafeDownCast(vtkActor::SafeDownCast(p)->GetProperty()))->show_all();
				//rendered = p->RenderFilteredOpaqueGeometry(s->GetRenderer(), s->GetRequiredKeys());

				glBegin(GL_POLYGON);
				glColor3f(1, 1, 1);
				glNormal3f(0, 0, 1);
				glVertex3f(0, 0, 0);
				glVertex3f(5, 0, 0);
				glVertex3f(5, 5, 0);
				glEnd();

				this->NumberOfRenderedProps += rendered;
			}
			a->pgm->Restore();

			// Need this line!! (alpha blending & depth testing)
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
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