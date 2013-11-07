// ***********************************************************************
// Assembly         : additive
// Author           : David Tran
// Created          : 03-10-2013
//
// Last Modified By : David Tran
// Last Modified On : 03-10-2013
// ***********************************************************************
// <copyright file="additive.h" company="">
//     Copyright (c) . All rights reserved.
// </copyright>
// <summary>
// Main window file, contains interactive events and main code
// Header file contains slots, (functions called in response to signals, such
// as clicking on a button on the QT form)
// </summary>
// ***********************************************************************

#ifndef ADDITIVE_H
#define ADDITIVE_H

#include "ui_additive.h"

// Custom
#include "Utility.h"

#include "vtkCollectionIterator.h"
#include "vtkShader.h"
#include "vtkGLSLShader.h"
#include "vtkXMLShader.h"
#include "vtkXMLMaterial.h"
#include "vtkXMLDataElement.h"
#include "vtkOpenGLExtensionManager.h"
#include "vtkgl.h"

#include "QVTKWidget.h"

#include "vtkOpenGLRenderer.h"
#include "vtkShaderProgram2.h"

#include "vtkPointPicker.h"
#include "vtkPropPicker.h"
#include "vtkCellPicker.h"
#include "vtkPicker.h"

#include "vtkCellLocator.h"

class MouseInteractorStylePP;

class MyPoint
{
public:
	MyPoint()
	{
	}

	vtkSmartPointer<vtkPoints> point;		// surface point
	vtkSmartPointer<vtkPoints> normal;		// surface normal
	vtkSmartPointer<vtkActor> actor;	// actor of point

	bool operator==(vtkActor *otherActor) const {
		return (this->actor.GetPointer() == otherActor);
	}
};

class MyWidgetElem
{
public:
	MyWidgetElem()
	{
		p1_index = -1;
		p2_index = -1;
	}

	int p1_index;
	int p2_index;

	vtkSmartPointer<vtkPoints>  planenormal;
	vtkSmartPointer<vtkActor> actor;
};

class MyWidget
{
public:
	MyWidget()
	{
	}

	std::vector<MyWidgetElem> elems;
	std::vector<MyPoint> points;

	vtkSmartPointer<vtkPoints> avgnormal;

	//vtkSmartPointer<vtkCellLocator> cellLocator;
};
// ------------------------------------------------------------------------
/// <summary>
/// WidgetElem, represents a widget element (position, normal, actor, etc.)
/// </summary>
class WidgetElem
{
public:
	WidgetElem()
	{
		marked = false;
	}

	vtkSmartPointer<vtkActor> actor;
	vtkSmartPointer<vtkPoints> endpoints;	// really should be singular
	vtkSmartPointer<vtkPoints> normals;	// really should be singular
	vtkSmartPointer<vtkPoints> avgnormals;	// really should be singular

	/// <summary> Widget's CellLocator, Important for speeding up raycast/picking (BuildLocator must be called with new Widget)_</summary>
	vtkSmartPointer<vtkCellLocator> cellLocator;

	bool marked;

	bool operator==(const WidgetElem &other) const {
		return (this->actor.GetPointer() == other.actor);
	}

	bool operator==(vtkActor *otherActor) const {
		return (this->actor.GetPointer() == otherActor);
	}
};

// ------------------------------------------------------------------------
/// <summary>
/// CustomMesh, represents a mesh (opacity, name, actor, etc.)
/// </summary>
struct CustomMesh
{
	/// <summary> Mesh's opacity </summary>
	float opacity;
	/// <summary> Mesh's name (group name) </summary>
	char name[256];
	/// <summary> Mesh's color </summary>
	Utility::myColor color;
	/// <summary> Mesh's actor, contains Mapper->GetOutput() - vtkPolyData </summary>
	vtkSmartPointer<vtkActor> actor;
	/// <summary> Mesh's CellLocator, Important for speeding up raycast/picking (BuildLocator must be called with new CustomMesh)_</summary>
	vtkSmartPointer<vtkCellLocator> cellLocator;

	// Custom properties
	int selected;

	bool operator==(const CustomMesh &other) const {
		return (this->actor.GetPointer() == other.actor);
	}

	bool operator==(vtkActor *otherActor) const {
		return (this->actor.GetPointer() == otherActor);
	}
};

