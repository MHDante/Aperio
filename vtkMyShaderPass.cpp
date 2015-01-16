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
#include <vtkFrameBufferObject.h>
#include <vtkTextureObject.h>
#include <vtkTextureUnitManager.h>

vtkStandardNewMacro(vtkMyShaderPass);

vtkCxxSetObjectMacro(vtkMyShaderPass, DelegatePass, vtkRenderPass);

// Set up property key accessors using macro
//vtkInformationKeyMacro(vtkMyShaderPass,UNIFORMS, ObjectBase);

// ----------------------------------------------------------------------------
vtkMyShaderPass::vtkMyShaderPass()
{
	// Front faces
	vtkMyTextureObject depthSelectedF;
	depthSelectedF.name = "depthSelectedF";

	// Back faces
	vtkMyTextureObject depthSelected;
	depthSelected.name = "depthSelected";

	vtkMyTextureObject depthSQ;
	depthSQ.name = "depthSQ";

	vtkMyTextureObject selectedColours;
	selectedColours.name = "selectedColours";

	// Order matters (Index of COLOR_ATTACHMENT)
	textures.push_back(depthSelectedF);
	textures.push_back(depthSelected);
	textures.push_back(depthSQ);
	textures.push_back(selectedColours);

	this->DelegatePass = 0;
}
// ----------------------------------------------------------------------------
vtkMyShaderPass::~vtkMyShaderPass()
{
	if (this->DelegatePass != 0)
	{
		this->DelegatePass->Delete();
	}
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

	if (this->DelegatePass != nullptr)
	{
		vtkRenderer *r = s->GetRenderer();

		// Test for Hardware support. If not supported, just render the delegate.
		bool supported = vtkFrameBufferObject::IsSupported(r->GetRenderWindow());

		if (!supported)
		{
			vtkErrorMacro("FBOs are not supported by the context. Cannot post-process.");
		}
		if (supported)
		{
			supported = vtkTextureObject::IsSupported(r->GetRenderWindow());
			if (!supported)
			{
				vtkErrorMacro("Texture Objects are not supported by the context. Cannot post-process.");
			}
		}

		if (supported)
		{
			supported = vtkShaderProgram2::IsSupported(static_cast<vtkOpenGLRenderWindow *>(r->GetRenderWindow()));
			if (!supported)
			{
				vtkErrorMacro("GLSL is not supported by the context. Cannot post-process.");
			}
		}

		if (!supported)
		{
			this->DelegatePass->Render(s);
			this->NumberOfRenderedProps +=
				this->DelegatePass->GetNumberOfRenderedProps();
			return;
		}

		GLint savedDrawBuffer;
		glGetIntegerv(GL_DRAW_BUFFER, &savedDrawBuffer);

		// 1. Create a new render state with an FBO.

		int width = 0;
		int height = 0;
		int size[2];
		s->GetWindowSize(size);
		width = size[0];
		height = size[1];

		const int extraPixels = 1; // one on each side

		int w = width + 2 * extraPixels;
		int h = height + 2 * extraPixels;

		for (auto &t : textures)
		{
			if (t.texture == nullptr)
			{
				t.texture = vtkSmartPointer<vtkTextureObject>::New();
				t.texture->SetContext(r->GetRenderWindow());
			}
		}

		if (this->DepthTexture == nullptr)
		{
			this->DepthTexture = vtkSmartPointer<vtkTextureObject>::New();
			this->DepthTexture->SetContext(r->GetRenderWindow());
		}
		if (this->FrameBufferObject == nullptr)
		{
			this->FrameBufferObject = vtkSmartPointer<vtkFrameBufferObject>::New();
			this->FrameBufferObject->SetContext(r->GetRenderWindow());
		}

		// Render to FrameBufferObject (Set up texture attachments too)
		this->MyRenderDelegate(s, width, height, w, h);

		// Unbind the framebuffer so we can draw to screen
		this->FrameBufferObject->UnBind();

		//glDrawBuffer(savedDrawBuffer);	// Not needed in Shader Pass (No FBO attachment changes yet)

		vtkTextureUnitManager *tu = static_cast<vtkOpenGLRenderWindow *>(r->GetRenderWindow())->GetTextureUnitManager();

		auto texEnvParams = []()
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		};

		// Bind textures
		for (auto &t : textures)
		{
			t.id = tu->Allocate();
			
			vtkgl::ActiveTexture(vtkgl::TEXTURE0 + t.id);
			t.texture->Bind();
			texEnvParams();

			/*if (a->glew_available && t.name == "selectedColours")
			{
				GLint width, height;
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

				//char pixels[width*height * 4];
				char *pixels = new char[width * height * 4];

				std::cout << width << "," << height << "\n";
				glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

				std::cout << (int)pixels[(width / 2) * (height / 2) * 4] << "," << 
					(int)pixels[(width / 2) * (height / 2) * 4 + 1] << "," <<
					(int)pixels[(width / 2) * (height / 2) * 4 + 2] << "," <<
					(int)pixels[(width / 2) * (height / 2) * 4 + 3] << "\n";

				delete[] pixels;
			}*/
		}

		if (a->glew_available)
		{
			CustomTexture &matcap = a->matcap;

			glActiveTexture(GL_TEXTURE0 + matcap.unit);
			glBindTexture(GL_TEXTURE_2D, matcap.name);
			//texEnvParams();
		}

		vtkgl::ActiveTexture(vtkgl::TEXTURE0);	// No active texture (no funny colour artifacts)

		// Set uniforms
		float fsize[2] = { w, h };
		uniforms->SetUniformf("frameBufSize", 2, fsize);

		for (auto &t : textures)
			uniforms->SetUniformi(t.name.c_str(), 1, &t.id);

		this->RenderGeometry(s, false);	// Render opaque geometry first
		this->RenderGeometry(s, true);	// Render translucent geometry

		// Cleanup
		textures[0].texture->UnBind();
		vtkgl::ActiveTexture(vtkgl::TEXTURE0);

		// Free textures
		for (auto &t : textures)
			tu->Free(t.id);
	}
	else
	{
		//vtkWarningMacro(<< " no delegate.");
		this->RenderGeometry(s, false);	// Render opaque geometry first
		this->RenderGeometry(s, true);	// Render translucent geometry
	}
}

