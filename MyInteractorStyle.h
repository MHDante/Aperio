// *******************************************************************************
// Interactor Style - Interactor for QVTKWidget, includes keypresses, mouse 
//					  drags, any interaction on-screen (potentially multi-touch)
//					  May split .cpp file 
// *******************************************************************************

#ifndef __MYINTERACTORSTYLE_H
#define __MYINTERACTORSTYLE_H

// Inherits from
#include "vtkInteractorStyleTrackballCamera.h"

// VTK Includes
#include <vtkQuad.h>
#include <vtkAppendPolyData.h>

// Custom
#include "vtkMyShaderPass.h"
#include "CarveConnector.h"

//-----------------------------------------------------------------------------------------
/// <summary> Class MyInteractorStyle, contains direct key press, mouse events etc.
/// </summary>
class MyInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	/// <summary> Pointer to the mainwindow class so we can access instance variables, etc. </summary>
	illustrator * a;
	vtkSmartPointer<vtkCellPicker> cellPicker;

private:
	unsigned int NumberOfClicks;	// For double clicks
	int PreviousPosition[2];
	int ResetPixelDistance;

	bool dragging;
	bool creation;
public:

	//--------------------------------------------------------------------------------------------------
	/// <summary> Creates a new instance (Factory creator declaration) - definition is outside of class
	/// </summary>
	static MyInteractorStyle* New();

	MyInteractorStyle() : NumberOfClicks(0), ResetPixelDistance(5)
	{
		this->PreviousPosition[0] = 0;
		this->PreviousPosition[1] = 0;

		this->cellPicker = vtkSmartPointer<vtkCellPicker>::New();
		this->cellPicker->SetTolerance(0.0005);

		dragging = false;
		creation = false;

		//this->PickingManagedOn();
	}
	//--------------------------------------------------------------------------------------------------
	/// <summary> Used to initialize a pointer to the main QT window
	/// </summary>
	void initialize(illustrator *window)
	{
		this->a = window;
	}
	//--------------------------------------------------------------------------------------------------
	void setPickList(int z)
	{
		//worldPicker->GetPickList()->RemoveAllItems();
		//worldPicker->AddPickList(a->meshes[z].actor);
		//worldPicker->PickFromListOn();
	}
	vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleTrackballCamera);

	/// ---------------------------------------------------------------------------------------------
	/// <summary> Called on key press (handles QT key events)
	/// </summary>
	virtual void MyInteractorStyle::OnKeyPress() override
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
			a->meshes[a->selectedIndex].opacity = 0;
			a->meshes[a->selectedIndex].actor->GetProperty()->SetOpacity(0);
			a->updateOpacitySliderAndList();
		}
		if (this->Interactor->GetKeyCode() == 'k')	// Cut
		{
			// Instead of using indices, should use a dictionary or something easily mapped
			int selectedindex = a->selectedIndex;
			int eselectedindex = a->myelems.size() - 1;

			CarveConnector connector;
			vtkSmartPointer<vtkPolyData> thepolydata(vtkPolyData::SafeDownCast(a->meshes[selectedindex].actor->GetMapper()->GetInput()));
			thepolydata = CarveConnector::cleanVtkPolyData(thepolydata);

			vtkSmartPointer<vtkPolyData> mypoly(vtkPolyData::SafeDownCast(a->myelems[eselectedindex].actor->GetMapper()->GetInput()));
			vtkSmartPointer<vtkPolyData> thepolydata2 = CarveConnector::cleanVtkPolyData(mypoly);

			// Make MeshSet from vtkPolyData
			std::unique_ptr<carve::mesh::MeshSet<3> > first(CarveConnector::vtkPolyDataToMeshSet(thepolydata));
			std::unique_ptr<carve::mesh::MeshSet<3> > second(CarveConnector::vtkPolyDataToMeshSet(thepolydata2));
			//std::unique_ptr<carve::mesh::MeshSet<3> > second(CarveConnector::makeCube(55, carve::math::Matrix::IDENT()));

			std::unique_ptr<carve::mesh::MeshSet<3> > c(CarveConnector::performDifference(first, second));
			vtkSmartPointer<vtkPolyData> c_poly(CarveConnector::meshSetToVTKPolyData(c));
			//Utility::generateTexCoords(c_poly);

			vtkSmartPointer<vtkPolyDataNormals> dataset = vtkSmartPointer<vtkPolyDataNormals>::New();
			dataset->SetInputData(c_poly);
			dataset->ComputePointNormalsOn();
			dataset->ComputeCellNormalsOff();
			dataset->SplittingOn();
			dataset->SetFeatureAngle(60);
			dataset->Update();

			// Update cell locator's dataset so program doesn't slow down after cutting
			a->meshes[selectedindex].cellLocator->SetDataSet(dataset->GetOutput());

			// Create a mapper and actor
			vtkSmartPointer<vtkActor> actor = Utility::sourceToActor(dataset->GetOutput(), a->meshes[selectedindex].color.r,
				a->meshes[selectedindex].color.g,
				a->meshes[selectedindex].color.b,
				a->meshes[selectedindex].opacity >= 1 ? 1 : a->meshes[selectedindex].opacity * 0.5f);	// My opacity (Must change 0.5f)

			// Now replace mesh too
			a->renderer->GetViewProps()->ReplaceItem(selectedindex, actor);
			a->meshes[selectedindex].actor = actor;

			// Remove superquadric 
			a->renderer->RemoveActor(a->myelems[eselectedindex].actor);

			// Probably should remove from list as well (myelems)
			a->myelems.erase(a->myelems.end() - 1);
		}
		if (this->Interactor->GetKeyCode() == '+')	// bigger peek brush
		{
			a->brushDivide--;
			this->OnMouseMove();
		}
		if (this->Interactor->GetKeyCode() == '-')	// smaller peek brush
		{
			a->brushDivide++;
			this->OnMouseMove();
		}
		if (this->Interactor->GetKeyCode() == 't')	// Toggle peek
		{
			a->peerInside = (a->peerInside + 1) % 2;
		}

		if (this->Interactor->GetKeyCode() == 'c')	// change roundness (-phi)
		{
			float change = 0.1;

			if (a->myn - change >= 0)
				a->myn -= change;

			a->superquad->SetPhiRoundness(a->myn);
			a->superquad->Update();
		}
		if (this->Interactor->GetKeyCode() == 'v')	// change roundness (phi)
		{
			float change = 0.1;

			if (a->myn + change <= 20)
				a->myn += change;

			a->superquad->SetPhiRoundness(a->myn);
			a->superquad->Update();
		}
		if (this->Interactor->GetKeyCode() == 'z')	// change roundness (-theta)
		{
			float change = 0.1;

			if (a->myexp - change >= 0)
				a->myexp -= change;

			a->superquad->SetThetaRoundness(a->myexp);
			a->superquad->Update();
		}
		if (this->Interactor->GetKeyCode() == 'x')	// change roundness (theta)
		{
			float change = 0.1;

			if (a->myexp + change <= 20)
				a->myexp += change;

			a->superquad->SetThetaRoundness(a->myexp);
			a->superquad->Update();
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

				vtkSmartPointer<vtkActor> sactor = Utility::sourceToActor(a->superquad->GetOutput(), 1, 0, 0, 1);
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

		//------------- Last superquad --------------------------------------------------------------------------
		if (this->Interactor->GetKeyCode() == 'u' && a->myelems.size() > 0)	// Resize last superquad placed
		{
			int i = a->myelems.size() - 1;
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
			a->myelems.at(i).actor->GetProperty()->SetDiffuseColor(0, 1, 0);
			a->myelems.at(i).actor->GetMapper()->Update();
		}
		if (this->Interactor->GetKeyCode() == 'o' && a->myelems.size() > 0)	// Resize last superquad placed
		{
			int i = a->myelems.size() - 1;
			vtkTransform* transform = vtkTransform::SafeDownCast(a->myelems.at(i).transformFilter->GetTransform());

			double elements1[16] = {
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1.1, 0,
				0, 0, 0, 1
			};
			transform->PreMultiply();
			transform->Concatenate(elements1);

			a->myelems.at(i).transformFilter->Update();
			a->myelems.at(i).actor->GetProperty()->SetDiffuseColor(0, 1, 0);
			a->myelems.at(i).actor->GetMapper()->Update();
		}
		if (this->Interactor->GetKeyCode() == 'e' && a->myelems.size() > 0)		// Resize last superquad placed
		{
			int i = a->myelems.size() - 1;
			vtkTransform* transform = vtkTransform::SafeDownCast(a->myelems.at(i).transformFilter->GetTransform());

			double elements1[16] = {
				1, 0, 0, 0,
				0, 1.1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			};
			transform->PreMultiply();
			transform->Concatenate(elements1);

			a->myelems.at(i).transformFilter->Update();
			a->myelems.at(i).actor->GetProperty()->SetDiffuseColor(0, 1, 0);
			a->myelems.at(i).actor->GetMapper()->Update();
		}
		if (this->Interactor->GetKeyCode() == 'q' && a->myelems.size() > 0)	// Resize last superquad placed
		{
			int i = a->myelems.size() - 1;
			vtkTransform* transform = vtkTransform::SafeDownCast(a->myelems.at(i).transformFilter->GetTransform());

			double elements1[16] = {
				1, 0, 0, 0,
				0, 0.9, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			};
			transform->PreMultiply();
			transform->Concatenate(elements1);

			a->myelems.at(i).transformFilter->Update();
			a->myelems.at(i).actor->GetProperty()->SetDiffuseColor(0, 1, 0);
			a->myelems.at(i).actor->GetMapper()->Update();
		}

		if (this->Interactor->GetKeyCode() == 'w')	// Resize last superquad placed
		{
		}
		vtkInteractorStyleTrackballCamera::OnKeyPress();
	}

	//----------------------------------------------------------------------------
	/// <summary>
	/// Called when [mouse move].
	/// </summary>
	/// ----------------------------------------------------------------------------
	virtual void OnMouseMove()  override
	{
		int x = this->Interactor->GetEventPosition()[0];
		int y = this->Interactor->GetEventPosition()[1];

		cellPicker->Pick(this->Interactor->GetEventPosition()[0],
			this->Interactor->GetEventPosition()[1], 0,  // always zero.
			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

		cellPicker->GetPickPosition(a->mouse);
		cellPicker->GetPickNormal(a->mouseNorm);

		// Gets hovered actor
		vtkSmartPointer<vtkActor> actorHovered = cellPicker->GetActor();

		vtkInteractorStyleTrackballCamera::OnMouseMove();
	}

	//----------------------------------------------------------------------------
	/// <summary>
	/// Called when [mouse left button click].
	/// </summary>
	/// ----------------------------------------------------------------------------
	virtual void OnLeftButtonDown() override
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
				auto it = std::find_if(a->meshes.begin(), a->meshes.end(),
					[=](CustomMesh &c) { return actorSingle.GetPointer() == c.actor.GetPointer(); });

				if (it != a->meshes.end())
				{
					a->meshes[a->selectedIndex].selected = 0;
					it->selected = 1;

					int newindex = it._Ptr - &a->meshes[0];
					a->selectedIndex = newindex;

					a->updateOpacitySliderAndList();
				}
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
				auto it = std::find_if(a->meshes.begin(), a->meshes.end(),
					[=](CustomMesh &c) { return c.actor.GetPointer() == actorDouble.GetPointer(); });

				if (it != a->meshes.end())
				{
					// if dbl clicked mesh, set it as selected (update slider/list, etc.)
					a->meshes[a->selectedIndex].selected = 0;
					it->selected = 1;

					int newindex = it._Ptr - &a->meshes[0];
					a->selectedIndex = newindex;

					a->updateOpacitySliderAndList();
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
			auto it = std::find_if(a->meshes.begin(), a->meshes.end(),
				[=](CustomMesh &c) { return actorSingle.GetPointer() == c.actor.GetPointer(); });

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
	////----------------------------------------------------------------------------
	/// <summary> Called when [left button up].
	/// </summary>
	virtual void OnLeftButtonUp() override
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

			vtkSmartPointer<MySuperquadricSource> superquad = vtkSmartPointer<MySuperquadricSource>::New();
			superquad->SetToroidal(false);
			superquad->SetThetaResolution(32);
			superquad->SetPhiResolution(32);

			superquad->SetPhiRoundness(0.25);
			superquad->SetThetaRoundness(0.25);
			superquad->Update();

			// Get forward/up/right vectors (to orient superquad)
			vtkVector3f forward = vtkVector3f((elem.p1.normal.GetX() + elem.p2.normal.GetX()) / 2.0f,
				(elem.p1.normal.GetY() + elem.p2.normal.GetY()) / 2.0f,
				(elem.p1.normal.GetZ() + elem.p2.normal.GetZ()) / 2.0f
				);
			forward.Normalize();

			vtkVector3f right = vtkVector3f(
				elem.p2.point.GetX() - elem.p1.point.GetX(),
				elem.p2.point.GetY() - elem.p1.point.GetY(),
				elem.p2.point.GetZ() - elem.p1.point.GetZ()
				);
			right.Normalize();

			// up = cross product of right and forward
			vtkVector3f up = forward.Cross(right);
			up.Normalize();

			// vtk does row-major matrix operations
			vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

			double elements3[16] = {
				1, 0, 0, (elem.p1.point.GetX() + elem.p2.point.GetX()) / 2.0f,
				0, 1, 0, (elem.p1.point.GetY() + elem.p2.point.GetY()) / 2.0f,
				0, 0, 1, (elem.p1.point.GetZ() + elem.p2.point.GetZ()) / 2.0f,
				0, 0, 0, 1
			};

			//float rads = vtkMath::RadiansFromDegrees(45.0);
			double elements2[16] = {
				right.GetX(), up.GetX(), forward.GetX(), 0,
				right.GetY(), up.GetY(), forward.GetY(), 0,
				right.GetZ(), up.GetZ(), forward.GetZ(), 0,
				0, 0, 0, 1
			};

			double elements1[16] = {
				dist(a->pos1, a->pos2), 0, 0, 0,
				0, .1, 0, 0,
				0, 0, .1, 0,
				0, 0, 0, 1
			};
			transform->SetMatrix(elements1);

			transform->PostMultiply();
			transform->Concatenate(elements2);
			transform->Concatenate(elements3);

			vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
			transformFilter->SetTransform(transform);
			transformFilter->SetInputData(superquad->GetOutput());
			transformFilter->Update();

			elem.actor = Utility::sourceToActor(transformFilter->GetOutput(), 1, 0.5, 0.5, .5);
			elem.actor->PickableOff();

			elem.source = superquad;
			elem.transformFilter = transformFilter;

			a->renderer->AddActor(elem.actor);

			a->myelems.push_back(elem);

			auto addSphere = [](illustrator *a, float x, float y, float z){
				float  mouseSizeDivide = 5.0;

				if (a->meshes.size() > 0 && a->selectedIndex != -1)
				{
					vtkPolyData* thepolydata = vtkPolyData::SafeDownCast(a->meshes.at(a->selectedIndex).actor->GetMapper()->GetInput());
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
			addSphere(a, a->pos1[0], a->pos1[1], a->pos1[2]);
			addSphere(a, a->pos2[0], a->pos2[1], a->pos2[2]);
		}
		vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
	}
	//----------------------------------------------------------------------------
	virtual void OnMouseWheelForward()  override
	{
		vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
	}
	//----------------------------------------------------------------------------
	virtual void OnMouseWheelBackward() override
	{
		vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
	}
	//----------------------------------------------------------------------------
	virtual void OnChar() override
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
		}
			break;
		case 'r':
		case 'R':
			break;
			this->FindPokedRenderer(rwi->GetEventPosition()[0],
				rwi->GetEventPosition()[1]);
			this->CurrentRenderer->ResetCamera();
			rwi->Render();
			break;
		}
	}
	//--------------------------------------------------------------------------------------------
};
vtkStandardNewMacro(MyInteractorStyle);
#endif