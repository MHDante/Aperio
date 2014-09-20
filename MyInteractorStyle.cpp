#include "stdafx.h"

#include "MyInteractorStyle.h"

// Custom
#include "aperio.h"
//#include "vtkMyShaderPass.h"
#include "CarveConnector.h"

// VTK Includes
#include <vtkQuad.h>
#include <vtkAppendPolyData.h>
#include <vtkSphereSource.h>

#include <vtkOutlineSource.h>
#include <vtkMyShaderPass.h>
#include <vtkPickingManager.h>

vtkStandardNewMacro(MyInteractorStyle);
 
//---------------------------------------------------------------------------------------------------------------
MyInteractorStyle::MyInteractorStyle() : NumberOfClicks(0), ResetPixelDistance(5)
{
	this->PreviousPosition[0] = 0;
	this->PreviousPosition[1] = 0;

	this->cellPicker = vtkSmartPointer<vtkCellPicker>::New();
	//this->cellPicker->SetTolerance(0.0005);	

	dragging = false;
	creation = false;

	a = nullptr;
}
//---------------------------------------------------------------------------------------------------------------
void MyInteractorStyle::initialize(aperio *window)
{
	this->a = window;
}
//---------------------------------------------------------------------------------------------------------------
void MyInteractorStyle::setPickList(int z)
{
	//worldPicker->GetPickList()->RemoveAllItems();
	//worldPicker->AddPickList(a->meshes[z].actor);
	//worldPicker->PickFromListOn();
}
//---------------------------------------------------------------------------------------------
void MyInteractorStyle::OnKeyPress()
{
	// Show all
	if (GetKeyState(VK_MENU) & 0x1000 && GetKeyState('H') & 0x1000)
	{
		for (int i = 0; i < a->meshes.size(); i++)
		{
			a->meshes[i].opacity = 1.0;
			a->meshes[i].actor->GetProperty()->SetOpacity(1.0);
		}
		a->updateOpacitySliderAndList();
	}
	//if (strcmp(this->Interactor->GetKeySym(), "Delete") == 0)	// Del key	 ( probably should put this in QT keycheck, b/c QVTKWidget requires focus)
	else if (strcmp(this->Interactor->GetKeySym(), "h") == 0)	// Del key	 ( probably should put this in QT keycheck, b/c QVTKWidget requires focus)
	{
		a->slot_btnHide();
	}
	if (this->Interactor->GetKeyCode() == '9')	// Explode it out
	{
		if (a->selectedMesh == a->meshes.end())
			return;
		if (!a->selectedMesh->generated)	// Make sure it is a generated mesh (Rather than original mesh)
			return;

		double pos[3];
		a->selectedMesh->actor->GetPosition(pos);
		float scale = 0.1f;
		pos[0] += a->selectedMesh->snormal.GetX() * scale;
		pos[1] += a->selectedMesh->snormal.GetY() * scale;
		pos[2] += a->selectedMesh->snormal.GetZ() * scale;

		a->selectedMesh->actor->SetPosition(pos);
	}
	if (this->Interactor->GetKeyCode() == '6')	// Explode it in
	{
		if (a->selectedMesh == a->meshes.end())
			return;
		if (!a->selectedMesh->generated)	// Make sure it is a generated mesh (Rather than original mesh)
			return;

		double pos[3];
		a->selectedMesh->actor->GetPosition(pos);
		float scale = 0.1f;
		pos[0] -= a->selectedMesh->snormal.GetX() * scale;
		pos[1] -= a->selectedMesh->snormal.GetY() * scale;
		pos[2] -= a->selectedMesh->snormal.GetZ() * scale;

		a->selectedMesh->actor->SetPosition(pos);
	}
	if (this->Interactor->GetKeyCode() == '3')	// Rotate around hinge	
	{
		a->slot_hingeSlider(128);
	}
	if (this->Interactor->GetKeyCode() == 'l')	// Hinge
	{
		a->slot_btnSlice();
	}
	
	float step = 1;

	if (this->Interactor->GetKeyCode() == '+')	// bigger peek brush
	{
		//a->brushDivide--;

		//if (a->brushSize + step <= 50)
			a->brushSize += step;

		this->OnMouseMove();
	}
	if (this->Interactor->GetKeyCode() == '-')	// smaller peek brush
	{
		//a->brushDivide++;

		if (a->brushSize - step >= 0)
			a->brushSize -= step;

		this->OnMouseMove();
	}
	if (this->Interactor->GetKeyCode() == 't')	// Toggle peek
	{
		a->slot_btnGlass();
	}
	if (this->Interactor->GetKeyCode() == 'c')	// change roundness (-phi)
	{
		float change = 0.1;

		if (a->myn - change >= 0)
			a->myn -= change;

		//a->superquad->SetPhiRoundness(a->myn);
		//a->superquad->Update();

		/*int i = a->myelems.size() - 1;
		a->myelems[i].source->SetPhiRoundness(a->myn);
		a->myelems[i].source->Update();
		a->myelems[i].transformFilter->Update();		// Must update transformfilter for transforms to show
		*/
	}
	if (this->Interactor->GetKeyCode() == 'v')	// change roundness (phi)
	{
		float change = 0.1;

		if (a->myn + change <= 20)
			a->myn += change;

		//a->superquad->SetPhiRoundness(a->myn);
		//a->superquad->Update();

		/*int i = a->myelems.size() - 1;
		a->myelems[i].source->SetPhiRoundness(a->myn);
		a->myelems[i].source->Update();
		a->myelems[i].transformFilter->Update();		// Must update transformfilter for transforms to show
		*/
	}
	if (this->Interactor->GetKeyCode() == 'z')	// change roundness (-theta)
	{
		float change = 0.1;

		if (a->myexp - change >= 0)
			a->myexp -= change;

		//a->superquad->SetThetaRoundness(a->myexp);
		//a->superquad->Update();

		/*int i = a->myelems.size() - 1;
		a->myelems[i].source->SetThetaRoundness(a->myexp);
		a->myelems[i].source->Update();
		a->myelems[i].transformFilter->Update();		// Must update transformfilter for transforms to show
		*/
	}
	if (this->Interactor->GetKeyCode() == 'x')	// change roundness (theta)
	{
		float change = 0.1;

		if (a->myexp + change <= 20)
			a->myexp += change;

		//a->superquad->SetThetaRoundness(a->myexp);
		//a->superquad->Update();

		/*int i = a->myelems.size() - 1;
		a->myelems[i].source->SetThetaRoundness(a->myexp);
		a->myelems[i].source->Update();
		a->myelems[i].transformFilter->Update();		// Must update transformfilter for transforms to show
		*/
	}
	if (this->Interactor->GetKeyCode() == 's')	// Create toroidal superquad 
	{
		if (!a->superquad)
		{
			a->superquad = vtkSmartPointer<MySuperquadricSource>::New();
			a->superquad->SetPhiResolution(20);
			a->superquad->SetThetaResolution(20);
			a->superquad->ToroidalOn();

			a->superquad->SetScale(2, 1, 1);
			a->superquad->SetSize(1.0);

			a->superquad->SetPhiRoundness(0.5);
			//a->superquad->SetThickness(0.43);
			a->superquad->SetThetaRoundness(a->myexp);

			a->superquad->SetCenter(a->mouse[0], a->mouse[1], a->mouse[2]);

			vtkSmartPointer<vtkActor> sactor = Utility::sourceToActor(a, a->superquad->GetOutput(), 1, 1, 1, 1);
			sactor->PickableOff();

			a->renderer->AddActor(sactor);
		}
		//a->renderer->GetActors()->
		a->superquad->SetCenter(a->mouse[0], a->mouse[1], a->mouse[2]);
		a->superquad->Update();
	}
	if (this->Interactor->GetKeyCode() == '0')		// Change shading model
	{
		a->shadingnum = (a->shadingnum + 1) % 3;
	}

	float thestep = 0.05;

	//------------- Last superquad --------------------------------------------------------------------------
	if (this->Interactor->GetKeyCode() == 'u' && a->myelems.size() > 0)	// Resize last superquad placed
	{
		int i = a->myelems.size() - 1;
		MyElem &elem = a->myelems.at(i);

		elem.scale.SetZ(elem.scale.GetZ() - thestep);
		elem.transformFilter->SetTransform(a->makeCompositeTransform(elem));
		elem.transformFilter->Update();	// Must update transform filter for updates to show

		/*int i = a->myelems.size() - 1;
		vtkTransform* transform = vtkTransform::SafeDownCast(a->myelems.at(i).transformFilter->GetTransform());

		double elements1[16] = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 0.9, 0,
			0, 0, 0, 1
		};
		transform->PreMultiply();
		transform->Concatenate(elements1);

		a->myelems.at(i).transformFilter->Update();
		//a->myelems.at(i).actor->GetProperty()->SetDiffuseColor(0, 1, 0);
		//a->myelems.at(i).actor->GetMapper()->Update();*/
	}
	if (this->Interactor->GetKeyCode() == 'o' && a->myelems.size() > 0)	// Resize last superquad placed
	{
		int i = a->myelems.size() - 1;
		MyElem &elem = a->myelems.at(i);

		elem.scale.SetZ(elem.scale.GetZ() + thestep);
		elem.transformFilter->SetTransform(a->makeCompositeTransform(elem));
		elem.transformFilter->Update();	// Must update transform filter for updates to show

		/*int i = a->myelems.size() - 1;
		vtkTransform* transform = vtkTransform::SafeDownCast(a->myelems.at(i).transformFilter->GetTransform());

		double elements1[16] = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1.1, 0,
			0, 0, 0, 1
		};
		transform->PreMultiply();
		transform->Concatenate(elements1);

		a->myelems.at(i).transformFilter->Update();*/
		//a->myelems.at(i).actor->GetProperty()->SetDiffuseColor(0, 1, 0);
		//a->myelems.at(i).actor->GetMapper()->Update();
	}
	if (this->Interactor->GetKeyCode() == 'e' && a->myelems.size() > 0)		// Resize last superquad placed
	{
		int i = a->myelems.size() - 1;
		MyElem &elem = a->myelems.at(i);

		elem.scale.SetY(elem.scale.GetY() + thestep);
		elem.transformFilter->SetTransform(a->makeCompositeTransform(elem));
		elem.transformFilter->Update();	// Must update transform filter for updates to show

		//a->myelems.at(i).actor->GetProperty()->SetDiffuseColor(0, 1, 0);
		//a->myelems.at(i).actor->GetMapper()->Update();
	}
	if (this->Interactor->GetKeyCode() == 'q' && a->myelems.size() > 0)	// Resize last superquad placed
	{
		int i = a->myelems.size() - 1;
		MyElem &elem = a->myelems.at(i);

		elem.scale.SetY(elem.scale.GetY() - thestep);
		elem.transformFilter->SetTransform(a->makeCompositeTransform(elem));
		elem.transformFilter->Update();	// Must update transform filter for updates to show


		/*int i = a->myelems.size() - 1;
		vtkTransform* transform = vtkTransform::SafeDownCast(a->myelems.at(i).transformFilter->GetTransform());

		double elements1[16] = {
			1, 0, 0, 0,
			0, 0.9, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		transform->PreMultiply();
		transform->Concatenate(elements1);

		a->myelems.at(i).transformFilter->Update();*/
		//a->myelems.at(i).actor->GetProperty()->SetDiffuseColor(0, 1, 0);
		//a->myelems.at(i).actor->GetMapper()->Update();
	}

	if (this->Interactor->GetKeyCode() == 'w')	// Resize last superquad placed
	{
	}
	// Only put AsyncKeyStates/KeyStates at the end (or in update function)

	vtkInteractorStyleTrackballCamera::OnKeyPress();
}
//----------------------------------------------------------------------------------------
void MyInteractorStyle::OnMouseMove()
{
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	cellPicker->Pick(x, y, 0,  // always zero.
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	
	cellPicker->GetPickPosition(a->mouse);
	cellPicker->GetPickNormal(a->mouseNorm);

	// Gets hovered actor
	vtkSmartPointer<vtkActor> actorHovered = cellPicker->GetActor();

	vtkInteractorStyleTrackballCamera::OnMouseMove();
}
//------------------------------------------------------------------------------------------------------------
void MyInteractorStyle::OnLeftButtonDown()
{
	// ---- Checking for double clicks
	this->NumberOfClicks++;
	int pickPosition[2];
	this->GetInteractor()->GetEventPosition(pickPosition);

	int xdist = pickPosition[0] - this->PreviousPosition[0];
	int ydist = pickPosition[1] - this->PreviousPosition[1];

	this->PreviousPosition[0] = pickPosition[0];
	this->PreviousPosition[1] = pickPosition[1];

	int moveDistance = (int)sqrt((double)(xdist*xdist + ydist*ydist));

	// Reset numClicks - If mouse moved further than resetPixelDistance
	if (moveDistance > this->ResetPixelDistance)
	{
		this->NumberOfClicks = 1;
	}

	if (this->NumberOfClicks == 1)
	{
		cellPicker->Pick(this->Interactor->GetEventPosition()[0],
			this->Interactor->GetEventPosition()[1],
			0,  // always zero.
			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

		vtkSmartPointer<vtkActor> actorSingle = cellPicker->GetActor();
		if (actorSingle)
		{
			// If single clicked on actor, check if single-clicked mesh
			auto it = a->getMeshByActor(actorSingle);

			if (it != a->meshes.end())
				a->setSelectedMesh(it);
		}
	}

	if (this->NumberOfClicks == 2)
	{
		// Double clicked
		cellPicker->Pick(this->Interactor->GetEventPosition()[0],
			this->Interactor->GetEventPosition()[1],
			0,  // always zero.
			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

		vtkSmartPointer<vtkActor> actorDouble = cellPicker->GetActor();
		if (actorDouble)
		{
			// If double clicked on actor, check if dbl-clicked mesh
			auto it = a->getMeshByActor(actorDouble);

			if (it != a->meshes.end())
			{
				// if dbl clicked mesh, set it as selected (update slider/list, etc.)
				a->setSelectedMesh(it);	// Update selectedMesh to one clicked 
			}
		}
		this->NumberOfClicks = 0;
	}

	// Check if you clicked on widget then set dragging == true,

	cellPicker->Pick(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1],
		0,  // always zero.
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

	vtkSmartPointer<vtkActor> actorSingle = cellPicker->GetActor();
	if (actorSingle && GetKeyState(VK_CONTROL) & 0x1000)				// Holding Control (Creation mode)
	{
		// If actor selected is one of the CustomMesh
		auto it = a->getMeshByActor(actorSingle);

		if (it != a->meshes.end())
		{
			dragging = true;

			a->pos1[0] = cellPicker->GetPickPosition()[0];
			a->pos1[1] = cellPicker->GetPickPosition()[1];
			a->pos1[2] = cellPicker->GetPickPosition()[2];

			a->norm1[0] = cellPicker->GetPickNormal()[0];
			a->norm1[1] = cellPicker->GetPickNormal()[1];
			a->norm1[2] = cellPicker->GetPickNormal()[2];

			// only do if creation == true
		}
		// Otherwise, if clicked on widget in creation mode...
	}

	if (dragging == true) // if dragging, cannot forward events
	{
	}
	else
	{
		//return;
		// forward events (Default VTK code!)
		this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
			this->Interactor->GetEventPosition()[1]);

		if (this->CurrentRenderer == NULL)
		{
			return;
		}
		//this->GrabFocus(this->EventCallbackCommand);
		//this->GrabFocus( (vtkCommand*) vtkInteractorStyleTrackballCamera::EventCallbackCommand);
		if (this->Interactor->GetShiftKey())
		{
			if (this->Interactor->GetControlKey())
			{
				this->StartDolly();
			}
			else
			{
				this->StartPan();
			}
		}
		else
		{
			if ((GetKeyState(VK_MENU) & 0x1000))	// Alt key down
			{
				this->StartSpin();
			}
			else
			{
				this->StartRotate();
			}
		}
	}
}
//----------------------------------------------------------------------------------------------
void MyInteractorStyle::OnLeftButtonUp()
{
	if (dragging)
	{
		dragging = false;

		// check if p2 is on an actor (unless freeform mode)

		a->pos2[0] = a->mouse[0];
		a->pos2[1] = a->mouse[1];
		a->pos2[2] = a->mouse[2];

		a->norm2[0] = a->mouseNorm[0];
		a->norm2[1] = a->mouseNorm[1];
		a->norm2[2] = a->mouseNorm[2];

		auto dist = [](float pos1[3], float pos2[3]) -> float
		{
			return sqrtf(pow(pos2[2] - pos1[2], 2.0f) + pow(pos2[1] - pos1[1], 2.0f) + pow(pos2[0] - pos1[0], 2.0f));
		};

		if (dist(a->pos1, a->pos2) < 0.05)	// Not big enough superquad, return;
			return;

		// Otherwise, large enough size
		MyElem elem;

		elem.p1.point = vtkVector3f(a->pos1[0], a->pos1[1], a->pos1[2]);
		elem.p1.normal = vtkVector3f(a->norm1[0], a->norm1[1], a->norm1[2]);

		elem.p2.point = vtkVector3f(a->pos2[0], a->pos2[1], a->pos2[2]);
		elem.p2.normal = vtkVector3f(a->norm2[0], a->norm2[1], a->norm2[2]);

		elem.scale = vtkVector3f(dist(a->pos1, a->pos2), 0.1, 0.1);

		vtkSmartPointer<MySuperquadricSource> superquad = vtkSmartPointer<MySuperquadricSource>::New();
		superquad->SetToroidal(a->ui.chkToroid->isChecked());
		superquad->SetThetaResolution(16);
		superquad->SetPhiResolution(16);
		superquad->SetSize(0.5);

		//superquad->SetScale(elem.scale.GetX(), elem.scale.GetY(), elem.scale.GetZ());

		//superquad->SetThickness(5);
		superquad->SetPhiRoundness(a->ui.phiSlider->value() / a->roundnessScale);
		superquad->SetThetaRoundness(a->ui.thetaSlider->value() / a->roundnessScale);
		superquad->Update();

		//a->transform = vtkSmartPointer<vtkMatrix4x4>::New();
		//a->transform->DeepCopy(transform->GetMatrix());
		//a->transform->Transpose();

		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter->SetTransform(a->makeCompositeTransform(elem));
		transformFilter->SetInputData(superquad->GetOutput());
		transformFilter->Update();

		// TODO: superquad opacity
		elem.actor = Utility::sourceToActor(a, transformFilter->GetOutput(), 1.0, 1.0, 1.0, 0.25);
		elem.actor->PickableOff();

		elem.actor->SetUserMatrix(vtkSmartPointer<vtkMatrix4x4>::New());
		elem.actor->SetTexture(a->cutterTexture);

		elem.source = superquad;
		elem.transformFilter = transformFilter;

		a->renderer->AddActor(elem.actor);

		a->myelems.push_back(elem);

		auto addSphere = [](aperio *a, float x, float y, float z){
			float  mouseSizeDivide = 5.0;

			if (a->selectedMesh != a->meshes.end())
			{
				vtkPolyData* thepolydata = vtkPolyData::SafeDownCast(a->selectedMesh->actor->GetMapper()->GetInput());
				float diffx = thepolydata->GetBounds()[1] - thepolydata->GetBounds()[0];	// diff in maxx and minx
				float diffy = thepolydata->GetBounds()[3] - thepolydata->GetBounds()[2];	// diff in maxy and miny
				float diffz = thepolydata->GetBounds()[5] - thepolydata->GetBounds()[4];	// diff in maxz and minz

				float avg = (diffx + diffy + diffz) / 3.0;
				a->mouseSize = avg / a->brushDivide;
			}
			vtkSmartPointer<vtkSphereSource> spherec = vtkSmartPointer<vtkSphereSource>::New();
			spherec->SetRadius(a->mouseSize / mouseSizeDivide);
			spherec->Update();

			vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
			transform->Translate(x, y, z);

			vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
			transformFilter->SetInputData(spherec->GetOutput());
			transformFilter->SetTransform(transform);
			transformFilter->Update();

			vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(transformFilter->GetOutputPort());

			vtkSmartPointer<vtkActor> actors = vtkSmartPointer<vtkActor>::New();
			actors->SetMapper(mapper);
			actors->GetProperty()->SetDiffuseColor(1, 1, 0);
			actors->SetPickable(false);

			a->renderer->AddActor(actors);
		};
		//addSphere(a, a->pos1[0], a->pos1[1], a->pos1[2]);
		//addSphere(a, a->pos2[0], a->pos2[1], a->pos2[2]);
	}
	vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}
//---------------------------------------------------------------------------------------------
void MyInteractorStyle::OnMouseWheelForward()
{
	vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
}
//---------------------------------------------------------------------------------------------
void MyInteractorStyle::OnMouseWheelBackward()
{
	vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
}
//---------------------------------------------------------------------------------------------
void MyInteractorStyle::OnChar()
{
	//----- Default OnChar code

	vtkRenderWindowInteractor *rwi = this->Interactor;

	switch (rwi->GetKeyCode())
	{
	case 'f':
	case 'F':
	{
		this->AnimState = VTKIS_ANIM_ON;
		vtkAssemblyPath *path = NULL;
		this->FindPokedRenderer(rwi->GetEventPosition()[0],
			rwi->GetEventPosition()[1]);

		cellPicker->Pick(rwi->GetEventPosition()[0],
			rwi->GetEventPosition()[1],
			0.0,
			this->CurrentRenderer);

		path = cellPicker->GetPath();
		if (path != NULL)
		{
			rwi->FlyTo(this->CurrentRenderer, cellPicker->GetPickPosition());
		}
		this->AnimState = VTKIS_ANIM_OFF;
		a->resetClippingPlane();
	}
		break;
	case 'r':
	case 'R':
/*		this->FindPokedRenderer(rwi->GetEventPosition()[0],
			rwi->GetEventPosition()[1]);
		this->CurrentRenderer->ResetCamera();
		rwi->Render();*/
		break;
	}
}
//----------------------------------------------------------------------------
void MyInteractorStyle::Rotate()
{
	// ---- Forward all events from superclass (except Render call to limit framerate)

	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;

	int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
	int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

	int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

	double delta_elevation = -20.0 / size[1];
	double delta_azimuth = -20.0 / size[0];

	double rxf = dx * delta_azimuth * this->MotionFactor;
	double ryf = dy * delta_elevation * this->MotionFactor;

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	camera->Azimuth(rxf);
	camera->Elevation(ryf);
	camera->OrthogonalizeViewUp();

	if (this->AutoAdjustCameraClippingRange)
	{
		this->CurrentRenderer->ResetCameraClippingRange();
	}

	if (rwi->GetLightFollowCamera())
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	//rwi->Render();
}
//----------------------------------------------------------------------------
void MyInteractorStyle::Dolly(double factor)
{
	// ---- Forward all events from superclass (except Render call to limit framerate)

	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	if (camera->GetParallelProjection())
	{
		camera->SetParallelScale(camera->GetParallelScale() / factor);
	}
	else
	{
		camera->Dolly(factor);
		if (this->AutoAdjustCameraClippingRange)
		{
			this->CurrentRenderer->ResetCameraClippingRange();
		}
	}

	if (this->Interactor->GetLightFollowCamera())
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	//this->Interactor->Render();
}
//----------------------------------------------------------------------------
void MyInteractorStyle::Pan()
{
	// ---- Forward all events from superclass (except Render call to limit framerate)

	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;

	double viewFocus[4], focalDepth, viewPoint[3];
	double newPickPoint[4], oldPickPoint[4], motionVector[3];

	// Calculate the focal depth since we'll be using it a lot

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	camera->GetFocalPoint(viewFocus);
	this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2],
		viewFocus);
	focalDepth = viewFocus[2];

	this->ComputeDisplayToWorld(rwi->GetEventPosition()[0],
		rwi->GetEventPosition()[1],
		focalDepth,
		newPickPoint);

	// Has to recalc old mouse point since the viewport has moved,
	// so can't move it outside the loop

	this->ComputeDisplayToWorld(rwi->GetLastEventPosition()[0],
		rwi->GetLastEventPosition()[1],
		focalDepth,
		oldPickPoint);

	// Camera motion is reversed

	motionVector[0] = oldPickPoint[0] - newPickPoint[0];
	motionVector[1] = oldPickPoint[1] - newPickPoint[1];
	motionVector[2] = oldPickPoint[2] - newPickPoint[2];

	camera->GetFocalPoint(viewFocus);
	camera->GetPosition(viewPoint);
	camera->SetFocalPoint(motionVector[0] + viewFocus[0],
		motionVector[1] + viewFocus[1],
		motionVector[2] + viewFocus[2]);

	camera->SetPosition(motionVector[0] + viewPoint[0],
		motionVector[1] + viewPoint[1],
		motionVector[2] + viewPoint[2]);

	if (rwi->GetLightFollowCamera())
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	//rwi->Render();
}
//----------------------------------------------------------------------------
void MyInteractorStyle::Spin()
{
	// ---- Forward all events from superclass (except Render call to limit framerate)

	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;

	double *center = this->CurrentRenderer->GetCenter();

	double newAngle =
		vtkMath::DegreesFromRadians(atan2(rwi->GetEventPosition()[1] - center[1],
		rwi->GetEventPosition()[0] - center[0]));

	double oldAngle =
		vtkMath::DegreesFromRadians(atan2(rwi->GetLastEventPosition()[1] - center[1],
		rwi->GetLastEventPosition()[0] - center[0]));

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	camera->Roll(newAngle - oldAngle);
	camera->OrthogonalizeViewUp();

	//rwi->Render();
}
//--------------------------------------------------------------------------------------------------
vtkSmartPointer<vtkActor> MyInteractorStyle::GetOutlineActor()
{
	vtkSmartPointer<vtkInformation> information = vtkSmartPointer<vtkInformation>::New();
	information->Set(vtkMyShaderPass::OUTLINEKEY(), 0);	// dummy value
	OutlineActor->SetPropertyKeys(information);
	
	return OutlineActor;
}
