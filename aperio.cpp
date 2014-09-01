#include "stdafx.h"

// QT Includes
#include <QLayout>
#include <QDesktopWidget>
#include <QVTKWidget.h>

// VTK includes
#include <vtkSphereSource.h>

// Custom
#include "tiny_obj_loader.h"
#include "CarveConnector.h"
#include "MyInteractorStyle.h"
#include "Utility.h"
#include "vtkMyShaderPass.h"
#include "vtkMyProcessingPass.h"
#include "vtkMyDepthPeelingPass.h"
#include "vtkMyAdvancedPass.h"

// More VTK
#include <vtkLightsPass.h>
#include <vtkCameraPass.h>
#include <vtkDefaultPass.h>
#include <vtkSequencePass.h>
#include <vtkRenderPassCollection.h>
#include <vtkGaussianBlurPass.h>
#include <vtkSobelGradientMagnitudePass.h>

#include <vtkOpaquePass.h>
#include <vtkTranslucentPass.h>
#include <vtkClearZPass.h>

#include <vtkRenderPass.h>
#include <vtkLight.h>

#include <vtkDoubleArray.h>
#include <vtkOutlineSource.h>

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

using namespace std;
#include "aperio.h"

// Include widgets
#include <vtkAffineWidget.h>
#include <vtkAffineRepresentation2D.h>

#include <vtkParametricEllipsoid.h>
#include <vtkParametricFunctionSource.h>

#include <vtkPlaneWidget.h>
#include <vtkBoxWidget.h>
#include <vtkBoxWidget2.h>
#include <vtkAngleWidget.h>
#include <vtkAxesTransformWidget.h>
#include <vtkBorderWidget.h>
#include <vtkButtonWidget.h>
#include <vtkCenteredSliderWidget.h>
#include <vtkContourWidget.h>
#include <vtkHandleWidget.h>
#include <vtkHoverWidget.h>
#include <vtkLineWidget2.h>
#include <vtkParallelopipedWidget.h>
#include <vtkResliceCursorWidget.h>
#include <vtkResliceCursorWidget.h>
#include <vtkSliderWidget.h>
#include <vtkSphereWidget2.h>
#include <vtkSplineWidget2.h>
#include <vtkTensorProbeWidget.h>

#include <vtkBoxRepresentation.h>

