// ****************************************************************************
// Aperio
// ----------------------------------------------------------------------------
// Main QT window file, contains interactive events and main code
// Header file contains slots, (functions called in response to signals, such
// as clicking on a button on the QT form)
//
// ****************************************************************************

#ifndef APERIO_H
#define APERIO_H

#include <QtWidgets/QMainWindow>
#include "ui_aperio.h"

// Forward declarations (pimpl)
class MyInteractorStyle;
class vtkMyShaderPass;
class vtkMyDepthPeelingPass;

// Custom
#include "Utility.h"
#include "CarveConnector.h"
#include "MySuperquadricSource.h"

// QT Includes
#include <QMessageBox>
#include <QColorDialog>

// VTK Includes
#include <QVTKWidget.h>
#include <vtkMyFillHolesFilter.h>
#include "vtkSplineWidget2.h"

//--------------------- Custom Entity Classes ----------------------------
class MyPoint
{
public:
	MyPoint() {}
	~MyPoint() {}

	vtkVector3f point;
	vtkVector3f normal;
};
///-------------------------------------------------------------------------------------------
/// <summary> MyElem, represents a Superquadric element - item inside elems vector (endpoints, avgnormal, actor, cellLocator etc.)
/// </summary>
class MyElem
{
public:
	MyElem() {}
	~MyElem() {}

	MyPoint p1;
	MyPoint p2;

	vtkVector3f scale;

	vtkSmartPointer<vtkActor> actor;								// Superquadric actor
	vtkSmartPointer<MySuperquadricSource> source;					// the superquadric source
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter;	// the transform filter

	/// <summary> Elem's CellLocator? (do we need it?) For speeding up raycast/picking (BuildLocator must be called with new Widget)_</summary>
	vtkSmartPointer<vtkCellLocator> cellLocator;

	vtkVector3f avgnormal;	// Average normals of sampled points?
};

///---------------------------------------------------------------------------------------------
/// <summary> CustomMesh, represents a mesh read-in from file - item inside meshes vector (opacity, name, actor, etc.)
/// </summary>
struct CustomMesh
{
	/// <summary> Mesh's opacity </summary>
	float opacity;
	/// <summary> Mesh's name (group name) </summary>
	std::string name;
	/// <summary> Mesh's color </summary>
	vtkColor3f color;
	/// <summary> Mesh's actor, contains Mapper->GetOutput() - vtkPolyData </summary>
	vtkSmartPointer<vtkActor> actor;
	/// <summary> Mesh's CellLocator, Important for speeding up raycast/picking (BuildLocator must be called with new CustomMesh)_</summary>
	vtkSmartPointer<vtkCellLocator> cellLocator;

	// Whether or not it is a generated mesh (cut piece) or original piece
	bool generated;
	// Generated piece properties:
	vtkVector3f snormal;	// Superquadric normal
	vtkVector3f sforward;		// Superquadric up vector
	vtkVector3f hingePivot; // Superquadric initial position is hinge

	float hingeAngle;
	float hingeAmount;

	// Custom properties
	bool selected;
};

// ----------------------------------------------------------------------------------------
/// <summary> Main window class
/// </summary>
class aperio : public QMainWindow
{
	Q_OBJECT

public:
	aperio(QWidget *parent = 0);
	~aperio();

	// public access variables (mostly in shader as uniforms)
	double mouse[3];				// Put in struct later	- on mouse move updated
	double mouseNorm[3];			// Put in struct later - on mouse move updated

	bool wiggle;
	float mouseSize;				// Recompute from bounds
	float brushDivide;				// division factor for mouseSize
	float brushSize;
	bool peerInside;
	int toon;
	float myexp;					// Superquadric roundness param
	float myn;
	int shadingnum;					// current shader (toon, phong, etc)

	float pos1[3];					// position of superquad (pt1 and pt2)
	float pos2[3];

	float norm1[3];					// normal of superquad (pt1 and pt2)
	float norm2[3];

	bool difftrans;
	int shininess;
	float darkness;

	float wavetime;

	// Public variables
	vtkSmartPointer<vtkSplineWidget2> splineWidget;

	vtkSmartPointer<vtkMyShaderPass> opaqueP;
	vtkSmartPointer<vtkMyShaderPass> transP;
	vtkSmartPointer<vtkMyDepthPeelingPass> peelP;