// ------------------------------------------------------------------------
/// <summary>
/// Main window class
/// </summary>
class additive : public QMainWindow
{
	Q_OBJECT

	// ------------------------------------------------------------------------
public:
	additive(QWidget *parent = 0, Qt::WFlags flags = 0);
	~additive();

	// public access variables
	double mouse[3];			// Put in struct later
	float mouseSize;
	float brushDivide;
	int peerInside;
	int toon;
	float myexp;

	float pos1[3];
	float pos2[3];

	/// <summary> Selected Widget Actor </summary>
	vtkSmartPointer<vtkActor> selectedWidgetActor;

	vtkSmartPointer<vtkActor> clickedWidgetActor;

	QVTKWidget *qv;
	vtkSmartPointer<MouseInteractorStylePP> estyle;

	//vtkSmartPointer<vtkShaderProgram2> shaderProgram ;

	//enum class State {WIREFRAME, POLYGON};

	/// <summary> VTK renderer </summary>
	vtkSmartPointer<vtkRenderer> renderer;

	/// <summary> Vector of CustomMesh objects </summary>
	std::vector<CustomMesh> meshes;
	std::vector<std::vector<WidgetElem> > widgets;

	std::vector<MyWidget> mywidgets;

	/// <summary> Currently selected mesh index </summary>
	int selectedIndex;

	vtkSmartPointer<vtkActor> mainCursor;

	// ------------------------------------------------------------------------
private:
	/// <summary> The UI object (access widgets from here) </summary>
	Ui::additiveClass ui;

	/// <summary> Label located inside the statusbar </summary>
	QLabel * status_label;

	/// <summary> Whether edges are visible (turn on/off for current mesh) </summary>
	bool edgevisible;

	/// <summary> Currently hovered over mesh index (in listbox)</summary>
	int hoveredIndex;

	/// <summary> Boolean to toggle pausing VTK rendering </summary>
	bool pause;

	/// <summary> Filename of currently opened file </summary>
	char fname[256];

	// ************* PUBLIC SLOTS *******************************************************************************
	public slots:

	// ------------------------------------------------------------------------
	/// <summary> Slot called when mouse is hovering over list item
	/// </summary>
	void slot_listitementered(QListWidgetItem * item)
	{
		//meshes[hoveredIndex].actor->GetProperty()->EdgeVisibilityOff();
		//meshes[hoveredIndex].actor->GetProperty()->SetOpacity(meshes[hoveredIndex].opacity);

		hoveredIndex = ui.listWidget->row(item);

		//meshes[hoveredIndex].actor->GetProperty()->EdgeVisibilityOn();
		//meshes[hoveredIndex].actor->GetProperty()->SetOpacity(1.0);		// myopacity
	}

	// ------------------------------------------------------------------------
	/// <summary> Slot called when list item clicked (i is the index of the clicked item)
	/// </summary>
	/// <param name="i">Clicked item index</param>
	void slot_listitemclicked(int i);

	// ------------------------------------------------------------------------
	/// <summary> Slot called when the Show Edge button clicked
	/// </summary>
	void slot_buttonclicked()
	{
		ui.btnHello->setText("Toggled");
		print_statusbar("Toon shading toggled!");

		if (selectedIndex > -1 && selectedIndex < meshes.size())
		{
			toon = (toon + 1) % 2;
			for (int z = 0; z < meshes.size(); z++)
			{
				meshes[z].actor->GetProperty()->AddShaderVariable("toon", toon);
			}
			updateDisplay();
		}

		//if (!edgevisible)
		//actor->GetProperty()->EdgeVisibilityOn();
		//else
		//actor->GetProperty()->EdgeVisibilityOff();

		edgevisible = !edgevisible;
	}

	// ------------------------------------------------------------------------
	/// <summary> Slot called when File->New Window menu item clicked
	/// </summary>
	void slot_newwindow()
	{
		ui.btnHello->setText("Many");
		print_statusbar("New window clicked!");
	}

