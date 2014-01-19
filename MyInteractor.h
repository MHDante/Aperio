// ***********************************************************************
// Assembly         : additive
// Author           : SUPER
// Created          : 03-26-2013
//
// Last Modified By : SUPER
// Last Modified On : 03-27-2013
// ***********************************************************************
// <copyright file="MyInteractor.h" company="">
//     Copyright (c) . All rights reserved.
// </copyright>
// <summary>
// Interactor for QVTKWidget, includes keypresses, mouse presses
// and potentially multi-touch events linking (to be added)</summary>
// ***********************************************************************

/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkInteractorStyleTrackballCamera.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Inherited from
#include "vtkInteractorStyleTrackballCamera.h"

// Additional includes
#include <vtkCellPicker.h>
#include <vtkLineSource.h>
#include <vtkRendererCollection.h>
#include <vtkTubeFilter.h>

#include <vtkWorldPointPicker.h>
#include "vtkPlaneSource.h"
#include "vtkQuad.h"
#include "vtkTriangle.h"
#include "vtkAppendPolyData.h"

#include "vtkPropPicker.h"
#include "vtkPickingManager.h"
#include "vtkCellLocator.h"

#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

//-----------------------------------------------------------------------------
/// <summary>
/// Class MouseInteractorStylePP, contains key press, mouse events etc.
/// </summary>
class MouseInteractorStylePP : public vtkInteractorStyleTrackballCamera
{
public:
	/// <summary> Pointer to the mainwindow class so we can call functions to manipulate/update shaders, etc. </summary>
	additive * a;
	vtkSmartPointer<vtkCellPicker> cellPicker;

private:
	unsigned int NumberOfClicks;	// For double clicks
	int PreviousPosition[2];
	int ResetPixelDistance;

	bool dragging;
	bool creation;
public:

	// Getters and setters for encapsulated variables
	// ...

	/// ---------------------------------------------------------------------------------------------
	/// <summary>
	/// Creates a new instance (Factory creator declaration) - definition is below outside of class
	/// </summary>
	static MouseInteractorStylePP* New();

	MouseInteractorStylePP() : NumberOfClicks(0), ResetPixelDistance(5)
	{
		this->PreviousPosition[0] = 0;
		this->PreviousPosition[1] = 0;

		this->cellPicker = vtkSmartPointer<vtkCellPicker>::New();
		this->cellPicker->SetTolerance(0.0005);

		dragging = false;
		creation = false;
		//this->PickingManagedOn();
	}

	/// ---------------------------------------------------------------------------------------------
	/// <summary>
	/// Since there is no constructor (just a factory creator), we have an initialize function
	/// </summary>
	void initialize(additive *window)
	{
		this->a = window;

		//this->worldPicker = vtkSmartPointer<vtkCellPicker>::New();
		//this->worldPicker->SetTolerance(0.0005);
		//this->GetInteractor()->SetPicker(worldPicker);

		//this->worldPicker->
		//this->GetInteractor()->GetPicker()->GetPickList()->RemoveAllItems();
		//this->GetInteractor()->GetPicker()->AddPickList(a->meshes[117].actor);
		//this->GetInteractor()->GetPicker()->PickFromListOn();
		//this->GetInteractor()->GetPickingManager()->EnabledOn();
	}

	void setPickList(int z)
	{
		//worldPicker->GetPickList()->RemoveAllItems();
		//worldPicker->AddPickList(a->meshes[z].actor);
		//worldPicker->PickFromListOn();
	}
	vtkTypeMacro(MouseInteractorStylePP, vtkInteractorStyleTrackballCamera);

	/// ---------------------------------------------------------------------------------------------
	//virtual void OnLeftButtonDown() override
	//{
	//	//std::cout << "Picking pixel: " << this->Interactor->GetEventPosition()[0] << " " << this->Interactor->GetEventPosition()[1] << std::endl;
	//	//this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
	//	//	this->Interactor->GetEventPosition()[1], 0,  // always zero.
	//	//	this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	//	//double picked[3];
	//	//this->Interactor->GetPicker()->GetPickPosition(picked);
	//	//std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;

	//	// Forward events
	//	//vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	//}
	//
	//	virtual void OnRightButtonDown()  override
	//	{
	//		cout << "ehehe";
	//	}
	//
	//	virtual void OnMouseMove()  override
	//	{
	//		//this->OnMouseMove();
	//		//double picked[3];
	//		this->Interactor->GetPicker()->GetPickPosition(mouse);
	//		cout << "hi";
	////		QApplication::processEvents();
	//		//QApplication::exec();
	//		this->GrabFocus(this->EventCallbackCommand);
	//		this->ReleaseFocus();
	//	}
	//
	//	virtual void OnLeftButtonUp();

	/// ---------------------------------------------------------------------------------------------
	/// <summary>
	/// Called on key press (handles QT key events)
	/// </summary>
	virtual void MouseInteractorStylePP::OnKeyPress() override
	{
		//cout << "hi";

		if (strcmp(this->Interactor->GetKeySym(), "Delete") == 0)	// Del key	 ( probably should put this in QT keycheck, b/c QVTKWidget requires focus)
		{
			std::cout << "HI";
			a->meshes[a->selectedIndex].opacity = 0;
			a->meshes[a->selectedIndex].actor->GetProperty()->SetOpacity(0);
			a->updateOpacitySliderAndList();
		}
		if (this->Interactor->GetKeyCode() == 'a')
		{
			// just for viewing, doesnt do anything
			vtkSmartPointer<vtkMatrix4x4> mat = a->renderer->GetActiveCamera()->GetModelViewTransformMatrix();

			int index = 0;
			float elems[16];

			std::ostringstream elems_str;

			for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				elems[index] = mat->GetElement(j, i);	// row j, column i (column major)
				elems_str << elems[index] << " ";
				index++;
			}
			std::cout << elems_str.str() << std::endl;
		}
		if (this->Interactor->GetKeyCode() == '+')
		{
			a->brushDivide--;
			this->OnMouseMove();
		}
		if (this->Interactor->GetKeyCode() == '-')
		{
			a->brushDivide++;
			this->OnMouseMove();
			cout << "m";
		}
		if (this->Interactor->GetKeyCode() == 't')
		{
			if (a->peerInside == 1) a->peerInside = 0;
			else if (a->peerInside == 0) a->peerInside = 1;
		}
		if (this->Interactor->GetKeyCode() == 13)	// enter
		{
			//std::cout << "enter";
			//a->widgets.push_back(std::vector<WidgetElem>());

			a->mywidgets.push_back(MyWidget());

			std::cout << "*** NEW WIDGET ***" << std::endl;
		}