//-------------------------------------------------------------------------------------------------------------
aperio::aperio(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Constructor (initialize variables)
	glew_available = false;

	QTimer::singleShot(0, this, SLOT(slot_afterShowWindow()));
}
///---------------------------------------------------------------------------------------
aperio::~aperio()
{
}
//-------------------------------------------------------------------------------------------------------------
void aperio::update_orig_size()
{
	_orig_size.setX(ui.mainWidget->geometry().x());
	_orig_size.setY(ui.mainWidget->geometry().y());
	_orig_size.setWidth(ui.mainWidget->geometry().width());
	_orig_size.setHeight(ui.mainWidget->geometry().height());
}
///---------------------------------------------------------------------------------------
void aperio::slot_afterShowWindow()
{
	update_orig_size();

	// Set up instance variables
	fps = 50.0;
	//std::string fname = "cube.obj";
	//std::string fname = "organs brain 250K.obj";
	//std::string fname = "hearttest.obj";
	std::string fname = "newestheart.obj";

	// QT Variables
	pause = false;
	preview = false;

	// Uniforms
	wiggle = true;
	shadingnum = 0;		// Toon/normal, etc.
	peerInside = false;
	brushDivide = 15.0;

	brushSize = 1.5;
	myexp = 2;

	// Default uniform variables
	mouse[0] = 0;
	mouse[1] = 0;
	mouse[2] = 0;

	wavetime = 0;
	difftrans = ui.btnLight->text().compare("On") == 0 ? true: false;
	shininess = ui.shininessSlider->value();
	darkness = (ui.darknessSlider->value() + 128) / 128.0f;

	QApplication::processEvents();

	// Setup label 
	status_label = new QLabel("Ready", this);
	status_label->setStyleSheet("background-color: rgba(0,0,0,0);");
	ui.statusBar->addWidget(status_label);

	QTimer* timer_instant = nullptr;	// Instantaneous timer (executed as fast as possible)
	timer_instant = new QTimer(this);
	timer_instant->setInterval(0);
	timer_instant->setTimerType(Qt::TimerType::PreciseTimer);
	//timer_instant->start();

	QTimer* timer_fps = nullptr;		// FPS timer (executed using FPS speed)
	timer_fps  = new QTimer(this);
	timer_fps->setInterval(1000.0 / fps);
	timer_fps->setTimerType(Qt::TimerType::PreciseTimer);
	timer_fps->start();

	QTimer* timer_delay = nullptr;				// Delayed timer (executes slower after a delay)
	timer_delay = new QTimer(this);
	timer_delay->setInterval(1000.0 / 7.5);
	timer_delay->start();

	colorDialog = new QColorDialog(this);
	colorDialog->setWindowTitle("Pick a color for the selected object.");
	colorDialog->setWindowOpacity(.85);
	colorDialog->setStyleSheet("background: rgba(0, 0, 0, 255); color: white; selection-color: black;");

	QRect scr = QApplication::desktop()->screenGeometry();
	colorDialog->move(scr.center() + QPoint(150, -150));

	// Start VTK
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AlphaBitPlanesOn();
	renderWindow->SetMultiSamples(0);

	renderer = vtkSmartPointer<vtkRenderer>::New();

	/* float bgcolor[3] = { 235, 235, 235 };
	float factor = 0.7;
	bgcolor[0] *= factor; bgcolor[1] *= factor; bgcolor[2] *= factor;
	renderer->SetBackground(bgcolor[0] / 255.0, bgcolor[1] / 255.0, bgcolor[2] / 255.0);
	*/

	// Change background texture
	vtkSmartPointer<vtkPNGReader> pngReader = vtkSmartPointer<vtkPNGReader>::New();
	pngReader->SetFileName("bg.png");
	pngReader->Update();

	vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
	texture->InterpolateOn();
	texture->SetInputData(pngReader->GetOutput());
	
	renderer->SetTexturedBackground(true);
	renderer->SetBackgroundTexture(texture);

	renderWindow->AddRenderer(renderer);

	// Dynamically create a new QVTKWidget (and set render window to it)
	qv = new QVTKWidget(this);
	qv->SetRenderWindow(renderWindow);

	ui.mainWidget->setLayout(new QGridLayout(ui.mainWidget));
	ui.mainWidget->layout()->setMargin(2);
	ui.mainWidget->layout()->addWidget(qv);

	// Prepare all the rendering passes for vtkMyShaderPass
	vtkSmartPointer<vtkCameraPass> cameraP = vtkSmartPointer<vtkCameraPass>::New();
	vtkSmartPointer<vtkSequencePass> seq = vtkSmartPointer<vtkSequencePass>::New();

	opaqueP = vtkSmartPointer<vtkMyShaderPass>::New();
	opaqueP->initialize(this, ShaderPassType::PASS_OPAQUE);

	transP = vtkSmartPointer<vtkMyShaderPass>::New();
	transP->initialize(this, ShaderPassType::PASS_TRANSLUCENT);

	peelP = vtkSmartPointer<vtkMyDepthPeelingPass>::New();
	peelP->SetMaximumNumberOfPeels(2);
	peelP->SetOcclusionRatio(0.1);
	peelP->SetTranslucentPass(transP);	//Peeling pass needs translucent pass

	// Put all passes into a collection then into a sequence
	passes = vtkSmartPointer<vtkRenderPassCollection>::New();
	//passes->AddItem(vtkSmartPointer<vtkClearZPass>::New());
	//passes->AddItem(vtkSmartPointer<vtkLightsPass>::New());
	passes->AddItem(opaqueP);
	passes->AddItem(transP);
	//passes->AddItem(vtkSmartPointer<vtkOpaquePass>::New());
	//passes->AddItem(vtkSmartPointer<vtkTranslucentPass>::New());
	passes->AddItem(vtkSmartPointer<vtkOverlayPass>::New());
	//passes->AddItem(peelP);

	seq->SetPasses(passes);
	cameraP->SetDelegatePass(seq);

	// Requires Depth from camera pass
	//vtkSmartPointer<vtkMyProcessingPass> dofP = vtkSmartPointer<vtkMyProcessingPass>::New();
	//dofP->setShaderFile("shader_dof.frag", true);
	//dofP->SetDelegatePass(cameraP);

	// Requires camera pass (actual geometry, depth stores into frame buffer objects)
	
	vtkSmartPointer<vtkMyAdvancedPass> advP = vtkSmartPointer<vtkMyAdvancedPass>::New();
	advP->SetDelegatePass(cameraP);

	// Requires Depth from camera pass
	vtkSmartPointer<vtkMyProcessingPass> ssaoP = vtkSmartPointer<vtkMyProcessingPass>::New();
	ssaoP->setShaderFile("shader_ssao.vert", false);
	ssaoP->setShaderFile("shader_ssao.frag", true);
	ssaoP->SetDelegatePass(cameraP);

	//vtkSmartPointer<vtkMyProcessingPass> dotP = vtkSmartPointer<vtkMyProcessingPass>::New();
	//dotP->setShaderFile("shader_dot.frag", true);
	//dotP->SetDelegatePass(ssaoP);

	vtkSmartPointer<vtkMyProcessingPass> fxaaP = vtkSmartPointer<vtkMyProcessingPass>::New();
	fxaaP->setShaderFile("shader_fxaa.vert", false);
	fxaaP->setShaderFile("shader_fxaa.frag", true);
	fxaaP->SetDelegatePass(ssaoP);

	vtkSmartPointer<vtkMyProcessingPass> bloomP = vtkSmartPointer<vtkMyProcessingPass>::New();
	bloomP->setShaderFile("shader_bloom.frag", true);
	bloomP->SetDelegatePass(fxaaP);

	vtkOpenGLRenderer::SafeDownCast(renderer.GetPointer())->SetPass(bloomP);

	// Render window interactor
	vtkSmartPointer<QVTKInteractor> renderWindowInteractor = vtkSmartPointer<QVTKInteractor>::New();

	interactorstyle = vtkSmartPointer<MyInteractorStyle>::New();
	interactorstyle->SetAutoAdjustCameraClippingRange(false);
	interactorstyle->initialize(this);

	renderWindowInteractor->SetInteractorStyle(interactorstyle);
	renderWindowInteractor->Initialize();

	qv->GetRenderWindow()->SetInteractor(renderWindowInteractor);

	QApplication::processEvents();

	// --- Initialize GLEW (After OpenGL context is set up!)
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cout << "Error: \n" << glewGetErrorString(err) << "\n";
		system("pause");
		exit(1);
	}
	else
	{
		glew_available = true;
		std::cout << "GLEW Initialized: " << glewGetString(GLEW_VERSION) << "\n";
	}

	// Set up text validators for QLineEdits in form
	ui.txtHingeAmount->setValidator(new QIntValidator(-360, 360, this));
	ui.txtExplodeAmount->setValidator(new QIntValidator(-1000, 1000, this));

	// Connect all signals to slots
	connect(ui.btnLight, &QPushButton::clicked, this, &aperio::slot_buttonclicked);
	connect(ui.btnColor, &QPushButton::clicked, this, &aperio::slot_colorclicked);

	connect(colorDialog, &QColorDialog::currentColorChanged, this, &aperio::slot_colorchanged);

	connect(timer_instant, &QTimer::timeout, this, &aperio::slot_timeout_instant);
	connect(timer_fps, &QTimer::timeout, this, &aperio::slot_timeout_fps);
	connect(timer_delay, &QTimer::timeout, this, &aperio::slot_timeout_delay);
	connect(ui.actionOpen, &QAction::triggered, this, &aperio::slot_open);
	connect(ui.actionExit, &QAction::triggered, this, &aperio::slot_exit);

	connect(ui.actionAbout, &QAction::triggered, this, &aperio::slot_about);
	connect(ui.actionToggle, &QAction::triggered, this, &aperio::slot_preview);
	connect(ui.actionFullScreen, &QAction::triggered, this, &aperio::slot_fullScreen);

	connect(ui.menuFile, &QMenu::aboutToShow, this, &aperio::slot_menuclick);
	connect(ui.menuHelp, &QMenu::aboutToShow, this, &aperio::slot_menuclick);

	connect(ui.opacitySlider, &QSlider::valueChanged, this, &aperio::slot_opacitySlider);
	connect(ui.shininessSlider, &QSlider::valueChanged, this, &aperio::slot_shininessSlider);
	connect(ui.darknessSlider, &QSlider::valueChanged, this, &aperio::slot_darknessSlider);

	connect(ui.listWidget, &QListWidget::itemEntered, this, &aperio::slot_listitementered);
	connect(ui.listWidget, &QListWidget::currentRowChanged, this, &aperio::slot_listitemclicked);

	// Options
	connect(ui.chkDepthPeel, &QCheckBox::toggled, this, &aperio::slot_chkDepthPeel);

	// Buttons
	connect(ui.btnSlice, &QPushButton::clicked, this, &aperio::slot_btnSlice);
	connect(ui.btnHide, &QPushButton::clicked, this, &aperio::slot_btnHide);
	connect(ui.btnGlass, &QPushButton::clicked, this, &aperio::slot_btnGlass);

	// Superquadric options
	connect(ui.phiSlider, &QSlider::valueChanged, this, &aperio::slot_phiSlider);
	connect(ui.thetaSlider, &QSlider::valueChanged, this, &aperio::slot_thetaSlider);
	connect(ui.chkToroid, &QCheckBox::toggled, this, &aperio::slot_chkToroid);

	// Transform sliders
	connect(ui.hingeSlider, &QSlider::valueChanged, this, &aperio::slot_hingeSlider);

	// Transform Textboxes (QLineEdit)
	connect(ui.txtHingeAmount, &QLineEdit::textChanged, this, &aperio::slot_txtHingeAmount);

	// Transform buttons
	//connect(ui.btnHinge, &QLabel::c, this, &aperio::slot_hingeSlider);
	
	readFile(fname);
}
// ------------------------------------------------------------------------
void aperio::resizeInternal(const QSize &newWindowSize, bool using_preview)
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
///---------------------------------------------------------------------------------------
void aperio::resizeEvent(QResizeEvent * event)
{
	resizeInternal(event->size(), false);
}
///---------------------------------------------------------------------------------------
void aperio::mouseMoveEvent(QMouseEvent *)
{
}
///---------------------------------------------------------------------------------------
void aperio::readFile(std::string filename)
{
	//vtkObject::GlobalWarningDisplayOff();	// dangerous (keep on for most part)

	// Reset values for new file
	toon = 0;

	renderer->RemoveAllViewProps();	// Remove from renderer, clear listwidget, clear vectors

	/*
	// Image Actor example
	vtkSmartPointer<vtkPNGReader> pngReader = vtkSmartPointer<vtkPNGReader>::New();
	pngReader->SetFileName("bg12.png");
	pngReader->Update();

	vtkSmartPointer<vtkImageMapper> imageMapper = vtkSmartPointer<vtkImageMapper>::New();
	imageMapper->SetColorLevel(127.5);
	imageMapper->SetColorWindow(255);
	imageMapper->SetRenderToRectangle(true);
	imageMapper->SetInputData(pngReader->GetOutput());
	imageMapper->Update();	

	vtkSmartPointer<vtkActor2D> imageActor = vtkSmartPointer<vtkActor2D>::New();
	imageActor->SetHeight(1.0);
	imageActor->SetWidth(1.0);
	imageActor->GetProperty()->SetDisplayLocationToBackground();
	imageActor->SetMapper(imageMapper);

	renderer->AddActor2D(imageActor);
	*/

	ui.listWidget->clear();
	myelems.clear();
	meshes.clear();

	qDebug() << " - reading file - \n";

	//read in cutter texture
	vtkSmartPointer<vtkJPEGReader> jpgReader = vtkSmartPointer<vtkJPEGReader>::New();
	jpgReader->SetFileName("cutter.jpg");
	jpgReader->Update();

	cutterTexture = vtkSmartPointer<vtkTexture>::New();
	cutterTexture->SetInputConnection(jpgReader->GetOutputPort());
	cutterTexture->InterpolateOn();

	if (path.isEmpty())				// Set path if it is empty
		path = QDir::currentPath();

	// Tiny OBJ reader
	std::vector<tinyobj::shape_t> shapes;
	std::string err = tinyobj::LoadObj(shapes, filename.c_str(), (path + "\\").toStdString().c_str());

	if (!err.empty()) {
		std::cerr << err << std::endl;
		system("pause");
		exit(1);
	}
	vtkSmartPointer<vtkPolyDataCollection> objectMeshCollection = vtkSmartPointer<vtkPolyDataCollection>::New();

	// Convert OBJ data to vtkPolyData
	for (size_t i = 0; i < shapes.size(); i++)
	{
		vtkSmartPointer<vtkPolyData> polydata = Utility::objToVtkPolyData(shapes[i]);
		objectMeshCollection->AddItem(polydata);
	}
	// VTK : Remove lights (light computation done in shader)
	renderer->AutomaticLightCreationOff();
	renderer->RemoveAllLights();

	//srand(time(nullptr));		//Random Seed

	// Create main shader
	pgm = Utility::makeShader(renderer->GetRenderWindow(), "shader_water.vert", "shader.frag");

	// Now we can traverse objectMeshCollection
	objectMeshCollection->InitTraversal();
	int numMeshes = objectMeshCollection->GetNumberOfItems();

	float r, g, b;
	for (int z = 0; z < numMeshes; z++)
	{
		if (numMeshes > 1) // assign a random colour if there's more than one object
		{
			r = ((rand() % 177 + 80) / 270.0);
			g = ((rand() % 177 + 80) / 270.0);
			b = ((rand() % 177 + 80) / 270.0);
		}
		else
		{
			r = 1;
			g = 1;
			b = 1;
		}
		vtkPolyData *nextMesh = objectMeshCollection->GetNextItem();
		std::string groupname = shapes.at(z).name;

		// Remove darkfactor when customizable colours implemented and saveable (1.4f currently in fragment shader)
		
		// Default random colours for heart
		if (QString(filename.c_str()).contains("newestheart.obj"))
		{
			if (QString::compare(QString(groupname.c_str()), QString("c_pericardium")) == 0)
			{
				r = 0.448148;
				g = 0.514215;
				b = 0.811111;
			}
			if (QString::compare(QString(groupname.c_str()), QString("c_arteriae_coronari")) == 0)
			{
				r = 0.803704;
				g = 0.307407;
				b = 0.744444;
			}
			if (QString::compare(QString(groupname.c_str()), QString("c_venae_coronari")) == 0)
			{
				r = 0.344444;
				g = 0.637037;
				b = 0.844444;
			}
			if (QString::compare(QString(groupname.c_str()), QString("c_venDex")) == 0)
			{
				r = 0.922222;
				g = 0.337037;
				b = 0.618519;
			}
			if (QString::compare(QString(groupname.c_str()), QString("c_vc_venSin")) == 0)
			{
				r = 0.770370;
				g = 0.807407;
				b = 0.762963;
			}
			if (QString::compare(QString(groupname.c_str()), QString("c_atrDex")) == 0)
			{
				r = 0.418519;
				g = 0.662963;
				b = 0.622222;
			}
		}

		if (tinyobj::info::matFileExists)
		{
			if (z == 0)	// Only print this once
				std::cout << "Material file found. Using colours from .mtl file.\n";

			r = shapes.at(z).material.diffuse[0];
			g = shapes.at(z).material.diffuse[1];
			b = shapes.at(z).material.diffuse[2];
		}

		//vtkSmartPointer<vtkOBBTree> objectOBBTree = vtkSmartPointer<vtkOBBTree>::New();
		//objectOBBTree->SetDataSet(nextMesh);
		//objectOBBTree->BuildLocator();
		//CommonData::objectOBBTrees.push_back(objectOBBTree);

		//vtkSmartPointer<vtkPolyData> dataset = nextMesh;
		vtkSmartPointer<vtkPolyData> dataset = Utility::smoothNormals(Utility::computeNormals(nextMesh));

		vtkColor3f c(r, g, b);
		float opacity = 1.0;
		Utility::addMesh(this, dataset, z, groupname, c, opacity);
	
		/*double bounds[6];		
		meshes[z].actor->GetBounds(bounds);

		std::cout << bounds[0] << ","
			<< bounds[1] << ","
			<< bounds[2] << ","
			<< bounds[3] << ","
			<< bounds[4] << ","
			<< bounds[5] << "\n";
			*/

		// add actor to renderer
		renderer->AddActor(meshes[z].actor);
		renderer->ResetCamera();
		resetClippingPlane();

		qv->GetRenderWindow()->Render();
	}
	// Reset clipping plane AFTER camera reset AND render (also after FlyTo call in interactor)
	resetClippingPlane();


	/*vtkSmartPointer<vtkParametricEllipsoid> ellipse = vtkSmartPointer<vtkParametricEllipsoid>::New();

	vtkSmartPointer<vtkParametricFunctionSource> source = vtkSmartPointer<vtkParametricFunctionSource>::New();
	source->SetParametricFunction(ellipse);
	source->Update();*/

	/*
	#include <vtkPlaneWidget.h>
#include <vtkBoxWidget.h>
#include <vtkBoxWidget2.h>
#include <vtkAngleWidget.h>
#include <vtkAxesTransformWidget.h>
#include <vtkBorderWidget.h>
#include <vtkButtonWidget.h>
#include <vtkCenteredSliderWidget.h>
#include <vtkContourWidget.h>
#include <vtkHandleWidget.h>
#include <vtkHoverWidget.h>
#include <vtkLineWidget2.h>
#include <vtkParallelopipedWidget.h>
#include <vtkResliceCursorWidget.h>
#include <vtkResliceCursorWidget.h>
#include <vtkSliderWidget.h>
#include <vtkSphereWidget2.h>
#include <vtkSplineWidget2.h>
#include <vtkTensorProbeWidget.h>
	*/
	//vtkSmartPointer<vtkPlaneWidget> planeWidget = vtkSmartPointer<vtkPlaneWidget>::New();
	//vtkSmartPointer<vtkBoxWidget> planeWidget = vtkSmartPointer<vtkBoxWidget>::New();
	vtkSmartPointer<vtkBoxWidget> planeWidget = vtkSmartPointer<vtkBoxWidget>::New();
	
	planeWidget->SetInteractor(renderer->GetRenderWindow()->GetInteractor());
	//planeWidget->On();
	/*vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(planeWidget);
	//mapper->set
	
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->DragableOn();

	renderer->AddActor(actor);*/


	/*
	class vtkAffineCallback : public vtkCommand
	{
	public:
		static vtkAffineCallback *New()
		{
			return new vtkAffineCallback;
		}
		virtual void Execute(vtkObject *caller, unsigned long, void*);
		vtkAffineCallback() :Actor(0), AffineRep(0)
		{
			this->Transform = vtkTransform::New();
		}
		~vtkAffineCallback()
		{
			this->Transform->Delete();
		}
		vtkActor *Actor;
		vtkAffineRepresentation2D *AffineRep;
		vtkTransform *Transform;
	};

	void vtkAffineCallback::Execute(vtkObject*, unsigned long vtkNotUsed(event), void*)
	{
		this->AffineRep->GetTransform(this->Transform);
		this->Actor->SetUserTransform(this->Transform);
	}

	vtkSmartPointer<vtkAffineWidget> affine = vtkSmartPointer<vtkAffineWidget>::New();
	affine
	renderer->
	*/

	// Added meshes, now set selectedMesh to last one (Might have to update this for future selections)
	setSelectedMesh(meshes.end());	// Reset selectedMesh to nothing

	// add mesh groupnames to listbox
	for (int i = 0; i < meshes.size(); i++)
	{
		QListWidgetItem* item = new QListWidgetItem(meshes[i].name.c_str(), ui.listWidget);

		item->setCheckState(Qt::Checked);
		ui.listWidget->addItem(item);
		//ui.listWidget->itemAt(0, i)->setCheckState(Qt::Checked);
	}
}
//--------------------------------------------------------------------------------------
void aperio::slot_chkToroid(bool checked)
{
	if (myelems.size() < 1)
		return;

	int i = myelems.size() - 1;
	myelems[i].source->SetToroidal(checked);
	myelems[i].source->Update();
	myelems[i].transformFilter->Update();		// Must update transformfilter for transforms to show
}
//--------------------------------------------------------------------------------------
void aperio::slot_thetaSlider(int value)
{
	if (myelems.size() < 1)
		return;

	int i = myelems.size() - 1;
	myelems[i].source->SetThetaRoundness(value / roundnessScale);
	myelems[i].source->Update();
	myelems[i].transformFilter->Update();		// Must update transformfilter for transforms to show
}
//--------------------------------------------------------------------------------------
void aperio::slot_phiSlider(int value)
{
	if (myelems.size() < 1)
		return;

	int i = myelems.size() - 1;
	myelems[i].source->SetPhiRoundness(value / roundnessScale);
	myelems[i].source->Update();
	myelems[i].transformFilter->Update();		// Must update transformfilter for transforms to show
}
//--------------------------------------------------------------------------------------
void aperio::slot_btnSlice()
{
	// Check if there is selected mesh and widget exists too...
	if (selectedMesh == meshes.end() || myelems.size() < 1)
		return;

	// Change index for eselected to selected element...
	int eselectedindex = myelems.size() - 1;
	MyElem& elem = myelems[eselectedindex];

	CarveConnector connector;
	vtkSmartPointer<vtkPolyData> thepolydata(vtkPolyData::SafeDownCast(selectedMesh->actor->GetMapper()->GetInput()));
	thepolydata = CarveConnector::cleanVtkPolyData(thepolydata);

	vtkSmartPointer<vtkPolyData> mypoly(vtkPolyData::SafeDownCast(elem.actor->GetMapper()->GetInput()));
	vtkSmartPointer<vtkPolyData> thepolydata2 = CarveConnector::cleanVtkPolyData(mypoly);

	// Make MeshSet from vtkPolyData
	std::unique_ptr<carve::mesh::MeshSet<3> > first(CarveConnector::vtkPolyDataToMeshSet(thepolydata));
	std::unique_ptr<carve::mesh::MeshSet<3> > second(CarveConnector::vtkPolyDataToMeshSet(thepolydata2));
	//std::unique_ptr<carve::mesh::MeshSet<3> > second(CarveConnector::makeCube(55, carve::math::Matrix::IDENT()));

	std::unique_ptr<carve::mesh::MeshSet<3> > c(CarveConnector::perform(first, second, carve::csg::CSG::A_MINUS_B, ui.chkTriangulate->isChecked()));
	vtkSmartPointer<vtkPolyData> c_poly(CarveConnector::meshSetToVTKPolyData(c));

	// Create second piece (the cut piece)
	std::unique_ptr<carve::mesh::MeshSet<3> > d(CarveConnector::perform(first, second, carve::csg::CSG::INTERSECTION, ui.chkTriangulate->isChecked()));
	vtkSmartPointer<vtkPolyData> d_poly(CarveConnector::meshSetToVTKPolyData(d));

	// Create normals for resulting polydatas
	vtkSmartPointer<vtkPolyData> dataset = Utility::computeNormals(c_poly);
	vtkSmartPointer<vtkPolyData> datasetd = Utility::computeNormals(d_poly);

	// Update cell locator's dataset so program doesn't slow down after cutting
	selectedMesh->cellLocator->SetDataSet(dataset);

	vtkColor3f color(selectedMesh->color.GetRed(),
		selectedMesh->color.GetGreen(),
		selectedMesh->color.GetBlue());

	// Run through list and see if name with + already exists, while it exists, add another + 
	// to generate unique name
	std::stringstream ss;
	ss << selectedMesh->name << "+";

	while (getListItemByName(ss.str()) != nullptr)
		ss << "+";
	std::string name = ss.str();

	float opacity = selectedMesh->opacity >= 1 ? 1 : selectedMesh->opacity * 0.5f;

	// Create a mapper and actor
	vtkSmartPointer<vtkActor> actor = Utility::sourceToActor(this, dataset, color.GetRed(),
		color.GetGreen(), color.GetBlue(), opacity);	// My opacity (Must change 0.5f)

	// Replace old actor with new actor
	replaceActor(selectedMesh->actor, actor);		// First replace selectedMesh->actor (old) with new actor in Renderer
	selectedMesh->actor = actor;						// Then assign selectedMesh->actor to the new actor

	color.Set(min(color.GetRed() + 0.1, 1.0),
		min(color.GetGreen() + 0.1, 1.0),
		min(color.GetBlue() + 0.1, 1.0));

	// Add second actor (the cut piece) to renderer (as well as to meshes vector)
	CustomMesh & mesh = Utility::addMesh(this, datasetd, meshes.size(), name, color, 1.0);
	mesh.generated = true;

	// Set mesh saved parameters
	mesh.hingeAngle = 0;
	mesh.hingeAmount = 170;

	if (selectedMesh->generated)	// Piece we are cutting is already generated, so reuse snormal
	{
		mesh.snormal = vtkVector3f(selectedMesh->snormal.GetX(), selectedMesh->snormal.GetY(),
			selectedMesh->snormal.GetZ());
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
	renderer->AddActor(mesh.actor);

	// Also add mesh to listWidget
	addToList(name);

	// Remove superquadric from Renderer
	renderer->RemoveActor(myelems[eselectedindex].actor);

	// Probably should remove from list as well (myelems)
	myelems.erase(myelems.end() - 1);

	// Set selected mesh to newly cut
	setSelectedMesh(getMeshByName(name));
}
//-------------------------------------------------------------------------------------
void aperio::slot_listitemclicked(int i)
{
	if (i == -1)
		return;

	std::string itemString = ui.listWidget->item(i)->text().toStdString();	// get new selectedMesh string from list
	setSelectedMesh(getMeshByName(itemString));
}
//-------------------------------------------------------------------------------------------
void aperio::slot_timeout_fps()
{
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		this->close();
	}

	// Increment wave-time in seconds for wiggle
	wavetime = wavetime + 0.0125;
	if (wavetime > 500)
		wavetime = 0;

	// ---- Control wiggle
	if (ui.chkWiggle->isChecked())
		wiggle = true;
	else
		wiggle = false;

	if (GetAsyncKeyState(VK_CONTROL))
		wiggle = false;

	if (!pause)
		qv->update();
}
//-------------------------------------------------------------------------------------
void aperio::slot_chkDepthPeel(bool checked)
{
	if (checked)
		passes->ReplaceItem(passes->GetNumberOfItems() - 1, peelP);
	else
		passes->ReplaceItem(passes->GetNumberOfItems() - 1, transP);
}
//-------------------------------------------------------------------------------------
void aperio::updateOpacitySliderAndList()
{
	if (selectedMesh != meshes.end())
	{
		ui.opacitySlider->setValue(selectedMesh->opacity * 100);

		ui.listWidget->clearSelection();	// unselect all previous selected items in list

		if (selectedMesh != meshes.end())
		{
			auto item = getListItemByName(selectedMesh->name);

			if (item)
			{
				item->setSelected(true);
				ui.listWidget->scrollToItem(item);

				// Update other sliders too (get hinging amount)
				if (selectedMesh->generated)
				{
					float hingeAmount = selectedMesh->hingeAmount;
					float hingeAngle = selectedMesh->hingeAngle;

					ui.txtHingeAmount->setText(QString::number(hingeAmount));
					ui.hingeSlider->setValue((hingeAngle / hingeAmount) * 100.0);

					//interactorstyle->HighlightProp3D(NULL);
					//interactorstyle->HighlightProp3D(selectedMesh->actor);

					//vtkOutlineSource::SafeDownCast(interactorstyle->GetOutlineActor()->GetMapper()->GetInput())->Update();
					//interactorstyle->GetOutlineActor()->highli
					//interactorstyle->GetOutline()->SetInputData(selectedMesh->actor->GetMapper()->GetInputDataObject());

					//interactorstyle->GetOutline()->SetInputData(selectedMesh->actor->GetMapper()->GetInput());
					//interactorstyle->GetOutline()->Update();
				}

				vtkColor3f mycol = selectedMesh->color;
				colorDialog->setCurrentColor(QColor(mycol.GetRed() * 255, mycol.GetGreen() * 255, mycol.GetBlue() * 255));

				// Update check states in listbox
				for (int i = 0; i < meshes.size(); i++)
				{
					auto item = getListItemByName(meshes[i].name);

					if (meshes[i].opacity <= 0)
						item->setCheckState(Qt::Unchecked);
					else
						item->setCheckState(Qt::Checked);
				}
			}
		}
	}
}
//----------------------------------------------------------------------------------------------
void aperio::slot_hingeSlider(int value)
{
	if (selectedMesh == meshes.end() || !selectedMesh->generated)	//	Make sure selected & generated mesh (Rather than original mesh)
		return;

	float angle = (value / 100.0) * ui.txtHingeAmount->text().toInt();
	
	selectedMesh->hingeAngle  = angle;
	selectedMesh->hingeAmount = ui.txtHingeAmount->text().toInt();

	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply();
	transform->Translate(-selectedMesh->hingePivot.GetX(), -selectedMesh->hingePivot.GetY(), -selectedMesh->hingePivot.GetZ());
	transform->RotateWXYZ(-angle, selectedMesh->sup.GetX(), selectedMesh->sup.GetY(),
		selectedMesh->sup.GetZ());
	transform->Translate(selectedMesh->hingePivot.GetX(), selectedMesh->hingePivot.GetY(), selectedMesh->hingePivot.GetZ());

	selectedMesh->actor->SetUserTransform(transform);

	// Update highlight
	interactorstyle->HighlightProp3D(selectedMesh->actor);

	QApplication::processEvents();
	//}
}
//-------------------------------------------------------------------------------
void aperio::slot_btnHide()
{
	if (selectedMesh != meshes.end())
	{
		float newopacity;

		if (selectedMesh->opacity == 0)
		{
			newopacity = 100;
			//interactorstyle->HighlightProp3D(selectedMesh->actor);
		}
		else
		{
			newopacity = 0;
			//interactorstyle->HighlightProp3D(nullptr);
		}

		selectedMesh->opacity = newopacity;
		selectedMesh->actor->GetProperty()->SetOpacity(newopacity);
		updateOpacitySliderAndList();
	}
}
//-------------------------------------------------------------------------------------
void aperio::slot_txtHingeAmount(const QString &string)
{
	if (selectedMesh == meshes.end() || !selectedMesh->generated)	//	Make sure selected & generated mesh (Rather than original mesh)
		return;

	selectedMesh->hingeAmount = ui.txtHingeAmount->text().toInt();
}
//-----------------------------------------------------------------------------------------
void aperio::setSelectedMesh(std::vector<CustomMesh>::iterator &it)
{
	// Reset all meshes' selection parameter to false
	for (int i = 0; i < meshes.size(); i++)
		meshes[i].selected = false;

	selectedMesh = it;			// Set selectedMesh to mesh clicked 

	if (it != meshes.end())
	{
		selectedMesh->selected = true;	// Set selected property to true
		updateOpacitySliderAndList();	// Update list

		// Remove previous highlight
		interactorstyle->HighlightProp(NULL);

		// Highlight new prop
		interactorstyle->HighlightProp3D(selectedMesh->actor);

		interactorstyle->GetOutlineActor()->GetProperty()->SetLineWidth(1.65);
		interactorstyle->GetOutlineActor()->GetProperty()->SetOpacity(0.9);
	}
}
//--------------------------------------------------------------------------------------------------------------
void aperio::resetClippingPlane()
{
	//double d[2];
	renderer->ResetCameraClippingRange();

	renderer->GetActiveCamera()->SetClippingRange(0.05, 7500);
	
	std::cout << renderer->GetRenderWindow()->GetDepthBufferSize();
	//renderer->toler
}