	// ------------------------------------------------------------------------
	/// <summary> Slot called as frequently as possible (refreshes qvtkWidget)
	/// </summary>
	void slot_timeout()
	{
		//((vtkOpenGLRenderer *)(vtkRenderer*)renderer)->SetShaderProgram(shaderProgram);

		//renderer->Render();
		//meshes[selectedIndex].actor->GetProperty()->AddShaderVariable("translucency",  mouse[0], mouse[1], mouse[2]);
		//	cout << "hi";

		//this->updateDisplay();
		if (GetAsyncKeyState(VK_ESCAPE))
			exit(0);
		if (!pause)
		{
			//ui.qvtkWidget->update();
			qv->update();
		}
	}

	void slot_timeout2();

	// ------------------------------------------------------------------------
	/// <summary> Slot called when File->Open clicked
	/// </summary>
	void slot_open()
	{
		pause = true;

		QString selectedFilter;
		QFileDialog::Options options;

		QString fileName = QFileDialog::getOpenFileName(this,
			"Select a file.", "",
			"OBJ Files (*.obj);;All Files (*)",
			&selectedFilter,
			options);

		pause = false;

		if (!fileName.isEmpty())
		{
			print_statusbar("Loading file...please be patient!");

			readFile(fileName.toLocal8Bit().data());
			renderer->ResetCamera();
			print_statusbar("File loaded");
		}
		else
			print_statusbar("No file specified.");
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when File->Exit clicked
	/// </summary>
	void slot_exit()
	{
		int response = MessageBoxA(nullptr, "Are you sure?", "Really?", MB_YESNO | MB_ICONQUESTION | MB_TASKMODAL);

		if (response == IDYES)
			exit(0);
		else
			;
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when fullscreen menu item clicked
	/// </summary>
	void slot_fullScreen()
	{
		if (this->isFullScreen())
			this->showNormal();
		else
			this->showFullScreen();
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when opacity slider value changed
	/// </summary>
	/// <param name="i">new opacity value</param>
	void slot_valueChanged(int i)
	{
		setDiffuseAndOpacity();

		// opacity is perceived opacity, i / 100.0f is actual opacity;
		float actualopacity = i / 100.0f;
		float opacity;

		opacity = actualopacity;//* 0.4f;
		if (i >= 100)
			opacity = 1;

		// if opacity = 0, disabled
		// if opacity > 0, enabled

		meshes[selectedIndex].opacity = actualopacity;

		meshes[selectedIndex].actor->GetProperty()->SetOpacity(opacity);
		//meshes[selectedIndex].actor->GetProperty()->SetDiffuseColor()

		//meshes[selectedIndex].actor->GetProperty()->AddShaderVariable("translucency",  meshes[selectedIndex].opacity);
		meshes[selectedIndex].actor->GetProperty()->AddShaderVariable("translucency", opacity);

		for (int z = 0; z < meshes.size(); z++)
		{
			//if (z != selectedIndex)
			//meshes[z].actor->GetProperty()->AddShaderVariable("translucency",  1.0);
		}
		//renderer->ResetCameraClippingRange();
		//ResetCameraClippingRange
		updateDisplay();
	}

	// ------------------------------------------------------------------------
	/// <summary> Slot called after window is loaded and shown
	/// </summary>
	void slot_afterShowWindow();

	// ************* END SLOTS *******************************************************************************

	// Public Methods ----------------------------------------------------------------------------------------------
public:

	virtual void resizeEvent(QResizeEvent *) override;
	virtual void mouseMoveEvent(QMouseEvent *) override;

	// ------------------------------------------------------------------------
	void readFile(char *filename);

	// ------------------------------------------------------------------------
	/// <summary> Changes label in status bar to a new message
	/// </summary>
	/// <param name="text">The message.</param>
	void print_statusbar(std::string text)
	{
		status_label->setText(text.c_str());
		QApplication::processEvents();
	}

	void updateOpacitySliderAndList();

	// ------------------------------------------------------------------------
	void updateDisplay()
	{
		//meshes[selectedIndex].actor->GetProperty()->AddShaderVariable("translucency",  mouse[0], mouse[1], mouse[2]);
		//renderer->Render();
		//ui.qvtkWidget->update();
		qv->update();
		qv->GetRenderWindow()->Render();
		//QApplication::processEvents();
	}

	// ------------------------------------------------------------------------
	/// <summary>
	/// Sets the diffuse and opacity properties of all objects to the ones set in meshes vector
	/// </summary>
	void setDiffuseAndOpacity()
	{
		for (int j = 0; j < meshes.size(); j++)
		{
			meshes[j].actor->GetProperty()->SetDiffuseColor(meshes[j].color.r, meshes[j].color.g, meshes[j].color.b);
			//meshes[j].actor->GetProperty()->SetOpacity(meshes[j].opacity);
		}
	}

	// ------------------------------------------------------------------------
	void updateMouseShader()
	{
		/*for (int z = 0; z < meshes.size(); z++)
		{
		meshes[z].actor->GetProperty()->AddShaderVariable("mouse",  mouse[0], mouse[1], mouse[2]);
		meshes[z].actor->GetProperty()->AddShaderVariable("mouseSize",  mouseSize);
		meshes[z].actor->GetProperty()->AddShaderVariable("peerInside",  peerInside);
		}*/

		meshes[selectedIndex].actor->GetProperty()->AddShaderVariable("mouse", mouse[0], mouse[1], mouse[2]);
		meshes[selectedIndex].actor->GetProperty()->AddShaderVariable("mouseSize", mouseSize);
		meshes[selectedIndex].actor->GetProperty()->AddShaderVariable("peerInside", peerInside);
	}

	class vtkMyShader : public vtkGLSLShader
	{
	public:

		int MyGetUniformLocation(const char* name)
		{
			if (!this->IsShader())
			{
				return -1;
			}
			if (!name)
			{
				vtkErrorMacro("NULL uniform shader parameter name.");
				return -1;
			}

			if (vtkgl::IsProgram(this->GetProgram()) != GL_TRUE)
			{
				vtkErrorMacro("NULL shader program.");
				return -1;
			}

			int location;
			location = vtkgl::GetUniformLocation(this->GetProgram(), name);
			if (location == -1)
			{
				vtkErrorMacro("No such shader parameter. " << name);
			}
			return location;
		}

		void dosomething(vtkActor *actor, vtkRenderer * renderer)
		{
			//float mou[3] = {1,1,1};
			//this->AddShaderVariable("mouse", 3, mou);

			//std::cout << "doing" << std::endl;
			//this->AddShaderVariable("mat", 16, elems);
			//this->AddShaderVariable()

			//vtkCxxSetObjectMacro(vtkShader, XMLShader, vtkXMLShader);
			//this->GetXMLShader()->GetRootElement();

			vtkShader::GetXMLShader()->GetName();
			//super->GetXMLShader()->GetName();
			//this->XMLShader->GetRootElement();

			vtkXMLDataElement* root = this->XMLShader->GetRootElement();

			if (root == NULL)
			{
				return;
			}

			int max = root->GetNumberOfNestedElements();
			for (int i = 0; i < max; i++)
			{
				vtkXMLDataElement* elem = root->GetNestedElement(i);
				// Decide what to do with the elem element.
				const char* name = elem->GetAttribute("name");
				if (!name)
				{
					vtkErrorMacro("Uniform parameter missing required attribute 'name' " << *elem);
					continue;
				}

				// MY STUFF
				if (strcmp(name, "mat") == 0)
				{
					vtkSmartPointer<vtkMatrix4x4> mat = renderer->GetActiveCamera()->GetModelViewTransformMatrix();

					int index = 0;
					float elems[16];

					std::ostringstream elems_str;

					for (int i = 0; i < 4; i++)
					for (int j = 0; j < 4; j++)
					{
						elems[index] = mat->GetElement(j, i);	// row j, column i (column major)
						//std::cout << elems[index];
						elems_str << elems[index] << " ";
						index++;
					}

					elem->SetAttribute("value", elems_str.str().c_str());
				}
			}
		}
	};

	void updateMat()
	{
		return;
		for (int z = 0; z < meshes.size(); z++)
		{
			updateMat(z);
		}
	}
	void updateMat(int z)
	{
		return;
		vtkCollectionIterator* iter = meshes[z].actor->GetProperty()->GetShaderProgram()->NewShaderIterator();
		//std::cout << meshes[0].actor->GetProperty()->GetShaderProgram()->GetNumberOfShaders() << std::endl;
		for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
		{
			vtkMyShader* shader = (vtkMyShader*)(iter->GetCurrentObject());
			//shader->dosomething(meshes[z].actor, qv->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
			shader->dosomething(meshes[z].actor, renderer);
			//system("pause");
		}
	}
};

#endif // ADDITIVE_H
