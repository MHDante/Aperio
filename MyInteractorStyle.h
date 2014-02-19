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

#include "vtkVector.h"

//-----------------------------------------------------------------------------
/// <summary> Class MouseInteractorStylePP, contains key press, mouse events etc.
/// </summary>
class MyInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	/// <summary> Pointer to the mainwindow class so we can access instance variables, etc. </summary>
	additive * a;
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
	void initialize(additive *window)
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
		if (strcmp(this->Interactor->GetKeySym(), "Delete") == 0)	// Del key	 ( probably should put this in QT keycheck, b/c QVTKWidget requires focus)
		{
			a->meshes[a->selectedIndex].opacity = 0;
			a->meshes[a->selectedIndex].actor->GetProperty()->SetOpacity(0);
			a->updateOpacitySliderAndList();
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
		}
		if (this->Interactor->GetKeyCode() == 't')
		{
			a->peerInside = (a->peerInside + 1) % 2;
		}
		if (this->Interactor->GetKeyCode() == 13)	// enter
		{
			//a->mywidgets.push_back(MyWidget());
			//std::cout << "*** NEW WIDGET ***" << std::endl;
			a->myelems.push_back(MyElem());
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

				//mapper->ImmediateModeRenderingOn();

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

			vtkSmartPointer<vtkSphereSource> ss = vtkSmartPointer<vtkSphereSource>::New();
			ss->SetRadius(0.1);
			ss->SetCenter(picked[0], picked[1], picked[2]);
			ss->Update();

			vtkSmartPointer<vtkPolyDataMapper> pp = vtkSmartPointer<vtkPolyDataMapper>::New();
			pp->SetInputData(ss->GetOutput());

			vtkSmartPointer<vtkActor> xx = vtkSmartPointer<vtkActor>::New();
			xx->SetMapper(pp);

			//
			a->renderer->AddActor(xx);
		}
		if (this->Interactor->GetKeyCode() == 'u')
		{
			for (int i = 0; i < a->myelems.size(); i++)
			{
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
		}
		if (this->Interactor->GetKeyCode() == 'o')
		{
			for (int i = 0; i < a->myelems.size(); i++)
			{
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
		}
		if (this->Interactor->GetKeyCode() == 'e')
		{
			for (int i = 0; i < a->myelems.size(); i++)
			{				
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
		}
		if (this->Interactor->GetKeyCode() == 'a')
		{
			int x = 30000;
			std::cout << x << "\n";
		}
		if (this->Interactor->GetKeyCode() == 'q')
		{
			for (int i = 0; i < a->myelems.size(); i++)
			{
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

		}
		if (this->Interactor->GetKeyCode() == ' ')
		{
			if (a->myelems.size() == 0)
			{
				// If elements empty, add the first one
				a->myelems.push_back(MyElem());
			}

			// Pick from this location.
			cellPicker->Pick(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1], 0,
				this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
		
			vtkActor *pickedActor = cellPicker->GetActor();

			if (pickedActor == nullptr)
				cout << "No actor picked\n";
			else
			{
				cout << "Actor picked\n";

				double poss[3];
				cellPicker->GetPickPosition(poss);

				vtkPolyData* thepolydata = vtkPolyData::SafeDownCast(pickedActor->GetMapper()->GetInput());		

				double normal[3];
				cellPicker->GetPickNormal(normal);


				return;


				int selectedwidgetind = a->mywidgets.size() - 1;

				MyWidgetPoint p;
				p.normal = vtkSmartPointer<vtkPoints>::New();
				p.normal->InsertNextPoint(normal[0], normal[1], normal[2]);

				p.point = vtkSmartPointer<vtkPoints>::New();
				p.point->InsertNextPoint(poss[0], poss[1], poss[2]);

				p.actor = vtkSmartPointer<vtkActor>::New();
				//p.actor->SetMapper(mapper);
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
						//newSphere->SetRadius(a->mouseSize / mouseSizeDivide);
						newSphere->Update();

						vtkSmartPointer<vtkAppendPolyData> appendPoly = vtkSmartPointer<vtkAppendPolyData>::New();
						appendPoly->AddInputData(myNewPoly);
						appendPoly->AddInputData(newSphere->GetOutput());
						appendPoly->Update();

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
			}
			a->renderer->Render();
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

		vtkSmartPointer<vtkActor> actorHovered = cellPicker->GetActor();
		
		vtkInteractorStyleTrackballCamera::OnMouseMove();
		return;
		if (actorHovered)
		{
			// Only drag if you hovered over the correct actor (selected one)
			std::vector<CustomMesh>::iterator it = std::find(a->meshes.begin(), a->meshes.end(), actorHovered.GetPointer());

			if (it != a->meshes.end())
			{
				// Mouse dragged over actor
			}
			return;


			if (it != a->meshes.end())
			{
				if (dragging)
				{
					if (a->clickedWidgetActor)
					{
						for (int i = 0; i < a->mywidgets.size(); i++)
						{
							std::vector<MyWidgetPoint>::iterator it2 = std::find(a->mywidgets.at(i).points.begin(), a->mywidgets.at(i).points.end(), a->clickedWidgetActor);

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
				std::vector<MyWidgetPoint>::iterator it2 = std::find(a->mywidgets.at(i).points.begin(), a->mywidgets.at(i).points.end(), actorHovered);

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

		if (a->meshes.size() > 0)
		{
			
			vtkPolyData* thepolydata = vtkPolyData::SafeDownCast(a->meshes.at(a->selectedIndex).actor->GetMapper()->GetInput());
			float diffx = thepolydata->GetBounds()[1] - thepolydata->GetBounds()[0];	// diff in maxx and minx
			float diffy = thepolydata->GetBounds()[3] - thepolydata->GetBounds()[2];	// diff in maxx and minx
			float diffz = thepolydata->GetBounds()[5] - thepolydata->GetBounds()[4];	// diff in maxx and minx

			float avg = (diffx + diffy + diffz) / 3.0;
			a->mouseSize = avg / a->brushDivide;
		}
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
		if (actorSingle)
		{
			// If actor selected is one of the CustomMesh
			auto it = std::find(a->meshes.begin(), a->meshes.end(), actorSingle.GetPointer());

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
				//std::cout << "start dragging\n";
			}

			// If actor single-clicked is one of the widgets
			//for (int i = 0; i < a->mywidgets.size(); i++)
			//{
			//	std::vector<MyWidgetPoint>::iterator it2 = std::find(a->mywidgets.at(i).points.begin(), a->mywidgets.at(i).points.end(), actorSingle);

			//	if (it2 != a->mywidgets.at(i).points.end())	// found
			//	{
			//		// set dragging true and we will now bound picking to selectedIndex actor until left button released
			//		// So set pickable off for all widgets until left released - and maybe off for all meshes except active
			//		dragging = true;
			//		a->clickedWidgetActor = it2->actor;

			//		for (int i = 0; i < a->mywidgets.size(); i++)
			//		{
			//			for (int j = 0; j < a->mywidgets.at(i).points.size(); j++)
			//			{
			//				a->mywidgets.at(i).points.at(j).actor->PickableOff();
			//			}
			//		}

			//		for (int i = 0; i < a->meshes.size(); i++)
			//		{
			//			a->meshes.at(i).actor->PickableOff();
			//		}
			//		a->meshes.at(a->selectedIndex).actor->PickableOn();

			//		//if (a->selectedWidgetActor)
			//		//a->selectedWidgetActor->GetProperty()->SetDiffuseColor(1, 0, 0);

			//		//it2->actor->GetProperty()->SetDiffuseColor(0, 1, 0);

			//		//a->selectedWidgetActor = it2->actor;
			//		//std::cout << "actor hit" << std::endl;
			//	}
			//}

			// If actor single-clicked is one of the widgets
			//for (int i = 0; i < a->widgets.size(); i++)
			//{
			//	std::vector<WidgetElem>::iterator it2 = std::find(a->widgets[i].begin(), a->widgets[i].end(), actorSingle);

			//	if (it2 != a->widgets[i].end())	// found
			//	{
			//		// set dragging true and we will now bound picking to selectedIndex actor until left button released
			//		// So set pickable off for all widgets until left released - and maybe off for all meshes except active
			//		dragging = true;
			//		a->clickedWidgetActor = it2->actor;

			//		for (int i = 0; i < a->widgets.size(); i++)
			//		{
			//			for (int j = 0; j < a->widgets[i].size(); j++)
			//			{
			//				a->widgets.at(i).at(j).actor->PickableOff();
			//			}
			//		}

			//		for (int i = 0; i < a->meshes.size(); i++)
			//		{
			//			a->meshes.at(i).actor->PickableOff();
			//		}
			//		a->meshes.at(a->selectedIndex).actor->PickableOn();

			//		//if (a->selectedWidgetActor)
			//		//a->selectedWidgetActor->GetProperty()->SetDiffuseColor(1, 0, 0);

			//		//it2->actor->GetProperty()->SetDiffuseColor(0, 1, 0);

			//		//a->selectedWidgetActor = it2->actor;
			//		//std::cout << "actor hit" << std::endl;
			//	}
			//}
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
	}
	////----------------------------------------------------------------------------
	/// <summary> Called when [left button up].
	/// </summary>
	virtual void OnLeftButtonUp() override
	{
		if (dragging)
		{
			dragging = false;

			// check if p2 is on an actor

			a->pos2[0] = a->mouse[0];
			a->pos2[1] = a->mouse[1];
			a->pos2[2] = a->mouse[2];

			a->norm2[0] = a->mouseNorm[0];
			a->norm2[1] = a->mouseNorm[1];
			a->norm2[2] = a->mouseNorm[2];

			auto dist = [](float pos1[3], float pos2[3]) -> float
			{
				return sqrtf( pow(pos2[2] - pos1[2], 2.0f) + pow(pos2[1] - pos1[1], 2.0f) + pow(pos2[0] - pos1[0], 2.0f) );
			};
			
			//if (dist(a->pos1, a->pos2) < 0.05)
			//{
				//std::cout << "stop dragging NO GOOD\n";
				//return;
			//}
			// Otherwise, good

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

			// Get forward/up/right vectors
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
		
			//transform->Concatenate()
			vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
			transformFilter->SetTransform(transform);
			transformFilter->SetInputData(superquad->GetOutput());
			transformFilter->Update();

			vtkSmartPointer<vtkPolyDataMapper> smapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			smapper->SetInputData(transformFilter->GetOutput());

			vtkSmartPointer<vtkActor> sactor = vtkSmartPointer<vtkActor>::New();
			sactor->SetMapper(smapper);
			sactor->GetProperty()->SetDiffuseColor(1, .5, .5);
			sactor->GetProperty()->SetOpacity(.6);
			sactor->GetProperty()->BackfaceCullingOn();
			sactor->PickableOff();

			a->renderer->AddActor(sactor);

			elem.actor = sactor;
			elem.source = superquad;
			elem.transformFilter = transformFilter;

			a->myelems.push_back(elem);

			auto addSphere = [](additive *a, float x, float y, float z){
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
		// set all widgets back to pickable, and all meshes pickable
		//for (int i = 0; i < a->widgets.size(); i++)
		//{
		//	for (int j = 0; j < a->widgets[i].size(); j++)
		//	{
		//		a->widgets.at(i).at(j).actor->PickableOn();
		//	}
		//}
		//for (int i = 0; i < a->meshes.size(); i++)
		//{
		//	a->meshes.at(i).actor->PickableOn();
		//}
		vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
	}
	//----------------------------------------------------------------------------
	virtual void OnMouseWheelForward()  override
	{
		//std::cout << "forward\n";
		vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
	}
	//----------------------------------------------------------------------------
	virtual void OnMouseWheelBackward() override
	{
		//std::cout << "backward\n";
		vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
	}
};
vtkStandardNewMacro(MyInteractorStyle);