//------------------------------------------------------------------------------------------------
// Description:
// Render delegate with a image of different dimensions than the
// original one.
// \pre s_exists: s!=0
// \pre fbo_exists: fbo!=0
// \pre fbo_has_context: fbo->GetContext()!=0
// \pre target_exists: target!=0
// \pre target_has_context: target->GetContext()!=0
void vtkMyShaderPass::MyRenderDelegate(const vtkRenderState *s,
	int width,
	int height,
	int newWidth,
	int newHeight)
{
	assert("pre: s_exists" && s != 0);
	assert("pre: fbo_exists" && FrameBufferObject != 0);
	assert("pre: fbo_has_context" && FrameBufferObject->GetContext() != 0);
	assert("pre: target_depth_exists" && DepthTexture != 0);
	assert("pre: target_depth_has_context" && DepthTexture->GetContext() != 0);

	vtkRenderer *r = s->GetRenderer();
	vtkRenderState s2(r);
	s2.SetPropArrayAndCount(s->GetPropArray(), s->GetPropArrayCount());

	// Adapt camera to new window size
	vtkCamera *savedCamera = r->GetActiveCamera();
	savedCamera->Register(this);
	vtkCamera *newCamera = vtkCamera::New();
	newCamera->DeepCopy(savedCamera);

	r->SetActiveCamera(newCamera);

	if (newCamera->GetParallelProjection())
	{
		newCamera->SetParallelScale(
			newCamera->GetParallelScale()*newHeight / static_cast<double>(height));
	}
	else
	{
		double large;
		double small;
		if (newCamera->GetUseHorizontalViewAngle())
		{
			large = newWidth;
			small = width;
		}
		else
		{
			large = newHeight;
			small = height;
		}
		double angle = vtkMath::RadiansFromDegrees(newCamera->GetViewAngle());

		//angle = atan(tan(angle)*large / static_cast<double>(small));
		angle = 2.0*atan(tan(angle/2.0)*large / static_cast<double>(small));

		newCamera->SetViewAngle(vtkMath::DegreesFromRadians(angle));
	}

	s2.SetFrameBuffer(FrameBufferObject);

	// Create 2D textures
	for (auto &t : textures)
	{
		if (t.texture->GetWidth() != static_cast<unsigned int>(newWidth) ||
			t.texture->GetHeight() != static_cast<unsigned int>(newHeight))
		{
			t.texture->SetGenerateMipmap(true);
			t.texture->Create2D(newWidth, newHeight, 4, VTK_UNSIGNED_CHAR, false);

		}
	}

	if (DepthTexture->GetWidth() != static_cast<unsigned int>(newWidth) ||
		DepthTexture->GetHeight() != static_cast<unsigned int>(newHeight))
	{
		DepthTexture->SetRequireDepthBufferFloat(true);
		DepthTexture->SetRequireTextureFloat(true);

		DepthTexture->SetGenerateMipmap(true);
		DepthTexture->Create2D(newWidth, newHeight, 1, VTK_VOID, false);
	}

	// Set color attachments
	FrameBufferObject->SetNumberOfRenderTargets(textures.size());
	FrameBufferObject->SetDepthBuffer(DepthTexture);

	int num_textures = textures.size();
	for (int i = 0; i < num_textures; i++)
		FrameBufferObject->SetColorBuffer(i, textures[i].texture);

	// Render into n attachments (indices)
	unsigned int *indices = new unsigned int[num_textures];	// Make {0, 1, 2, etc }
	for (int i = 0; i < num_textures; i++)
		indices[i] = i;

	FrameBufferObject->SetActiveBuffers(textures.size(), indices);
	delete[] indices;

	// because the same FBO can be used in another pass but with several color
	// buffers, force this pass to use 1, to avoid side effects from the
	// render of the previous frame.
	//fbo->SetActiveBuffer(0);

	FrameBufferObject->SetDepthBufferNeeded(true);
	FrameBufferObject->StartNonOrtho(newWidth, newHeight, false);
	glViewport(0, 0, newWidth, newHeight);
	glScissor(0, 0, newWidth, newHeight);

	// 2. Delegate render in FBO
	this->DelegatePass->Render(&s2);
	this->NumberOfRenderedProps += this->DelegatePass->GetNumberOfRenderedProps();

	newCamera->Delete();
	r->SetActiveCamera(savedCamera);
	savedCamera->UnRegister(this);
}