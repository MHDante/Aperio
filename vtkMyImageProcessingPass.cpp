#include "stdafx.h"

/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkMyProcessingPass.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMyImageProcessingPass.h"

#include <vtkObjectFactory.h>
#include <assert.h>
#include <vtkRenderState.h>
#include <vtkRenderer.h>
#include <vtkgl.h>
#include <vtkFrameBufferObject.h>
#include <vtkTextureObject.h>
#include <vtkShaderProgram2.h>
#include <vtkShader2.h>
#include <vtkShader2Collection.h>
#include <vtkUniformVariables.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkTextureUnitManager.h>

vtkStandardNewMacro(vtkMyImageProcessingPass);

vtkCxxSetObjectMacro(vtkMyImageProcessingPass, DelegatePass, vtkRenderPass);

// ----------------------------------------------------------------------------
vtkMyImageProcessingPass::vtkMyImageProcessingPass()
{
	vtkMyTextureObject ColourTexture;
	ColourTexture.name = "source";

	vtkMyTextureObject NormalTexture;
	NormalTexture.name = "sourceNormal";

	vtkMyTextureObject capTexture;
	capTexture.name = "sourceCap";

	textures.push_back(ColourTexture);
	textures.push_back(NormalTexture);
	textures.push_back(capTexture);
}
// ----------------------------------------------------------------------------
vtkMyImageProcessingPass::~vtkMyImageProcessingPass()
{
}
// ----------------------------------------------------------------------------
void vtkMyImageProcessingPass::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
// ----------------------------------------------------------------------------
// Description:
// Perform rendering according to a render state \p s.
// \pre s_exists: s!=0
void vtkMyImageProcessingPass::Render(const vtkRenderState *s)
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

		// Depth Texture (Default)
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

		glDrawBuffer(savedDrawBuffer);

		if (this->Program1 == nullptr)
		{
			this->Program1 = vtkSmartPointer<vtkShaderProgram2>::New();
			this->Program1->SetContext(static_cast<vtkOpenGLRenderWindow *>(this->FrameBufferObject->GetContext()));

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

		vtkTextureUnitManager *tu = static_cast<vtkOpenGLRenderWindow *>(r->GetRenderWindow())->GetTextureUnitManager();
		
		auto texEnvParams = []()
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		};

		for (auto &t : textures)
		{
			t.id = tu->Allocate();

			vtkgl::ActiveTexture(vtkgl::TEXTURE0 + t.id);
			t.texture->Bind();			
			texEnvParams();
		}
		
		int id0 = tu->Allocate();	// For depth

		vtkgl::ActiveTexture(vtkgl::TEXTURE0 + id0);
		this->DepthTexture->Bind();
		texEnvParams();

		vtkgl::ActiveTexture(vtkgl::TEXTURE0);

		vtkUniformVariables *var = this->Program1->GetUniformVariables();

		float fsize[2] = { w, h };
		var->SetUniformf("frameBufSize", 2, fsize);

		for (auto &t : textures)
			var->SetUniformi(t.name.c_str(), 1, &t.id);

		var->SetUniformi("sourceDepth", 1, &id0);

		/*float d[2];
		d[0] = r->GetActiveCamera()->GetClippingRange()[0];
		d[1] = r->GetActiveCamera()->GetClippingRange()[1];
		var->SetUniformf("clipping", 2, d);*/

		// --- Get projection matrix (for accurate eye positions in SSAO)
		double aspect[2];
		int  lowerLeft[2];
		int usize, vsize;
		vtkMatrix4x4 *matrix = vtkMatrix4x4::New();

		r->GetTiledSizeAndOrigin(&usize, &vsize, lowerLeft, lowerLeft + 1);

		// Projection Matrix uniform
		r->ComputeAspect();
		r->GetAspect(aspect);
		double aspect2[2];
		r->vtkViewport::ComputeAspect();
		r->vtkViewport::GetAspect(aspect2);
		double aspectModification = aspect[0] * aspect2[1] / (aspect[1] * aspect2[0]);

		if (usize && vsize)
		{
			matrix->DeepCopy(r->GetActiveCamera()->GetProjectionTransformMatrix(aspectModification * usize / vsize, -1, 1));
			matrix->Transpose();

			float projMat[16];
			int z = 0;
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					projMat[z++] = matrix->GetElement(i, j);

			var->SetUniformMatrix("projMat", 4, 4, projMat);
		}

		// Start using program
		this->Program1->Use();

		if (!this->Program1->IsValid())
		{
			vtkErrorMacro(<< this->Program1->GetLastValidateLog());
		}
		
		// Prepare blitting
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_SCISSOR_TEST);

		// Trigger a draw on a TextureObject (Draws Quad - could be called on any texture object)
		textures[0].texture->CopyToFrameBuffer(extraPixels, extraPixels, w - 1 - extraPixels, h - 1 - extraPixels, 0, 0, width, height);

		// Cleanup
		textures[0].texture->UnBind();
		vtkgl::ActiveTexture(vtkgl::TEXTURE0);

		for (auto &t : textures)
			tu->Free(t.id);
		
		tu->Free(id0);

		this->Program1->Restore();
	}
	else
	{
		vtkWarningMacro(<< " no delegate.");
	}
}

// ----------------------------------------------------------------------------
// Description:
// Render delegate with a image of different dimensions than the
// original one.
// \pre s_exists: s!=0
// \pre fbo_exists: fbo!=0
// \pre fbo_has_context: fbo->GetContext()!=0
// \pre target_exists: target!=0
// \pre target_has_context: target->GetContext()!=0
void vtkMyImageProcessingPass::MyRenderDelegate(const vtkRenderState *s,
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

		angle = atan(tan(angle)*large / static_cast<double>(small));

		newCamera->SetViewAngle(vtkMath::DegreesFromRadians(angle));
	}

	s2.SetFrameBuffer(FrameBufferObject);

	for (auto &t : textures)
	{
		if (t.texture->GetWidth() != static_cast<unsigned int>(newWidth) ||
			t.texture->GetHeight() != static_cast<unsigned int>(newHeight))
		{
			t.texture->Create2D(newWidth, newHeight, 4, VTK_UNSIGNED_CHAR, false);
		}
	}

	// Depth Texture
	if (DepthTexture->GetWidth() != static_cast<unsigned int>(newWidth) ||
		DepthTexture->GetHeight() != static_cast<unsigned int>(newHeight))
	{
		DepthTexture->SetRequireDepthBufferFloat(true);
		DepthTexture->SetRequireTextureFloat(true);

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
	delete indices;

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
	this->NumberOfRenderedProps +=
		this->DelegatePass->GetNumberOfRenderedProps();

	newCamera->Delete();
	r->SetActiveCamera(savedCamera);
	savedCamera->UnRegister(this);
}
// ----------------------------------------------------------------------------
// Description:
// Release graphics resources and ask components to release their own
// resources.
// \pre w_exists: w!=0
void vtkMyImageProcessingPass::ReleaseGraphicsResources(vtkWindow *w)
{
	assert("pre: w_exists" && w != 0);

	this->Superclass::ReleaseGraphicsResources(w);
}