		if (this->Interactor->GetKeyCode() == 'c')	//
		{
			//std::cout << "cut";
			//creation = !creation;

			float change = 0.1;

			if (a->myn - change >= 0)
				a->myn -= change;

			a->superquad->SetPhiRoundness(a->myn);
			a->superquad->Update();
		}
		if (this->Interactor->GetKeyCode() == 'v')
		{
			float change = 0.1;

			if (a->myn + change <= 20)
				a->myn += change;

			a->superquad->SetPhiRoundness(a->myn);
			a->superquad->Update();
		}
		if (this->Interactor->GetKeyCode() == 'z')
		{
			float change = 0.1;

			if (a->myexp - change >= 0)
				a->myexp -= change;

			a->superquad->SetThetaRoundness(a->myexp);
			a->superquad->Update();
		}
		if (this->Interactor->GetKeyCode() == 'x')
		{
			float change = 0.1;

			if (a->myexp + change <= 20)
				a->myexp += change;

			a->superquad->SetThetaRoundness(a->myexp);
			a->superquad->Update();
		}
		if (this->Interactor->GetKeyCode() == 's')
		{
			if (!a->superquad)
			{
				std::cout << "making\n";
				a->superquad = vtkSmartPointer<MySuperquadricSource>::New();
				a->superquad->SetPhiResolution(20);
				a->superquad->SetThetaResolution(20);
				a->superquad->ToroidalOff();

				a->superquad->SetScale(2, 1, 1);
				a->superquad->SetSize(1.0);

				//a->superquad->SetSize(0.5);
				//a->superquad->SetThickness(.333);
				
				a->superquad->SetPhiRoundness(0.5);
				//a->superquad->SetThickness(0.43);
				a->superquad->SetThetaRoundness(a->myexp);

				//a->superquad->SetCenter(0, 0, 0);
				a->superquad->SetCenter(a->mouse[0], a->mouse[1], a->mouse[2]);

				vtkSmartPointer<vtkPolyDataMapper>  mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
				mapper->SetInputData(a->superquad->GetOutput());

				mapper->ImmediateModeRenderingOn();

				mapper->Update();

				a->sactor = vtkSmartPointer<vtkActor>::New();
				a->sactor->SetMapper(mapper);
				a->sactor->PickableOff();
				a->sactor->GetProperty()->SetDiffuseColor(1, 0, 0);

				a->renderer->AddActor(a->sactor);

				//a->superquad->SetCenter(a->mouse[0], a->mouse[1], a->mouse[2]);
				a->superquad->Update();
			}

			//a->sactor->SetPosition(a->mouse[0], a->mouse[1], a->mouse[2]);
			//a->superquad->SetCenter(a->mouse[0], a->mouse[1], a->mouse[2]);
			a->superquad->SetCenter(a->mouse[0], a->mouse[1], a->mouse[2]);
			a->superquad->Update();
			
			//a->superquad->Update();


		}
		if (this->Interactor->GetKeyCode() == 'b')
		{
			this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
				this->Interactor->GetEventPosition()[1],
				0,  // always zero.
				this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
			double picked[3];
			this->Interactor->GetPicker()->GetPickPosition(picked);

			std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;
			std::cout << "Mouse value: " << a->mouse[0] << " " << a->mouse[1] << " " << a->mouse[2] << std::endl;

			vtkSphereSource * ss = vtkSphereSource::New();
			ss->SetRadius(0.1);
			ss->SetCenter(picked[0], picked[1], picked[2]);
			ss->Update();

			vtkPolyDataMapper *pp = vtkPolyDataMapper::New();
			pp->SetInputData(ss->GetOutput());

			vtkActor * xx = vtkActor::New();
			xx->SetMapper(pp);

			//
			a->renderer->AddActor(xx);
		}

