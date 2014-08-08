#include "stdafx.h"

#include "MyInteractorStyle.h"

// Custom
#include "illustrator.h"
#include "vtkMyShaderPass.h"
#include "CarveConnector.h"

// VTK Includes
#include <vtkQuad.h>
#include <vtkAppendPolyData.h>
#include <vtkSphereSource.h>

vtkStandardNewMacro(MyInteractorStyle);

//---------------------------------------------------------------------------------------------------------------
MyInteractorStyle::MyInteractorStyle() : NumberOfClicks(0), ResetPixelDistance(5)
{
	this->PreviousPosition[0] = 0;
	this->PreviousPosition[1] = 0;

	this->cellPicker = vtkSmartPointer<vtkCellPicker>::New();
	this->cellPicker->SetTolerance(0.0005);

	dragging = false;
	creation = false;

	//this->PickingManagedOn();
}
//---------------------------------------------------------------------------------------------------------------
void MyInteractorStyle::initialize(illustrator *window)
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
		if (a->selectedMesh != a->meshes.end())
		{
			a->selectedMesh->opacity = 0;
			a->selectedMesh->actor->GetProperty()->SetOpacity(0);
			a->updateOpacitySliderAndList();
		}
	}
	if (this->Interactor->GetKeyCode() == 'k')	// Cut
	{
		if (a->selectedMesh == a->meshes.end())
			return;

		// Change index for eselected to selected element...
		int eselectedindex = a->myelems.size() - 1;

		CarveConnector connector;
		vtkSmartPointer<vtkPolyData> thepolydata(vtkPolyData::SafeDownCast(a->selectedMesh->actor->GetMapper()->GetInput()));
		thepolydata = CarveConnector::cleanVtkPolyData(thepolydata);

		vtkSmartPointer<vtkPolyData> mypoly(vtkPolyData::SafeDownCast(a->myelems[eselectedindex].actor->GetMapper()->GetInput()));
		vtkSmartPointer<vtkPolyData> thepolydata2 = CarveConnector::cleanVtkPolyData(mypoly);

		// Make MeshSet from vtkPolyData
		std::unique_ptr<carve::mesh::MeshSet<3> > first(CarveConnector::vtkPolyDataToMeshSet(thepolydata));
		std::unique_ptr<carve::mesh::MeshSet<3> > second(CarveConnector::vtkPolyDataToMeshSet(thepolydata2));
		//std::unique_ptr<carve::mesh::MeshSet<3> > second(CarveConnector::makeCube(55, carve::math::Matrix::IDENT()));

		std::unique_ptr<carve::mesh::MeshSet<3> > c(CarveConnector::perform(first, second, carve::csg::CSG::A_MINUS_B));
		vtkSmartPointer<vtkPolyData> c_poly(CarveConnector::meshSetToVTKPolyData(c));

		vtkSmartPointer<vtkPolyDataNormals> dataset = vtkSmartPointer<vtkPolyDataNormals>::New();
		dataset->SetInputData(c_poly);
		dataset->ComputePointNormalsOn();
		dataset->ComputeCellNormalsOff();
		dataset->SplittingOn();
		dataset->SetFeatureAngle(60);
		dataset->Update();

		// Update cell locator's dataset so program doesn't slow down after cutting
		a->selectedMesh->cellLocator->SetDataSet(dataset->GetOutput());

		// Create a mapper and actor
		vtkSmartPointer<vtkActor> actor = Utility::sourceToActor(dataset->GetOutput(), a->selectedMesh->color.GetRed(),
			a->selectedMesh->color.GetGreen(),
			a->selectedMesh->color.GetBlue(),
			a->selectedMesh->opacity >= 1 ? 1 : a->selectedMesh->opacity * 0.5f);	// My opacity (Must change 0.5f)

		// Replace old actor with new actor
		a->replaceActor(a->selectedMesh->actor, actor);		// First replace selectedMesh->actor (old) with new actor in Renderer
		a->selectedMesh->actor = actor;						// Then assign selectedMesh->actor to the new actor

		// Remove superquadric from Renderer
		a->renderer->RemoveActor(a->myelems[eselectedindex].actor);

		// Probably should remove from list as well (myelems)
		a->myelems.erase(a->myelems.end() - 1);
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
		if (a->selectedMesh == a->meshes.end())
			return;
		if (!a->selectedMesh->generated)	// Make sure it is a generated mesh (Rather than original mesh)
			return;

		static int x = 5;

		//while (x < 60)
		//{
			x += 5;

			vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
			transform->PostMultiply();
			transform->Translate(-a->selectedMesh->hingePivot.GetX(), -a->selectedMesh->hingePivot.GetY(), -a->selectedMesh->hingePivot.GetZ());
			transform->RotateWXYZ(-x, a->selectedMesh->sup.GetX(), a->selectedMesh->sup.GetY(),
				a->selectedMesh->sup.GetZ());
			transform->Translate(a->selectedMesh->hingePivot.GetX(), a->selectedMesh->hingePivot.GetY(), a->selectedMesh->hingePivot.GetZ());

			a->selectedMesh->actor->SetUserTransform(transform);

			QApplication::processEvents();
		//}

	}
	if (this->Interactor->GetKeyCode() == '2')	// Rotate around hinge (-)
	{
		if (a->selectedMesh == a->meshes.end())
			return;
		if (!a->selectedMesh->generated)	// Make sure it is a generated mesh (Rather than original mesh)
			return;

		a->selectedMesh->actor->RotateWXYZ(-5, a->selectedMesh->sup.GetX(), a->selectedMesh->sup.GetY(),
			a->selectedMesh->sup.GetZ());
	}
	if (this->Interactor->GetKeyCode() == 'l')	// Hinge
	{
		// Check if there is selected widget too...

		if (a->selectedMesh == a->meshes.end())
			return;

		// Change index for eselected to selected element...
		int eselectedindex = a->myelems.size() - 1;
		MyElem& elem = a->myelems[eselectedindex];

		CarveConnector connector;
		vtkSmartPointer<vtkPolyData> thepolydata(vtkPolyData::SafeDownCast(a->selectedMesh->actor->GetMapper()->GetInput()));
		thepolydata = CarveConnector::cleanVtkPolyData(thepolydata);

		vtkSmartPointer<vtkPolyData> mypoly(vtkPolyData::SafeDownCast(elem.actor->GetMapper()->GetInput()));
		vtkSmartPointer<vtkPolyData> thepolydata2 = CarveConnector::cleanVtkPolyData(mypoly);

		// Make MeshSet from vtkPolyData
		std::unique_ptr<carve::mesh::MeshSet<3> > first(CarveConnector::vtkPolyDataToMeshSet(thepolydata));
		std::unique_ptr<carve::mesh::MeshSet<3> > second(CarveConnector::vtkPolyDataToMeshSet(thepolydata2));
		//std::unique_ptr<carve::mesh::MeshSet<3> > second(CarveConnector::makeCube(55, carve::math::Matrix::IDENT()));

		std::unique_ptr<carve::mesh::MeshSet<3> > c(CarveConnector::perform(first, second, carve::csg::CSG::A_MINUS_B));
		vtkSmartPointer<vtkPolyData> c_poly(CarveConnector::meshSetToVTKPolyData(c));

		// Create second piece (the cut piece)
		std::unique_ptr<carve::mesh::MeshSet<3> > d(CarveConnector::perform(first, second, carve::csg::CSG::INTERSECTION));
		vtkSmartPointer<vtkPolyData> d_poly(CarveConnector::meshSetToVTKPolyData(d));

		// Create normals for resulting polydatas
		vtkSmartPointer<vtkPolyDataNormals> dataset = vtkSmartPointer<vtkPolyDataNormals>::New();
		dataset->SetInputData(c_poly);
		dataset->ComputePointNormalsOn();
		dataset->ComputeCellNormalsOff();
		dataset->SplittingOn();
		dataset->SetFeatureAngle(60);
		dataset->Update();

		vtkSmartPointer<vtkPolyDataNormals> datasetd = vtkSmartPointer<vtkPolyDataNormals>::New();
		datasetd->SetInputData(d_poly);
		datasetd->ComputePointNormalsOn();
		datasetd->ComputeCellNormalsOff();
		datasetd->SplittingOn();
		datasetd->SetFeatureAngle(60);
		datasetd->Update();

		// Update cell locator's dataset so program doesn't slow down after cutting
		a->selectedMesh->cellLocator->SetDataSet(dataset->GetOutput());

		vtkColor3f color(a->selectedMesh->color.GetRed(), 
			a->selectedMesh->color.GetGreen(), 
			a->selectedMesh->color.GetBlue());

		// Run through list and see if name with + already exists, while it exists, add another + 
		// to generate unique name
		std::stringstream ss;
		ss << a->selectedMesh->name << "+";

		while (a->getListItemByName(ss.str()) != nullptr)
			ss << "+";
		std::string name = ss.str();

		float opacity = a->selectedMesh->opacity >= 1 ? 1 : a->selectedMesh->opacity * 0.5f;

		// Create a mapper and actor
		vtkSmartPointer<vtkActor> actor = Utility::sourceToActor(dataset->GetOutput(), color.GetRed(),
			color.GetGreen(), color.GetBlue(), opacity);	// My opacity (Must change 0.5f)

		// Replace old actor with new actor
		a->replaceActor(a->selectedMesh->actor, actor);		// First replace selectedMesh->actor (old) with new actor in Renderer
		a->selectedMesh->actor = actor;						// Then assign selectedMesh->actor to the new actor

		color.Set(min(color.GetRed() + 0.1, 1.0),
			min(color.GetGreen() + 0.1, 1.0),
			min(color.GetBlue() + 0.1, 1.0));

		// Add second actor (the cut piece) to renderer (as well as to meshes vector)
		CustomMesh & mesh = Utility::addMesh(a, datasetd->GetOutput(), a->meshes.size(), name , color, 1.0);
		mesh.generated = true;
		
		if (a->selectedMesh->generated)	// Piece we are cutting is already generated, so reuse snormal
		{
			mesh.snormal = vtkVector3f(a->selectedMesh->snormal.GetX(), a->selectedMesh->snormal.GetY(),
				a->selectedMesh->snormal.GetZ());
		}
		else	// Generate a new snormal
		{
			mesh.snormal = vtkVector3f((elem.p1.normal.GetX() + elem.p2.normal.GetX()) / 2.0f,
				(elem.p1.normal.GetY() + elem.p2.normal.GetY()) / 2.0f,
				(elem.p1.normal.GetZ() + elem.p2.normal.GetZ()) / 2.0f);
			mesh.snormal.Normalize();
		}
		// Note: snormal is shared by nested superquadrics because nested squadrics must explode in same direction
		//		 but sup (up vector) is different for nested squadrics b/c they are hinged individually around the
		//		 up vector.
		vtkVector3f forward = vtkVector3f((elem.p1.normal.GetX() + elem.p2.normal.GetX()) / 2.0f,
			(elem.p1.normal.GetY() + elem.p2.normal.GetY()) / 2.0f,
			(elem.p1.normal.GetZ() + elem.p2.normal.GetZ()) / 2.0f);
		forward.Normalize();

		vtkVector3f right = vtkVector3f(elem.p2.point.GetX() - elem.p1.point.GetX(),
			elem.p2.point.GetY() - elem.p1.point.GetY(),
			elem.p2.point.GetZ() - elem.p1.point.GetZ());
		right.Normalize();

		mesh.sup = forward.Cross(right);
		mesh.sup.Normalize();

		// Also set the hinge pivot point
		mesh.hingePivot = vtkVector3f(elem.p1.point.GetX(), elem.p1.point.GetY(), elem.p1.point.GetZ());
		a->renderer->AddActor(mesh.actor);

		// Also add mesh to listWidget
		a->addToList(name);

		// Remove superquadric from Renderer
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

		//a->superquad->SetPhiRoundness(a->myn);
		//a->superquad->Update();

		int i = a->myelems.size() - 1;
		a->myelems[i].source->SetPhiRoundness(a->myn);
		a->myelems[i].source->Update();
		a->myelems[i].transformFilter->Update();		// Must update transformfilter for transforms to show
	}
	if (this->Interactor->GetKeyCode() == 'v')	// change roundness (phi)
	{
		float change = 0.1;

		if (a->myn + change <= 20)
			a->myn += change;

		//a->superquad->SetPhiRoundness(a->myn);
		//a->superquad->Update();

		int i = a->myelems.size() - 1;
		a->myelems[i].source->SetPhiRoundness(a->myn);
		a->myelems[i].source->Update();
		a->myelems[i].transformFilter->Update();		// Must update transformfilter for transforms to show
	}
	if (this->Interactor->GetKeyCode() == 'z')	// change roundness (-theta)
	{
		float change = 0.1;

		if (a->myexp - change >= 0)
			a->myexp -= change;

		//a->superquad->SetThetaRoundness(a->myexp);
		//a->superquad->Update();

		int i = a->myelems.size() - 1;
		a->myelems[i].source->SetThetaRoundness(a->myexp);
		a->myelems[i].source->Update();
		a->myelems[i].transformFilter->Update();		// Must update transformfilter for transforms to show
	}
	if (this->Interactor->GetKeyCode() == 'x')	// change roundness (theta)
	{
		float change = 0.1;

		if (a->myexp + change <= 20)
			a->myexp += change;

		//a->superquad->SetThetaRoundness(a->myexp);
		//a->superquad->Update();

		int i = a->myelems.size() - 1;
		a->myelems[i].source->SetThetaRoundness(a->myexp);
		a->myelems[i].source->Update();
		a->myelems[i].transformFilter->Update();		// Must update transformfilter for transforms to show
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

			vtkSmartPointer<vtkActor> sactor = Utility::sourceToActor(a->superquad->GetOutput(), 1, 1, 1, 1);
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
	/*if (this->Interactor->GetKeyCode() == ' ')		// Toggle wiggle
	{
		a->wiggle = !a->wiggle;
	}*/

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
		//a->myelems.at(i).actor->GetProperty()->SetDiffuseColor(0, 1, 0);
		//a->myelems.at(i).actor->GetMapper()->Update();
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
		//a->myelems.at(i).actor->GetProperty()->SetDiffuseColor(0, 1, 0);
		//a->myelems.at(i).actor->GetMapper()->Update();
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
		//a->myelems.at(i).actor->GetProperty()->SetDiffuseColor(0, 1, 0);
		//a->myelems.at(i).actor->GetMapper()->Update();
	}

	if (a->myelems.size() > 0)	// Resize last superquad placed
	{
		int i = a->myelems.size() - 1;

		double col[3];
		a->myelems.at(i).actor->GetProperty()->GetAmbientColor(col);

		if (this->Interactor->GetKeyCode() == 'a')
			if (col[0] < 1.0)
				col[0] += 0.1;
		if (this->Interactor->GetKeyCode() == 'A')
			if (col[0] >= 0.1)
				col[0] -= 0.1;
		if (this->Interactor->GetKeyCode() == 's')
			if (col[1] < 1.0)
				col[1] += 0.1;
		if (this->Interactor->GetKeyCode() == 'S')
			if (col[1] >= 0.1)
				col[1] -= 0.1;
		if (this->Interactor->GetKeyCode() == 'd')
			if (col[2] < 1.0)
				col[2] += 0.1;
		if (this->Interactor->GetKeyCode() == 'D')
			if (col[2] >= 0.1)
				col[2] -= 0.1;
		if (this->Interactor->GetKeyCode() == '7')
			std::cout << col[0] << ", " << col[1] << "," << col[2] << "\n";

		a->myelems.at(i).actor->GetProperty()->SetAmbientColor(col);
		a->myelems.at(i).actor->GetProperty()->SetDiffuseColor(col);
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

	cellPicker->Pick(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1], 0,  // always zero.
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
			{
				for (int i = 0; i < a->meshes.size(); i++)
					a->meshes[i].selected = false;	// Reset all meshes to unselected

				a->selectedMesh = it;			// set selectedMesh to mesh clicked and its selected property to true
				a->selectedMesh->selected = true;

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
			auto it = a->getMeshByActor(actorDouble);

			if (it != a->meshes.end())
			{
				// if dbl clicked mesh, set it as selected (update slider/list, etc.)
				for (int i = 0; i < a->meshes.size(); i++)
					a->meshes[i].selected = false;	// Reset all meshes to unselected

				a->selectedMesh = it;			// Update selectedMesh to one clicked and set selected property to true
				a->selectedMesh->selected = true;

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

		vtkSmartPointer<MySuperquadricSource> superquad = vtkSmartPointer<MySuperquadricSource>::New();
		superquad->SetToroidal(false);
		superquad->SetThetaResolution(1);
		superquad->SetPhiResolution(1);

		superquad->SetPhiRoundness(0.0);
		superquad->SetThetaRoundness(0.0);
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

		// TODO: superquad opacity
		elem.actor = Utility::sourceToActor(transformFilter->GetOutput(), 1.0, 1.0, 1.0, 1.0);
		elem.actor->PickableOff();
		
		elem.actor->SetTexture(a->colorTexture);	

		elem.source = superquad;
		elem.transformFilter = transformFilter;

		a->renderer->AddActor(elem.actor);

		a->myelems.push_back(elem);

		auto addSphere = [](illustrator *a, float x, float y, float z){
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
