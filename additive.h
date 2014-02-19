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

#include "MySuperquadricSource.h"

#include <QtGui/QMessageBox>

#include <vtkVector.h>

class MyInteractorStyle;

class MyPoint
{
public:
	MyPoint() {}
	~MyPoint() {}

	//vtkSmartPointer<vtkPoints> point;
	//vtkSmartPointer<vtkPoints> normal;
	vtkVector3f point;
	vtkVector3f normal;
};

class MyElem
{
public:
	MyElem() {}
	~MyElem() {}

	MyPoint p1;
	MyPoint p2;
	vtkSmartPointer<vtkActor> actor;								// Superquadric actor
	vtkSmartPointer<MySuperquadricSource> source;					// the superquadric source
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter;	// the transform filter
};

class MyWidgetPoint
{
public:
	MyWidgetPoint()
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
	std::vector<MyWidgetPoint> points;

	vtkSmartPointer<vtkPoints> avgnormal;

	//vtkSmartPointer<vtkCellLocator> cellLocator;
};
///---------------------------------------------------------------------------------------------
/// <summary> WidgetElem, represents a widget element (position, normal, actor, etc.)
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

///------------------------------------------------------------------------
/// <summary> CustomMesh, represents a mesh (opacity, name, actor, etc.)
/// </summary>
struct CustomMesh
{
	/// <summary> Mesh's opacity </summary>
	float opacity;
	/// <summary> Mesh's name (group name) </summary>
	std::string name;
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
/// <summary> Main window class
/// </summary>
class additive : public QMainWindow
{
	Q_OBJECT

public:
	/// <summary> The constructor (Fires a timer that starts slot_afterShowWindow which has all initialization code)
	/// </summary>
	additive(QWidget *parent = 0, Qt::WFlags flags = 0);

	/// <summary> Finalizes an instance of the <see cref="additive" /> class.
	/// </summary>
	~additive();

	// public access variables
	double mouse[3];				// Put in struct later
	double mouseNorm[3];			// Put in struct later

	float mouseSize;
	float brushDivide;
	int peerInside;
	int toon;
	float myexp;
	float myn;

	float pos1[3];
	float pos2[3];

	float norm1[3];
	float norm2[3];

	QRect _orig_size;
	bool preview;

	QString path;

	/// <summary> QVTK Widget </summary>
	QVTKWidget *qv;

	/// <summary> VTK renderer </summary>
	vtkSmartPointer<vtkRenderer> renderer;

	/// <summary> Interactor style </summary>
	vtkSmartPointer<MyInteractorStyle> interactorstyle;

	/// <summary> Vector of CustomMesh objects </summary>
	std::vector<CustomMesh> meshes;

	/// <summary> Currently selected mesh index </summary>
	int selectedIndex;

	vtkSmartPointer<MySuperquadricSource> superquad;
	vtkSmartPointer<vtkActor> sactor;

	/// <summary> Selected Widget Actor </summary>
	vtkSmartPointer<vtkActor> selectedWidgetActor;
	vtkSmartPointer<vtkActor> clickedWidgetActor;

	std::vector<std::vector<WidgetElem> > widgets;
	std::vector<MyWidget> mywidgets;

	std::vector<MyElem> myelems;

	//------------------------------------------------------------------------
private:
	/// <summary> The UI object (access QWidgets from here) </summary>
	Ui::additiveClass ui;

	/// <summary> Label located inside the statusbar </summary>
	QLabel* status_label;

	/// Frame rate (frames per second)
	float fps;

	/// <summary> Currently hovered over mesh index (in listbox)</summary>
	int hoveredIndex;

	/// <summary> Boolean to toggle pausing VTK rendering </summary>
	bool pause;

	/// <summary> Filename of currently opened file </summary>
	std::string fname;

	/////////////////////////////////////// PUBLIC SLOTS //////////////////////////////////////////////////////////////////
	public slots:

	// ------------------------------------------------------------------------
	/// <summary> Slot called when mouse is hovering over list item
	/// </summary>
	void slot_listitementered(QListWidgetItem * item)
	{
		hoveredIndex = ui.listWidget->row(item);
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
		print_statusbar("Depth peeling toggled!");
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when Help->About menu item clicked
	/// </summary>
	void slot_about()
	{
		QMessageBox messageBox(this);

		messageBox.setIconPixmap(QPixmap("about.png"));

		messageBox.setWindowTitle("About Mesh Illustrator");
		messageBox.setText("Mesh Illustrator ");
		messageBox.setInformativeText("1.0.0.2 (Released 01-18-2014) \n\nCopyright © 2011-2014 David Tran\n");
		messageBox.setWindowOpacity(.85);
		messageBox.setStyleSheet("background: rgba(0, 0, 0, 255); color: white; selection-color: black;");

		QPushButton *okButton = messageBox.addButton(tr("Ok"), QMessageBox::ActionRole);

		okButton->setStyleSheet(this->styleSheet());
		messageBox.exec();
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called some milliseconds after clicking top-level menu (File, Help, etc.) to fade opacity
	/// </summary>
	void slot_menuclick2()
	{
		float menu_opacity = 0.8f;
		ui.menuHelp->setWindowOpacity(menu_opacity);
		ui.menuFile->setWindowOpacity(menu_opacity);
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called upon clicking a top-level menu (File, Help, etc.)
	/// </summary>
	void slot_menuclick()
	{
		// Wait for menu fade-in animation to complete before setting opacity
		QTimer::singleShot(200, this, SLOT(slot_menuclick2()));
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called as frequently as possible (refreshes qvtkWidget)
	/// </summary>
	void slot_timeout()
	{
		if (GetAsyncKeyState(VK_ESCAPE))
			exit(0);
		if (!pause)
			qv->update();
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when File->Open clicked
	/// </summary>
	void slot_open()
	{
		if (path.isEmpty())
			path = QDir::currentPath();

		pause = true;

		QString selectedFilter;
		QFileDialog::Options options;

		QString fileName = QFileDialog::getOpenFileName(this,
			"Select a file.", QString(path),
			"OBJ Files (*.obj);;All Files (*)",
			&selectedFilter,
			options);

		pause = false;

		if (!fileName.isEmpty())
		{
			path = QFileInfo(fileName).path(); // store path for next time

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
			QMessageBox mb(this);
			mb.setStyleSheet("color: white; background: black; selection-color: black;");
			mb.setWindowOpacity(0.9);
			mb.setWindowTitle(this->windowTitle());
			mb.setText("Are you sure you want to quit?");
			mb.setIcon(QMessageBox::Warning);

			QPushButton *yesButton = mb.addButton(tr("Yes"), QMessageBox::ActionRole);
			QPushButton *noButton = mb.addButton(tr("No"), QMessageBox::ActionRole);
			//noButton->setFocus();

			yesButton->setStyleSheet(this->styleSheet());
			noButton->setStyleSheet(this->styleSheet());

			mb.exec();
			if (mb.clickedButton() == yesButton)
				QApplication::quit();
	}
	// ------------------------------------------------------------------------
	void additive::resizeInternal(const QSize &newWindowSize, bool using_preview)
	{
		int marginWidth = 20;
		int marginHeight = marginWidth * 3;	// extra border-width margin of qframe

		QRect main = ui.mainWidget->geometry();
		QRect newRect;

		// Previewing resize
		if (using_preview)
		{
			// Check state of preview (true or false and draw appropriately)
			if (!preview)
			{
				preview = true;
				newRect.setCoords(_orig_size.x(), 0, newWindowSize.width() - marginWidth, newWindowSize.height() - marginHeight);
				ui.mainWidget->raise();
			}
			else
			{
				preview = false;
				newRect.setCoords(_orig_size.x(), _orig_size.y(), newWindowSize.width() - marginWidth, newWindowSize.height() - marginHeight);
				ui.mainWidget->lower();
			}
		}
		else  // Regular resize
		{
			newRect.setCoords(main.topLeft().x(), main.topLeft().y(), newWindowSize.width() - marginWidth, newWindowSize.height() - marginHeight);
		}
		ui.mainWidget->setGeometry(newRect);
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when File->New Window menu item clicked
	/// </summary>
	void slot_preview()
	{
		resizeInternal(this->size(), true);
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
		// opacity is perceived opacity, i / 100.0f is actual opacity;
		float actualopacity = i / 100.0f;
		float opacity;

		opacity = actualopacity *0.4f;
		if (i >= 100)
			opacity = 1;

		// if opacity = 0, disabled
		// if opacity > 0, enabled

		meshes[selectedIndex].opacity = actualopacity;
		meshes[selectedIndex].actor->GetProperty()->SetOpacity(opacity);
	}
	///-----------------------------------------------------------------------------------------------
	/// <summary> Slot called after the window is fully loaded (contains all initialization code)
	/// </summary>
	void slot_afterShowWindow();

	////////////////////////////////////////////// END SLOTS //////////////////////////////////////////////////////////

	// Public Methods ----------------------------------------------------------------------------------------------
public:

	/// <summary> Event called when window resized (resizes qvtkwidget)	
	/// </summary>
	/// <param name="event">Event object (Qt-based)</param>
	virtual void resizeEvent(QResizeEvent *) override;

	/// <summary> Event called when mouse moves over window 
	/// </summary>
	/// <param name="">Event object (Qt-based)</param>
	virtual void mouseMoveEvent(QMouseEvent *) override;

	// Custom methods
	void update_orig_size();

	// ------------------------------------------------------------------------
	/// <summary> Reads an .obj file and loads into meshes vector
	/// </summary>
	/// <param name="filename">filename.</param>
	void readFile(std::string filename);

	// ------------------------------------------------------------------------
	/// <summary> Changes label in status bar to a new message
	/// </summary>
	/// <param name="text">The message.</param>
	void print_statusbar(std::string text)
	{
		status_label->setText(text.c_str());
		QApplication::processEvents();
	}
	// ------------------------------------------------------------------------
	/// <summary> Updates slider opacity value; called when list item clicked
	/// </summary>
	void updateOpacitySliderAndList();
};

#endif // ADDITIVE_H