		if (this->Interactor->GetKeyCode() == ' ')
		{
			if (a->widgets.size() == 0)
			{
				// If widget empty, add the first one
				//a->widgets.push_back(std::vector<WidgetElem>());
			}

			if (a->mywidgets.size() == 0)
			{
				// If widget empty, add the first one
				a->mywidgets.push_back(MyWidget());
			}

			//this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
			//	this->Interactor->GetEventPosition()[1], 0,  // always zero.
			//	this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

			//double pos[3];
			//this->Interactor->GetPicker()->GetPickPosition(pos);

			//vtkSmartPointer<vtkWorldPointPicker> picker2 = vtkSmartPointer<vtkWorldPointPicker>::New();
			//picker2->

			// Pick from this location.
			cellPicker->Pick(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1], 0,
				this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

			//double* worldPosition = picker->GetPickPosition();

			//int cellid = cellPicker->GetCellId();
			
			vtkActor *pickedActor = cellPicker->GetActor();
			//std::cout << "Cell id is: " << picker->GetCellId() << std::endl;

			if (pickedActor == nullptr)
				cout << "none\n";
			else
			{
				//picker->GetActor()->GetProperty()->SetDiffuseColor(1, 1, 1);
				double poss[3];
				//picker->GetPickNormal(poss);
				cellPicker->GetPickPosition(poss);

				//return;
				//picker->getposit
				//std::cout << "posss: " << poss[0] << ", " << poss[1] << ", " << poss[2] << "\n";

				//std::cout << ", " << picker->GetActor()->GetProperty()->GetRepresentationAsString() << "\n";
				//picker->GetActor()->GetProperty()->SetDiffuseColor(1, 0, 0);
				//auto coll = cellPicker->GetActors();

				//int id = cellPicker->GetPointId();

				//if (coll->GetNumberOfItems() == 0)
				//{
				//std::cout << "none\n";
				//}
				//if (coll->GetNumberOfItems() > 0)
				//{
				//std::cout << "id" << id << std::endl;
				//std::cout << "cellid" << cellid << std::endl;

				//std::cout << "noneeee\n" ;
				auto thepolydata = static_cast<vtkPolyData *> (pickedActor->GetMapper()->GetInput());

				//thepolydata->GetPointData()->GetNormals()->GetTuple(id);
				//std::cout << id << "\n";
				double normal[3];//= thepolydata->GetPointData()->GetNormals()->GetTuple(id);

				cellPicker->GetPickNormal(normal);

				//std::cout << "" << thepolydata->GetPointData()->GetNumberOfTuples();

				//for (int i = 0 ; i < 8; i++)
				//{
				//std::cout << "" << thepolydata->GetPointData()->GetTuple(i)[0] << "," << thepolydata->GetPointData()->GetTuple(i)[1] <<
				//"," << thepolydata->GetPointData()->GetTuple(i)[2] << "\n";
				//}

				//double *normal = thepolydata->GetCellData()->GetNormals()->GetTuple(cellid);

				//thepolydata->GetPoint(id);

				float tubeWidth = a->mouseSize / 20.0f;
				//vtkMath::Normalize(normal);

				//vtkSmartPointer<vtkLineSource> sphere = vtkSmartPointer<vtkLineSource>::New();
				//sphere->SetPoint1(0, 0, 0);
				////sphere->SetPoint2(normal[0], normal[1], normal[2]);
				//sphere->SetPoint2(0.0, 0.0, 0.0);

				//vtkSmartPointer<vtkTubeFilter> tube = vtkSmartPointer<vtkTubeFilter>::New();
				//tube->SetInputConnection(sphere->GetOutputPort());
				//tube->SetRadius(tubeWidth); //default is .5
				//tube->SetNumberOfSides(50);
				//tube->Update();

				float  mouseSizeDivide = 5.0;

				vtkSmartPointer<vtkSphereSource> spherec = vtkSmartPointer<vtkSphereSource>::New();
				spherec->SetRadius(a->mouseSize / mouseSizeDivide);
				spherec->Update();

				//vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
				//transform->Translate(poss[0], poss[1], poss[2]);

				//vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
				//transformFilter->SetInputData(spherec->GetOutput());
				//transformFilter->SetTransform(transform);
				//transformFilter->Update();

				vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
				mapper->SetInputConnection(spherec->GetOutputPort());

				//WidgetElem elem;

				//elem.actor = vtkSmartPointer<vtkActor>::New();
				//elem.actor->SetMapper(mapper);
				//elem.actor->PickableOn();
				//elem.actor->GetProperty()->SetDiffuseColor(1, 0.0, 0);
				//elem.actor->GetProperty()->SetOpacity(0.5);
				//elem.actor->GetProperty()->BackfaceCullingOff();

				//elem.actor->SetProperty()

				//a->renderer->AddActor(elem.actor);
				//
				//elem.endpoints = vtkSmartPointer<vtkPoints>::New();
				//elem.endpoints->InsertNextPoint(poss[0], poss[1], poss[2]);

				//elem.normals = vtkSmartPointer<vtkPoints>::New();
				//elem.normals->InsertNextPoint(normal[0], normal[1], normal[2]);

				//a->widgets.at(a->widgets.size() - 1).push_back(elem);

				// NEW CODE

				//myelem.actor->GetProperty()->BackfaceCullingOff();

				int selectedwidgetind = a->mywidgets.size() - 1;

				MyPoint p;
				p.normal = vtkSmartPointer<vtkPoints>::New();
				p.normal->InsertNextPoint(normal[0], normal[1], normal[2]);

				p.point = vtkSmartPointer<vtkPoints>::New();
				p.point->InsertNextPoint(poss[0], poss[1], poss[2]);

				p.actor = vtkSmartPointer<vtkActor>::New();
				p.actor->SetMapper(mapper);
				p.actor->PickableOn();
				p.actor->GetProperty()->SetDiffuseColor(1, 0.0, 0);
				p.actor->GetProperty()->SetOpacity(0.5);

				p.actor->SetPosition(poss[0], poss[1], poss[2]);

				a->renderer->AddActor(p.actor);
				a->mywidgets.at(selectedwidgetind).points.push_back(p);

				MyWidgetElem myelem;

				if (a->mywidgets.at(selectedwidgetind).elems.size() == 0)
				{
					myelem.p1_index = a->mywidgets.at(selectedwidgetind).points.size() - 1;
					myelem.p2_index = -1;
				}
				else
				{
					a->mywidgets.at(selectedwidgetind).elems.at(a->mywidgets.at(selectedwidgetind).elems.size() - 1).p2_index =
						a->mywidgets.at(selectedwidgetind).points.size() - 1;

					myelem.p1_index = a->mywidgets.at(selectedwidgetind).points.size() - 1;
					myelem.p2_index = -1;
				}
				a->mywidgets.at(selectedwidgetind).elems.push_back(myelem);

				// Print out

				std::cout << "\n\n\n\n";
				for (int i = 0; i < a->mywidgets.size(); i++)
				{
					std::cout << "*****\n";
					for (int j = 0; j < a->mywidgets.at(i).elems.size(); j++)
					{
						int p1_index = a->mywidgets.at(i).elems.at(j).p1_index;
						int p2_index = a->mywidgets.at(i).elems.at(j).p2_index;

						std::cout << "elem: ";
						if (p1_index != -1)
							std::cout << a->mywidgets.at(i).points.at(p1_index).point->GetPoint(0)[0] << ",";
						else
							std::cout << "none\n";

						if (p2_index != -1)
							std::cout << a->mywidgets.at(i).points.at(p2_index).point->GetPoint(0)[0] << "\n";
						else
							std::cout << "none\n";
					}
				}
				//p1.
				//myelem.endpoints = vtkSmartPointer<vtkPoints>::New();
				//elem.endpoints->InsertNextPoint(poss[0], poss[1], poss[2]);

				//elem.normals = vtkSmartPointer<vtkPoints>::New();
				//elem.normals->InsertNextPoint(normal[0], normal[1], normal[2]);
				//MyWidgetElem myelem;

				// Calculate avg normals
				for (int i = 0; i < a->mywidgets.size(); i++)
				{
					float normalssum[3] = { 0, 0, 0 };

					for (int j = 0; j < a->mywidgets.at(i).points.size(); j++)
					{
						double *norm = a->mywidgets.at(i).points.at(j).normal->GetPoint(0);

						normalssum[0] += norm[0];
						normalssum[1] += norm[1];
						normalssum[2] += norm[2];
					}
					normalssum[1] = normalssum[1] / a->mywidgets.at(i).points.size();
					normalssum[0] = normalssum[0] / a->mywidgets.at(i).points.size();
					normalssum[2] = normalssum[2] / a->mywidgets.at(i).points.size();

					a->mywidgets.at(i).avgnormal = vtkSmartPointer<vtkPoints>::New();
					a->mywidgets.at(i).avgnormal->InsertNextPoint(normalssum[0], normalssum[1], normalssum[2]);
				}

				//a->mywidgets.at(a->mywidgets.size() - 1).elems.push_back(myelem);
				// Run through all widgets and calculate avgnormals for each widget
				for (int i = 0; i < a->widgets.size(); i++)	// Vector of Vector of Widget Elems (n sets of separate Widgets)
				{
					float normalssum[3] = { 0, 0, 0 };

					for (int j = 0; j < a->widgets[i].size(); j++)
					{
						double *norm = a->widgets.at(i).at(j).normals->GetPoint(0);
						normalssum[0] += norm[0];
						normalssum[1] += norm[1];
						normalssum[2] += norm[2];
					}
					normalssum[0] = normalssum[0] / a->widgets[i].size();
					normalssum[1] = normalssum[1] / a->widgets[i].size();
					normalssum[2] = normalssum[2] / a->widgets[i].size();

					for (int j = 0; j < a->widgets[i].size(); j++)
					{
						a->widgets.at(i).at(j).avgnormals = vtkSmartPointer<vtkPoints>::New();
						a->widgets.at(i).at(j).avgnormals->InsertNextPoint(normalssum[0], normalssum[1], normalssum[2]);
					}
				}

				// Run through all widgets and update actors
				for (int i = 0; i < a->mywidgets.size(); i++)	// Vector of Vector of Widget Elems (n sets of separate Widgets)
				{
					double *myavgnorm = a->mywidgets.at(i).avgnormal->GetPoint(0);

					for (int j = 0; j < a->mywidgets.at(i).elems.size(); j++)
					{
						vtkSmartPointer<vtkActor> myActor = a->mywidgets.at(i).elems.at(j).actor;

						if (!myActor)
						{
							a->mywidgets.at(i).elems.at(j).actor = vtkSmartPointer<vtkActor>::New();
							a->renderer->AddActor(a->mywidgets.at(i).elems.at(j).actor);
						}

						//vtkPolyDataMapper * myMapper = (vtkPolyDataMapper *)(myActor->GetMapper());
						//vtkPolyData * myPoly= (vtkPolyData*)(myMapper->GetInput());

						int p1_index = a->mywidgets.at(i).elems.at(j).p1_index;
						int p2_index = a->mywidgets.at(i).elems.at(j).p2_index;

						if (p1_index == -1 || p2_index == -1)
							continue;

						double *beforepts = a->mywidgets.at(i).points.at(p1_index).point->GetPoint(0);
						double *pts = a->mywidgets.at(i).points.at(p2_index).point->GetPoint(0);

						double *beforenorm = a->mywidgets.at(i).points.at(p1_index).normal->GetPoint(0);
						double *norm = a->mywidgets.at(i).points.at(p2_index).normal->GetPoint(0);

						float beforetoafter[3];
						beforetoafter[0] = pts[0] - beforepts[0];
						beforetoafter[1] = pts[1] - beforepts[1];
						beforetoafter[2] = pts[2] - beforepts[2];

						vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
						points->InsertNextPoint(0, 0, 0);
						points->InsertNextPoint(pts[0] - beforepts[0], pts[1] - beforepts[1], pts[2] - beforepts[2]);
						points->InsertNextPoint(pts[0] - beforepts[0] + myavgnorm[0], pts[1] - beforepts[1] + myavgnorm[1], pts[2] - beforepts[2] + myavgnorm[2]);
						points->InsertNextPoint(myavgnorm[0], myavgnorm[1], myavgnorm[2]);

						// Create a quad on the four points
						vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
						quad->GetPointIds()->SetId(0, 0);
						quad->GetPointIds()->SetId(1, 1);
						quad->GetPointIds()->SetId(2, 2);
						quad->GetPointIds()->SetId(3, 3);

						// Create a cell array to store the quad in
						vtkSmartPointer<vtkCellArray> quads = vtkSmartPointer<vtkCellArray>::New();
						quads->InsertNextCell(quad);

						vtkSmartPointer<vtkPolyData> myNewPoly = vtkSmartPointer<vtkPolyData>::New();
						myNewPoly->SetPoints(points);
						myNewPoly->SetPolys(quads);

						vtkSmartPointer<vtkPolyDataMapper> myMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
						myMapper->SetInputData(myNewPoly);
						myMapper->Update();

						a->mywidgets.at(i).elems.at(j).actor->SetMapper(myMapper);
						a->mywidgets.at(i).elems.at(j).actor->GetProperty()->SetDiffuseColor(1, 0.0, 0);
						a->mywidgets.at(i).elems.at(j).actor->GetProperty()->SetOpacity(0.5);
						a->mywidgets.at(i).elems.at(j).actor->SetPosition(beforepts[0], beforepts[1], beforepts[2]);
						a->mywidgets.at(i).elems.at(j).actor->PickableOff();

						// COMMENT OFF
						//elem.cellLocator  = vtkSmartPointer<vtkCellLocator>::New();
						//elem.cellLocator  ->SetDataSet(appendPoly->GetOutput());
						//elem.cellLocator  ->BuildLocator();
						//elem.cellLocator  ->LazyEvaluationOn();
						//cellPicker->AddLocator(elem.cellLocator);

						//worldPicker->AddLocator(elem.cellLocator);
					}
				}

				// Run through all widgets and update actors
				for (int i = 0; i < a->widgets.size(); i++)	// Vector of Vector of Widget Elems (n sets of separate Widgets)
				{
					for (int j = 1; j < a->widgets[i].size(); j++)
					{
						vtkPolyDataMapper * myMapper = (vtkPolyDataMapper *)(a->widgets.at(i).at(j).actor->GetMapper());
						vtkPolyData * myPoly = (vtkPolyData*)(a->widgets.at(i).at(j).actor->GetMapper()->GetInput());

						double *beforepts = a->widgets.at(i).at(j - 1).endpoints->GetPoint(0);
						double *pts = a->widgets.at(i).at(j).endpoints->GetPoint(0);

						double *beforenorm = a->widgets.at(i).at(j - 1).normals->GetPoint(0);
						double *norm = a->widgets.at(i).at(j).normals->GetPoint(0);

						double *myavgnorm = a->widgets.at(i).at(j).avgnormals->GetPoint(0);

						float beforetoafter[3];
						beforetoafter[0] = pts[0] - beforepts[0];
						beforetoafter[1] = pts[1] - beforepts[1];
						beforetoafter[2] = pts[2] - beforepts[2];

						vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
						//points->InsertNextPoint(beforepts[0], beforepts[1], beforepts[2] );
						//points->InsertNextPoint(pts[0], pts[1], pts[2] );
						//points->InsertNextPoint(pts[0] + myavgnorm[0], pts[1] + myavgnorm[1], pts[2] + myavgnorm[2]);
						//points->InsertNextPoint(beforepts[0] + myavgnorm[0], beforepts[1]+ myavgnorm[1], beforepts[2] + myavgnorm[2]);
						points->InsertNextPoint(0, 0, 0);
						points->InsertNextPoint(pts[0] - beforepts[0], pts[1] - beforepts[1], pts[2] - beforepts[2]);
						points->InsertNextPoint(pts[0] - beforepts[0] + myavgnorm[0], pts[1] - beforepts[1] + myavgnorm[1], pts[2] - beforepts[2] + myavgnorm[2]);
						points->InsertNextPoint(myavgnorm[0], myavgnorm[1], myavgnorm[2]);

						// Create a quad on the four points
						vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
						quad->GetPointIds()->SetId(0, 0);
						quad->GetPointIds()->SetId(1, 1);
						quad->GetPointIds()->SetId(2, 2);
						quad->GetPointIds()->SetId(3, 3);

						// Create a cell array to store the quad in
						vtkSmartPointer<vtkCellArray> quads = vtkSmartPointer<vtkCellArray>::New();
						quads->InsertNextCell(quad);

						vtkSmartPointer<vtkPolyData> myNewPoly = vtkSmartPointer<vtkPolyData>::New();
						myNewPoly->SetPoints(points);
						myNewPoly->SetPolys(quads);

						vtkSmartPointer<vtkSphereSource> newSphere = vtkSmartPointer<vtkSphereSource>::New();
						//newSphere->SetCenter(pts[0] , pts[1] , pts[2] );
						newSphere->SetCenter(pts[0] - beforepts[0], pts[1] - beforepts[1], pts[2] - beforepts[2]);
						newSphere->SetRadius(a->mouseSize / mouseSizeDivide);
						newSphere->Update();

						vtkSmartPointer<vtkAppendPolyData> appendPoly = vtkSmartPointer<vtkAppendPolyData>::New();
						appendPoly->AddInputData(myNewPoly);
						appendPoly->AddInputData(newSphere->GetOutput());
						appendPoly->Update();

						//vtkSmartPointer<vtkTransform> transformm = vtkSmartPointer<vtkTransform>::New();
						//transformm->Translate(beforepts[0], beforepts[1], beforepts[2]);

						//vtkSmartPointer<vtkTransformPolyDataFilter> transformPoly = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
						//transformPoly->SetInputData(appendPoly->GetOutput());
						//transformPoly->SetTransform(transformm);
						//transformPoly->Update();
						a->widgets.at(i).at(j).actor->SetPosition(beforepts[0], beforepts[1], beforepts[2]);

						//myPoly->SetPoints(points);
						//myPoly->SetPolys(quads);
						myMapper->SetInputData(appendPoly->GetOutput());

						// COMMENT OFF
						//elem.cellLocator  = vtkSmartPointer<vtkCellLocator>::New();
						//elem.cellLocator  ->SetDataSet(appendPoly->GetOutput());
						//elem.cellLocator  ->BuildLocator();
						//elem.cellLocator  ->LazyEvaluationOn();
						//cellPicker->AddLocator(elem.cellLocator);

						//worldPicker->AddLocator(elem.cellLocator);
					}
				}

				//for (int i = 0; i < a->widgets.size(); i++)	// Vector of Vector of Widget Elems (n sets of separate Widgets)
				//{
				//	//std::cout << "\n----";
				//	//std::cout << "\n" << widgets[i].size() << "\n";

				//	for (int j = 1; j < a->widgets[i].size(); j++)
				//	{
				//		break;
				//		//double *pts = widgets.at(i).at(j).endpoints->GetPoint(0);
				//		//std::cout << pts[0] << ", " << pts[1] << ", " << pts[2] << "\n";

				//		// get j-1 and j points to link together to make plane
				//		double *beforepts = a->widgets.at(i).at(j-1).endpoints->GetPoint(0);
				//		double *pts = a->widgets.at(i).at(j).endpoints->GetPoint(0);

				//		double *beforenorm = a->widgets.at(i).at(j-1).normals->GetPoint(0);
				//		double *norm = a->widgets.at(i).at(j).normals->GetPoint(0);

				//		float avgnorm[3];
				//		avgnorm[0] = (beforenorm[0] + norm[0]) / 2.0f;
				//		avgnorm[1] = (beforenorm[1] + norm[1]) / 2.0f;
				//		avgnorm[2] = (beforenorm[2] + norm[2]) / 2.0f;

				//		//widgets.at(i).at(j-1).marked = true;
				//		//widgets.at(i).at(j).marked = true;

				//		// if at least one not marked
				//		if (a->widgets.at(i).at(j-1).marked == false || a->widgets.at(i).at(j).marked == false)
				//		{
				//			a->widgets.at(i).at(j-1).marked = true;
				//			a->widgets.at(i).at(j).marked = true;

				//			float beforetoafter[3];
				//			beforetoafter[0] = pts[0] - beforepts[0];
				//			beforetoafter[1] = pts[1] - beforepts[1];
				//			beforetoafter[2] = pts[2] - beforepts[2];

				//			// Draw the plane connecting
				//			vtkSmartPointer<vtkLineSource> line2 = vtkSmartPointer<vtkLineSource>::New();
				//			line2->SetPoint1(0, 0, 0);
				//			line2->SetPoint2(beforetoafter[0], beforetoafter[1], beforetoafter[2]);

				//			vtkSmartPointer<vtkTubeFilter> tube2 = vtkSmartPointer<vtkTubeFilter>::New();
				//			tube2->SetInputConnection(line2->GetOutputPort());
				//			tube2->SetRadius(tubeWidth); //default is .5
				//			tube2->SetNumberOfSides(50);
				//			tube2->Update();

				//			vtkSmartPointer<vtkTransform> transform2 = vtkSmartPointer<vtkTransform>::New();
				//			transform2->Translate(beforepts[0], beforepts[1], beforepts[2]);
				//
				//			vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
				//			transformFilter2->SetInputData(tube2->GetOutput());
				//			transformFilter2->SetTransform(transform2);
				//			transformFilter2->Update();
				//
				//			vtkSmartPointer<vtkPolyDataMapper> mapper2 =vtkSmartPointer<vtkPolyDataMapper>::New();
				//			mapper2->SetInputConnection(transformFilter2->GetOutputPort());

				//			vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
				//			actor2->SetMapper(mapper2);
				//			actor2->PickableOff();
				//			actor2->GetProperty()->SetDiffuseColor(0, 1, .5);

				//			a->renderer->AddActor(actor2);

				//			// Draw a line from first to second point (where second point is now avg of two normals)
				//			// Draw the plane connecting

				//			vtkSmartPointer<vtkLineSource> line3 = vtkSmartPointer<vtkLineSource>::New();
				//			line3->SetPoint1(0, 0, 0);
				//			line3->SetPoint2(beforetoafter[0] + avgnorm[0], beforetoafter[1] + avgnorm[1], beforetoafter[2] + avgnorm[2]);

				//			vtkMath::Normalize(avgnorm);
				//			vtkMath::Normalize(beforenorm);

				//			double aa[3];
				//			double bb[3];
				//			double cc[3];

				//			bb[0] = beforetoafter[0];
				//			bb[1] = beforetoafter[1];
				//			bb[2] = beforetoafter[2];
				//

				//			aa[0] = beforetoafter[0] + avgnorm[0];//beforetoafter[0];
				//			aa[1] = beforetoafter[1] + avgnorm[1];//beforetoafter[1];
				//			aa[2] = beforetoafter[2] + avgnorm[2];//beforetoafter[2];

				//			//cross((c-a),(b-a))

				//			vtkMath::Cross(bb, aa, cc);

				//			std::cout << aa[0] << aa[1] << aa[2] << std::endl;
				//			std::cout << bb[0] << bb[1] << bb[2] << std::endl;
				//			std::cout << cc[0] << cc[1] << cc[2] << std::endl;

				//
				//			vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
				//			points->InsertNextPoint(0, 0, 0);
				//			points->InsertNextPoint(beforetoafter[0], beforetoafter[1], beforetoafter[2]);
				//			points->InsertNextPoint(beforetoafter[0]+avgnorm[0],beforetoafter[1]+avgnorm[1],beforetoafter[2]+avgnorm[2]);
				//			points->InsertNextPoint(avgnorm[0], avgnorm[1], avgnorm[2]);

				//			// Create a quad on the four points
				//			vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
				//			quad->GetPointIds()->SetId(0,0);
				//			quad->GetPointIds()->SetId(1,1);
				//			quad->GetPointIds()->SetId(2,2);
				//			quad->GetPointIds()->SetId(3,3);

				//			// Create a cell array to store the quad in
				//			vtkSmartPointer<vtkCellArray> quads = vtkSmartPointer<vtkCellArray>::New();
				//			quads->InsertNextCell(quad);

				//			// Create a polydata to store everything in
				//			vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();

				//			// Add the points and quads to the dataset
				//			polydata->SetPoints(points);
				//			polydata->SetPolys(quads);

				//			vtkSmartPointer<vtkPlaneSource> planeSource = vtkSmartPointer<vtkPlaneSource>::New();
				//			//planeSource->SetCenter(beforetoafter[0] + avgnorm[0], beforetoafter[1] + avgnorm[1], beforetoafter[2] + avgnorm[2]);
				//			planeSource->SetCenter(0,0,0);
				//			//planeSource->SetNormal(cc);
				//			//planeSource->set
				//			planeSource->SetNormal(cc);
				//			//planeSource->SetOrigin(0, 0, 0);
				//			//planeSource->SetPoint1(aa);
				//			//planeSource->SetPoint1(bb);
				//			planeSource->Update();

				//			vtkSmartPointer<vtkTubeFilter> tube3 = vtkSmartPointer<vtkTubeFilter>::New();
				//			tube3->SetInputConnection(line3->GetOutputPort());
				//			tube3->SetRadius(tubeWidth); //default is .5
				//			tube3->SetNumberOfSides(50);
				//			tube3->Update();

				//			vtkSmartPointer<vtkTransform> transform3 = vtkSmartPointer<vtkTransform>::New();
				//			transform3->Translate(beforepts[0], beforepts[1], beforepts[2]);

				//			vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter3 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
				//			//transformFilter3->SetInputData(tube3->GetOutput());
				//			//transformFilter3->SetInputData(planeSource->GetOutput());
				//			transformFilter3->SetInputData(polydata);
				//			transformFilter3->SetTransform(transform3);
				//			transformFilter3->Update();

				//			vtkSmartPointer<vtkPolyDataMapper> mapper3 =vtkSmartPointer<vtkPolyDataMapper>::New();
				//			mapper3->SetInputConnection(transformFilter3->GetOutputPort());

				//			vtkSmartPointer<vtkActor> actor3 = vtkSmartPointer<vtkActor>::New();
				//			actor3->SetMapper(mapper3);
				//			actor3->PickableOff();
				//			actor3->GetProperty()->SetDiffuseColor(0, 0., 1);
				//			actor3->GetProperty()->SetOpacity(0.2);

				//			a->renderer->AddActor(actor3);

				//			// Draw a line from second to second point + avgnorm
				//			// Draw the plane connecting

				//			vtkSmartPointer<vtkLineSource> line4 = vtkSmartPointer<vtkLineSource>::New();
				//			line4->SetPoint1(beforetoafter[0], beforetoafter[1], beforetoafter[2]);
				//			line4->SetPoint2(beforetoafter[0] + avgnorm[0], beforetoafter[1] + avgnorm[1], beforetoafter[2] + avgnorm[2]);

				//			vtkSmartPointer<vtkTubeFilter> tube4 = vtkSmartPointer<vtkTubeFilter>::New();
				//			tube4->SetInputConnection(line4->GetOutputPort());
				//			tube4->SetRadius(tubeWidth); //default is .5
				//			tube4->SetNumberOfSides(50);
				//			tube4->Update();

				//			vtkSmartPointer<vtkTransform> transform4 = vtkSmartPointer<vtkTransform>::New();
				//			transform4->Translate(beforepts[0], beforepts[1], beforepts[2]);

				//			vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter4 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
				//			transformFilter4->SetInputData(tube4->GetOutput());
				//			transformFilter4->SetTransform(transform4);
				//			transformFilter4->Update();

				//			vtkSmartPointer<vtkPolyDataMapper> mapper4 =vtkSmartPointer<vtkPolyDataMapper>::New();
				//			mapper4->SetInputConnection(transformFilter4->GetOutputPort());

				//			vtkSmartPointer<vtkActor> actor4 = vtkSmartPointer<vtkActor>::New();
				//			actor4->SetMapper(mapper4);
				//			actor4->PickableOff();
				//			actor4->GetProperty()->SetDiffuseColor(0, 0., 1);

				//			a->renderer->AddActor(actor4);

				//		}

				//	}
				//}
				//	}
			}

			a->renderer->Render();
			//vtkSmartPointer<vtkMapper> mapper = vtkSmartPointer<vtkMapper>::New();
			//mapper->SetInputConnection(sphere->GetOutputPort());
		}
		//OnMouseMove();

