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

#include "vtkMyProcessingPass.h"

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

vtkStandardNewMacro(vtkMyProcessingPass);

// ----------------------------------------------------------------------------
vtkMyProcessingPass::vtkMyProcessingPass()
{
	this->FrameBufferObject = 0;
	this->Pass1 = 0;

	// CUSTOM - Depth texture
	this->Pass1Depth = 0;
	this->Pass1Normal = 0;
	this->Pass1Noise = 0;

	this->Program1 = 0;
}
// ----------------------------------------------------------------------------
vtkMyProcessingPass::~vtkMyProcessingPass()
{
	if (this->FrameBufferObject != 0)
	{
		vtkErrorMacro(<< "FrameBufferObject should have been deleted in ReleaseGraphicsResources().");
	}
	if (this->Pass1 != 0)
	{
		vtkErrorMacro(<< "Pass1 should have been deleted in ReleaseGraphicsResources().");
	}
	if (this->Pass1Depth != 0)
	{
		vtkErrorMacro(<< "Pass1Depth should have been deleted in ReleaseGraphicsResources().");
	}
	if (this->Pass1Normal != 0)
	{
		vtkErrorMacro(<< "Pass1Normal should have been deleted in ReleaseGraphicsResources().");
	}
	if (this->Pass1Noise != 0)
	{
		vtkErrorMacro(<< "Pass1Noise should have been deleted in ReleaseGraphicsResources().");
	}
	if (this->Program1 != 0)
	{
		this->Program1->Delete();
	}
}
// ----------------------------------------------------------------------------
void vtkMyProcessingPass::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
// ----------------------------------------------------------------------------
// Description:
// Perform rendering according to a render state \p s.
// \pre s_exists: s!=0
void vtkMyProcessingPass::Render(const vtkRenderState *s)
{
	assert("pre: s_exists" && s != 0);

	this->NumberOfRenderedProps = 0;

	if (this->DelegatePass != 0)
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

		if (this->Pass1 == 0)
		{
			this->Pass1 = vtkTextureObject::New();
			this->Pass1->SetContext(r->GetRenderWindow());
		}

		// CUSTOM - Depth texture object
		if (this->Pass1Depth == 0)
		{
			this->Pass1Depth = vtkTextureObject::New();
			this->Pass1Depth->SetContext(r->GetRenderWindow());
		}
		if (this->Pass1Normal == 0)
		{
			this->Pass1Normal = vtkTextureObject::New();
			this->Pass1Normal->SetContext(r->GetRenderWindow());
		}
		if (this->Pass1Noise == 0)
		{
			this->Pass1Noise = vtkTextureObject::New();
			this->Pass1Noise->SetContext(r->GetRenderWindow());
		}

		if (this->FrameBufferObject == 0)
		{
			this->FrameBufferObject = vtkFrameBufferObject::New();
			this->FrameBufferObject->SetContext(r->GetRenderWindow());
		}

		this->MyRenderDelegate(s, width, height, w, h, this->FrameBufferObject, this->Pass1, this->Pass1Depth, this->Pass1Normal, this->Pass1Noise);

		// Unbind the framebuffer so we can draw to screen
		this->FrameBufferObject->UnBind();

		glDrawBuffer(savedDrawBuffer);


		if (this->Program1 == 0)
		{
			this->Program1 = vtkShaderProgram2::New();
			this->Program1->SetContext(static_cast<vtkOpenGLRenderWindow *>(this->FrameBufferObject->GetContext()));

			if (bufferV.str().size() > 0)
			{
				vtkShader2 *shader = vtkShader2::New();
				shader->SetType(VTK_SHADER_TYPE_VERTEX);
				shader->SetSourceCode(bufferV.str().c_str());
				shader->SetContext(this->Program1->GetContext());

				this->Program1->GetShaders()->AddItem(shader);
				shader->Delete();
			}

			if (bufferF.str().size() > 0)
			{
				vtkShader2 *shader2 = vtkShader2::New();
				shader2->SetType(VTK_SHADER_TYPE_FRAGMENT);
				shader2->SetSourceCode(bufferF.str().c_str());
				shader2->SetContext(this->Program1->GetContext());

				this->Program1->GetShaders()->AddItem(shader2);
				shader2->Delete();
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

		// id0 is source
		int id0 = tu->Allocate();
		int id1 = tu->Allocate();
		int id2 = tu->Allocate();
		int id3 = tu->Allocate();

		vtkgl::ActiveTexture(vtkgl::TEXTURE0 + id0);
		this->Pass1->Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


		vtkgl::ActiveTexture(vtkgl::TEXTURE0 + id1);
		this->Pass1Depth->Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		vtkgl::ActiveTexture(vtkgl::TEXTURE0 + id2);
		this->Pass1Normal->Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		vtkgl::ActiveTexture(vtkgl::TEXTURE0 + id3);
		this->Pass1Noise->Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		vtkUniformVariables *var = this->Program1->GetUniformVariables();

		float fsize[2] = { w, h };

		var->SetUniformi("source", 1, &id0);
		var->SetUniformi("sourceDepth", 1, &id1);
		var->SetUniformi("sourceNormal", 1, &id2);
		var->SetUniformi("sourceNoise", 1, &id3);
		var->SetUniformf("frameBufSize", 2, fsize);

		// --- Get projection matrix
		double aspect[2];
		int  lowerLeft[2];
		int usize, vsize;
		vtkMatrix4x4 *matrix = vtkMatrix4x4::New();

		r->GetTiledSizeAndOrigin(&usize, &vsize, lowerLeft, lowerLeft + 1);

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

		// Trigger a draw on Gy1 (could be called on Gx1).
		this->Pass1->CopyToFrameBuffer(extraPixels, extraPixels, w - 1 - extraPixels, h - 1 - extraPixels, 0, 0, width, height);

		this->Pass1Noise->UnBind();
		vtkgl::ActiveTexture(vtkgl::TEXTURE0 + id3);

		this->Pass1Normal->UnBind();
		vtkgl::ActiveTexture(vtkgl::TEXTURE0 + id2);

		this->Pass1Depth->UnBind();
		vtkgl::ActiveTexture(vtkgl::TEXTURE0 + id1);

		this->Pass1->UnBind();
		vtkgl::ActiveTexture(vtkgl::TEXTURE0 + id0);

		tu->Free(id0);
		tu->Free(id1);
		tu->Free(id2);
		tu->Free(id3);
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
void vtkMyProcessingPass::MyRenderDelegate(const vtkRenderState *s,
	int width,
	int height,
	int newWidth,
	int newHeight,
	vtkFrameBufferObject *fbo,
	vtkTextureObject *target,
	vtkTextureObject *targetDepth,
	vtkTextureObject *targetNormal,
	vtkTextureObject * targetNoise)
{
	assert("pre: s_exists" && s != 0);
	assert("pre: fbo_exists" && fbo != 0);
	assert("pre: fbo_has_context" && fbo->GetContext() != 0);
	assert("pre: target_exists" && target != 0);
	assert("pre: target_has_context" && target->GetContext() != 0);
	assert("pre: target_depth_exists" && targetDepth != 0);
	assert("pre: target_depth_has_context" && targetDepth->GetContext() != 0);

	assert("pre: target_normal_exists" && targetNormal != 0);
	assert("pre: target_normal_has_context" && targetNormal->GetContext() != 0);

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

	s2.SetFrameBuffer(fbo);

	if (target->GetWidth() != static_cast<unsigned int>(newWidth) ||
		target->GetHeight() != static_cast<unsigned int>(newHeight))
	{
		target->Create2D(newWidth, newHeight, 4, VTK_UNSIGNED_CHAR, false);
	}

	// CUSTOM - Depth texture

	if (targetDepth->GetWidth() != static_cast<unsigned int>(newWidth) ||
		targetDepth->GetHeight() != static_cast<unsigned int>(newHeight))
	{
		targetDepth->SetRequireDepthBufferFloat(true);
		targetDepth->SetRequireTextureFloat(true);

		//targetDepth->Create2D(newWidth, newHeight, 1, VTK_VOID, false);
		targetDepth->Create2D(newWidth, newHeight, 1, VTK_VOID, false);
	}

	if (targetNormal->GetWidth() != static_cast<unsigned int>(newWidth) ||
		targetNormal->GetHeight() != static_cast<unsigned int>(newHeight))
	{
		targetNormal->Create2D(newWidth, newHeight, 4, VTK_UNSIGNED_CHAR, false);
	}
	if (targetNoise->GetWidth() != static_cast<unsigned int>(newWidth) ||
		targetNoise->GetHeight() != static_cast<unsigned int>(newHeight))
	{
		targetNoise->Create2D(newWidth, newHeight, 4, VTK_UNSIGNED_CHAR, false);
	}

	fbo->SetNumberOfRenderTargets(3);
	fbo->SetColorBuffer(0, target);
	fbo->SetColorBuffer(1, targetNormal);
	fbo->SetColorBuffer(2, targetNoise);
	fbo->SetDepthBuffer(targetDepth);

	unsigned int indices[3] = { 0, 1, 2};
	fbo->SetActiveBuffers(3, indices);

	// because the same FBO can be used in another pass but with several color
	// buffers, force this pass to use 1, to avoid side effects from the
	// render of the previous frame.
	//fbo->SetActiveBuffer(0);

	fbo->SetDepthBufferNeeded(true);
	fbo->StartNonOrtho(newWidth, newHeight, false);
	glViewport(0, 0, newWidth, newHeight);
	glScissor(0, 0, newWidth, newHeight);

	// 2. Delegate render in FBO
	glEnable(GL_DEPTH_TEST);

	// Need this line!! (Enables alpha blending for FBO)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	this->DelegatePass->Render(&s2);
	this->NumberOfRenderedProps +=
		this->DelegatePass->GetNumberOfRenderedProps();

	glDisable(GL_BLEND);

	newCamera->Delete();
	r->SetActiveCamera(savedCamera);
	savedCamera->UnRegister(this);
}
// ----------------------------------------------------------------------------
// Description:
// Release graphics resources and ask components to release their own
// resources.
// \pre w_exists: w!=0
void vtkMyProcessingPass::ReleaseGraphicsResources(vtkWindow *w)
{
	assert("pre: w_exists" && w != 0);

	this->Superclass::ReleaseGraphicsResources(w);

	if (this->Program1 != 0)
	{
		this->Program1->ReleaseGraphicsResources();
	}

	if (this->FrameBufferObject != 0)
	{
		this->FrameBufferObject->Delete();
		this->FrameBufferObject = 0;
	}

	if (this->Pass1 != 0)
	{
		this->Pass1->Delete();
		this->Pass1 = 0;
	}
	if (this->Pass1Depth != 0)
	{
		this->Pass1Depth->Delete();
		this->Pass1Depth = 0;
	}
	if (this->Pass1Normal != 0)
	{
		this->Pass1Normal->Delete();
		this->Pass1Normal = 0;
	}
	if (this->Pass1Noise != 0)
	{
		this->Pass1Noise->Delete();
		this->Pass1Noise = 0;
	}
}
//--------------------------------------------------------------------------------------------------
void vtkMyProcessingPass::setShaderFile(std::string filename, bool frag)
{
	std::ifstream file1(filename);

	if (frag)
		bufferF << file1.rdbuf();
	else
		bufferV << file1.rdbuf();
}