	vtkSmartPointer<vtkRenderPassCollection> passes;

	float roundnessScale = 50.0;	// Superquadric roundness (divider)

	// Shader programs
	vtkSmartPointer<vtkShaderProgram2> pgm;
	vtkSmartPointer<vtkTexture> cutterTexture;
	vtkSmartPointer<vtkTexture> bumpTexture;

	// QT Window variables (preview and original size of window)
	QRect _orig_size;
	bool preview;

	bool glew_available;

	QString path;

	/// <summary> QVTK Widget </summary>
	QVTKWidget *qv;

	/// <summary> Color Dialog </summary>
	QColorDialog* colorDialog;

	/// <summary> VTK renderer </summary>
	vtkSmartPointer<vtkRenderer> renderer;

	/// <summary> Interactor style </summary>
	vtkSmartPointer<MyInteractorStyle> interactorstyle;

	/// <summary> Vector of CustomMesh objects </summary>
	std::vector<CustomMesh> meshes;

	/// <summary> Vector of Element objects </summary>
	std::vector<MyElem> myelems;

	/// <summary> Currently selected mesh index </summary>
	vector<CustomMesh>::iterator selectedMesh;

	vtkSmartPointer<MySuperquadricSource> superquad;
	vtkSmartPointer<vtkMatrix4x4> transform;

	friend class MyInteractorStyle;

protected:
	Ui::aperioClass ui;

private:
	/// <summary> Label located inside the statusbar </summary>
	QLabel* status_label;

	/// Frame rate (frames per second)
	float fps;

	/// <summary> Boolean to toggle pausing VTK rendering </summary>
	bool pause;

	/////////////////////////////////////// PUBLIC SLOTS //////////////////////////////////////////////////////////////////
	public slots:

	// ------------------------------------------------------------------------
	/// <summary> Slot called when mouse is hovering over list item
	/// </summary>
	void slot_listitementered(QListWidgetItem * item)
	{
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when Magnifying glass button clicked
	/// </summary>
	void slot_btnGlass()
	{
		peerInside = !peerInside;

		QPixmap pixmap;
		if (peerInside)
			pixmap = QPixmap(":/aperio/glass2.png");
		else
			pixmap = QPixmap(":/aperio/glass.png");
		ui.btnGlass->setIcon(QIcon(pixmap));
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when Slice button clicked
	/// </summary>
	void slot_btnSlice();

	// ------------------------------------------------------------------------
	/// <summary> Slot called when Show/Hide button clicked
	/// </summary>
	void slot_btnHide();

	// ------------------------------------------------------------------------
	/// <summary> Slot called when Phi Slider changed
	/// </summary>
	void slot_phiSlider(int value);

	// ------------------------------------------------------------------------
	/// <summary> Slot called when Theta Slider changed
	/// </summary>
	void slot_thetaSlider(int value);

	// ------------------------------------------------------------------------
	/// <summary> Slot called when Toroidal checkbox checked
	/// </summary>
	void slot_chkToroid(bool checked);

	// ------------------------------------------------------------------------
	/// <summary> Slot called depth peeling checkbox is togggled
	/// </summary>
	void slot_chkDepthPeel(bool checked);

	// ------------------------------------------------------------------------
	/// <summary> Slot called when list item clicked (i is the index of the clicked item)
	/// </summary>
	/// <param name="i">Clicked item index</param>
	void slot_listitemclicked(int i);

	// ------------------------------------------------------------------------
	/// <summary> Slot called when hinge slider's value changed
	/// </summary>
	/// <param name="i">value changed</param>
	void slot_hingeSlider(int value);

	// ------------------------------------------------------------------------
	/// <summary> Slot called when Hinge Amount changed in QLineEdit (textbox)
	/// </summary>
	void slot_txtHingeAmount(const QString &string);

	// ------------------------------------------------------------------------
	/// <summary> Slot called when the Diffuse Translucency (Light) button clicked
	/// </summary>
	void slot_buttonclicked()
	{
		difftrans = !difftrans;
		ui.btnLight->setText((difftrans ? QString("On") : QString("Off")));

		QPixmap pixmap;
		if (difftrans)
			pixmap = QPixmap(":/aperio/flashlight2.png");
		else
			pixmap = QPixmap(":/aperio/flashlight.png");
		ui.btnLight->setIcon(QIcon(pixmap));

		print_statusbar("Light toggled!");
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when the Color button clicked
	/// </summary>
	void slot_colorclicked()
	{
		QColor color;
		colorDialog->setVisible(true);
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when the Color is changed
	/// </summary>
	void slot_colorchanged(const QColor & color)
	{
		if (selectedMesh != meshes.end())
		{
			float r = color.red() / 255.0;
			float g = color.green() / 255.0;
			float b = color.blue() / 255.0;

			selectedMesh->actor->GetProperty()->SetDiffuseColor(r, g, b);
			selectedMesh->actor->GetProperty()->SetAmbientColor(r, g, b);

			vtkColor3f mycol(r, g, b);
			selectedMesh->color = mycol;
		}
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when Help->About menu item clicked
	/// </summary>
	void slot_about()
	{
		QMessageBox messageBox(this);

		messageBox.setIconPixmap(QPixmap("about.png"));

		QChar co(169);

		messageBox.setWindowTitle("About Aperio");
		messageBox.setText("Aperio");
		messageBox.setInformativeText("1.0.0.3 (Released 07-11-2014) \n\nCopyright " + QString(co) + " 2011-2014 David Tran\n");
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
	/// <summary> Slot called as frequently as possible - instantaneous timer
	/// </summary>
	void slot_timeout_instant()
	{
		if (!pause) qv->update();
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called according to FPS speed (updates qv widget)
	/// </summary>
	void slot_timeout_fps();

	// ------------------------------------------------------------------------
	/// <summary> Slot called less frequently (few times a second)
	/// </summary>
	void slot_timeout_delay()
	{
		// Update Shaders Periodically (so we can make real-time changes to shaders and reload) [Debugging Purposes!]
		// Disable if graphics card heats up
		Utility::updateShader(pgm, "shader_water.vert", "shader.frag");
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when File->Open clicked
	/// </summary>
	void slot_open()
	{
		pause = true;

		if (path.isEmpty())
			path = QDir::currentPath();

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
	void slot_opacitySlider(int i)
	{
		// opacity is perceived opacity, i / 100.0f is actual opacity;
		float actualopacity = i / 100.0f;
		float opacity;

		opacity = actualopacity * 0.5f; //0.4f
		if (i >= 100)
			opacity = 1;

		if (selectedMesh != meshes.end())
		{
			selectedMesh->opacity = actualopacity;
			selectedMesh->actor->GetProperty()->SetOpacity(opacity);

			// Update list check boxes
			auto item = getListItemByName(selectedMesh->name);

			if (opacity == 0)
				item->setCheckState(Qt::Unchecked);
			else
				item->setCheckState(Qt::Checked);
		}
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when shininess slider value changed
	/// </summary>
	/// <param name="i">new shininess value</param>
	void slot_shininessSlider(int i)
	{
		shininess = i;
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when darkness slider value changed
	/// </summary>
	/// <param name="i">new darkness value</param>
	void slot_darknessSlider(int i)
	{
		darkness = (i + 128.0f) / 128.0f;
	}
	///-----------------------------------------------------------------------------------------------
	/// <summary> Slot called after the window is fully loaded (contains all initialization code)
	/// </summary>
	void slot_afterShowWindow();

	////////////////////////////////////////////// END SLOTS //////////////////////////////////////////////////////////

	// Public Methods ----------------------------------------------------------------------------------------------
public:

	/// <summary> Make composite transform matrix (translate, rotate, scale) from elem's points, normals and scale
	/// </summary>
	vtkSmartPointer<vtkTransform> makeCompositeTransform(MyElem &elem);

	/// <summary> Set SelectedMesh to an instance of vector<CustomMesh>::iterator (update list, variable & selected shader var)
	/// </summary>
	/// <param name="name">Name of object (string) </param>
	void setSelectedMesh(std::vector<CustomMesh>::iterator &it);

	/// <summary> Obtain CustomMesh by name
	/// </summary>
	/// <param name="name">Name of object (string) </param>
	std::vector<CustomMesh>::iterator getMeshByName(std::string name)
	{
		auto it = std::find_if(meshes.begin(), meshes.end(), [=](CustomMesh &c) { return c.name.compare(name) == 0; });
		return it;
	}
	/// <summary> Obtain CustomMesh by vtkActor smartpointer
	/// </summary>
	/// <param name="name">vtkActor SmartPointer to compare with all CustomMeshes' actors</param>
	std::vector<CustomMesh>::iterator getMeshByActor(vtkSmartPointer<vtkActor> actor)
	{
		auto it = std::find_if(meshes.begin(), meshes.end(), [=](CustomMesh &c)
		{ return c.actor.GetPointer() == actor.GetPointer(); });
		return it;
	}
	/// <summary> Obtain CustomMesh by vtkActor raw pointer
	/// </summary>
	/// <param name="name">vtkActor raw pointer to compare with all CustomMeshes' actors</param>
	std::vector<CustomMesh>::iterator getMeshByActorRaw(vtkActor* actor)
	{
		auto it = std::find_if(meshes.begin(), meshes.end(), [=](CustomMesh &c)
		{ return c.actor.GetPointer() == actor; });
		return it;
	}
	/// <summary> Obtain Widget/Element by vtkActor raw pointer
	/// </summary>
	/// <param name="name">vtkActor raw pointer to compare with all myelems' actors</param>
	std::vector<MyElem>::iterator getElemByActorRaw(vtkActor* actor)
	{
		auto it = std::find_if(myelems.begin(), myelems.end(), [=](MyElem &e)
		{ return e.actor.GetPointer() == actor; });
		return it;
	}
	/// <summary> Obtain ListItem and QListWidget by name
	/// </summary>
	/// <param name="name">Name of item (string) </param>
	QListWidgetItem* getListItemByName(std::string name)
	{
		for (int i = 0; i < ui.listWidget->count(); i++) {
			std::string itemString = ui.listWidget->item(i)->text().toStdString();

			if (itemString.compare(name) == 0)	// If list item text == selected item text
			{
				return ui.listWidget->item(i);
			}
		}
		return nullptr;
	}
	/// <summary> Add to list a new item with name
	/// </summary>
	/// <param name="name">Name of item (string) </param>
	void addToList(std::string name)
	{
		QListWidgetItem* item = new QListWidgetItem(name.c_str(), ui.listWidget);

		item->setCheckState(Qt::Checked);
		ui.listWidget->addItem(item);
	}
	/// <summary> Replace old actor by new actor in Renderer
	/// </summary>
	/// <param name="oldActor">Reference to old actor </param>
	/// <param name="newActor">Reference to new actor </param>
	void replaceActor(vtkSmartPointer<vtkActor> oldActor, vtkSmartPointer<vtkActor> newActor)
	{
		// Now replace mesh too
		vtkPropCollection* actors = renderer->GetViewProps();
		actors->InitTraversal();

		vtkProp* current_actor = nullptr;
		int i = -1;
		while ((current_actor = actors->GetNextProp()) != nullptr)
		{
			i++;
			if (vtkActor::SafeDownCast(current_actor) == oldActor)	// Find selectedMesh's old actor to replace (Found)
			{
				renderer->GetViewProps()->ReplaceItem(i, newActor);
				break;
			}
		}
	}
	/// <summary> Event called when window resized (resizes qvtkwidget)
	/// </summary>
	/// <param name="event">Event object (Qt-based)</param>
	virtual void resizeEvent(QResizeEvent *) override;

	/// <summary> Event called when mouse moves over window
	/// </summary>
	/// <param name="">Event object (Qt-based)</param>
	virtual void mouseMoveEvent(QMouseEvent *) override;

	// Custom methods
	void resizeInternal(const QSize &newWindowSize, bool using_preview);
	void update_orig_size();

	// ------------------------------------------------------------------------
	/// <summary> Reads an .obj file and loads into meshes vector
	/// </summary>
	/// <param name="filename">filename.</param>
	void readFile(std::string filename);

	// ------------------------------------------------------------------------------------------
	/// <summary> Reset clipping plane (call this after any resetCamera calls, flyTo, etc.)
	/// </summary>
	void resetClippingPlane();

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

	Ui::aperioClass& getUI()
	{
		return ui;
	}
};

#endif // APERIO_H
