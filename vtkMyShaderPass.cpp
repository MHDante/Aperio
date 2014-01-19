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
#include "stdafx.h"		// Precompiled header files
//
#include "vtkMyShaderPass.h"
#include "vtkObjectFactory.h"

// Custom
#include "vtkRenderState.h"
#include "vtkShader2.h"
#include "vtkShaderProgram2.h"
#include "vtkShader2Collection.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkUniformVariables.h"
#include "vtkInformation.h"

#include <assert.h>
//
vtkStandardNewMacro(vtkMyShaderPass);

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

	this->NumberOfRenderedProps = 0;

	this->RenderGeometry(s);
}

bool vtkMyShaderPass::BuildShader(vtkSmartPointer<vtkShaderProgram2> & shaderProgram, vtkOpenGLRenderWindow* glContext, char * vs, char * fs)
{
	if (shaderProgram)
		return true;

	//std::cout << "BUILDING" ;
	shaderProgram = vtkSmartPointer<vtkShaderProgram2>::New();

	vtkSmartPointer<vtkShader2> shader = vtkSmartPointer<vtkShader2>::New();
	vtkSmartPointer<vtkShader2> shader2 = vtkSmartPointer<vtkShader2>::New();

	std::ifstream file1(vs);
	std::stringstream buffer1;
	buffer1 << file1.rdbuf();

	shader->SetSourceCode(buffer1.str().c_str());

	std::ifstream file2(fs);
	std::stringstream buffer2;
	buffer2 << file2.rdbuf();

	shader2->SetSourceCode(buffer2.str().c_str());

	shader->SetType(VTK_SHADER_TYPE_VERTEX);
	shader2->SetType(VTK_SHADER_TYPE_FRAGMENT);

	shaderProgram->GetShaders()->AddItem(shader);
	shaderProgram->GetShaders()->AddItem(shader2);

	shaderProgram->SetContext(glContext);
	shader->SetContext(shaderProgram->GetContext());
	shader2->SetContext(shaderProgram->GetContext());

	shaderProgram->Build(); 

	if (shaderProgram->GetLastBuildStatus() == VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
	{
		//isCompiled = true;
		return true;
	}
	else
	{
		return false;
	}
}

// ----------------------------------------------------------------------------
// Description:
// Opaque/Translucent pass with key checking.
// \pre s_exists: s!=0
void vtkMyShaderPass::RenderGeometry(const vtkRenderState *s)
{
	assert("pre: s_exists" && s != 0);

	if (passType == ShaderPassType::PASS_OPAQUE)
	{
		if (!BuildShader(this->shaderProgram, (vtkOpenGLRenderWindow *)s->GetRenderer()->GetRenderWindow(), "shader.glsl", "shaderfrag.glsl"))
			return;
	}

	if (passType == ShaderPassType::PASS_TRANSLUCENT)
	{
		if (!BuildShader(this->shaderProgram, (vtkOpenGLRenderWindow *)s->GetRenderer()->GetRenderWindow(), "shader.glsl", "shaderfrag.glsl"))
			return;
	}

	//if (!BuildShader(this->shaderProgram2, (vtkOpenGLRenderWindow *) s->GetRenderer()->GetRenderWindow(), "shader2.glsl", "shaderfrag2.glsl"))
	//return;

	//if (!BuildShader(this->shaderProgram3, (vtkOpenGLRenderWindow *) s->GetRenderer()->GetRenderWindow(), "shader3.glsl", "shaderfrag3.glsl"))
	//		return;

	int c = s->GetPropArrayCount();
	int i = 0;

	// Set global uniforms
	float moo[3];
	moo[0] = a->mouse[0];
	moo[1] = a->mouse[1];
	moo[2] = a->mouse[2];

	uniforms->SetUniformf("mouse", 3, moo);
	uniforms->SetUniformf("mouseSize", 1, &a->mouseSize);
	uniforms->SetUniformi("peerInside", 1, &a->peerInside);
	uniforms->SetUniformf("myexp", 1, &a->myexp);

	uniforms->SetUniformf("pos1", 3, a->pos1);
	uniforms->SetUniformf("pos2", 3, a->pos2);

	//std::cout << "------" << std::endl;
	class vtkMyShaderProgram2 : public vtkShaderProgram2
	{
	public:
		void UseProgram() {
			//assert(this->Context);
			vtkgl::UseProgram((GLuint)this->Id);
			//glUseProgram((GLuint) this->Id);
			//GLint id;
			//glGetIntegerv(GL_CURRENT_PROGRAM, &amp; id);

			//vtkOpenGLCheckErrorMacro("failed at glUseProgram");
		}
		void UnuseProgram()
		{
			//assert(this->Context);
			vtkgl::UseProgram((GLuint)0U);
			//vtkOpenGLCheckErrorMacro("failed at glUseProgram");
		}
	};

	vtkMyShaderProgram2 *currentProgram = static_cast<vtkMyShaderProgram2 *>(this->shaderProgram.GetPointer());

	//	bool none = false;

	while (i < c)
	{
		vtkProp *p = s->GetPropArray()[i];

		// Set actor-specific uniforms and send them to shader program to use

		// Find actor inside CustomMesh vector (I overrided the == operator for CustomMesh to compare with vtkActors)
		std::vector<CustomMesh>::iterator it = std::find(a->meshes.begin(), a->meshes.end(), ((vtkActor *)p));

		if (it != a->meshes.end())
		{
			//currentProgram = this->shaderProgram;
			// Found the CustomMesh object mapped to this actor (actor is a subclass of prop)
			uniforms->SetUniformi("selected", 1, &it->selected);

			//((vtkActor*) p)->GetProperty()->FrontfaceCullingOn();
			//((vtkActor*) p)->GetProperty()->FrontfaceCullingOff();
		}
		else
		{
			// Find actor inside WidgetElems vector (I overrided the == operator for WidgetElem to compare with vtkActors)
			//std::vector<WidgetElem>::iterator it2 = std::find(a->widgets.begin(), a->widgets.end(), ((vtkActor *)p));

			for (int i = 0; i < a->widgets.size(); i++)
			{
				std::vector<WidgetElem>::iterator it2 = std::find(a->widgets[i].begin(), a->widgets[i].end(), ((vtkActor *)p));

				if (it2 != a->widgets[i].end())	// found
				{
					//currentProgram = this->shaderProgram2;
				}
			}
			// Not found the CustomMesh object, must be extra objects
			//none = true;
			//currentProgram = this->shaderProgram3;
			//int silhouette = 1;
			//uniforms->SetUniformi("silhouette", 1, &silhouette);
		}
		currentProgram->SetUniformVariables(uniforms);

		if (passType == ShaderPassType::PASS_SILHOUETTE)
		{
			//glCullFace(GL_FRONT);
			//((vtkActor*)p)->GetProperty()->FrontfaceCullingOn();

			//currentProgram = shaderProgram2;
		}
		else
		{
			//{((vtkActor*)p)->GetProperty()->FrontfaceCullingOff();
			//((vtkActor*)p)->GetProperty()->BackfaceCullingOff();
		}

		if (p->HasKeys(s->GetRequiredKeys()))
		{
			//currentProgram->Use();
			int rendered;
			
			//vtkgl::UseProgram(0);
			currentProgram->Use();

			if (passType == ShaderPassType::PASS_TRANSLUCENT)
			{
				//if (!currentProgram->IsUsed())
				//currentProgram->Use();
				//currentProgram->Use();

				rendered = p->RenderFilteredTranslucentPolygonalGeometry(s->GetRenderer(), s->GetRequiredKeys());
				//std::cout << "transparennnttt" << std::endl;

				//currentProgram->Restore();

				//if (currentProgram->IsUsed())
				//					currentProgram->Restore();
			}
			else
			{
				//currentProgram->Use();
				//if (!currentProgram->IsUsed())
					//currentProgram->Use();
				//current
				//currentProgram->Id;
				//currentProgram->Use();

				//vtkgl::UseProgram((GLuint)this->Id);

				rendered = p->RenderFilteredOpaqueGeometry(s->GetRenderer(), s->GetRequiredKeys());

				//if (currentProgram->IsUsed())
//					currentProgram->Restore();
				//currentProgram->Restore();

				//currentProgram->Restore();
			}

			this->NumberOfRenderedProps += rendered;
			//currentProgram->Restore();
			currentProgram->Restore();
			

		}
		++i;
	}
}

void vtkMyShaderPass::initialize(additive * window, ShaderPassType::T passType)
{
	this->a = window;
	this->passType = passType;
}