		//a->updateMouseShader();
		a->updateDisplay();
		//throw std::exception("The method or operation is not implemented.");
		vtkInteractorStyleTrackballCamera::OnKeyPress();
	}

	//----------------------------------------------------------------------------
	/// <summary>
	/// Called when [mouse move].
	/// </summary>
	/// ----------------------------------------------------------------------------
	virtual void OnMouseMove()  override
	{
		// update shader
		//a->updateMat();

		int x = this->Interactor->GetEventPosition()[0];
		int y = this->Interactor->GetEventPosition()[1];

		//this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
		//	this->Interactor->GetEventPosition()[1], 0,  // always zero.
		//	this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

		//a->mouse[0] = this->Interactor->GetEventPosition()[0];
		//a->mouse[1] = this->Interactor->GetEventPosition()[1];
		//a->mouse[2] = 0;

		//worldPicker->GetActor()

		//worldPicker->Pick(this->Interactor->GetEventPosition()[0],
		//		this->Interactor->GetEventPosition()[1], 0,  // always zero.
		//		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

		//this->Interactor->GetPicker()->GetPickPosition(a->mouse);
		//worldPicker->GetPickPosition(a->mouse);

		cellPicker->Pick(this->Interactor->GetEventPosition()[0],
			this->Interactor->GetEventPosition()[1], 0,  // always zero.
			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

		cellPicker->GetPickPosition(a->mouse);

		//std::cout << a->mouse[0] << ", " << a->mouse[1] << ", " << a->mouse[2] << std::endl;

		vtkSmartPointer<vtkActor> actorHovered = cellPicker->GetActor();

		if (actorHovered)
		{
			// Only drag if you hovered over the correct actor (selected one)
			std::vector<CustomMesh>::iterator it = std::find(a->meshes.begin(), a->meshes.end(), actorHovered);

			if (it != a->meshes.end())
			{
				if (dragging)
				{
					if (a->clickedWidgetActor)
					{
						for (int i = 0; i < a->mywidgets.size(); i++)
						{
							std::vector<MyPoint>::iterator it2 = std::find(a->mywidgets.at(i).points.begin(), a->mywidgets.at(i).points.end(), a->clickedWidgetActor);

							if (it2 != a->mywidgets.at(i).points.end())	// found
							{
								it2->point->SetPoint(0, a->mouse[0], a->mouse[1], a->mouse[2]);

								double normal[3];
								cellPicker->GetPickNormal(normal);

								it2->normal->SetPoint(0, normal[0], normal[1], normal[2]);

								a->clickedWidgetActor->SetPosition(a->mouse[0], a->mouse[1], a->mouse[2]);
							}
						}

						// Calculate avg normals
						for (int i = 0; i < a->mywidgets.size(); i++)
						{
							float normalssum[3] = { 0, 0, 0 };

							for (int j = 0; j < a->mywidgets.at(i).points.size(); j++)
							{
								double *norm = a->mywidgets.at(i).points.at(j).normal->GetPoint(0);

								normalssum[0] += norm[0];
								normalssum[1] += norm[1];
								normalssum[2] += norm[2];
							}
							normalssum[1] = normalssum[1] / a->mywidgets.at(i).points.size();
							normalssum[0] = normalssum[0] / a->mywidgets.at(i).points.size();
							normalssum[2] = normalssum[2] / a->mywidgets.at(i).points.size();

							a->mywidgets.at(i).avgnormal = vtkSmartPointer<vtkPoints>::New();
							a->mywidgets.at(i).avgnormal->InsertNextPoint(normalssum[0], normalssum[1], normalssum[2]);
						}

						//a->clickedWidgetActor->SetPosition(a->mouse);

						// Run through all widgets and update actors
						for (int i = 0; i < a->mywidgets.size(); i++)	// Vector of Vector of Widget Elems (n sets of separate Widgets)
						{
							double *myavgnorm = a->mywidgets.at(i).avgnormal->GetPoint(0);

							for (int j = 0; j < a->mywidgets.at(i).elems.size(); j++)
							{
								vtkSmartPointer<vtkActor> myActor = a->mywidgets.at(i).elems.at(j).actor;

								if (!myActor)
								{
									a->mywidgets.at(i).elems.at(j).actor = vtkSmartPointer<vtkActor>::New();

									//vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
									//vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
									//vtkSmartPointer<vtkCellArray> quads = vtkSmartPointer<vtkCellArray>::New();
									//quads->InsertNextCell(quad);
									//vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
									//polydata->SetPoints(points);
									//polydata->SetPolys(quads);

									//vtkSmartPointer<vtkPolyDataMapper> tempMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
									//tempMapper->SetInputData(polydata);
									//tempMapper->Update();

									//a->mywidgets.at(i).elems.at(j).actor->SetMapper(tempMapper);
									a->renderer->AddActor(a->mywidgets.at(i).elems.at(j).actor);
								}

								int p1_index = a->mywidgets.at(i).elems.at(j).p1_index;
								int p2_index = a->mywidgets.at(i).elems.at(j).p2_index;

								if (p1_index == -1 || p2_index == -1)
									continue;

								vtkPolyDataMapper * myMapper = (vtkPolyDataMapper *)(myActor->GetMapper());
								vtkPolyData * myPoly = (vtkPolyData*)(myMapper->GetInput());

								double *beforepts = a->mywidgets.at(i).points.at(p1_index).point->GetPoint(0);
								double *pts = a->mywidgets.at(i).points.at(p2_index).point->GetPoint(0);

								double *beforenorm = a->mywidgets.at(i).points.at(p1_index).normal->GetPoint(0);
								double *norm = a->mywidgets.at(i).points.at(p2_index).normal->GetPoint(0);

								float beforetoafter[3];
								beforetoafter[0] = pts[0] - beforepts[0];
								beforetoafter[1] = pts[1] - beforepts[1];
								beforetoafter[2] = pts[2] - beforepts[2];

								//myPoly->GetPointData()->
								vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
								points->InsertNextPoint(0, 0, 0);
								points->InsertNextPoint(pts[0] - beforepts[0], pts[1] - beforepts[1], pts[2] - beforepts[2]);
								points->InsertNextPoint(pts[0] - beforepts[0] + myavgnorm[0], pts[1] - beforepts[1] + myavgnorm[1], pts[2] - beforepts[2] + myavgnorm[2]);
								points->InsertNextPoint(myavgnorm[0], myavgnorm[1], myavgnorm[2]);

								// Create a quad on the four points
								vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
								quad->GetPointIds()->SetId(0, 0);
								quad->GetPointIds()->SetId(1, 1);
								quad->GetPointIds()->SetId(2, 2);
								quad->GetPointIds()->SetId(3, 3);

								// Create a cell array to store the quad in
								vtkSmartPointer<vtkCellArray> quads = vtkSmartPointer<vtkCellArray>::New();
								quads->InsertNextCell(quad);

								//vtkSmartPointer<vtkPolyData> myNewPoly = vtkSmartPointer<vtkPolyData>::New();
								//myNewPoly->SetPoints(points);
								//myNewPoly->SetPolys(quads);

								myPoly->SetPoints(points);
								myPoly->SetPolys(quads);

								//vtkSmartPointer<vtkPolyDataMapper> myMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
								//myMapper->SetInputData(myNewPoly);
								//a->mywidgets.at(i).elems.at(j).actor->SetMapper(myMapper);

								a->mywidgets.at(i).elems.at(j).actor->GetProperty()->SetDiffuseColor(1, 0.0, 0);
								a->mywidgets.at(i).elems.at(j).actor->GetProperty()->SetOpacity(0.5);
								a->mywidgets.at(i).elems.at(j).actor->SetPosition(beforepts[0], beforepts[1], beforepts[2]);
								a->mywidgets.at(i).elems.at(j).actor->PickableOff();

								// COMMENT OFF
								//elem.cellLocator  = vtkSmartPointer<vtkCellLocator>::New();
								//elem.cellLocator  ->SetDataSet(appendPoly->GetOutput());
								//elem.cellLocator  ->BuildLocator();
								//elem.cellLocator  ->LazyEvaluationOn();
								//cellPicker->AddLocator(elem.cellLocator);

								//worldPicker->AddLocator(elem.cellLocator);
							}
						}
					}
					//return;
				}
			}

			for (int i = 0; i < a->mywidgets.size(); i++)
			{
				std::vector<MyPoint>::iterator it2 = std::find(a->mywidgets.at(i).points.begin(), a->mywidgets.at(i).points.end(), actorHovered);

				if (it2 != a->mywidgets.at(i).points.end())	// found
				{
					if (a->selectedWidgetActor)
						a->selectedWidgetActor->GetProperty()->SetDiffuseColor(1, 0, 0);

					it2->actor->GetProperty()->SetDiffuseColor(0, 1, 0);

					a->selectedWidgetActor = it2->actor;
					//std::cout << "actor hit" << std::endl;
				}
			}

			for (int i = 0; i < a->widgets.size(); i++)
			{
				std::vector<WidgetElem>::iterator it2 = std::find(a->widgets[i].begin(), a->widgets[i].end(), actorHovered);

				if (it2 != a->widgets[i].end())	// found
				{
					if (a->selectedWidgetActor)
						a->selectedWidgetActor->GetProperty()->SetDiffuseColor(1, 0, 0);

					it2->actor->GetProperty()->SetDiffuseColor(0, 1, 0);

					a->selectedWidgetActor = it2->actor;
					std::cout << "actor hit" << std::endl;
				}
			}
		}

		//std::cout << a->mouse[0] << ", " << a->mouse[1] << ", " << a->mouse[2] << std::endl;

		//if (worldPicker->GetActor())
		//;std::cout << worldPicker->GetActor()->GetProperty()->GetOpacity() << std::endl;
		//else
		//;std::cout << "M" << std::endl;

		//std::cout << a->mouse[0] << ", " << a->mouse[1] << ", " << a->mouse[2] << std::endl;

		// move cursor

		//const vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
		//transform->Translate(a->mouse[0], a->mouse[1], a->mouse[2]+1);

		//transform->Scale(2, 2, 2);

		//a->mainCursor->SetUserTransform(transform);

		//a->renderer->GetActors()->GetLastActor()->transform
		//a->renderer->GetActors()->GetLastActor()->Transform(transform);
		//a->renderer->GetActors()->GetLastActor()->

		//a->meshes[a->selectedIndex].actor->GetMapper()->GetOutputDataObject()

		vtkPolyData * thepolydata = static_cast<vtkPolyData *> (a->meshes[a->selectedIndex].actor->GetMapper()->GetInput());

		float diffx = thepolydata->GetBounds()[1] - thepolydata->GetBounds()[0];	// diff in maxx and minx
		float diffy = thepolydata->GetBounds()[3] - thepolydata->GetBounds()[2];	// diff in maxx and minx
		float diffz = thepolydata->GetBounds()[5] - thepolydata->GetBounds()[4];	// diff in maxx and minx

		float avg = (diffx + diffy + diffz) / 3.0;
		a->mouseSize = avg / a->brushDivide;

		//if (this->GetCurrentRenderer())
		//				this->GetCurrentRenderer()->Render();

		//a->updateMouseShader();
		//a->updateDisplay();

		//a->myPicker->PickFromListOn();
		//a->myPicker->GetPickList()->RemoveAllItems();
		//a->myPicker->AddPickList(a->meshes[0].actor);

		//pointPicker->Pick(this->Interactor->GetEventPosition()[0],
		//this->Interactor->GetEventPosition()[1], 0,  // always zero.
		//this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

		/*switch (this->State)
		{
		case VTKIS_ROTATE:
		this->FindPokedRenderer(x, y);
		this->Rotate();
		this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
		break;

		case VTKIS_PAN:
		this->FindPokedRenderer(x, y);
		this->Pan();
		this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
		break;

		case VTKIS_DOLLY:
		this->FindPokedRenderer(x, y);
		this->Dolly();
		this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
		break;

		case VTKIS_SPIN:
		this->FindPokedRenderer(x, y);
		this->Spin();
		this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
		break;
		default:

		break;
		}*/
		vtkInteractorStyleTrackballCamera::OnMouseMove();
	}

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
				std::vector<CustomMesh>::iterator it = std::find(a->meshes.begin(), a->meshes.end(), actorDouble);

				if (it != a->meshes.end())
				{
					// if dbl clicked mesh, set it as selected (update slider/list, etc.)
					a->meshes[a->selectedIndex].selected = 0;
					it->selected = 1;

					int newindex = it._Ptr - &a->meshes[0];
					std::cout << newindex;
					a->selectedIndex = newindex;

					a->updateOpacitySliderAndList();

					std::cout << actorDouble->GetProperty()->GetOpacity() << std::endl;
				}
			}

			this->NumberOfClicks = 0;
		}

		// Check if you clicked on widget then set dragging == true,

		cellPicker->Pick(this->Interactor->GetEventPosition()[0],
			this->Interactor->GetEventPosition()[1],
			0,  // always zero.
			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

		if (creation)
		{
			a->pos1[0] = cellPicker->GetPickPosition()[0];
			a->pos1[1] = cellPicker->GetPickPosition()[1];
			a->pos1[2] = cellPicker->GetPickPosition()[2];
		}

		vtkSmartPointer<vtkActor> actorSingle = cellPicker->GetActor();
		if (actorSingle)
		{
			// If actor single-clicked is one of the widgets
			for (int i = 0; i < a->mywidgets.size(); i++)
			{
				std::vector<MyPoint>::iterator it2 = std::find(a->mywidgets.at(i).points.begin(), a->mywidgets.at(i).points.end(), actorSingle);

				if (it2 != a->mywidgets.at(i).points.end())	// found
				{
					// set dragging true and we will now bound picking to selectedIndex actor until left button released
					// So set pickable off for all widgets until left released - and maybe off for all meshes except active
					dragging = true;
					a->clickedWidgetActor = it2->actor;

					for (int i = 0; i < a->mywidgets.size(); i++)
					{
						for (int j = 0; j < a->mywidgets.at(i).points.size(); j++)
						{
							a->mywidgets.at(i).points.at(j).actor->PickableOff();
						}
					}

					for (int i = 0; i < a->meshes.size(); i++)
					{
						a->meshes.at(i).actor->PickableOff();
					}
					a->meshes.at(a->selectedIndex).actor->PickableOn();

					//if (a->selectedWidgetActor)
					//a->selectedWidgetActor->GetProperty()->SetDiffuseColor(1, 0, 0);

					//it2->actor->GetProperty()->SetDiffuseColor(0, 1, 0);

					//a->selectedWidgetActor = it2->actor;
					//std::cout << "actor hit" << std::endl;
				}
			}

			// If actor single-clicked is one of the widgets
			for (int i = 0; i < a->widgets.size(); i++)
			{
				std::vector<WidgetElem>::iterator it2 = std::find(a->widgets[i].begin(), a->widgets[i].end(), actorSingle);

				if (it2 != a->widgets[i].end())	// found
				{
					// set dragging true and we will now bound picking to selectedIndex actor until left button released
					// So set pickable off for all widgets until left released - and maybe off for all meshes except active
					dragging = true;
					a->clickedWidgetActor = it2->actor;

					for (int i = 0; i < a->widgets.size(); i++)
					{
						for (int j = 0; j < a->widgets[i].size(); j++)
						{
							a->widgets.at(i).at(j).actor->PickableOff();
						}
					}

					for (int i = 0; i < a->meshes.size(); i++)
					{
						a->meshes.at(i).actor->PickableOff();
					}
					a->meshes.at(a->selectedIndex).actor->PickableOn();

					//if (a->selectedWidgetActor)
					//a->selectedWidgetActor->GetProperty()->SetDiffuseColor(1, 0, 0);

					//it2->actor->GetProperty()->SetDiffuseColor(0, 1, 0);

					//a->selectedWidgetActor = it2->actor;
					//std::cout << "actor hit" << std::endl;
				}
			}
		}

		if (dragging == true)
		{
			// if dragging, cannot forward events
		}
		else
		{
			// forward events
			vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
		}

		//vtkSmartPointer<vtkWorldPointPicker> pickerType = vtkSmartPointer<vtkWorldPointPicker>::New();
		//this->Interactor->SetPicker(pickerType);

		//std::cout << "Picking pixel: " << this->Interactor->GetEventPosition()[0] << " " << this->Interactor->GetEventPosition()[1] << std::endl;

		//this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
		//	this->Interactor->GetEventPosition()[1],
		//	0,  // always zero.
		//	this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
		//double picked[3];
		//
		////((vtkCellPicker*)this->Interactor->GetPicker())->GetPickNormal(picked);
		//this->Interactor->GetPicker()->GetPickPosition(picked);

		//this->Interactor->GetPicker()->GetPickPosition(picked);

		//this->Interactor->GetPickingManager()
		//std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;
		//std::cout << "Mouse value: " << a->mouse[0] << " " << a->mouse[1] << " " << a->mouse[2] << std::endl;

		//vtkSphereSource * ss = vtkSphereSource::New();
		//ss->SetRadius(0.1);
		//ss->SetCenter(picked[0], picked[1], picked[2]);
		//ss->Update();

		//vtkPolyDataMapper *pp = vtkPolyDataMapper::New();
		//pp->SetInputData(ss->GetOutput());

		//vtkActor * xx = vtkActor::New();
		//xx->SetMapper(pp);

		//
		//a->renderer->AddActor(xx);
		//this->Interactor->GetPicker()->get
		// Forward events
		//vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}

	////----------------------------------------------------------------------------
	/// <summary>
	/// Called when [left button up].
	/// </summary>
	virtual void OnLeftButtonUp() override
	{
		if (dragging)
		{
			a->pos2[0] = a->mouse[0];
			a->pos2[1] = a->mouse[1];
			a->pos2[2] = a->mouse[2];
		}

		dragging = false;

		// set all widgets back to pickable
		for (int i = 0; i < a->mywidgets.size(); i++)
		{
			for (int j = 0; j < a->mywidgets.at(i).points.size(); j++)
			{
				a->mywidgets.at(i).points.at(j).actor->PickableOn();
			}
		}

		// set all widgets back to pickable, and all meshes pickable
		for (int i = 0; i < a->widgets.size(); i++)
		{
			for (int j = 0; j < a->widgets[i].size(); j++)
			{
				a->widgets.at(i).at(j).actor->PickableOn();
			}
		}
		for (int i = 0; i < a->meshes.size(); i++)
		{
			a->meshes.at(i).actor->PickableOn();
		}

		vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
	}

	////----------------------------------------------------------------------------
	/// <summary>
	/// Called when [left button down].
	/// </summary>
	//virtual void OnLeftButtonDown()  override
	//{
	//	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
	//		this->Interactor->GetEventPosition()[1]);
	//	if (this->CurrentRenderer == nullptr)
	//	{
	//		return;
	//	}

	//	this->GrabFocus(this->EventCallbackCommand);
	//
	//	if (this->Interactor->GetShiftKey())
	//	{
	//		if (this->Interactor->GetControlKey())
	//		{
	//			this->StartDolly();
	//		}
	//		else
	//		{
	//			this->StartPan();
	//		}
	//	}
	//	else
	//	{
	//		if (this->Interactor->GetControlKey())
	//		{
	//			this->StartSpin();
	//		}
	//		else
	//		{
	//			this->StartRotate();
	//		}
	//	}
	//}

	//////----------------------------------------------------------------------------
	//virtual void OnLeftButtonUp() override
	//{
	//	switch (this->State)
	//	{
	//	case VTKIS_DOLLY:
	//		this->EndDolly();
	//		break;

	//	case VTKIS_PAN:
	//		this->EndPan();
	//		break;

	//	case VTKIS_SPIN:
	//		this->EndSpin();
	//		break;

	//	case VTKIS_ROTATE:
	//		this->EndRotate();
	//		break;
	//	}

	//	if ( this->Interactor )
	//	{
	//		this->ReleaseFocus();
	//	}
	//}

	//////----------------------------------------------------------------------------
	//virtual void OnMiddleButtonDown()  override
	//{
	//	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
	//		this->Interactor->GetEventPosition()[1]);
	//	if (this->CurrentRenderer == nullptr)
	//	{
	//		return;
	//	}

	//	this->GrabFocus(this->EventCallbackCommand);
	//	this->StartPan();
	//}

	//////----------------------------------------------------------------------------
	////virtual void MouseInteractorStylePP::OnMiddleButtonUp() override
	//{
	//	switch (this->State)
	//	{
	//	case VTKIS_PAN:
	//		this->EndPan();
	//		if ( this->Interactor )
	//		{
	//			this->ReleaseFocus();
	//		}
	//		break;
	//	}
	//}

	//////----------------------------------------------------------------------------
	//virtual void OnRightButtonDown()  override
	//{
	//	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
	//		this->Interactor->GetEventPosition()[1]);
	//	if (this->CurrentRenderer == nullptr)
	//	{
	//		return;
	//	}

	//	this->GrabFocus(this->EventCallbackCommand);
	//	this->StartDolly();
	//}

	//////----------------------------------------------------------------------------
	//virtual void OnRightButtonUp() override
	//{
	//	switch (this->State)
	//	{
	//	case VTKIS_DOLLY:
	//		this->EndDolly();

	//		if ( this->Interactor )
	//		{
	//			this->ReleaseFocus();
	//		}
	//		break;
	//	}
	//}

	//////----------------------------------------------------------------------------
	//virtual void OnMouseWheelForward()  override
	//{
	//	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
	//		this->Interactor->GetEventPosition()[1]);
	//	if (this->CurrentRenderer == nullptr)
	//	{
	//		return;
	//	}

	//	this->GrabFocus(this->EventCallbackCommand);
	//	this->StartDolly();
	//	double factor = this->MotionFactor * 0.2 * this->MouseWheelMotionFactor;
	//	this->Dolly(pow(1.1, factor));
	//	this->EndDolly();
	//	this->ReleaseFocus();
	//}

	//////----------------------------------------------------------------------------
	//virtual void OnMouseWheelBackward() override
	//{
	//	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
	//		this->Interactor->GetEventPosition()[1]);
	//	if (this->CurrentRenderer == nullptr)
	//	{
	//		return;
	//	}

	//	this->GrabFocus(this->EventCallbackCommand);
	//	this->StartDolly();
	//	double factor = this->MotionFactor * -0.2 * this->MouseWheelMotionFactor;
	//	this->Dolly(pow(1.1, factor));
	//	this->EndDolly();
	//	this->ReleaseFocus();
	//}

	//virtual void Rotate() override
	//{
	//	throw std::exception("The method or operation is not implemented.");
	//}
};
vtkStandardNewMacro(MouseInteractorStylePP);
