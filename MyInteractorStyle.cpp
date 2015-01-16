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

#include <vtkIntersectionPolyDataFilter.h>
#include <vtkCubeSource.h>
#include <vtkMath.h>

#include "MySuperquadricSource.h"

vtkStandardNewMacro(MyInteractorStyle);

//---------------------------------------------------------------------------------------------------------------
MyInteractorStyle::MyInteractorStyle() : NumberOfClicks(0), NumberOfClicksR(0), ResetPixelDistance(5)
{
	this->PreviousPosition[0] = 0;
	this->PreviousPosition[1] = 0;

	this->PreviousPositionR[0] = 0;
	this->PreviousPositionR[1] = 0;

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
	char keypressed = tolower(this->Interactor->GetKeyCode());

	auto toolTip = a->toolTip.lock();

	// CTRL+Z
	if ((GetKeyState(VK_CONTROL) & 0x1000) && (GetKeyState('Z') & 0x1000) && a->selectedMeshes.size() > 0)
	{
		a->removeSelectedMesh(a->selectedMeshes.size() - 1);
	}

	// Plant the object
	if (GetKeyState(VK_RETURN) & 0x1000)
	{
		a->slot_btnPlant();
	}

	if (GetKeyState(VK_DELETE) & 0x1000)
	{
		a->removeElem(a->toolTip);
		a->toolTip.reset();
		a->toolTipOn = false;

		a->setCursor(true);
	}


	if (GetKeyState(VK_SPACE) & 0x1000 && a->snapToBBOXReal)
	{
		if (a->spinAngle_BBOX <= 0)
			a->spinAngle_BBOX = 90;
		else if (a->spinAngle_BBOX >= 90)
			a->spinAngle_BBOX = 0;

		if (!a->selectedMeshes.empty() && toolTip != nullptr)
		{
			auto selectedMesh = a->selectedMeshes.back().lock();

			vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = toolTip->transformFilter;
			transformFilter->SetTransform(a->makeCompositeTransformFromSinglePoint(*toolTip, selectedMesh));
			transformFilter->Update();
		}
		OnMouseMove();
	}
	
	if (GetKeyState(VK_BACK) & 0x1000)
	{
		a->slot_btnRestore();
	}

	// Show all
	if (GetKeyState(VK_MENU) & 0x1000 && GetKeyState('H') & 0x1000)
	{
		for (auto &mesh : a->meshes)
		{
			Utility::setMeshOpacity(a, mesh, 1.0);
		}
		a->updateOpacitySliderAndList();
	}

	// Hide all
	else if (GetKeyState(VK_SHIFT) & 0x1000 && GetKeyState('H') & 0x1000)
	{
		for (auto &mesh : a->meshes)
		{
			Utility::setMeshOpacity(a, mesh, 0.0);
		}
		a->updateOpacitySliderAndList();
	}
	else if (GetKeyState('H') & 0x1000)	// Holding H alone
	{
		a->slot_btnHide();
	}


	// Scroll through list widget with page up/down
	bool pageup = GetKeyState(VK_PRIOR) & 0x1000;
	bool pagedown = GetKeyState(VK_NEXT) & 0x1000;
	auto listWidget = a->ui.listWidget;

	if ((pagedown || pageup) && listWidget->count() > 1)	// Only scroll if at least two items
	{
		if (listWidget->selectedItems().count() > 0)		// Only scroll if an item selected
		{
			auto selectedItem = listWidget->selectedItems().first();
			int row = listWidget->row(selectedItem);

			int change = 1;
			if (pageup)
				change = -1;

			if (row + change >= listWidget->count() || row + change < 0)	// out of bounds
				change = 0;

			int nextrow = (row + change) % listWidget->count();
			auto nextitem = listWidget->item(nextrow);

			a->slot_listitemclicked(nextitem);
		}
	}

	
	//if (strcmp(this->Interactor->GetKeySym(), "Delete") == 0)	// Del key	 ( probably should put this in QT keycheck, b/c QVTKWidget requires focus)
	if (keypressed == 'c')	// Rod for linear exploding
	{
		a->slot_btnCookie();
	}
	if (keypressed == 'g')	// Pick up
	{
		a->slot_btnPickUp();
	}

	float step = 1;

	if (keypressed == 'r')	// Toggle Ribbons
	{
		a->slot_btnRibbons();
	}
	if (keypressed == 't')	// Toggle peek
	{
		a->slot_btnGlass();
	}
	if (keypressed == '0')		// Change shading model
	{
		a->shadingnum = (a->shadingnum + 1) % 2;
	}
	float thestep = 0.05;

	if (keypressed == 'z' )	// Show elements
	{	
		cout << "\n--- Elements ---\n\n";
		for (auto &elem : a->myelems)
		{
			if (elem != nullptr)
			{
				if (elem->toolType == CUTTER)
					cout << "CUTTER\n";
				if (elem->toolType == KNIFE)
					cout << "KNIFE\n";
				if (elem->toolType == ROD)
					cout << "ROD\n";
				if (elem->toolType == RING)
					cout << "RING\n";
				if (elem->toolType == HINGE)
					cout << "HINGE\n";
			}				
		}
	}
	// Only put AsyncKeyStates/KeyStates at the end (or in update function)

	vtkInteractorStyleTrackballCamera::OnKeyPress();
}
//----------------------------------------------------------------------------------------
void MyInteractorStyle::OnMouseMove()
{
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];
	int prevx = this->Interactor->GetLastEventPosition()[0];
	int prevy = this->Interactor->GetLastEventPosition()[1];

	cellPicker->Pick(x, y, 0,this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	cellPicker->GetPickPosition(a->mouse);
	cellPicker->GetPickNormal(a->mouseNorm);

	auto toolTip = a->toolTip.lock();

	if (toolTip)
	{
		a->makeOutline(toolTip);
	}
	if (a->toolTipOn && toolTip)
	{
		
		if (GetKeyState(VK_MENU) & 0x1000)
		{
			// Spinning in Cutter and Ring is okay (No Rods and No Knife)
			if (toolTip->toolType != ROD && toolTip->toolType != KNIFE)
				toolTip->spinAngle += 0.02*(y - prevy);
		}

		// Move along a plane defined by last picked point and view plane normal
		else if ((GetKeyState(VK_CAPITAL) & 0x0001) && !this->Interactor->GetControlKey())
		{
			double viewFocus[4], focalDepth, viewNormal[4];
			double newPickPoint[4], oldPickPoint[4], motionVector[3];

			vtkRenderWindowInteractor *rwi = this->Interactor;
			
			this->ComputeWorldToDisplay(toolTip->p1.point.GetX(), toolTip->p1.point.GetY(), toolTip->p1.point.GetZ(), viewFocus);
			focalDepth = viewFocus[2];
			this->ComputeDisplayToWorld(rwi->GetEventPosition()[0], rwi->GetEventPosition()[1], focalDepth, newPickPoint);
			this->ComputeDisplayToWorld(rwi->GetLastEventPosition()[0], rwi->GetLastEventPosition()[1], focalDepth, oldPickPoint);
			motionVector[0] = newPickPoint[0] - oldPickPoint[0];
			motionVector[1] = newPickPoint[1] - oldPickPoint[1];
			motionVector[2] = newPickPoint[2] - oldPickPoint[2];
			vtkVector3f mv = vtkVector3f(motionVector[0], motionVector[1], motionVector[2]);
			if (a->selectedMeshes.size() == 0)
				return;

			auto selectedMesh = a->selectedMeshes.back().lock();
			/*
			float deltaR= mv.Dot(a->toolTip.right);
			float deltaU= mv.Dot(a->toolTip.up);
			float deltaF = mv.Dot(a->toolTip.forward);
			*/
			
			if (toolTip->toolType == RING)
			{

				float deltaR = mv.Dot(selectedMesh->axesOBB[0]);
				float deltaU = mv.Dot(selectedMesh->axesOBB[1]);
				float deltaF = mv.Dot(selectedMesh->axesOBB[2]);
				vtkVector3f delta;
				if (this->Interactor->GetShiftKey())
				{
					delta.SetX(deltaF*selectedMesh->axesOBB[2].GetX());
					delta.SetY(deltaF*selectedMesh->axesOBB[2].GetY());
					delta.SetZ(deltaF*selectedMesh->axesOBB[2].GetZ());
					/*
					delta.SetX(deltaF*a->toolTip.forward.GetX());
					delta.SetY(deltaF*a->toolTip.forward.GetY());
					delta.SetZ(deltaF*a->toolTip.forward.GetZ());
					*/
				}
				else
				{
					delta.SetX(deltaR*selectedMesh->axesOBB[0].GetX() + deltaU*selectedMesh->axesOBB[1].GetX());
					delta.SetY(deltaR*selectedMesh->axesOBB[0].GetY() + deltaU*selectedMesh->axesOBB[1].GetY());
					delta.SetZ(deltaR*selectedMesh->axesOBB[0].GetZ() + deltaU*selectedMesh->axesOBB[1].GetZ());
					/*
					delta.SetX(deltaR*a->toolTip.right.GetX() + deltaU*a->toolTip.up.GetX());
					delta.SetY(deltaR*a->toolTip.right.GetY() + deltaU*a->toolTip.up.GetY());
					delta.SetZ(deltaR*a->toolTip.right.GetZ() + deltaU*a->toolTip.up.GetZ());
					*/
				}
				toolTip->p1.point = toolTip->p1.point + delta;
				toolTip->p2.point = toolTip->p2.point + delta;

				//toolTip->p1.normal = vtkVector3f(selectedMesh->axesOBB[0].GetX(),
				//	selectedMesh->axesOBB[0].GetY(),
				//	selectedMesh->axesOBB[0].GetZ());
				
				//toolTip->p2.normal = vtkVector3f(selectedMesh->axesOBB[0].GetX(),
					//selectedMesh->axesOBB[0].GetY(),
					//selectedMesh->axesOBB[0].GetZ());

			}
			else
			{
				toolTip->p1.point = toolTip->p1.point + vtkVector3f(motionVector[0], motionVector[1], motionVector[2]);
				toolTip->p2.point = toolTip->p2.point + vtkVector3f(motionVector[0], motionVector[1], motionVector[2]);
	
				vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
				camera->GetViewPlaneNormal(viewNormal);
				toolTip->p1.normal = vtkVector3f(viewNormal[0], viewNormal[1], viewNormal[2]);
				toolTip->p2.normal = vtkVector3f(viewNormal[0], viewNormal[1], viewNormal[2]);
			}
		}
		// Slide along mesh surface
		else
		{
			// Only slide if NOT in widget select mode and NOT in knife mode
			// (otherwise it'll slide on the widget's surface)
			// Since it is pickable during widget select mode
			if (!a->widgetSelectMode && toolTip->toolType != KNIFE)
			{
				avgNormal(a->mouseNorm, a->mouseNorm);
				toolTip->p1.point = vtkVector3f(a->mouse[0], a->mouse[1], a->mouse[2]);
				toolTip->p1.normal = vtkVector3f(a->mouseNorm[0], a->mouseNorm[1], a->mouseNorm[2]);
				toolTip->p2.point = vtkVector3f(a->mouse[0], a->mouse[1], a->mouse[2]);
				toolTip->p2.normal = vtkVector3f(a->mouseNorm[0], a->mouseNorm[1], a->mouseNorm[2]);
			}
		}

		/*shared_ptr<CustomMesh> selectedMesh;
		if (a->selectedMeshes.size() != 0)
			selectedMesh = a->selectedMeshes.back().lock();
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = a->toolTip.transformFilter;
		transformFilter->SetTransform(a->makeCompositeTransformFromSinglePoint(a->toolTip,nullptr));
		transformFilter->Update();*/

		if (toolTip->toolType != KNIFE)
		{
			vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = toolTip->transformFilter;
			transformFilter->SetTransform(a->makeCompositeTransformFromSinglePoint(*toolTip));
			transformFilter->Update();


			if (!a->selectedMeshes.empty())
			{
				auto selectedMesh = a->selectedMeshes.back().lock();

				//std::cout << "switched" << rand();
				//if (GetKeyState(VK_MENU) & 0x1000)
				if (a->snapToBBOXReal)
				{
					transformFilter->SetTransform(a->makeCompositeTransformFromSinglePoint(*toolTip, selectedMesh));
					transformFilter->Update();
				}				
			}
		}

		if (dragging && toolTip->toolType == KNIFE)
		{
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

			// Otherwise, large enough size - check if first time
			if (toolTip == nullptr)
			{
				a->createIncisionElement();
				return;
			}
			else
			{
				
				toolTip->p1.point = vtkVector3f(a->pos1[0], a->pos1[1], a->pos1[2]);
				toolTip->p1.normal = vtkVector3f(a->norm1[0], a->norm1[1], a->norm1[2]);
				toolTip->p2.point = vtkVector3f(a->pos2[0], a->pos2[1], a->pos2[2]);
				toolTip->p2.normal = vtkVector3f(a->norm2[0], a->norm2[1], a->norm2[2]);
				toolTip->scale = vtkVector3f(dist(a->pos1, a->pos2), 0.1, 0.1);

				toolTip->source->SetThetaRoundness(0);
				toolTip->scale.SetY(10000);

				vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = toolTip->transformFilter;
				transformFilter->SetTransform(a->makeCompositeTransform(*toolTip));
				//transformFilter->SetInputData(superquad->GetOutput());
				transformFilter->Update();
				return;
			}
		}
	}
	// Gets hovered actor
	vtkSmartPointer<vtkActor> actorHovered = cellPicker->GetActor();

	vtkInteractorStyleTrackballCamera::OnMouseMove();
}
//------------------------------------------------------------------------------------------------------------
void MyInteractorStyle::OnLeftButtonDown()
{
	bool cuttermode = false;

	auto toolTip = a->toolTip.lock();

	if (a->toolTipOn && toolTip && (toolTip->toolType == CUTTER || toolTip->toolType == KNIFE))
		cuttermode = true;

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

	if (this->NumberOfClicks == 2)
	{
		// turn off picking of already selected meshes (to allow clicking inner meshes)
		if (cuttermode)
		{
			for (auto &mesh : a->meshes)
			{
				if (mesh->selected == true)
					mesh->actor->SetPickable(false);
			}
		}

		// Double clicked
		cellPicker->Pick(this->Interactor->GetEventPosition()[0],
			             this->Interactor->GetEventPosition()[1],
			             0,  // always zero.
			             this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

		vtkSmartPointer<vtkActor> actorDouble = cellPicker->GetActor();
		
		// turn back on picking of already selected meshes
		if (cuttermode)
		{
			for (auto &mesh : a->meshes)
			{
				if (mesh->selected == true)
					mesh->actor->SetPickable(true);
			}
		}


		if (actorDouble)
		{
			// If double clicked on actor, check if dbl-clicked mesh
			auto it = a->getMeshByActor(actorDouble).lock();

			// if mesh double clicked on and already selected and knife tool tip and this mesh has incision
			// then do Carve, check for pieces etc (see btn_slice())
			
			if (it != nullptr)
			{

				if (GetKeyState(VK_SHIFT) & 0x1000)
				{
					a->addSelectedMesh(it);
					
				}
				else
				{
					a->clearSelectedMeshes();
					a->addSelectedMesh(it);
				}
				// Add to selectedMeshes list
				
			}
			else
			{
				// Not a mesh did you select a widget?
			}
		}
		this->NumberOfClicks = 0;
	}
	/*
	// Check if you clicked on widget then set dragging == true,
	cellPicker->Pick(this->Interactor->GetEventPosition()[0],
		             this->Interactor->GetEventPosition()[1],
		             0,  // always zero.
		             this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

	vtkSmartPointer<vtkActor> actorSingle = cellPicker->GetActor();
	*/

	// Double clicked
	cellPicker->Pick(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1],
		0,  // always zero.
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

	vtkSmartPointer<vtkActor> actorSingle = cellPicker->GetActor();

	// Not a mesh, did you double click on a widget?
	auto it = a->getElemByActorRaw(actorSingle.Get()).lock();

	if (it != nullptr && a->widgetSelectMode)
	{
		// It's a widget! Set activetooltip to this widget again
		a->toolTip = it;
		a->toolTipOn = true;

		// Also remove widget actor from renderer and re-add (last element in vtkRenderer)
		// So that it is the last one that gets the discard properties in shader
		a->renderer->RemoveActor(it->actor);
		a->renderer->AddActor(it->actor);

		// move to end of myelems
		auto tomove = a->getElemIterator(it);
		a->myelems.erase(tomove);
		a->myelems.push_back(it);

		//DOUBLECLICK()

		// Reset positions of all meshes with this widget reference (AND their elem ref too)
		// ALSO only select the meshes that are associated to the rod/ring
		//a->selectedMeshes.clear();

		for (auto &mesh : a->meshes)
		{
			if (mesh->elem.lock() == a->toolTip.lock())
			{
				mesh->actor->SetUserMatrix(vtkSmartPointer<vtkMatrix4x4>::New());
				mesh->elem = weak_ptr<MyElem>();

				//a->addSelectedMesh(mesh);
			}
		}
		a->setCursor(false);
		a->setWidgetSelectMode(false);
	}


	if (a->toolTipOn && toolTip)
	{
		// Tooltips is on (pressing M with wheel will bypass superquad code and perform default rotation)
		//if (GetKeyState('M') & 0x1000)
		if (GetKeyState('M') & 0x1000 || GetKeyState(VK_TAB) & 0x1000)
		{
			forwardLeftButtonDown();
			return;
		}

		if (toolTip->toolType != KNIFE)
		{
			forwardLeftButtonDown();
			//return;
		}

		toolTip->p1.point = vtkVector3f(cellPicker->GetPickPosition()[0],
			cellPicker->GetPickPosition()[1],
			cellPicker->GetPickPosition()[2]);
		toolTip->p1.normal = vtkVector3f(cellPicker->GetPickNormal()[0],
			cellPicker->GetPickNormal()[1],
			cellPicker->GetPickNormal()[2]);

		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = toolTip->transformFilter;
		transformFilter->SetTransform(a->makeCompositeTransformFromSinglePoint(*toolTip));
		//transformFilter->SetInputData(superquad->GetOutput());
		transformFilter->Update();

		if (toolTip->toolType == KNIFE) //GetKeyState(VK_CONTROL) & 0x1000)
		{
			// If actor selected is one of the CustomMesh
			//auto it = a->getMeshByActor(actorSingle);

			//if (it != a->meshes.end() && it->selected)
			//{
			dragging = true;

			a->pos1[0] = cellPicker->GetPickPosition()[0];
			a->pos1[1] = cellPicker->GetPickPosition()[1];
			a->pos1[2] = cellPicker->GetPickPosition()[2];

			a->norm1[0] = cellPicker->GetPickNormal()[0];
			a->norm1[1] = cellPicker->GetPickNormal()[1];
			a->norm1[2] = cellPicker->GetPickNormal()[2];

			// only do if creation == true
			//}
			// Otherwise, if clicked on widget in creation mode...
		}
		return;
	}

	if (dragging == true) // if dragging, cannot forward events
	{
	}
	// camera control
	else
	{
		//return;
		forwardLeftButtonDown();
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
		if (a->myelems.empty())
			return;
		
		auto toolTip = a->toolTip.lock();
		
		if (toolTip)
		{
			toolTip->p1.point = vtkVector3f(a->pos1[0], a->pos1[1], a->pos1[2]);
			toolTip->p1.normal = vtkVector3f(a->norm1[0], a->norm1[1], a->norm1[2]);
			toolTip->p2.point = vtkVector3f(a->pos2[0], a->pos2[1], a->pos2[2]);
			toolTip->p2.normal = vtkVector3f(a->norm2[0], a->norm2[1], a->norm2[2]);
			
			//toolTip->scale = vtkVector3f(dist(a->pos1, a->pos2), 0.1, 0.1);
			toolTip->scale.SetX(10000);	// 10000

			vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = toolTip->transformFilter;
			transformFilter->SetTransform(a->makeCompositeTransform(*toolTip));
			//transformFilter->SetInputData(superquad->GetOutput());
			transformFilter->Update();
		}
	}
	vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}

void MyInteractorStyle::OnRightButtonDown()
{
	// ---- Checking for double clicks
	this->NumberOfClicksR++;
	int pickPosition[2];
	this->GetInteractor()->GetEventPosition(pickPosition);

	int xdist = pickPosition[0] - this->PreviousPositionR[0];
	int ydist = pickPosition[1] - this->PreviousPositionR[1];

	this->PreviousPositionR[0] = pickPosition[0];
	this->PreviousPositionR[1] = pickPosition[1];

	int moveDistance = (int)sqrt((double)(xdist*xdist + ydist*ydist));

	// Reset numClicks - If mouse moved further than resetPixelDistance
	if (moveDistance > this->ResetPixelDistance)
	{
		this->NumberOfClicksR = 1;
	}

	if (this->NumberOfClicksR == 2 && clock() - lastClickTimeR < 400)
	{
		for (auto &mesh : a->meshes)
		{
			mesh->selected = false;
		}

		a->clearSelectedMeshes();

		this->NumberOfClicksR = 0;
	}

	// Otherwise, single click
	lastClickTimeR = clock();

	cellPicker->Pick(this->Interactor->GetEventPosition()[0],
			            this->Interactor->GetEventPosition()[1],
						0,  // always zero.
			            this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

	vtkSmartPointer<vtkActor> actorSingle = cellPicker->GetActor();
	if (actorSingle)
	{
		// If single clicked on actor, check if single-clicked mesh
		auto it = a->getMeshByActor(actorSingle).lock();

		if (it != nullptr)
		{
			// Remove from selectedMeshes list
			a->removeSelectedMesh(it);
		}
	}
	
}
float wheelspeed = 0.005;
float deepness = 3;

//---------------------------------------------------------------------------------------------
void MyInteractorStyle::OnMouseWheelForward()
{

	auto toolTip = a->toolTip.lock();

	if (a->toolTipOn && toolTip)
	{
		// Tooltips is on (pressing M with wheel will bypass superquad code and perform default zooming)
		//if (GetKeyState('M') & 0x1000)
		if (GetKeyState(VK_MENU) & 0x1000 || GetKeyState(VK_SHIFT) & 0x1000 || GetKeyState(VK_CONTROL) & 0x1000
			|| GetKeyState(VK_UP) & 0x1000 || GetKeyState(VK_DOWN) & 0x1000 || GetKeyState(VK_RIGHT) & 0x1000 || GetKeyState(VK_LEFT) & 0x1000
			)
		{
		}
		else
		{
			vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
			return;
		}
		
		MyElem *currentElem;
		bool incisionElem = false;
		if (a->myelems.size() > 0 && toolTip->toolType == KNIFE)
		{
			currentElem = a->myelems[a->myelems.size() - 1].get();
			incisionElem = true;
		}
		else
			currentElem = toolTip.get();
		
		if (GetKeyState(VK_MENU) & 0x1000 && this->Interactor->GetShiftKey() && toolTip->toolType == RING)
			//((strcmp(this->Interactor->GetKeySym(), "z") == 0)||
			                                  //(strcmp(this->Interactor->GetKeySym(), "Z") == 0)))
		{
			currentElem->spinFlipped += this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
		}
		
		else if (GetKeyState(VK_MENU) & 0x1000 && this->Interactor->GetShiftKey() && toolTip->toolType == ROD)
		{
			currentElem->tilt += this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
		}
		
		else if (GetKeyState(VK_MENU) & 0x1000)
		{
			// Tilt available for rod and ring
			if (toolTip->toolType == ROD || toolTip->toolType == RING)
				currentElem->tilt += this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
		}
		else if ((GetKeyState(VK_CONTROL) & 0x1000) && (GetKeyState(VK_SHIFT) & 0x1000))
		{			
			currentElem->scale.SetY(currentElem->scale.GetY() + this->MotionFactor * deepness * wheelspeed * this->MouseWheelMotionFactor);
			
		}
		else if (GetKeyState(VK_CONTROL) & 0x1000)
		{
			currentElem->scale.SetZ(currentElem->scale.GetZ() + this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor);
			currentElem->scale.SetX(currentElem->scale.GetX() + this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor);
			
		}
		else if (this->Interactor->GetShiftKey())
		{
			double sf = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
			
			if (GetKeyState(VK_CAPITAL) & 0x0001)
			{
				double viewNormal[4];
				vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
				camera->GetViewPlaneNormal(viewNormal);
				currentElem->p1.normal = vtkVector3f(viewNormal[0], viewNormal[1], viewNormal[2]);
				currentElem->p2.normal = vtkVector3f(viewNormal[0], viewNormal[1], viewNormal[2]);
			}
			currentElem->p1.point = currentElem->p1.point - sf * currentElem->p1.normal;
			currentElem->p2.point = currentElem->p2.point - sf * currentElem->p2.normal;
		}
		else if (
			((GetKeyState(VK_UP) & 0x1000) && (GetKeyState(VK_DOWN) & 0x1000)) ||
			((GetKeyState('W') & 0x1000) && (GetKeyState('S') & 0x1000)) 
		)
		{
			double delta = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
			currentElem->scale.SetX(currentElem->scale.GetX() + delta);
		}
		
		else if (GetKeyState(VK_UP) & 0x1000 ||
				 GetKeyState('W') & 0x1000
		)
		{
			double delta = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
			currentElem->p1.point = currentElem->p1.point - 0.5*delta * currentElem->right;
			currentElem->p2.point = currentElem->p2.point - 0.5*delta * currentElem->right;
			currentElem->scale.SetX(currentElem->scale.GetX() + delta);
		}
		else if (GetKeyState(VK_DOWN) & 0x1000 ||
			GetKeyState('S') & 0x1000
		)
		{
			double delta = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;

			currentElem->p1.point = currentElem->p1.point + 0.5*delta * currentElem->right;
			currentElem->p2.point = currentElem->p2.point + 0.5*delta * currentElem->right;
			currentElem->scale.SetX(currentElem->scale.GetX() + delta);
		}
		else if (
			((GetKeyState(VK_RIGHT) & 0x1000) && (GetKeyState(VK_LEFT) & 0x1000)) ||
			((GetKeyState('D') & 0x1000) && (GetKeyState('A') & 0x1000))
		)
		{
			double delta = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
			currentElem->scale.SetZ(currentElem->scale.GetZ() + delta);
		}

		else if (GetKeyState(VK_RIGHT) & 0x1000 ||
				 GetKeyState('D') & 0x1000
		)
		{
			double delta = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;

			currentElem->p1.point = currentElem->p1.point - 0.5*delta * currentElem->forward;
			currentElem->p2.point = currentElem->p2.point - 0.5*delta * currentElem->forward;
			currentElem->scale.SetZ(currentElem->scale.GetZ() + delta);
		}
		else if (GetKeyState(VK_LEFT) & 0x1000 ||
			GetKeyState('A') & 0x1000
		)
		{
			double delta = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;

			currentElem->p1.point = currentElem->p1.point + 0.5*delta * currentElem->forward;
			currentElem->p2.point = currentElem->p2.point + 0.5*delta * currentElem->forward;
			currentElem->scale.SetZ(currentElem->scale.GetZ() + delta);
		}
		if (incisionElem)
		{
			currentElem->transformFilter->SetTransform(a->makeCompositeTransform(*currentElem));
			currentElem->transformFilter->Update();	// Must update transform filter for updates to show
		}
		else
		{
			currentElem->transformFilter->SetTransform(a->makeCompositeTransformFromSinglePoint(*currentElem));
			currentElem->transformFilter->Update();	// Must update transform filter for updates to show
		}
		return;
	}
	
	
	vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
}
//---------------------------------------------------------------------------------------------
void MyInteractorStyle::OnMouseWheelBackward()
{
	auto toolTip = a->toolTip.lock();

	if (a->toolTipOn && toolTip)
	{
		// Tooltips is on (pressing M with wheel will bypass superquad code and perform default zooming)
		//if (GetKeyState('M') & 0x1000)
		if (GetKeyState(VK_MENU) & 0x1000 || GetKeyState(VK_SHIFT) & 0x1000 || GetKeyState(VK_CONTROL) & 0x1000
			|| GetKeyState(VK_UP) & 0x1000 || GetKeyState(VK_DOWN) & 0x1000 || GetKeyState(VK_RIGHT) & 0x1000 || GetKeyState(VK_LEFT) & 0x1000
			)
		{
		}
		else
		{
			vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
			return;
		}

		
		MyElem *currentElem;
		bool incisionElem = false;
		if (a->myelems.size() > 0 && toolTip->toolType == KNIFE)
		{
			currentElem = a->myelems[a->myelems.size() - 1].get();
			incisionElem = true;
		}
		else
			currentElem = toolTip.get();

		if ((GetKeyState(VK_MENU) & 0x1000) && this->Interactor->GetShiftKey() && toolTip->toolType == RING)
			//((strcmp(this->Interactor->GetKeySym(), "z") == 0) ||
			                                    //(strcmp(this->Interactor->GetKeySym(), "Z") == 0)))
		{
			currentElem->spinFlipped -= this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
		}
		
		else if (GetKeyState(VK_MENU) & 0x1000 && this->Interactor->GetShiftKey() && toolTip->toolType == ROD)
		{
			currentElem->tilt -= this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
		}
		
		else if (GetKeyState(VK_MENU) & 0x1000)
		{
			// Tilt available for rod and ring
			if (toolTip->toolType == ROD || toolTip->toolType == RING)
				currentElem->tilt  -= this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
		}
		else if ((GetKeyState(VK_CONTROL) & 0x1000 ) && (GetKeyState(VK_SHIFT) & 0x1000))
		{
			currentElem->scale.SetY(currentElem->scale.GetY() - this->MotionFactor * deepness * wheelspeed * this->MouseWheelMotionFactor);
			
		}
		else if (GetKeyState(VK_CONTROL) & 0x1000)
		{
			currentElem->scale.SetZ(currentElem->scale.GetZ() - this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor);
			currentElem->scale.SetX(currentElem->scale.GetX() - this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor);
		}
		else if (this->Interactor->GetShiftKey())
		{
			double sf = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
			
			if (GetKeyState(VK_CAPITAL) & 0x0001)
			{
				double viewNormal[4];
				vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
				camera->GetViewPlaneNormal(viewNormal);
				currentElem->p1.normal = vtkVector3f(viewNormal[0], viewNormal[1], viewNormal[2]);
				currentElem->p2.normal = vtkVector3f(viewNormal[0], viewNormal[1], viewNormal[2]);
			}
			currentElem->p1.point = currentElem->p1.point + sf * currentElem->p1.normal;
			currentElem->p2.point = currentElem->p2.point + sf * currentElem->p2.normal;
		}
		else if (
				 ((GetKeyState(VK_UP) & 0x1000) && (GetKeyState(VK_DOWN) & 0x1000)) ||
				 ((GetKeyState('W') & 0x1000) && (GetKeyState('S') & 0x1000))
		)
		{
			double delta = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
			currentElem->scale.SetX(currentElem->scale.GetX() - delta);
		}
		else if (GetKeyState(VK_UP) & 0x1000 || GetKeyState('W') & 0x1000)
		{
			double delta = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;

			currentElem->p1.point = currentElem->p1.point + 0.5*delta * currentElem->right;
			currentElem->p2.point = currentElem->p2.point + 0.5*delta * currentElem->right;
			currentElem->scale.SetX(currentElem->scale.GetX() - delta);
		}
		else if (GetKeyState(VK_DOWN) & 0x1000 || GetKeyState('S') & 0x1000)
		{
			double delta = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;

			currentElem->p1.point = currentElem->p1.point - 0.5*delta * currentElem->right;
			currentElem->p2.point = currentElem->p2.point - 0.5*delta * currentElem->right;
			currentElem->scale.SetX(currentElem->scale.GetX() - delta);
		}
		else if (
			((GetKeyState(VK_RIGHT) & 0x1000) && (GetKeyState(VK_LEFT) & 0x1000)) ||
			((GetKeyState('D') & 0x1000) && (GetKeyState('A') & 0x1000)) 
		)
		{
			double delta = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;
			currentElem->scale.SetZ(currentElem->scale.GetZ() - delta);
		}
		else if (GetKeyState(VK_RIGHT) & 0x1000 || GetKeyState('D') & 0x1000)
		{
			double delta = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;

			currentElem->p1.point = currentElem->p1.point + 0.5*delta * currentElem->forward;
			currentElem->p2.point = currentElem->p2.point + 0.5*delta * currentElem->forward;
			currentElem->scale.SetZ(currentElem->scale.GetZ() - delta);
		}
		else if (GetKeyState(VK_LEFT) & 0x1000 || GetKeyState('A') & 0x1000)
		{
			double delta = this->MotionFactor * wheelspeed * this->MouseWheelMotionFactor;

			currentElem->p1.point = currentElem->p1.point - 0.5*delta * currentElem->forward;
			currentElem->p2.point = currentElem->p2.point - 0.5*delta * currentElem->forward;
			currentElem->scale.SetZ(currentElem->scale.GetZ() - delta);
		}
		if (incisionElem)
		{
			currentElem->transformFilter->SetTransform(a->makeCompositeTransform(*currentElem));
			currentElem->transformFilter->Update();	// Must update transform filter for updates to show
		}
		else
		{
			currentElem->transformFilter->SetTransform(a->makeCompositeTransformFromSinglePoint(*currentElem));
			currentElem->transformFilter->Update();	// Must update transform filter for updates to show
		}
		return;
	}
	
	vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
}
//---------------------------------------------------------------------------------------------
void MyInteractorStyle::OnChar()
{
	//----- Default OnChar code

	vtkRenderWindowInteractor *rwi = this->GetInteractor();
	
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
	case '`':
	//case 'R':
		a->realtimeupdate = !a->realtimeupdate;
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
	information->Set(vtkMyBasePass::OUTLINEKEY(), 0);	// dummy value
	OutlineActor->SetPropertyKeys(information);

	return OutlineActor;
}


void MyInteractorStyle::avgNormal(double *currentN, double *averageN)
{
	double avgN[3];
	avgN[0] = avgN[1] = avgN[2] = 0.0;
	
	mouseNorms[mouseNormI][0] = currentN[0];
	mouseNorms[mouseNormI][1] = currentN[1];
	mouseNorms[mouseNormI][2] = currentN[2];
	mouseNormI = (mouseNormI + 1) % 5;
	
	for (int i = 0; i < 5; i++)
	{
		avgN[0] += mouseNorms[i][0];
		avgN[1] += mouseNorms[i][1];
		avgN[2] += mouseNorms[i][2];
	}
	averageN[0] = avgN[0] / 5;
	averageN[1] = avgN[1] / 5;
	averageN[2] = avgN[2] / 5;
	
}
//------------------------------------------------------------------
void MyInteractorStyle::forwardLeftButtonDown()
{
	// forward events (Default VTK code!)
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

	if (this->CurrentRenderer == NULL)
	{
		return;
	}
	this->GrabFocus(this->EventCallbackCommand);
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
