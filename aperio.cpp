#include "stdafx.h"

// QT Includes
#include <QLayout>
#include <QDesktopWidget>
#include <vtk/QVTKWidget.h>

// VTK includes
#include <vtk/vtkSphereSource.h>
#include <vtk/vtkOBBTree.h>

// Custom
#include "CarveConnector.h"
#include "MyInteractorStyle.h"
#include "Utility.h"
#include "vtkMyBasePass.h"
#include "vtkMyPrePass.h"
#include "vtkMyShaderPass.h"
#include "vtkMyImageProcessingPass.h"

// More VTK
#include <vtk/vtkLightsPass.h>
#include <vtk/vtkCameraPass.h>
#include <vtk/vtkDefaultPass.h>
#include <vtk/vtkSequencePass.h>
#include <vtk/vtkRenderPassCollection.h>
#include <vtk/vtkGaussianBlurPass.h>
#include <vtk/vtkSobelGradientMagnitudePass.h>
#include <vtk/vtkPolyDataConnectivityFilter.h>
#include <vtk/vtkOpaquePass.h>
#include <vtk/vtkTranslucentPass.h>
#include <vtk/vtkClearZPass.h>

#include <vtk/vtkRenderPass.h>
#include <vtk/vtkLight.h>

#include <vtk/vtkDoubleArray.h>
#include <vtk/vtkOutlineSource.h>

#include "aperio.h"

// Include widgets
#include <vtk/vtkAffineWidget.h>
#include <vtk/vtkAffineRepresentation2D.h>

#include <vtk/vtkParametricEllipsoid.h>
#include <vtk/vtkParametricFunctionSource.h>

#include <vtk/vtkPlaneWidget.h>
#include <vtk/vtkBoxWidget.h>
#include <vtk/vtkBoxWidget2.h>
#include <vtk/vtkAngleWidget.h>
#include <vtk/vtkAxesTransformWidget.h>
#include <vtk/vtkBorderWidget.h>
#include <vtk/vtkButtonWidget.h>
#include <vtk/vtkCenteredSliderWidget.h>
#include <vtk/vtkContourWidget.h>
#include <vtk/vtkHandleWidget.h>
#include <vtk/vtkHoverWidget.h>
#include <vtk/vtkLineWidget2.h>
#include <vtk/vtkParallelopipedWidget.h>
#include <vtk/vtkResliceCursorWidget.h>
#include <vtk/vtkResliceCursorWidget.h>
#include <vtk/vtkSliderWidget.h>
#include <vtk/vtkSphereWidget2.h>
#include <vtk/vtkSplineWidget2.h>
#include <vtk/vtkSplineWidget.h>
#include <vtk/vtkTensorProbeWidget.h>
#include <vtk/vtkSliderWidget.h>

#include <vtk/vtkBoxRepresentation.h>

#include <vtk/vtkCubeSource.h>

#include <vtk/vtkCylinderSource.h>
#include <vtk/vtkSliderRepresentation3D.h>
#include <vtk/vtkPickingManager.h>
#include <vtk/vtkSplineRepresentation.h>

float aperio::RING_PERCENT = 1.0;// 0.90f; // 0.85

double aperio::DEFAULT_TAPER = 0.1;	// For Knife

double aperio::DEFAULT_RODSIZE = 0.15;	// For Rod

//-------------------------------------------------------------------------------------------------------------
aperio::aperio(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Constructor (initialize variables before window shown)
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
	fps = 45.0;
	//string fname = "cube.obj";
	//string fname = "organs brain 250K.obj";
	//string fname = "hearttest.obj";
	string fname = "newestheart.obj";
	//string fname = "open.obj";
	//string fname = "utah-teapot.obj";

	// QT Variables
	pause = false;
	preview = false;

	// Uniforms
	wiggle = true;
	shadingnum = 0;		// Toon/normal, etc.
	brushDivide = 15.0;

	brushSize = 1.5;

	// Default uniform variables
	mouse[0] = 0;
	mouse[1] = 0;
	mouse[2] = 0;

	wavetime = 0;
	difftrans = ui.btnLight->text().compare("On") == 0 ? true : false;
	shininess = ui.shininessSlider->value();
	darkness = (ui.darknessSlider->value() + 128) / 128.0f;

	// Set tool tip actor
	toolTip.reset();

	// Initially set invisible thickness Slider and label
	ui.lblThickness->setVisible(false);
	ui.thicknessSlider->setVisible(false);


	//ui.menuBar->setStyle()

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
	timer_fps = new QTimer(this);
	//timer_fps->setInterval(0);
	timer_fps->setInterval(1000.0 / fps);
	timer_fps->setTimerType(Qt::TimerType::PreciseTimer);
	timer_fps->start();

	QTimer* timer_delay = nullptr;				// Delayed timer (executes slower after a delay)
	timer_delay = new QTimer(this);
	timer_delay->setInterval(1000.0 / 0.5);		// 7.5
	timer_delay->start();

	// ---- Custom Thread Timers

	/*timer_explode = nullptr;	// Instantaneous timer (executed as fast as possible)
	timer_explode = new QTimer(this);
	timer_explode->setInterval(1000.0 / fps);
	timer_explode->setTimerType(Qt::TimerType::PreciseTimer);
	*/

	timer_highlight = new QTimer(this);
	timer_highlight->setInterval(1000.0 / fps);
	timer_highlight->setTimerType(Qt::TimerType::PreciseTimer);

	colorDialog = new QColorDialog(this);
	colorDialog->setWindowTitle("Pick a color for the selected object.");
	colorDialog->setWindowOpacity(.85);
	colorDialog->setStyleSheet("background: rgba(0, 0, 0, 255); color: white; selection-color: black;");

	QRect scr = QApplication::desktop()->screenGeometry();
	colorDialog->move(scr.center() + QPoint(150, -150));

	// Start VTK
	renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AlphaBitPlanesOn();
	renderWindow->SetMultiSamples(0);

	renderer = vtkSmartPointer<vtkRenderer>::New();

	/* float bgcolor[3] = { 235, 235, 235 };
	float factor = 0.7;
	bgcolor[0] *= factor; bgcolor[1] *= factor; bgcolor[2] *= factor;
	renderer->SetBackground(bgcolor[0] / 255.0, bgcolor[1] / 255.0, bgcolor[2] / 255.0);
	*/

	// Change background texture
	texture = vtkSmartPointer<vtkTexture>::New();
	texture->InterpolateOn();
	texture->SetInputData(Utility::getImageData("bg.png"));

	slot_btnBackground();

	renderWindow->AddRenderer(renderer);

	// Dynamically create a new QVTKWidget (and set render window to it)
	qv = new QVTKWidget(this);
	qv->SetRenderWindow(renderWindow);
	
	auto plant = QCursor(QPixmap(":/aperio/sliding2.png").scaled(30, 30));	



	ui.mainWidget->setLayout(new QGridLayout(ui.mainWidget));
	ui.mainWidget->layout()->setMargin(2);
	ui.mainWidget->layout()->addWidget(qv);

	// Prepare all the rendering passes
	preP = vtkSmartPointer<vtkMyPrePass>::New();
	preP->initialize(this);
	preP->setShaderFile("shader_water.vert", false);
	preP->setShaderFile("shader0.frag", true);

	vtkSmartPointer<vtkCameraPass> precameraP = vtkSmartPointer<vtkCameraPass>::New();
	precameraP->SetDelegatePass(preP);	// transforms prepass into camera coordinates

	mainP = vtkSmartPointer<vtkMyShaderPass>::New();
	mainP->initialize(this);
	mainP->setShaderFile("shader_water.vert", false);
	mainP->setShaderFile("shader.frag", true);
	mainP->SetDelegatePass(precameraP);	// We need the camera-transformed image in the FBO

	vtkSmartPointer<vtkCameraPass> cameraP = vtkSmartPointer<vtkCameraPass>::New();
	cameraP->SetDelegatePass(mainP);

	// Requires Depth from camera pass
	//vtkSmartPointer<vtkMyProcessingPass> dofP = vtkSmartPointer<vtkMyProcessingPass>::New();
	//dofP->setShaderFile("shader_dof.frag", true);
	//dofP->SetDelegatePass(cameraP);

	// Requires Depth from camera pass
	vtkSmartPointer<vtkMyImageProcessingPass> ssaoP = vtkSmartPointer<vtkMyImageProcessingPass>::New();
	ssaoP->setShaderFile("shader_pass.vert", false);
	ssaoP->setShaderFile("shader_ssao.frag", true);
	ssaoP->SetDelegatePass(cameraP);

	//vtkSmartPointer<vtkMyProcessingPass> dotP = vtkSmartPointer<vtkMyProcessingPass>::New();
	//dotP->setShaderFile("shader_dot.frag", true);
	//dotP->SetDelegatePass(ssaoP);

	vtkSmartPointer<vtkMyImageProcessingPass> fxaaP = vtkSmartPointer<vtkMyImageProcessingPass>::New();
	fxaaP->setShaderFile("shader_fxaa.vert", false);
	fxaaP->setShaderFile("shader_fxaa.frag", true);
	fxaaP->SetDelegatePass(ssaoP);

	vtkSmartPointer<vtkMyImageProcessingPass> bloomP = vtkSmartPointer<vtkMyImageProcessingPass>::New();
	bloomP->setShaderFile("shader_pass.vert", false);
	bloomP->setShaderFile("shader_bloom.frag", true);
	bloomP->SetDelegatePass(fxaaP);

	vtkOpenGLRenderer::SafeDownCast(renderer.GetPointer())->SetPass(bloomP);

	// Render window interactor
	vtkSmartPointer<QVTKInteractor> renderWindowInteractor = vtkSmartPointer<QVTKInteractor>::New();

	interactorstyle = vtkSmartPointer<MyInteractorStyle>::New();
	//interactorstyle->SetAutoAdjustCameraClippingRange(false);
	interactorstyle->initialize(this);

	renderWindowInteractor->SetInteractorStyle(interactorstyle);
	renderWindowInteractor->Initialize();

	renderWindowInteractor->GetPickingManager()->EnabledOn();
	renderWindowInteractor->GetPickingManager()->AddPicker(interactorstyle->cellPicker);

	qv->GetRenderWindow()->SetInteractor(renderWindowInteractor);
	
	QApplication::processEvents();

	// --- Initialize GLEW (After OpenGL context is set up!)
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		cout << "Error: \n" << glewGetErrorString(err) << "\n";
		system("pause");
		exit(1);
	}
	else
	{
		glew_available = true;
		cout << "GLEW Initialized: " << glewGetString(GLEW_VERSION) << "\n";
	}

	//---- GLEW loaded (place all OpenGL calls after this line
	loadMatCapTexture("mc19.jpg");

	// Set up text validators for QLineEdits in form
	ui.txtHingeAmount->setValidator(new QIntValidator(-360, 360, this));
	//ui.txtExplodeAmount->setValidator(new QIntValidator(-1000, 1000, this));

	// Connect all signals to slots
	//connect(ui.menuBar, &QMenuBar::, this, [](){cout << "hi"; }
	//);


	connect(ui.btnLight, &QPushButton::clicked, this, &aperio::slot_buttonclicked);
	connect(ui.btnColor, &QPushButton::clicked, this, &aperio::slot_colorclicked);
	
	connect(colorDialog, &QColorDialog::currentColorChanged, this, &aperio::slot_colorchanged);

	connect(timer_instant, &QTimer::timeout, this, &aperio::slot_timeout_instant);
	connect(timer_fps, &QTimer::timeout, this, &aperio::slot_timeout_fps);
	connect(timer_delay, &QTimer::timeout, this, &aperio::slot_timeout_delay);

	// ---- Custom Thread Timers
	//connect(timer_explode, &QTimer::timeout, this, &aperio::slot_timer_explode);
	connect(timer_highlight, &QTimer::timeout, this, &aperio::slot_timer_highlight);

	connect(ui.actionOpen, &QAction::triggered, this, &aperio::slot_open);
	connect(ui.actionAppend, &QAction::triggered, this, &aperio::slot_append);
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
	//connect(ui.listWidget, &QListWidget::currentRowChanged, this, &aperio::slot_listitemclicked);
	connect(ui.listWidget, &QListWidget::itemClicked, this, &aperio::slot_listitemclicked);
	connect(ui.listWidget, &QListWidget::itemDoubleClicked, this, &aperio::slot_listitemdoubleclicked);

	ui.listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.listWidget, &QListWidget::customContextMenuRequested, this, &aperio::slot_listitemrightclicked);

	// Options
	connect(ui.chkDepthPeel, &QCheckBox::toggled, this, &aperio::slot_chkDepthPeel);
	connect(ui.chkCap, &QCheckBox::toggled, this, &aperio::slot_chkCap);
	connect(ui.chkSnap, &QCheckBox::toggled, this, &aperio::slot_chkSnap);
	connect(ui.chkSnapReal, &QCheckBox::toggled, this, &aperio::slot_chkSnapReal);

	// Buttons
	connect(ui.btnBackground, &QPushButton::clicked, this, &aperio::slot_btnBackground);
	connect(ui.btnInvert, &QPushButton::clicked, this, &aperio::slot_btnInvert);

	connect(ui.btnPlant, &QPushButton::clicked, this, &aperio::slot_btnPlant);
	connect(ui.btnPickUp, &QPushButton::clicked, this, &aperio::slot_btnPickUp);
	connect(ui.btnCut, &QPushButton::clicked, this, &aperio::slot_btnCut);

	connect(ui.btnHinge, &QPushButton::clicked, this, &aperio::slot_btnHinge);
	connect(ui.btnKnife, &QPushButton::clicked, this, &aperio::slot_btnKnife);
	connect(ui.btnCookie, &QPushButton::clicked, this, &aperio::slot_btnCookie);
	connect(ui.btnHide, &QPushButton::clicked, this, &aperio::slot_btnHide);
	connect(ui.btnGlass, &QPushButton::clicked, this, &aperio::slot_btnGlass);

	connect(ui.btnRing, &QPushButton::clicked, this, &aperio::slot_btnRing);
	connect(ui.btnRod, &QPushButton::clicked, this, &aperio::slot_btnRod);
	connect(ui.btnRibbons, &QPushButton::clicked, this, &aperio::slot_btnRibbons);

	connect(ui.btnRestore, &QPushButton::clicked, this, &aperio::slot_btnRestore);

	// Superquadric options
	connect(ui.taperSlider, &QSlider::valueChanged, this, &aperio::slot_taperSlider);

	connect(ui.phiSlider, &QSlider::valueChanged, this, &aperio::slot_phiSlider);
	connect(ui.thetaSlider, &QSlider::valueChanged, this, &aperio::slot_thetaSlider);
	connect(ui.thicknessSlider, &QSlider::valueChanged, this, &aperio::slot_thicknessSlider);
	connect(ui.chkToroid, &QCheckBox::toggled, this, &aperio::slot_chkToroid);

	// Transform sliders
	connect(ui.hingeSlider, &QSlider::valueChanged, this, &aperio::slot_hingeSlider);
	connect(ui.ringSlider, &QSlider::valueChanged, this, &aperio::slot_ringSlider);
	connect(ui.rodSlider, &QSlider::valueChanged, this, &aperio::slot_rodSlider);
	connect(ui.chkFrontRibbons, &QCheckBox::toggled, this, &aperio::slot_chkFrontRibbons);

	

	connect(ui.ribbonwidthSlider, &QSlider::valueChanged, this, &aperio::slot_ribbonwidthSlider);
	connect(ui.ribbonfreqSlider, &QSlider::valueChanged, this, &aperio::slot_ribbonfreqSlider);
	connect(ui.ribbontiltSlider, &QSlider::valueChanged, this, &aperio::slot_ribbontiltSlider);

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
void aperio::readFile(string filename)
{
	Utility::start_clock('a');

	clearSelectedMeshes();

	//vtkObject::GlobalWarningDisplayOff();	// dangerous (keep on for most part)

	// Reset values for new file
	renderer->RemoveAllViewProps();	// Remove from renderer, clear listwidget, clear vectors

	ui.listWidget->clear();
	myelems.clear();
	meshes.clear();

	// Reset tooltip
	toolTip.reset();
	toolTipOn = false;

	qDebug() << " - reading file - \n";

	if (path.isEmpty())				// Set path if it is empty
		path = QDir::currentPath();

	// Read file using Open Asset Importer
	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_FAVOUR_SPEED, 1);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 600000);	// max vertices/tris before splitting mesh
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 600000);
	//importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	//Assimp::DefaultLogger::create("", Assimp::DefaultLogger::VERBOSE, aiDefaultLogStream_STDOUT);	// Log time

	QProgressDialog progress("Loading file. Please be patient...", "Abort", 0, 0, this);

	QRect newPos = progress.geometry();
	newPos.adjust(250, 300, 0, 0);
	progress.setGeometry(newPos);

	progress.setStyleSheet("background: rgba(0, 0, 0, 255); color: white;");
	progress.showNormal();

	QApplication::processEvents();

	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate |
		aiProcess_SplitLargeMeshes
		//aiProcess_OptimizeMeshes |
		//aiProcess_FindDegenerates |
		//aiProcess_SortByPType
		//aiProcess_FlipUVs
		);

	// If the import failed, report it
	if (!scene)
	{
		cout << "Error loading model: " << filename << "\n" << importer.GetErrorString() << "\n";
		return;
	}

	// VTK : Remove lights (light computation done in shader)
	renderer->AutomaticLightCreationOff();
	renderer->RemoveAllLights();

	// Compute scene bounding box
	aiVector3D min;
	aiVector3D max;
	Utility::get_bounding_box(scene, &min, &max);
	renderer->ResetCamera(min.x, max.x, min.y, max.y, min.z, max.z);

	// Strip filename only from path
	QFileInfo fileInfo(filename.c_str());
	string filenameOnly = fileInfo.fileName().toStdString();

	//srand(time(nullptr));		//Random Seed
	float r, g, b;
	for (int z = 0; z < scene->mNumMeshes; z++)
	{
		if (scene->mNumMeshes > 1) // assign a random colour if there's more than one object
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

		aiString groupname;
		// index z of mesh array is larger than number of meshes in AssimpNode (Create default name)
		if (z >= scene->mRootNode->FindNode(filenameOnly.c_str())->mNumChildren)
		{
			QString extra = QString("SplitMesh_") + QString::number(z);
			groupname.Set(extra.toStdString().c_str());
		}
		else
			groupname.Set(scene->mRootNode->FindNode(filenameOnly.c_str())->mChildren[z]->mName.C_Str());

		// Remove darkfactor when customizable colours implemented and saveable (1.4f currently in fragment shader)

		// Default random colours for heart
		if (QString(filename.c_str()).contains("newestheart.obj"))
		{
			if (QString::compare(QString(groupname.C_Str()), QString("c_pericardium")) == 0)
			{
				r = 0.448148;
				g = 0.514215;
				b = 0.811111;

				//r = 255 / 255.0;
				//g = 214 / 255.0;
				//b = 211 / 255.0;
				r = 225 / 255.0;
				g = 192 / 255.0;
				b = 192 / 255.0;

				// 204, 134, 134
			}
			if (QString::compare(QString(groupname.C_Str()), QString("c_arteriae_coronari")) == 0)
			{
				r = 222 / 255.0;
				g = 119 / 255.0;
				b = 88 / 255.0;
			}
			if (QString::compare(QString(groupname.C_Str()), QString("c_venae_coronari")) == 0)
			{
				r = 151 / 255.0;
				g = 180 / 255.0;
				b = 227 / 255.0;
			}
			if (QString::compare(QString(groupname.C_Str()), QString("c_venDex")) == 0)
			{
				r = 0.922222;
				g = 0.337037;
				b = 0.618519;
			}
			if (QString::compare(QString(groupname.C_Str()), QString("c_vc_venSin")) == 0)
			{
				r = 166 / 255.0;
				g = 212 / 255.0;
				b = 255 / 255.0;
			}
			if (QString::compare(QString(groupname.C_Str()), QString("c_atrDex")) == 0)
			{
				r = 255 / 255.0;
				g = 255 / 255.0;
				b = 255 / 255.0;
			}
		}
		// Material file
		aiColor3D color(0.6f, 0.6f, 0.6f);	// Dummy value (Assimp uses 0.6 for default diffuse)
		scene->mMaterials[scene->mMeshes[z]->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);

		if (color.r == 0.6f && color.g == 0.6f && color.b == 0.6f)	 // no materials	found
		{
			//r = 1;	// default to white for now
			//g = 1;
			//b = 1;
		}
		else
		{
			if (z == 0)	// Only print this once (found material file)
				cout << "Material file found. Using colours from .mtl file.\n";
			r = color.r;
			g = color.g;
			b = color.b;
		}
		vtkSmartPointer<vtkPolyData> nextMesh = Utility::assimpOBJToVtkPolyData(scene->mMeshes[z]);

		/*
		vtkSmartPointer<vtkFeatureEdges> feature = vtkSmartPointer<vtkFeatureEdges>::New();
		feature->SetInputData(nextMesh);
		feature->BoundaryEdgesOn();
		feature->FeatureEdgesOff();
		feature->NonManifoldEdgesOff();
		feature->ManifoldEdgesOff();
		feature->Update();
		cout << setw(25) << groupname.C_Str() << setw(15) << "Boundaries: " << feature->GetOutput()->GetNumberOfPoints() << "\n";
		*/

		nextMesh = CarveConnector::cleanVtkPolyData(nextMesh, false);
		nextMesh = Utility::computeNormals(nextMesh);

		//nextMesh->BuildCells();
		//nextMesh->BuildLinks();

		/*vtkSmartPointer<vtkMyFillHolesFilter> fillHolesFilter = vtkSmartPointer<vtkMyFillHolesFilter>::New();
		fillHolesFilter->SetInputData(nextMesh);
		fillHolesFilter->SetHoleSize(10);
		fillHolesFilter->Update();

		nextMesh = fillHolesFilter->GetOutput();*/

		Utility::addMesh(this, nextMesh, groupname.C_Str(), vtkColor3f(r, g, b), 1.0, nullptr);

		//renderer->AddActor(Utility::sourceToActor(this, nextMesh, 1, 1, 1, 1));

		//cout << setw(25) << groupname.C_Str() << setw(15) << "verts: " << nextMesh->GetNumberOfPoints() << setw(15) << "tris: " << nextMesh->GetNumberOfPolys() << "\n";
		//totalverts += nextMesh->GetNumberOfPoints();
		//totaltris += nextMesh->GetNumberOfPolys();

		if (progress.wasCanceled())
			break;

		//renderer->ResetCamera();
		//qv->GetRenderWindow()->Render();
	}
	//cout << "Total verts: " << totalverts << " | Total tris: " << totaltris << "\n";

	progress.hide();

	// Reset clipping plane AFTER camera reset AND render (also after FlyTo call in interactor)
	resetClippingPlane();

	/*vtkSmartPointer<vtkBoxWidget> boxWidget = vtkSmartPointer<vtkBoxWidget>::New();
	boxWidget->SetInteractor(renderWindow->GetInteractor());
	boxWidget->On();
	*/

	// Add a widget
	/*splineWidget = vtkSmartPointer<vtkSplineWidget2>::New();
	splineWidget->SetInteractor(renderer->GetRenderWindow()->GetInteractor());

	class vtkMySplineRepresentation : public vtkSplineRepresentation
	{
	public:
	void setTolerance()
	{
	this->HandlePicker->SetTolerance(0.0025);
	this->LinePicker->SetTolerance(0.0025);
	}
	};
	vtkSmartPointer<vtkSplineRepresentation> splineRep = vtkSmartPointer<vtkSplineRepresentation>::New();

	double points[3] = { 2, 2, 2 };
	splineRep->SetHandlePosition(0, points);
	double points2[3] = { -2, 2, 2 };
	splineRep->SetHandlePosition(1, points2);

	static_cast<vtkMySplineRepresentation *>(splineRep.GetPointer())->setTolerance();

	//splineRep->setTolerance();

	splineWidget->SetRepresentation(splineRep);

	//splineWidget->On();

	//boxWidget->CreateDefaultRepresentation();

	//vtkSmartPointer<vtkBoxRepresentation> boxRepresentation = vtkSmartPointer<vtkBoxRepresentation>::New();
	//boxWidget->SetRepresentation(boxRepresentation);
	*/

	//vtkWidgetRepresentation *boxRep = boxWidget->GetRepresentation();

	//double bounds[] = { 0, 5, -0.5, 0.5, -0.5, 0.5 };
	//boxRep->PlaceWidget(bounds);

	// planeWidget is amazing
	//vtkSmartPointer<vtkAxesTransformWidget> axesWidget = vtkSmartPointer<vtkAxesTransformWidget>::New();
	//axesWidget->SetInteractor(renderer->GetRenderWindow()->GetInteractor());
	//axesWidget->On();

	/*vtkSmartPointer<vtkSliderRepresentation3D> sliderRep = vtkSmartPointer<vtkSliderRepresentation3D>::New();
	sliderRep->SetMinimumValue(3.0);
	sliderRep->SetMaximumValue(50.0);
	sliderRep->SetValue(12);
	sliderRep->SetTitleText("Sphere Resolution");
	sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToWorld();
	sliderRep->GetPoint1Coordinate()->SetValue(-4, 6,1);
	sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToWorld();
	sliderRep->GetPoint2Coordinate()->SetValue(4, 6, 1);
	sliderRep->SetSliderLength(0.075);
	sliderRep->SetSliderWidth(0.05);
	sliderRep->SetEndCapLength(0.05);

	vtkSmartPointer<vtkSliderWidget> sliderWidget =vtkSmartPointer<vtkSliderWidget>::New();
	sliderWidget->SetInteractor(renderer->GetRenderWindow()->GetInteractor());
	sliderWidget->SetRepresentation(sliderRep);
	sliderWidget->SetAnimationModeToAnimate();
	sliderWidget->EnabledOn();
	sliderWidget->On();*/

	/*vtkSmartPointer<vtkAngleWidget> angleWidget = vtkSmartPointer<vtkAngleWidget>::New();
	angleWidget->SetInteractor(renderer->GetRenderWindow()->GetInteractor());
	angleWidget->CreateDefaultRepresentation();
	angleWidget->On();*/

	// add mesh groupnames to listbox
	//for (auto &mesh : meshes)
	//{
		
//		//ui.listWidget->itemAt(0, i)->setCheckState(Qt::Checked);
//	}
	Utility::end_clock('a');
}


///---------------------------------------------------------------------------------------
void aperio::appendFile(string filename)
{
	Utility::start_clock('a');
	
	clearSelectedMeshes();

	if (meshes.size() == 0)
	{
		// Reset values for new file
		renderer->RemoveAllViewProps();	// Remove from renderer, clear listwidget, clear vectors
		// VTK : Remove lights (light computation done in shader)
		renderer->AutomaticLightCreationOff();
		renderer->RemoveAllLights();

		ui.listWidget->clear();
		myelems.clear();
		meshes.clear();

		qDebug() << " - reading file - \n";
	}
	
	if (path.isEmpty())				// Set path if it is empty
		path = QDir::currentPath();

	// Read file using Open Asset Importer
	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_FAVOUR_SPEED, 1);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 600000);	// max vertices/tris before splitting mesh
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 600000);
	
	QProgressDialog progress("Appending file. Please be patient...", "Abort", 0, 0, this);

	QRect newPos = progress.geometry();
	newPos.adjust(250, 300, 0, 0);
	progress.setGeometry(newPos);

	progress.setStyleSheet("background: rgba(0, 0, 0, 255); color: white;");
	progress.showNormal();

	QApplication::processEvents();

	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate |
		aiProcess_SplitLargeMeshes
		//aiProcess_OptimizeMeshes |
		//aiProcess_FindDegenerates |
		//aiProcess_SortByPType
		//aiProcess_FlipUVs
		);

	// If the import failed, report it
	if (!scene)
	{
		cout << "Error appending model: " << filename << "\n" << importer.GetErrorString() << "\n";
		return;
	}
	
	if (meshes.size() == 0)
	{
		// Compute scene bounding box
		aiVector3D min;
		aiVector3D max;
		Utility::get_bounding_box(scene, &min, &max);
		renderer->ResetCamera(min.x, max.x, min.y, max.y, min.z, max.z);
	}
	

	// Strip filename only from path
	QFileInfo fileInfo(filename.c_str());
	string filenameOnly = fileInfo.fileName().toStdString();

	//srand(time(nullptr));		//Random Seed
	float r, g, b;

	for (int z = 0; z < scene->mNumMeshes; z++)
	{
		if (scene->mNumMeshes > 1) // assign a random colour if there's more than one object
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

		aiString groupname;
		// index z of mesh array is larger than number of meshes in AssimpNode (Create default name)
		if (z >= scene->mRootNode->FindNode(filenameOnly.c_str())->mNumChildren)
		{
			QString extra = QString("SplitMesh_") + QString::number(z);
			groupname.Set(extra.toStdString().c_str());
		}
		else
			groupname.Set(scene->mRootNode->FindNode(filenameOnly.c_str())->mChildren[z]->mName.C_Str());

		// Remove darkfactor when customizable colours implemented and saveable (1.4f currently in fragment shader)

		// Default random colours for heart
		if (QString(filename.c_str()).contains("newestheart.obj"))
		{
			if (QString::compare(QString(groupname.C_Str()), QString("c_pericardium")) == 0)
			{
				r = 0.448148;
				g = 0.514215;
				b = 0.811111;

				//r = 255 / 255.0;
				//g = 214 / 255.0;
				//b = 211 / 255.0;
				r = 225 / 255.0;
				g = 192 / 255.0;
				b = 192 / 255.0;

				// 204, 134, 134
			}
			if (QString::compare(QString(groupname.C_Str()), QString("c_arteriae_coronari")) == 0)
			{
				r = 222 / 255.0;
				g = 119 / 255.0;
				b = 88 / 255.0;
			}
			if (QString::compare(QString(groupname.C_Str()), QString("c_venae_coronari")) == 0)
			{
				r = 151 / 255.0;
				g = 180 / 255.0;
				b = 227 / 255.0;
			}
			if (QString::compare(QString(groupname.C_Str()), QString("c_venDex")) == 0)
			{
				r = 0.922222;
				g = 0.337037;
				b = 0.618519;
			}
			if (QString::compare(QString(groupname.C_Str()), QString("c_vc_venSin")) == 0)
			{
				r = 166 / 255.0;
				g = 212 / 255.0;
				b = 255 / 255.0;
			}
			if (QString::compare(QString(groupname.C_Str()), QString("c_atrDex")) == 0)
			{
				r = 255 / 255.0;
				g = 255 / 255.0;
				b = 255 / 255.0;
			}
		}
		// Material file
		aiColor3D color(0.6f, 0.6f, 0.6f);	// Dummy value (Assimp uses 0.6 for default diffuse)
		scene->mMaterials[scene->mMeshes[z]->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);

		if (color.r == 0.6f && color.g == 0.6f && color.b == 0.6f)	 // no materials	found
		{
			//r = 1;	// default to white for now
			//g = 1;
			//b = 1;
		}
		else
		{
			if (z == 0)	// Only print this once (found material file)
				cout << "Material file found. Using colours from .mtl file.\n";
			r = color.r;
			g = color.g;
			b = color.b;
		}
		
		vtkSmartPointer<vtkPolyData> nextMesh = Utility::assimpOBJToVtkPolyData(scene->mMeshes[z]);

		/*vtkSmartPointer<vtkFeatureEdges> feature = vtkSmartPointer<vtkFeatureEdges>::New();
		feature->SetInputData(nextMesh);
		feature->BoundaryEdgesOn();
		feature->FeatureEdgesOff();
		feature->NonManifoldEdgesOff();
		feature->ManifoldEdgesOff();
		feature->Update();
		cout << setw(25) << groupname.C_Str() << setw(15) << "Boundaries: " << feature->GetOutput()->GetNumberOfPoints() << "\n";
		*/

		nextMesh = CarveConnector::cleanVtkPolyData(nextMesh, false);
		nextMesh = Utility::computeNormals(nextMesh);

		Utility::addMesh(this, nextMesh, groupname.C_Str(), vtkColor3f(r, g, b), 1.0,nullptr);

		if (progress.wasCanceled())
			break;

		
	}
	//	cout << "Total verts: " << totalverts << " | Total tris: " << totaltris << "\n";

	progress.hide();

	// Reset clipping plane AFTER camera reset AND render (also after FlyTo call in interactor)
	resetClippingPlane();
	
	// add mesh groupnames to listbox
	//ui.listWidget->clear();

	/*for (auto &mesh : meshes)
	{

		//ui.listWidget->itemAt(0, i)->setCheckState(Qt::Checked);
	}*/
	
	Utility::end_clock('a');
}

// **************************** Slots Region ***************************************************
#pragma region ~~SLOTS

//--------------------------------------------------------------------------------------
void aperio::slot_chkToroid(bool checked)
{
	if (checked)
	{
		ui.lblThickness->setVisible(true);
		ui.thicknessSlider->setVisible(true);
	}
	else
	{
		ui.lblThickness->setVisible(false);
		ui.thicknessSlider->setVisible(false);
	}

	if (myelems.size() < 1)
		return;

	int i = myelems.size() - 1;
	myelems[i]->source->SetToroidal(checked);
	myelems[i]->source->Update();
	myelems[i]->transformFilter->Update();		// Must update transformfilter for transforms to show
	renderWindow->Render();
}
//--------------------------------------------------------------------------------------
void aperio::slot_thetaSlider(int value)
{
	if (myelems.empty())
		return;
	
	auto toolTip = this->toolTip.lock();
	if (!toolTip)
	{
		// If no tooltip active, then use the last planted widget (last elem in widgets vector)
		toolTip = myelems.back();
	}		

	auto ringRoundness = [](shared_ptr<MyElem> elem, double roundness) -> double {
		
		double result = roundness;
		if (elem->toolType == RING)
		{
			if (result < 0.4)
				result = 0.4;
		}
		return result;
	};

	if (toolTipOn)
	{
		double roundness = value / roundnessScale;
		roundness = ringRoundness(toolTip, roundness);

		toolTip->source->SetThetaRoundness(roundness);
		toolTip->source->Update();
		toolTip->transformFilter->SetTransform(makeCompositeTransformFromSinglePoint(*toolTip));
		toolTip->transformFilter->Update();
	}
	else
	{
		auto elem = myelems.back();
		double roundness = value / roundnessScale;
		roundness = ringRoundness(elem, roundness);

		elem->source->SetThetaRoundness(roundness);
		elem->source->Update();
		elem->transformFilter->Update();		// Must update transformfilter for transforms to show
	}
	renderWindow->Render();
}
//--------------------------------------------------------------------------------------
void aperio::slot_taperSlider(int value)
{
	if (myelems.empty())
		return;

	auto toolTip = this->toolTip.lock();
	if (!toolTip)
	{
		// If no tooltip active, then use the last planted widget (last elem in widgets vector)
		toolTip = myelems.back();
	}

	float totalTaper = 1000.0;
	if (toolTip->toolType == KNIFE)
	{
		totalTaper = 1000.0;	// 1/10 the amount, 1000
	}
	else if (toolTip->toolType == CUTTER)
	{
		totalTaper = 4000.0;	
	}
	else if (toolTip->toolType == ROD || toolTip->toolType == RING)
	{
		totalTaper = 100.0;		// 1/5 the amount, 500.0
	}

	if (toolTipOn)
	{
		toolTip->source->SetTaper(value / totalTaper);
		toolTip->source->Update();
		toolTip->transformFilter->SetTransform(makeCompositeTransformFromSinglePoint(*toolTip));
		toolTip->transformFilter->Update();

		//if (toolTip->outline)
//			toolTip->

	}
	else
	{
		int i = myelems.size() - 1;
		myelems[i]->source->SetTaper(value / totalTaper);
		myelems[i]->source->Update();
		myelems[i]->transformFilter->Update();		// Must update transformfilter for transforms to show
	}
	renderWindow->Render();
}
//--------------------------------------------------------------------------------------
void aperio::slot_phiSlider(int value)
{
	if (myelems.empty())
		return;

	auto toolTip = this->toolTip.lock();
	if (!toolTip)
	{
		// If no tooltip active, then use the last planted widget (last elem in widgets vector)
		toolTip = myelems.back();
	}

	if (toolTipOn)
	{
		toolTip->source->SetPhiRoundness(value / roundnessScale);
		toolTip->source->Update();
		toolTip->transformFilter->SetTransform(makeCompositeTransformFromSinglePoint(*toolTip));
		toolTip->transformFilter->Update();
	}
	else
	{
		int i = myelems.size() - 1;
		myelems[i]->source->SetPhiRoundness(value / roundnessScale);
		myelems[i]->source->Update();
		myelems[i]->transformFilter->Update();		// Must update transformfilter for transforms to show
	}
	renderWindow->Render();
}
//--------------------------------------------------------------------------------------
void aperio::slot_thicknessSlider(int value)
{
	if (myelems.empty())
		return;

	auto toolTip = this->toolTip.lock();
	if (!toolTip)
	{
		// If no tooltip active, then use the last planted widget (last elem in widgets vector)
		toolTip = myelems.back();
	}

	if (toolTipOn)
	{
		toolTip->source->SetThickness(value / thicknessScale);
		toolTip->source->Update();
		toolTip->transformFilter->SetTransform(makeCompositeTransformFromSinglePoint(*toolTip));
		toolTip->transformFilter->Update();
	}
	else
	{
		int i = myelems.size() - 1;
		myelems[i]->source->SetThickness(value / thicknessScale);	// 0..1
		myelems[i]->source->Update();
		myelems[i]->transformFilter->Update();		// Must update transformfilter for transforms to show
	}
	renderWindow->Render();
}
//---------------------------------------------------------------------------
void aperio::slot_btnInvert()
{
	inverseTrigger = true;
	slot_btnPlant();
	inverseTrigger = false;

	ui.ringSlider->setValue(0);
}
//---------------------------------------------------------------------------
void aperio::slot_btnBackground()
{
	texturedbackground = !texturedbackground;

	if (texturedbackground)
	{
		renderer->SetBackgroundTexture(texture);
		renderer->SetTexturedBackground(true);
	}
	else
	{
		renderer->SetBackground(0.99, 1, 1);
		renderer->SetTexturedBackground(false);

		/*renderer->SetBackground(1, 1, 0.5);
		renderer->SetTexturedBackground(false);*/
	}
}
//---------------------------------------------------------------------------
void aperio::slot_btnHinge()
{
	setCurrentToolTipType(HINGE);

	auto toolTip = this->toolTip.lock();
	if (!toolTip)
		return;

	// Transform element into new type
	toolTip->toolType = getCurrentToolTipType();

	updateToroidal();

	renderWindow->Render();
}
//--------------------------------------------------------------------------------------
void aperio::slot_btnRing()
{
	setCurrentToolTipType(RING);

	auto toolTip = this->toolTip.lock();

	makeToolTipIfDoesntExist();
	if (!toolTip)
		return;

	// Transform element into new type
	toolTip->toolType = getCurrentToolTipType();

	updateToroidal();

	renderWindow->Render();

	/*
	timer_explode->start();

	if (ui.explodeSlider->value() < (ui.explodeSlider->maximum() / 2.0))
		explode_out = true;
	else
		explode_out = false;
		*/
}

void aperio::slot_btnRod()
{
	setCurrentToolTipType(ROD);

	auto toolTip = this->toolTip.lock();

	makeToolTipIfDoesntExist();

	if (!toolTip)
		return;

	// Transform element into new type
	toolTip->toolType = getCurrentToolTipType();

	updateToroidal();

	renderWindow->Render();
}

// Timers
/*void aperio::slot_timer_explode()
{
	float delta = 10;
	if (!explode_out)
		delta = -delta;

	ui.explodeSlider->setValue(ui.explodeSlider->value() + delta);

	if (
		(explode_out && ui.explodeSlider->value() >= ui.explodeSlider->maximum()) ||
		(!explode_out && ui.explodeSlider->value() <= 0)
		)
		timer_explode->stop();
}*/
void aperio::slot_btnPickUp()
{
	setWidgetSelectMode(!widgetSelectMode);
}

//--------------------------------------------------------------------------------------
void aperio::slot_btnPlant()
{
	auto toolTip = this->toolTip.lock();
	if (!toolTip)
	{
		if (myelems.empty())
			return;
		else
		{
			this->toolTip = myelems.back();
			toolTip = this->toolTip.lock();
		}
	}

	if (inverseTrigger)
		toolTip->inverse = !toolTip->inverse;
		

	if (toolTip->toolType == CUTTER)
	{
	}
	if (toolTip->toolType == RING || toolTip->toolType == ROD)
	{
		createPath();
	}

	// Plant cutter - move elem to the end of vector (and set active toolTip back to nullptr)

	auto elem = this->toolTip.lock();	
	auto tomove = getElemIterator(elem);

	myelems.erase(tomove);
	myelems.push_back(elem);	// Move to end of vector = most recently planted

	this->toolTip.reset();
	this->toolTipOn = false;

	setCursor(true);

	if (toolTip->toolType == KNIFE)
	{
		slot_btnCut();
	}
}
//-------------------------------------------------------------------------------------
void aperio::slot_btnCut()
{
	if (myelems.empty())
		return;

	auto elem = myelems.back();

	if (elem->toolType != CUTTER && elem->toolType != KNIFE)
		return;

	QMessageBox mb(this);
	mb.setStyleSheet("color: white; background: black; selection-color: black;");
	mb.setWindowOpacity(0.9);
	mb.setWindowTitle(this->windowTitle());
	mb.setText("Are you sure you want to cut more than 3 pieces? (This can be slow and error-prone)");
	mb.setIcon(QMessageBox::Question);

	QPushButton *yesButton = mb.addButton(tr("Yes"), QMessageBox::ActionRole);
	QPushButton *noButton = mb.addButton(tr("Cancel"), QMessageBox::ActionRole);
	//noButton->setFocus();

	yesButton->setStyleSheet(this->styleSheet());
	noButton->setStyleSheet(this->styleSheet());

	if (selectedMeshes.size() >= 3)
	{
		mb.exec();
		if (mb.clickedButton() == yesButton)
			slice();
	}
	else
	{
		slice();
	}
}
//--------------------------------------------------------------------------------------
void aperio::slot_btnCookie()
{
	setCurrentToolTipType(CUTTER);

	auto toolTip = this->toolTip.lock();

	makeToolTipIfDoesntExist();

	if (!toolTip)
		return;

	// Transform element into new type
	setCursor(false);

	toolTip->toolType = getCurrentToolTipType();

	updateToroidal();

	renderWindow->Render();
}
//--------------------------------------------------------------------------------------
void aperio::slot_btnKnife()
{
	setCurrentToolTipType(KNIFE);

	auto toolTip = this->toolTip.lock();

	makeToolTipIfDoesntExist();

	if (!toolTip)
		return;

	// Transform element into new type
	toolTip->toolType = getCurrentToolTipType();

	updateToroidal();

	renderWindow->Render();
}
//---------------------------------------------------------------------------------
void aperio::slot_btnRestore()
{
	// Make sure there are parents to restore, and a mesh is selected
	if (parentMeshes.size() == 0 || selectedMeshes.size() == 0)
		return;

	for (auto &selectedMesh_wk : selectedMeshes)
	{
		auto selectedMesh = selectedMesh_wk.lock();
		slot_btnRestoreInternal(selectedMesh);
	}

	//auto selectedMesh = selectedMeshes.back().lock();
	
}
//------------------------------------------------------------------------------------------------
void aperio::slot_btnRestoreInternal(shared_ptr<CustomMesh> selectedMesh)
{
	// Find all children with the same parent as SelectedMesh's parent 
	if (selectedMesh == nullptr)
		return;

	// Return if the selectedMesh IS already a parent mesh
	if (selectedMesh->parentMesh.lock() == nullptr)
		return;

	// Delete all meshes with same root parent (in meshes vector)
	vector<weak_ptr<CustomMesh> > todelete;
	for (auto &mesh : meshes)
	{
		if (mesh->parentMesh.lock() == selectedMesh->parentMesh.lock())
		{
			todelete.push_back(mesh);
		}
	}
	for (auto &del : todelete)
	{
		Utility::removeMesh(this, del.lock());
	}

	// Delete all sub-parent meshes with same root parent (in parentMeshes vector)
	todelete.clear();
	for (auto & parent : parentMeshes)
	{
		if (parent->parentMesh.lock() == selectedMesh->parentMesh.lock())
		{
			todelete.push_back(parent);
		}
	}
	for (auto & del : todelete)
	{
		removeParentMesh(del);
	}

	// Find parent mesh and restore into meshes (also removes the root parent from parentMeshes)
	transferToMeshes(selectedMesh->parentMesh.lock());

	clearSelectedMeshes();
}

//------------------------------------------------------------------------------------
void aperio::slot_chkFrontRibbons(bool checked)
{
	if (myelems.empty())
		return;

	auto toolTip = this->toolTip.lock();
	if (!toolTip)
	{
		// If no tooltip active, then use the last planted widget (last elem in widgets vector)
		toolTip = myelems.back();
	}

	toolTip->frontRibbons = !toolTip->frontRibbons;
}
//------------------------------------------------------------------------------------
void aperio::slot_btnRibbons()
{
	if (myelems.empty())
		return;

	QString non_highlighted = "QPushButton{ background: rgba(86, 80, 72, 150); " + style_button_default;
	QString highlighted = "QPushButton{ background: rgba(255,255,175,200); " + style_button_default;

	auto toolTip = this->toolTip.lock();
	if (!toolTip)
	{
		// If no tooltip active, then use the last planted widget (last elem in widgets vector)
		toolTip = myelems.back();
	}

	toolTip->ribbons = !toolTip->ribbons;

	if (toolTip->ribbons)
		ui.btnRibbons->setStyleSheet(highlighted);
	else
		ui.btnRibbons->setStyleSheet(non_highlighted);

	renderWindow->Render();

}
//------------------------------------------------------------------------------------
void aperio::slot_ribbonwidthSlider(int value)
{
	if (myelems.empty())
		return;

	auto toolTip = this->toolTip.lock();
	if (!toolTip)
	{
		// If no tooltip active, then use the last planted widget (last elem in widgets vector)
		toolTip = myelems.back();
	}

	float maxwidth = 1.0;

	toolTip->ribbonWidth = (value / 100.0) * maxwidth;
}
//------------------------------------------------------------------------------------
void aperio::slot_ribbonfreqSlider(int value)
{
	if (myelems.empty())
		return;

	auto toolTip = this->toolTip.lock();
	if (!toolTip)
	{
		// If no tooltip active, then use the last planted widget (last elem in widgets vector)
		toolTip = myelems.back();
	}

	float maxfreq = 25.0;

	toolTip->ribbonFrequency = (value / 100.0) * maxfreq;
}
//------------------------------------------------------------------------------------
void aperio::slot_ribbontiltSlider(int value)
{
	if (myelems.empty())
		return;

	auto toolTip = this->toolTip.lock();
	if (!toolTip)
	{
		// If no tooltip active, then use the last planted widget (last elem in widgets vector)
		toolTip = myelems.back();
	}

	float percent = value / 100.0;
	float range = (percent - 0.5) * 1.25;	// Make it between (-0.5...0.5) * scale

	toolTip->ribbonTilt = range;
}
//------------------------------------------------------------------------------------
weak_ptr<CustomMesh> tempSelectedMesh;

//-------------------------------------------------------------------------------------
void aperio::slot_listitemclicked(QListWidgetItem *item)
{
	//if (i == -1)
	//		return;

	// Do the temporary highlight
	if (timer_highlight->isActive())
		Utility::setMeshOpacity(this, tempSelectedMesh, 1.0);

	timer_highlight->start();
	timer_highlight_start = clock();

	string itemString = item->text().toStdString();	// get new selectedMesh string from list
	tempSelectedMesh = getMeshByName(itemString);

	// Scroll to selected item
	setListItemSelected(item);
}
//-------------------------------------------------------------------------------------
void aperio::slot_listitemrightclicked(const QPoint & pos)
{
	auto listWidget = ui.listWidget;
	if (listWidget->selectedItems().count() == 0)
		return;

	auto item = listWidget->selectedItems().first();

	// Remove selected item from list
	string itemString = item->text().toStdString();
	auto it = getMeshByName(itemString);

	removeSelectedMesh(it);
}
//-------------------------------------------------------------------------------------
void aperio::slot_listitemdoubleclicked(QListWidgetItem *item)
{
	string itemString = item->text().toStdString();
	auto it = getMeshByName(itemString);

	addSelectedMesh(it);
}
static clock_t highlight_time = 0;
//-------------------------------------------------------------------------------------
void aperio::slot_timer_highlight()
{
	float duration = 600.0;
	float elapsed = clock() - timer_highlight_start;

	if (elapsed >= duration)
	{
		elapsed = duration;

		Utility::setMeshOpacity(this, tempSelectedMesh, 1.0);

		timer_highlight->stop();
		
		return;
	}
	
	float percent = elapsed / duration;

	Utility::setMeshOpacity(this, tempSelectedMesh, percent);
}

//-------------------------------------------------------------------------------------------
void aperio::slot_timeout_fps()
{
	if (this->isActiveWindow())
	{
		if (GetAsyncKeyState(VK_ESCAPE))
			this->close();

		if (GetKeyState(VK_F2) & 0x1000)
		{
			ui.txtSearchText->setFocus();
			string file = ui.txtSearchText->text().toStdString();
			loadMatCapTexture(file);
		}

		//if (GetAsyncKeyState(VK_CONTROL))
		//		wiggle = false;
	}

	// Increment wave-time in seconds for wiggle
	wavetime = wavetime + 0.01275;
	if (wavetime > 500)
		wavetime = 0;

	// ---- Control wiggle
	if (ui.chkWiggle->isChecked())
		wiggle = true;
	else
		wiggle = false;

	if (!pause)
	{
		if (realtimeupdate)
			qv->update();
		else
		{
			if (this->isActiveWindow() || colorDialog->isActiveWindow())
				qv->update();
		}
	}
}
//-------------------------------------------------------------------------------------
void aperio::slot_chkDepthPeel(bool checked)
{
}
//-------------------------------------------------------------------------------------
void aperio::slot_chkSnapReal(bool checked)
{
	snapToBBOXReal = !snapToBBOXReal;
}
//-------------------------------------------------------------------------------------
void aperio::slot_chkSnap(bool checked)
{
	snapToBBOX = !snapToBBOX;
}
//-------------------------------------------------------------------------------------
void aperio::slot_chkCap(bool checked)
{
	cap = !cap;
}
static int lastringvalue;

//----------------------------------------------------------------------------------------------
void aperio::slot_ringSlider(int value)
{
	explodeSlide(value);
}
//----------------------------------------------------------------------------------------------
void aperio::slot_rodSlider(int value)
{
	explodeSlide(ui.ringSlider->value(), value);
}
//----------------------------------------------------------------------------------------------
void aperio::slot_hingeSlider(int value)
{
	return;

	if (selectedMeshes.size() == 0)
		return;

	auto selectedMesh = selectedMeshes.back().lock();

	if (selectedMesh == nullptr || !selectedMesh->generated)	//	Make sure selected & generated mesh (Rather than original mesh)
		return;

	float angle = (value / 100.0) * ui.txtHingeAmount->text().toInt();

	selectedMesh->hingeAngle = angle;
	selectedMesh->hingeAmount = ui.txtHingeAmount->text().toInt();

	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply();
	transform->Translate( (-1 * selectedMesh->hingePivot).GetData() );
	transform->RotateWXYZ(angle, selectedMesh->sforward.GetData());
	transform->Translate(selectedMesh->hingePivot.GetData());


	selectedMesh->actor->SetUserTransform(transform);

	// Update highlight
	//interactorstyle->HighlightProp3D(selectedMesh->actor);

	QApplication::processEvents();
	//}
}
//-------------------------------------------------------------------------------
void aperio::slot_btnHide()
{
	if (selectedMeshes.size() == 0)
		return;

	// If some selected meshes hidden and some shown, just make them all same (all hidden/shown)
	bool allsame = true;
	int lastopacity = selectedMeshes.back().lock()->opacity;

	for (auto &selectedMesh_wk : selectedMeshes)
	{
		auto selectedMesh = selectedMesh_wk.lock();

		if (selectedMesh->opacity != lastopacity)
		{ 
			allsame = false;
			break;
		}			
	}

	if (!allsame)
	{
		float newopacity;
		if (lastopacity == 0)
			newopacity = 100;
		else
			newopacity = 0;

		for (auto &selectedMesh_wk : selectedMeshes)
		{
			auto selectedMesh = selectedMesh_wk.lock();
			Utility::setMeshOpacity(this, selectedMesh, newopacity);			
		}
	}
	else
	{
		for (auto &selectedMesh_wk : selectedMeshes)
		{
			auto selectedMesh = selectedMesh_wk.lock();

			float newopacity;

			if (selectedMesh->opacity == 0)
				newopacity = 100;
			else
				newopacity = 0;
			Utility::setMeshOpacity(this, selectedMesh, newopacity);
		}
	}
	updateOpacitySliderAndList();
}
//-------------------------------------------------------------------------------------
void aperio::slot_txtHingeAmount(const QString &string)
{
	if (selectedMeshes.size() == 0)
		return;

	auto selectedMesh = selectedMeshes.back().lock();

	if (selectedMesh == nullptr || !selectedMesh->generated)	//	Make sure selected & generated mesh (Rather than original mesh)
		return;

	selectedMesh->hingeAmount = ui.txtHingeAmount->text().toInt();
}

#pragma endregion
// *******************************************************************************

void aperio::makeToolTipIfDoesntExist()
{
	toolTipOn = true;

	if (toolTipOn)
	{
		// Turn on tooltip (all elems visible)
		for (auto &elem : myelems)
			elem->actor->VisibilityOn();

		setCursor(false);

		if (toolTip.lock() == nullptr) // Not holding any tooltip (make new one)
		{
			/*cellPicker->Pick(this->Interactor->GetEventPosition()[0],
			this->Interactor->GetEventPosition()[1],
			0,  // always zero.
			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

			vtkSmartPointer<vtkActor> actorPicked = cellPicker->GetActor();
			if (actorPicked)
			{
			a->pos1[0] = a->pos2[0] = cellPicker->GetPickPosition()[0];
			a->pos1[1] = a->pos2[1] = cellPicker->GetPickPosition()[1];
			a->pos1[2] = a->pos2[2] = cellPicker->GetPickPosition()[2];
			a->norm1[0] = a->norm2[0] = cellPicker->GetPickNormal()[0];
			a->norm1[1] = a->norm2[1] = cellPicker->GetPickNormal()[1];
			a->norm1[2] = a->norm2[2] = cellPicker->GetPickNormal()[2];
			}
			else
			{
			double viewNormal[4], focalPoint[4];
			vtkCamera *camera = a->renderer->GetActiveCamera();
			camera->GetViewPlaneNormal(viewNormal);
			camera->GetFocalPoint(focalPoint);
			a->pos1[0] = focalPoint[0];
			a->pos1[1] = focalPoint[1];
			a->pos1[2] = focalPoint[2];
			a->norm1[0] = viewNormal[0];
			a->norm1[1] = viewNormal[1];
			a->norm1[2] = viewNormal[2];
			}*/
			createtoolTipElement();

			auto toolTip = this->toolTip.lock();

			// Reset scales (Don't reset knife and cutter - likely turning tools into each other)
			if (toolTip->toolType == RING)
			{
				toolTip->scale = vtkVector3f(1.0, 1.5, 1.0);
			}
			else if (toolTip->toolType == ROD)
			{
				toolTip->scale = vtkVector3f(DEFAULT_RODSIZE, 1.0, DEFAULT_RODSIZE);
			}
		}
		else
		{
			// Turn on tooltip but already holding tooltip
		}
	}
	else
	{
		// Turn off tooltip (all elems invisible)
		for (auto &elem : myelems)
			elem->actor->VisibilityOff();
	}
}
//-------------------------------------------------------------------------------------
void aperio::updateToroidal()
{
	auto toolTip = this->toolTip.lock();
	if (!toolTip)
		return;

	if (toolTip->source != nullptr)
	{
		if (toolTip->toolType == CUTTER || toolTip->toolType == KNIFE)
		{
			makeOutline(toolTip);
		}
		else
		{
			// Delete all outlines (from renderer and from superquad)
			removeOutline(toolTip);
		}

		// Reset for knife
		if (toolTip->toolType == KNIFE)
		{
			toolTip->spinAngle = 0.0;	// Reset angles for Knife
			toolTip->spinFlipped = 0.0;
			toolTip->tilt = 0.0;

			toolTip->source->SetTaper(DEFAULT_TAPER);
			toolTip->source->Update();
		}
		else
		{
			toolTip->source->SetTaper(0.0);
			toolTip->source->Update();
			
			toolTip->scale = vtkVector3f(1.0, 1.0, 1.0);
		}

		// Set toroidal
		if (toolTip->toolType == RING)
		{
			// Make it round in Theta
			toolTip->source->SetThetaRoundness(1.0);
			toolTip->source->SetPhiRoundness(0.5);
			//toolTip->source->SetThetaResolution(64);
			//toolTip->source->SetPhiResolution(64);

			

			toolTip->source->SetToroidal(true);
			toolTip->source->Update();
		}
		else
		{
			toolTip->source->SetToroidal(false);
			toolTip->source->Update();
		}

		if (toolTip->actor != nullptr)
		{
			// Set opacity
			if (toolTip->toolType == RING || toolTip->toolType == ROD)
				toolTip->actor->GetProperty()->SetOpacity(1.0);
			else
				toolTip->actor->GetProperty()->SetOpacity(0.2);

			
			// Reset scales (Don't reset knife and cutter - likely turning tools into each other)
			if (toolTip->toolType == RING)
			{
				toolTip->scale = vtkVector3f(1.0, 1.5, 1.0);
			}
			else if (toolTip->toolType == ROD)
			{
				toolTip->scale = vtkVector3f(DEFAULT_RODSIZE, 1.0, DEFAULT_RODSIZE);
			}
		}
	}

	// Also update button highlights
	updateButtons(toolTip->toolType);
}
//-------------------------------------------------------------------------------------
void aperio::updateOpacitySliderAndList()
{
	// Update check states in listbox
	for (int i = 0; i < ui.listWidget->count(); i++)
	{
		QListWidgetItem *item = ui.listWidget->item(i);
		item->setCheckState(Qt::Unchecked);
	}

	for (auto &selectedMesh : selectedMeshes)
	{
		auto actualSelectedMesh = selectedMesh.lock();

		if (actualSelectedMesh != nullptr)
		{
			auto item = getListItemByName(actualSelectedMesh->name);
			
			if (item)
				item->setCheckState(Qt::Checked);
		}
	}

	if (selectedMeshes.size() == 0)
		return;

	auto lastSelectedMesh = selectedMeshes.back().lock();

	if (lastSelectedMesh != nullptr)
	{
		ui.opacitySlider->setValue(lastSelectedMesh->opacity * 100);

		//-- Update other sliders too (get hinging amount)
		if (lastSelectedMesh->generated)
		{
			float hingeAmount = lastSelectedMesh->hingeAmount;
			float hingeAngle = lastSelectedMesh->hingeAngle;

			ui.txtHingeAmount->setText(QString::number(hingeAmount));
			ui.hingeSlider->setValue((hingeAngle / hingeAmount) * 100.0);
		}

		// TODO
		/*if (lastSelectedMesh->path && lastSelectedMesh->pathType == RING)
		{
			float percent = lastSelectedMesh->path_pt / (float)lastSelectedMesh->path->GetNumberOfPoints();
			int num=  percent * 100;
			std::cout << num << "\n";
		}*/

		vtkColor3f mycol = lastSelectedMesh->color;
		colorDialog->setCurrentColor(QColor(mycol.GetRed() * 255, mycol.GetGreen() * 255, mycol.GetBlue() * 255));

		//-- Select item in list widget

		auto item = getListItemByName(lastSelectedMesh->name);
		if (item)
			setListItemSelected(item);			
	}
}
//--------------------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> aperio::makeCompositeTransform(MyElem &elem,
	bool customScale, float customScaleX, float customScaleY, float customScaleZ)
{
	// Get forward/up/right vectors (to orient superquad)
	vtkVector3f up = (elem.p1.normal + elem.p2.normal) * 0.5f;
	up.Normalize();	// forward
	elem.up = up;

	vtkVector3f right = elem.p2.point - elem.p1.point;
	right.Normalize();
	elem.right = right;

	// up = cross product of right and forward
	vtkVector3f forward = right.Cross(up);	//	up
	forward.Normalize();
	elem.forward = forward;

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
		elem.scale.GetX(), 0, 0, 0,
		0, elem.scale.GetY(), 0, 0,
		0, 0, elem.scale.GetZ(), 0,
		0, 0, 0, 1
	};

	if (customScale)
	{
		elements1[0] = customScaleX;
		elements1[5] = customScaleY;
		elements1[10] = customScaleZ;
	}

	float s = sin(elem.spinAngle);
	float c = cos(elem.spinAngle);
	float oc = 1.0 - c;

	double elementsspin[16] = {
		oc * up.GetX() * up.GetX() + c, oc * up.GetX() * up.GetY() - up.GetZ() * s, oc * up.GetZ() * up.GetX() + up.GetY() * s, 0,
		oc * up.GetX() * up.GetY() + up.GetZ() * s, oc * up.GetY() * up.GetY() + c, oc * up.GetY() * up.GetZ() - up.GetX() * s, 0,
		oc * up.GetZ() * up.GetX() - up.GetY() * s, oc * up.GetY() * up.GetZ() + up.GetX() * s, oc * up.GetZ() * up.GetZ() + c, 0,
		0, 0, 0, 1
	};

	transform->SetMatrix(elements1);
	transform->PostMultiply();
	transform->Concatenate(elements2);
	transform->Concatenate(elementsspin);
	transform->Concatenate(elements3);

	return transform;
}



//--------------------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> aperio::makeCompositeTransformFromSinglePoint(MyElem &elem, 
	shared_ptr<CustomMesh> selected,
	bool customScale, float customScaleX, float customScaleY, float customScaleZ)
{
	double viewup[4];
	vtkCamera *camera = this->renderer->GetActiveCamera();
	camera->GetViewUp(viewup);

	// Get forward/up/right vectors (to orient superquad)
	vtkVector3f up = elem.p1.normal;
	up.Normalize();
	elem.up = up;

	/*
	vtkVector3f cameraUp = vtkVector3f(viewup[0], viewup[1], viewup[2]);

	vtkVector3f right = cameraUp.Cross(up);
	right.Normalize();
	elem.right = right;

	vtkVector3f forward = right.Cross(up);	//	up
	forward.Normalize();
	elem.forward = forward;*/

	vtkVector3f right = vtkVector3f(-viewup[0], -viewup[1], -viewup[2]);
	right.Normalize();
	
	vtkVector3f forward = right.Cross(up);	//	up
	forward.Normalize();
	elem.forward = forward;

	// fix up right
	right = up.Cross(forward);
	right.Normalize();
	elem.right = right;

	// vtk does row-major matrix operations
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

	double elements3[16] = {
		1, 0, 0, elem.p1.point.GetX(),
		0, 1, 0, elem.p1.point.GetY(),
		0, 0, 1, elem.p1.point.GetZ(),
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
		elem.scale.GetX(), 0, 0, 0,
		0, elem.scale.GetY(), 0, 0,
		0, 0, elem.scale.GetZ(), 0,
		0, 0, 0, 1
	};

	if (customScale)
	{
		elements1[0] = customScaleX;
		elements1[5] = customScaleY;
		elements1[10] = customScaleZ;
	}

	float s = sin(elem.spinAngle);
	float c = cos(elem.spinAngle);
	float oc = 1.0 - c;

	double elementsSpin[16] = {
        oc * up.GetX() * up.GetX() + c, oc * up.GetX() * up.GetY() - up.GetZ() * s, oc * up.GetZ() * up.GetX() + up.GetY() * s, 0,
		oc * up.GetX() * up.GetY() + up.GetZ() * s, oc * up.GetY() * up.GetY() + c, oc * up.GetY() * up.GetZ() - up.GetX() * s, 0,
		oc * up.GetZ() * up.GetX() - up.GetY() * s, oc * up.GetY() * up.GetZ() + up.GetX() * s, oc * up.GetZ() * up.GetZ() + c, 0,
		0, 0, 0, 1
    };

	s = sin(elem.spinFlipped);
	c = cos(elem.spinFlipped);
	oc = 1.0 - c;
	double elementsSpinFlipped[16] = {
		oc * up.GetX() * up.GetX() + c, oc * up.GetX() * up.GetY() - up.GetZ() * s, oc * up.GetZ() * up.GetX() + up.GetY() * s, 0,
		oc * up.GetX() * up.GetY() + up.GetZ() * s, oc * up.GetY() * up.GetY() + c, oc * up.GetY() * up.GetZ() - up.GetX() * s, 0,
		oc * up.GetZ() * up.GetX() - up.GetY() * s, oc * up.GetY() * up.GetZ() + up.GetX() * s, oc * up.GetZ() * up.GetZ() + c, 0,
		0, 0, 0, 1
	};

	if (elem.toolType == RING && selected != nullptr)
	{
		// Find the 'spinned' right axis
		double rightAxis[4];
		double axis[4];
		axis[0] = right.GetX(); axis[1] = right.GetY(); axis[2] = right.GetZ(); axis[3] = 1.0;
		vtkMatrix4x4::MultiplyPoint(elementsSpin, axis, rightAxis);
		vtkVector3f spinRight = vtkVector3f((float)rightAxis[0], (float)rightAxis[1], (float)rightAxis[2]);
		spinRight.Normalize();


		double forwardAxis[4];
		axis[0] = forward.GetX(); axis[1] = forward.GetY(); axis[2] = forward.GetZ(); axis[3] = 1.0;
		vtkMatrix4x4::MultiplyPoint(elementsSpin, axis, forwardAxis);
		vtkVector3f spinForward = vtkVector3f((float)forwardAxis[0], (float)forwardAxis[1], (float)forwardAxis[2]);
		spinForward.Normalize();

		axis[0] = spinRight.GetX(); axis[1] = spinRight.GetY(); axis[2] = spinRight.GetZ(); axis[3] = 1.0;
		vtkMatrix4x4::MultiplyPoint(elementsSpinFlipped, axis, rightAxis);
		spinRight = vtkVector3f((float)rightAxis[0], (float)rightAxis[1], (float)rightAxis[2]);
		spinRight.Normalize();

		// Determine closest obb axis (and sign)
		float dotOBB[3];
		uint closestToRight;
		dotOBB[0] = abs(spinRight.Dot(selected->axesOBB[0]));
		dotOBB[1] = abs(spinRight.Dot(selected->axesOBB[1]));
		dotOBB[2] = abs(spinRight.Dot(selected->axesOBB[2]));
		if (dotOBB[0] > dotOBB[1])
		{
			if (dotOBB[0] > dotOBB[2])
				closestToRight = 0;
			else
				closestToRight = 2;
		}
		else
		{
			if (dotOBB[1] > dotOBB[2])
				closestToRight = 1;
			else
				closestToRight = 2;
		}
		// Determine closest obb axis (and sign)
		uint closestToForward;
		dotOBB[0] = abs(spinForward.Dot(selected->axesOBB[0]));
		dotOBB[1] = abs(spinForward.Dot(selected->axesOBB[1]));
		dotOBB[2] = abs(spinForward.Dot(selected->axesOBB[2]));
		if (dotOBB[0] > dotOBB[1])
		{
			if (dotOBB[0] > dotOBB[2])
				closestToForward = 0;
			else
				closestToForward = 2;
		}
		else
		{
			if (dotOBB[1] > dotOBB[2])
				closestToForward = 1;
			else
				closestToForward = 2;
		}
		if (closestToRight == closestToForward)
		{
			closestToRight = closestToForward;
		}
		// Cross product to get the up (forward x right)
		vtkVector3f upOBB = selected->axesOBB[closestToForward].Cross(selected->axesOBB[closestToRight]);
		upOBB.Normalize();
		SetMatrix4x4(elements2, selected->axesOBB[closestToRight], upOBB, selected->axesOBB[closestToForward]);
		elem.right = right = selected->axesOBB[closestToRight];
		elem.forward = forward = selected->axesOBB[closestToForward];
		if (closestToRight == 0 && closestToForward == 1)
			elem.up = up = selected->axesOBB[2];
		else if (closestToRight == 0 && closestToForward == 2)
			elem.up = up = selected->axesOBB[1];
		else if (closestToRight == 1 && closestToForward == 2)
			elem.up = up = selected->axesOBB[0];
		//elem.up = up = upOBB;
		// Set spin angle to 0, tilt to 0 degrees
		SetRotationMatrix4x4(elementsSpin, 0, upOBB);
		elem.spinAngle = 0;
		//vtkMath::RadiansFromDegrees(spinAngle_BBOX);
		//elem.tilt = 0;
		//elem.spinFlipped = 0;
	}

	s = sin(elem.tilt);
	c = cos(elem.tilt);
	oc = 1.0 - c;

	double elementsTilt[16] = {
		oc * right.GetX() * right.GetX() + c, oc * right.GetX() * right.GetY() - right.GetZ() * s, oc * right.GetZ() * right.GetX() + right.GetY() * s, 0,
		oc * right.GetX() * right.GetY() + right.GetZ() * s, oc * right.GetY() * right.GetY() + c, oc * right.GetY() * right.GetZ() - right.GetX() * s, 0,
		oc * right.GetZ() * right.GetX() - right.GetY() * s, oc * right.GetY() * right.GetZ() + right.GetX() * s, oc * right.GetZ() * right.GetZ() + c, 0,
		0, 0, 0, 1
	};
	
	

	transform->SetMatrix(elements1);

	transform->PostMultiply();
	
	transform->Concatenate(elements2);
	
	if (elem.toolType == RING)
	{
		s = sin(elem.tilt + vtkMath::RadiansFromDegrees(90.0)); //1.0;
		c = cos(elem.tilt + vtkMath::RadiansFromDegrees(90.0)); //0.0;
		//s = 1.0;
		//c = 0.0;
		oc = 1.0 - c;

		double elementsTilt[16] = {
			oc * right.GetX() * right.GetX() + c, oc * right.GetX() * right.GetY() - right.GetZ() * s, oc * right.GetZ() * right.GetX() + right.GetY() * s, 0,
			oc * right.GetX() * right.GetY() + right.GetZ() * s, oc * right.GetY() * right.GetY() + c, oc * right.GetY() * right.GetZ() - right.GetX() * s, 0,
			oc * right.GetZ() * right.GetX() - right.GetY() * s, oc * right.GetY() * right.GetZ() + right.GetX() * s, oc * right.GetZ() * right.GetZ() + c, 0,
			0, 0, 0, 1
		};
		transform->Concatenate(elementsSpinFlipped);
		transform->Concatenate(elementsTilt);
	}
	if (elem.toolType == ROD)
	{
		s = sin(elem.tilt);
		c = cos(elem.tilt);
		oc = 1.0 - c;

		double elementsTilt[16] = {
			oc * right.GetX() * right.GetX() + c, oc * right.GetX() * right.GetY() - right.GetZ() * s, oc * right.GetZ() * right.GetX() + right.GetY() * s, 0,
			oc * right.GetX() * right.GetY() + right.GetZ() * s, oc * right.GetY() * right.GetY() + c, oc * right.GetY() * right.GetZ() - right.GetX() * s, 0,
			oc * right.GetZ() * right.GetX() - right.GetY() * s, oc * right.GetY() * right.GetZ() + right.GetX() * s, oc * right.GetZ() * right.GetZ() + c, 0,
			0, 0, 0, 1
		};
		transform->Concatenate(elementsTilt);
	}
	transform->Concatenate(elementsSpin);
	transform->Concatenate(elements3);

	return transform;
}


void aperio::SetMatrix4x4(double *matrix4x4, vtkVector3f v0, vtkVector3f v1, vtkVector3f v2 )
{
	matrix4x4[0] = v0.GetX(); matrix4x4[1] = v1.GetX(); matrix4x4[2] = v2.GetX();
	matrix4x4[4] = v0.GetY(); matrix4x4[5] = v1.GetY(); matrix4x4[6] = v2.GetY();
	matrix4x4[8] = v0.GetZ(); matrix4x4[9] = v1.GetZ(); matrix4x4[10] = v2.GetZ();
}

void aperio::SetRotationMatrix4x4(double *matrix4x4, float angle, vtkVector3f axis)
{
	axis.Normalize();
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;
	
	matrix4x4[0] = oc * axis.GetX() * axis.GetX() + c; 
	matrix4x4[1] = oc * axis.GetX() * axis.GetY() - axis.GetZ() * s;
	matrix4x4[2] = oc * axis.GetZ() * axis.GetX() + axis.GetY() * s;
	matrix4x4[3] = 0;
	matrix4x4[4] = oc * axis.GetX() * axis.GetY() + axis.GetZ() * s;
	matrix4x4[5] = oc * axis.GetY() * axis.GetY() + c;
	matrix4x4[6] = oc * axis.GetY() * axis.GetZ() - axis.GetX() * s;
	matrix4x4[7] = 0;
	matrix4x4[8] = oc * axis.GetZ() * axis.GetX() - axis.GetY() * s;
	matrix4x4[9] = oc * axis.GetY() * axis.GetZ() + axis.GetX() * s;
	matrix4x4[10] = oc * axis.GetZ() * axis.GetZ() + c;
	matrix4x4[11] = 0;
	matrix4x4[12] = 0;
	matrix4x4[13] = 0;
	matrix4x4[14] = 0;
	matrix4x4[15] = 1;
}

//--------------------------------------------------------------------------------------------------------------
void aperio::resetClippingPlane()
{
	interactorstyle->SetAutoAdjustCameraClippingRange(false);
	
	renderer->ResetCameraClippingRange();
	double d[2];
	renderer->GetActiveCamera()->GetClippingRange(d);
	renderer->GetActiveCamera()->SetClippingRange(0.05, 5000);
	//renderer->GetActiveCamera()->SetClippingRange(0.1, 1000);
	
	//renderer->GetActiveCamera()->SetClippingRange(0.05, d[1] + 1000.0);
}


void aperio::createtoolTipElement()
{
	auto toolTip = this->toolTip.lock();

	auto modifyToolTip = [=](weak_ptr<MyElem> t)
	{
		auto toolTip = t.lock();

		toolTip->p1.point = vtkVector3f(this->pos1[0], this->pos1[1], this->pos1[2]);
		toolTip->p1.normal = vtkVector3f(this->norm1[0], this->norm1[1], this->norm1[2]);
		toolTip->p2.point = vtkVector3f(this->pos1[0], this->pos1[1], this->pos1[2]);
		toolTip->p2.normal = vtkVector3f(this->norm1[0], this->norm1[1], this->norm1[2]);

		if (toolTip->toolType == KNIFE)
		{
			toolTip->scale = vtkVector3f(0.1, 0.1, 0.1);
		}
		else
		{
			toolTip->scale = vtkVector3f(1.0, 1.0, 1.0);
		}

		toolTip->spinAngle = 0.0;
		toolTip->spinFlipped = 0.0;
		toolTip->tilt = 0.0;

		vtkSmartPointer<MySuperquadricSource> superquad = vtkSmartPointer<MySuperquadricSource>::New();
		if (toolTip->toolType == RING)
			superquad->SetToroidal(true);
		else
			superquad->SetToroidal(false);

		if (toolTip->toolType == KNIFE)
		{
			superquad->SetTaper(DEFAULT_TAPER);
		}
		else
		{
			superquad->SetTaper(0);
		}			

		if (toolTip->toolType == RING)
		{
			superquad->SetThetaResolution(64);
			superquad->SetPhiResolution(64);
		}
		else
		{
			superquad->SetThetaResolution(32);
			superquad->SetPhiResolution(32);
		}
		superquad->SetSize(0.5);

		superquad->SetPhiRoundness(this->ui.phiSlider->value() / this->roundnessScale);
		if (toolTip->toolType == ROD)
		{
			superquad->SetThetaRoundness(1.0);
			superquad->SetPhiRoundness(0.1);
		}
		else if (toolTip->toolType == RING)
		{
			superquad->SetThetaRoundness(1.0);
			superquad->SetPhiRoundness(0.5);
		}
		else
		{
			superquad->SetThetaRoundness(1.0);// this->ui.thetaSlider->value() / this->roundnessScale);
			superquad->SetPhiRoundness(0.2);// this->ui.phiSlider->value() / this->roundnessScale);
		}
		superquad->SetThickness(0.04);// this->ui.thicknessSlider->value() / this->thicknessScale);
		superquad->Update();

		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter->SetTransform(this->makeCompositeTransformFromSinglePoint(*toolTip));
		transformFilter->SetInputData(superquad->GetOutput());
		transformFilter->Update();

		// Remove old actor from renderer (if exists)
		if (toolTip->actor)
			renderer->RemoveActor(toolTip->actor);

		// TODO: superquad opacity
		if (toolTip->toolType == RING || toolTip->toolType == ROD)
			toolTip->actor = Utility::sourceToActor(this, transformFilter->GetOutput(), 1.0, 1.0, 1.0, 1.0);
		else
			toolTip->actor = Utility::sourceToActor(this, transformFilter->GetOutput(), 1.0, 1.0, 1.0, 0.2);

		toolTip->actor->PickableOff();
		toolTip->actor->SetUserMatrix(vtkSmartPointer<vtkMatrix4x4>::New());
		toolTip->source = superquad;
		toolTip->transformFilter = transformFilter;
	};

	if (toolTip == nullptr)
	{
		// No tooltip active, make a new one (and set as active)
		toolTip = make_shared<MyElem>();

		toolTip->toolType = getCurrentToolTipType();

		modifyToolTip(toolTip);

		// Add to vector and set as active
		addElem(toolTip);
		this->toolTip = toolTip;
	}
	else
	{
		// Tooltip currently active (change tooltip to new type?)
		//cout << "CHANGE TOOL\n";


	}
}




void aperio::createIncisionElement()
{
	auto elem = make_shared<MyElem>();

	auto dist = [](float pos1[3], float pos2[3]) -> float
	{
		return sqrtf(pow(pos2[2] - pos1[2], 2.0f) + pow(pos2[1] - pos1[1], 2.0f) + pow(pos2[0] - pos1[0], 2.0f));
	};
	elem->toolType = KNIFE;
	elem->p1.point = vtkVector3f(this->pos1[0], this->pos1[1], this->pos1[2]);
	elem->p1.normal = vtkVector3f(this->norm1[0], this->norm1[1], this->norm1[2]);
	elem->p2.point = vtkVector3f(this->pos2[0], this->pos2[1], this->pos2[2]);
	elem->p2.normal = vtkVector3f(this->norm2[0], this->norm2[1], this->norm2[2]);
	//float cutDepth = elem->scale.GetY();
	elem->scale = vtkVector3f(dist(this->pos1, this->pos2), 1.0, 1.0);
	elem->spinAngle = 0.0;
	elem->spinFlipped = 0.0;
	elem->tilt = 0.0;

	vtkSmartPointer<MySuperquadricSource> superquad = vtkSmartPointer<MySuperquadricSource>::New();
	superquad->SetToroidal(this->ui.chkToroid->isChecked());
	superquad->SetThetaResolution(16);
	superquad->SetPhiResolution(16);
	superquad->SetSize(0.5);
	superquad->SetPhiRoundness(this->ui.phiSlider->value() / this->roundnessScale);
	superquad->SetThetaRoundness(this->ui.thetaSlider->value() / this->roundnessScale);
	superquad->SetThickness(this->ui.thicknessSlider->value() / this->thicknessScale);
	superquad->Update();

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetTransform(this->makeCompositeTransform(*elem));
	transformFilter->SetInputData(superquad->GetOutput());
	transformFilter->Update();

	// TODO: superquad opacity
	elem->actor = Utility::sourceToActor(this, transformFilter->GetOutput(), 1.0, 1.0, 1.0, 0.2);
	elem->actor->PickableOff();

	elem->actor->SetUserMatrix(vtkSmartPointer<vtkMatrix4x4>::New());
	elem->source = superquad;
	elem->transformFilter = transformFilter;

	addElem(elem);
	this->toolTip = elem;
}
//--------------------------------------------------------------------------
void aperio::addSelectedMesh(weak_ptr<CustomMesh> it)
{
	auto exist = getSelectedMeshIterator(it);
	
	if (exist != selectedMeshes.end())
	{
		selectedMeshes.erase(exist);
	}
	selectedMeshes.push_back(it);

	updateSelectedMeshes();
}
//--------------------------------------------------------------------------
void aperio::removeSelectedMesh(weak_ptr<CustomMesh> it)
{
	auto todelete = getSelectedMeshIterator(it);

	if (todelete != selectedMeshes.end())
		selectedMeshes.erase(todelete);

	updateSelectedMeshes();
}
//--------------------------------------------------------------------------
void aperio::removeSelectedMesh(int index)
{
	if (selectedMeshes.size() > 0)
	{
		selectedMeshes.erase(selectedMeshes.begin() + index);
	}
	updateSelectedMeshes();
}

//--------------------------------------------------------------------------
void aperio::clearSelectedMeshes()
{
	selectedMeshes.clear();

	updateSelectedMeshes();
}
//--------------------------------------------------------------------------
vector<weak_ptr<CustomMesh> >::iterator aperio::getSelectedMeshIterator(weak_ptr<CustomMesh> it)
{
	auto result = find_if(selectedMeshes.begin(), selectedMeshes.end(), [=](weak_ptr<CustomMesh> &c) { return c.lock() == it.lock(); });
	return result;
}

//--------------------------------------------------------------------------
void aperio::updateSelectedMeshes()
{
	// Reset all meshes' selection parameter to false
	for (auto &mesh : meshes)
		mesh->selected = false;

	for (auto &selectedMesh : selectedMeshes)
	{
		auto actualSelectedMesh = selectedMesh.lock();

		if (actualSelectedMesh != nullptr)		
			actualSelectedMesh->selected = true;
	}

	updateOpacitySliderAndList();

	showSelected();
}
//---------------------------------------------------------------------------------
class MyQVTKWidget : QVTKWidget
{
public:
	void forwardKeyPress(QKeyEvent *event)
	{
		// Copied from QVTKWidget class
		mIrenAdapter->ProcessEvent(event, this->mRenWin->GetInteractor());
	}
};
void aperio::keyPressEvent(QKeyEvent * event)
{
	((MyQVTKWidget*)qv)->forwardKeyPress(event);
}
//----------------------------------------------------------------------------
void aperio::slice()
{
	// Exit if no selected meshes
	if (selectedMeshes.empty() || myelems.empty())
		return;

	auto elem = myelems.back();
	if (elem->toolType != CUTTER && elem->toolType != KNIFE)
		return;

	//auto selectedMesh = selectedMeshes.back().lock();
	vector<string> newselectedmeshes;

	for (auto &selectedMesh : selectedMeshes)
	{
		string newpiece = sliceInternal(selectedMesh);

		if (newpiece == "`")	// Error occurred
		{
			break;
		}
		newselectedmeshes.push_back(newpiece);
	}

	// Remove superquadric  (From renderer and myelems, including outline)
	removeElem(elem);

	// Set selected mesh to newly cut
	clearSelectedMeshes();

	for (auto &newpiece : newselectedmeshes)
		addSelectedMesh(getMeshByName(newpiece));

	toolTipOn = false;
	//toolTip.lock()->actor->VisibilityOff();

}
//----------------------------------------------------------------------------
string aperio::sliceInternal(weak_ptr<CustomMesh> selectedMesh_wk)
{
	auto selectedMesh = selectedMesh_wk.lock();

	// Exit if selectedMesh points to nothing or no widgets placed ...
	if (selectedMesh == nullptr)
		return "";

	// Change index for eselected to selected element...
	//int eselectedindex = myelems.size() - 1;
	auto elem = myelems.back();

	//elem->source->SetThetaRoundness(0);
	//elem->source->SetPhiRoundness(0);
	//elem->source->Update();
	//elem->transformFilter->Update();

	vtkSmartPointer<vtkPolyData> meshpoly_r = vtkPolyData::SafeDownCast(selectedMesh->actor->GetMapper()->GetInput());
	vtkSmartPointer<vtkPolyData> meshpoly = CarveConnector::cleanVtkPolyData(meshpoly_r, true);

	vtkSmartPointer<vtkPolyData> elempoly_r = vtkPolyData::SafeDownCast(elem->actor->GetMapper()->GetInput());
	vtkSmartPointer<vtkPolyData> elempoly = CarveConnector::cleanVtkPolyData(elempoly_r, true);

	// Make MeshSet from vtkPolyData

	unique_ptr<carve::mesh::MeshSet<3> > mesh_carve(CarveConnector::vtkPolyDataToMeshSet(meshpoly));
	unique_ptr<carve::mesh::MeshSet<3> > elem_carve(CarveConnector::vtkPolyDataToMeshSet(elempoly));
	//unique_ptr<carve::mesh::MeshSet<3> > second(CarveConnector::makeCube(55, carve::math::Matrix::IDENT()));

	/*if (!first->isClosed())
	{
	cout << "The mesh you are trying to cut is NOT a manifold! (Either not closed, contains degenerate "
	<< "points/lines/faces, duplicate points or edges with more than 2 adjacent faces - non-manifold). Please fix with an external modelling package.";

	// Remove superquadric from Renderer
	renderer->RemoveActor(myelems[eselectedindex].actor);

	// Probably should remove from list as well (myelems)
	myelems.erase(myelems.end() - 1);

	return;
	}*/

	//if (!first->isClosed())
	//{
	//cout << "Not a closed mesh! (not solid) so no edge classification . Using normal classification.\n";
	//totriangulate = false;
	//}

	vtkSmartPointer<vtkPolyData> c_poly;
	vtkSmartPointer<vtkPolyData> d_poly;

	if (elem->toolType == CUTTER) // CUTTER
	{
		unique_ptr<carve::mesh::MeshSet<3> > c(CarveConnector::perform(this, mesh_carve, elem_carve, carve::csg::CSG::A_MINUS_B));
		c_poly = CarveConnector::meshSetToVTKPolyData(c);

		// Create second piece (the cut piece)
		unique_ptr<carve::mesh::MeshSet<3> > d(CarveConnector::perform(this, mesh_carve, elem_carve, carve::csg::CSG::INTERSECTION));
		d_poly = CarveConnector::meshSetToVTKPolyData(d);
	}
	else if (elem->toolType == KNIFE)
	{
		
		unique_ptr<carve::mesh::MeshSet<3> > parts_carve(CarveConnector::perform(this, mesh_carve, elem_carve, carve::csg::CSG::A_MINUS_B));
		vtkSmartPointer<vtkPolyData> parts = CarveConnector::meshSetToVTKPolyData(parts_carve);

		//renderer->AddActor(Utility::sourceToActor(this, parts));

		vtkSmartPointer<vtkPolyDataConnectivityFilter> conn = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
		conn->SetInputData(parts);
		conn->SetExtractionModeToAllRegions();
		conn->ScalarConnectivityOff();
		conn->ColorRegionsOff();
		conn->Update();

		cout << conn->GetNumberOfExtractedRegions() << " regions\n";

		if (conn->GetNumberOfExtractedRegions() == 2)
		{
			// Get the regions
			conn->SetExtractionModeToSpecifiedRegions();

			conn->InitializeSpecifiedRegionList();
			conn->AddSpecifiedRegion(0);
			conn->Update();

			c_poly = vtkSmartPointer<vtkPolyData>::New();
			c_poly->DeepCopy(conn->GetOutput());

			conn->InitializeSpecifiedRegionList();
			conn->AddSpecifiedRegion(1);
			conn->Update();

			d_poly = vtkSmartPointer<vtkPolyData>::New();
			d_poly->DeepCopy(conn->GetOutput()); 
		}
			


/*		vtkSmartPointer<vtkPolyData> obbpoly_r = vtkPolyData::SafeDownCast(selectedMesh->actorOBB->GetMapper()->GetInput());
		vtkSmartPointer<vtkPolyData> obbpoly = obbpoly_r;// CarveConnector::cleanVtkPolyData(obbpoly_r, true);
		unique_ptr<carve::mesh::MeshSet<3> > obb_carve(CarveConnector::vtkPolyDataToMeshSet(obbpoly_r));

		unique_ptr<carve::mesh::MeshSet<3> > parts_carve(CarveConnector::perform(this, obb_carve, elem_carve, carve::csg::CSG::A_MINUS_B));
		vtkSmartPointer<vtkPolyData> parts = CarveConnector::meshSetToVTKPolyData(parts_carve);

		vtkSmartPointer<vtkPolyDataConnectivityFilter> conn = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
		conn->SetInputData(parts);
		conn->SetExtractionModeToAllRegions();
		conn->ScalarConnectivityOff();
		conn->ColorRegionsOff();
		conn->Update();

		renderer->AddActor(Utility::sourceToActor(this, parts));

		cout << conn->GetNumberOfExtractedRegions() << " regions\n";

		if (conn->GetNumberOfExtractedRegions() == 2)
		{			
			// Get the regions
			conn->SetExtractionModeToSpecifiedRegions();

			conn->InitializeSpecifiedRegionList();
			conn->AddSpecifiedRegion(0);
			conn->Update();

			vtkSmartPointer<vtkPolyData> box1_r = vtkSmartPointer<vtkPolyData>::New();
			box1_r->DeepCopy(conn->GetOutput());
			
			conn->InitializeSpecifiedRegionList();
			conn->AddSpecifiedRegion(1);
			conn->Update();

			vtkSmartPointer<vtkPolyData> box2_r = vtkSmartPointer<vtkPolyData>::New();
			box2_r->DeepCopy(conn->GetOutput());

			// Do cutting with two halves of OBBs

			---vtkSmartPointer<vtkFillHolesFilter> fillbox1 = vtkSmartPointer<vtkFillHolesFilter>::New();
			fillbox1->SetInputData(box1_r);
			fillbox1->SetHoleSize(1000.0);
			fillbox1->Update();
			box1_r = fillbox1->GetOutput();


			vtkSmartPointer<vtkFillHolesFilter> fillbox2 = vtkSmartPointer<vtkFillHolesFilter>::New();
			fillbox2->SetInputData(box2_r);
			fillbox2->SetHoleSize(1000.0);
			fillbox2->Update();
			box2_r = fillbox2->GetOutput();

			vtkSmartPointer<vtkPolyData> box1 = CarveConnector::cleanVtkPolyData(box1_r, true);
			vtkSmartPointer<vtkPolyData> box2 = CarveConnector::cleanVtkPolyData(box2_r, true);

			unique_ptr<carve::mesh::MeshSet<3> > box1_carve(CarveConnector::vtkPolyDataToMeshSet(box1));
			unique_ptr<carve::mesh::MeshSet<3> > box2_carve(CarveConnector::vtkPolyDataToMeshSet(box2));

			unique_ptr<carve::mesh::MeshSet<3> > result1(CarveConnector::perform(this, mesh_carve, box1_carve, carve::csg::CSG::INTERSECTION));
			unique_ptr<carve::mesh::MeshSet<3> > result2(CarveConnector::perform(this, mesh_carve, box2_carve, carve::csg::CSG::INTERSECTION));
			
			//vtkSmartPointer<vtkPolyData> themesh = CarveConnector::meshSetToVTKPolyData(mesh_carve);
			//vtkSmartPointer<vtkPolyData> thebox = CarveConnector::meshSetToVTKPolyData(box2_carve);

			//renderer->AddActor(Utility::sourceToActor(this, themesh));

			//auto theboxactor = Utility::sourceToActor(this, thebox);
			//theboxactor->GetProperty()->SetOpacity(1.0);
			//renderer->AddActor(theboxactor);

			c_poly = CarveConnector::meshSetToVTKPolyData(result1);
			d_poly = CarveConnector::meshSetToVTKPolyData(result2);
		}*/



		else
		{			
			QMessageBox msgBox;
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.setText("Mesh was not cut into two halves. Either the knife did not split the mesh into two separate halves or the mesh contains more than 2 connected structures. \n\nPlease try again or use the Cookie Cutter to cut multiple connected structures.");
			msgBox.exec();

			c_poly = parts;
			d_poly = parts;

			return "`";	// Error occurred
		}		
	}

	// Create normals for resulting polydatas
	vtkSmartPointer<vtkPolyData> dataset = Utility::computeNormals(c_poly);
	vtkSmartPointer<vtkPolyData> datasetd = Utility::computeNormals(d_poly);

	// Run through list and see if name with + already exists, while it exists, add another +
	// to generate unique name
	stringstream ss;
	ss << selectedMesh->name << "+";

	while (getListItemByName(ss.str()) != nullptr)
		ss << "+";
	string name = ss.str();

	while (getListItemByName(ss.str()) != nullptr || ss.str() == name)
		ss << "+";
	string name2 = ss.str();

	// Generate new properties
	//float opacity = selectedMesh->opacity >= 1 ? 1 : selectedMesh->opacity * 0.5f;

	vtkColor3f color(selectedMesh->color.GetRed(),
		selectedMesh->color.GetGreen(),
		selectedMesh->color.GetBlue());

	// Add Meshes
	vtkSmartPointer<vtkPolyData> finaldata = vtkSmartPointer<vtkPolyData>::New();
	finaldata->DeepCopy(dataset);

	auto parent = selectedMesh;
	
	// SelectedMesh is NOT the root parent (meaning its parent is NOT nullptr)
	if (parent->parentMesh.lock() != nullptr)
	{
		// Assign parent as the selectedMesh's parent (since THAT is the root parent)
		// - Doing this every cut ensures every mesh's parentMesh is always root parent.
		parent = parent->parentMesh.lock();
	}
	auto mesh0 = Utility::addMesh(this, finaldata, name, color, 1.0, parent, selectedMesh).lock();
	
	/*mesh0->actorOBB->GetProperty()->SetOpacity(0.1);
	mesh0->actorOBB->VisibilityOn();
	renderer->AddActor(mesh0->actorOBB);*/

	color.Set(std::min(color.GetRed() + 0.1, 1.0),
		std::min(color.GetGreen() + 0.1, 1.0),
		std::min(color.GetBlue() + 0.1, 1.0));

	vtkSmartPointer<vtkPolyData> finaldata2 = vtkSmartPointer<vtkPolyData>::New();
	finaldata2->DeepCopy(datasetd);

	// Add second actor (the cut piece) to renderer (as well as to meshes vector)
	auto mesh = Utility::addMesh(this, finaldata2, name2, color, 1.0, parent, selectedMesh).lock();

	mesh->generated = true;

	// Set mesh saved parameters
	mesh->hingeAngle = 0;
	mesh->hingeAmount = 170;

	if (selectedMesh->generated)	// Piece we are cutting is already generated, so reuse snormal
	{
		mesh->snormal = selectedMesh->snormal;
	}
	else	// Generate a new snormal
	{
		mesh->snormal = (elem->p1.normal + elem->p2.normal) * 0.5f;
		mesh->snormal.Normalize();
	}
	// Note: snormal is shared by nested superquadrics because nested squadrics must explode in same direction
	//		 but sup (up vector) is different for nested squadrics b/c they are hinged individually around the
	//		 up vector.
	vtkVector3f up = (elem->p1.normal + elem->p2.normal) * 0.5f;
	up.Normalize();

	vtkVector3f right = elem->p2.point - elem->p1.point;
	right.Normalize();

	mesh->sforward = right.Cross(up);
	mesh->sforward.Normalize();

	// Also set the hinge pivot point
	mesh->hingePivot = elem->p1.point;	

	// Finally Remove old mesh
	//Utility::removeMesh(this, selectedMesh);
	transferToParentMeshes(selectedMesh);

	return name2;
}
//---------------------------------------------------------------------------------
void aperio::transferToParentMeshes(shared_ptr<CustomMesh> mesh)
{
	parentMeshes.push_back(mesh);

	// Remove mesh from list, renderer and meshes vector
	Utility::removeMesh(this, mesh);
}
//---------------------------------------------------------------------------------
void aperio::transferToMeshes(shared_ptr<CustomMesh> parent)
{
	addToList(parent->name);
	meshes.push_back(parent);
	renderer->AddActor(parent->actor);

	// Remove from parentMeshes vector
	removeParentMesh(parent);
}
//---------------------------------------------------------------------------------
void aperio::removeParentMesh(weak_ptr<CustomMesh> it)
{
	auto todelete = find_if(parentMeshes.begin(), parentMeshes.end(), [=](shared_ptr<CustomMesh> &c) 
	{ return c == it.lock(); });

	if (todelete != parentMeshes.end())
		parentMeshes.erase(todelete);
}

void aperio::loadMatCapTexture(string filename)
{
	if (filename.length() <= 4)
		return;

	vtkSmartPointer<vtkImageData> imageData = Utility::getImageData(filename);

	matcap.texture = vtkSmartPointer<vtkTexture>::New();
	matcap.texture->SetInterpolate(true);
	matcap.texture->EdgeClampOn();
	matcap.texture->RepeatOff();
	matcap.texture->SetQualityTo32Bit();

	matcap.texture->SetInputData(imageData);

	// Read input into pixel data
	vtkImageData * input = matcap.texture->GetInput();
	matcap.pointer = (unsigned char *)input->GetScalarPointer();
	matcap.width = input->GetDimensions()[0];
	matcap.height = input->GetDimensions()[1];

	if (glew_available)
	{
		if (!matcap.alreadygenerated)
		{
			glGenTextures(1, &matcap.name);
			matcap.alreadygenerated = true;
		}

		auto texEnvParams = []()
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		};

		glActiveTexture(GL_TEXTURE0 + matcap.unit);
		glBindTexture(GL_TEXTURE_2D, matcap.name);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, matcap.width, matcap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, matcap.pointer);
		glGenerateMipmap(GL_TEXTURE_2D);
		texEnvParams();

		glActiveTexture(GL_TEXTURE0);
	}
}

//------------------------------------------------------------------------
void aperio::addElem(weak_ptr<MyElem> it)
{
	auto actualElem = it.lock();

	if (actualElem == nullptr)
		return;

	// Add to renderer and to myelems vector if NOT already added
	auto exist = find_if(myelems.begin(), myelems.end(), [=](shared_ptr<MyElem> &c) { return c == actualElem; });

	if (exist != myelems.end()) // Already exists in elems, don't add
		return;
	
	// Otherwise, it's new, add it to vector and renderer
	myelems.push_back(actualElem);

	makeOutline(actualElem);

	auto transformCallback = [](vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
	{
		aperio *a = static_cast<aperio *>(clientData);
		vtkTransformPolyDataFilter* trans = vtkTransformPolyDataFilter::SafeDownCast(caller);

		auto elem_it = find_if(a->myelems.begin(), a->myelems.end(), [=](shared_ptr<MyElem> &e)
		{ return e->transformFilter.GetPointer() == trans; });

		if (elem_it != a->myelems.end())
		{
			auto elem = *elem_it;
			a->makeOutline(elem);

			// Make sure source scale parameters are > 0
			float minval = 0.025;
			if (elem->toolType == CUTTER)
				minval = 0;

			if (elem->scale.GetX() < minval)
			{
				elem->scale.SetX(minval);				
			}
			if (elem->scale.GetY() < minval)
			{
				elem->scale.SetY(minval);
			}
			if (elem->scale.GetZ() < minval)
			{
				elem->scale.SetZ(minval);
			}
		}
	};

	auto sourceCallback = [](vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
	{
		aperio *a = static_cast<aperio *>(clientData);
		MySuperquadricSource* source = MySuperquadricSource::SafeDownCast(caller);

		auto elem_it = find_if(a->myelems.begin(), a->myelems.end(), [=](shared_ptr<MyElem> &e)
		{ return e->source.GetPointer() == source; });

		if (elem_it != a->myelems.end())
		{
			auto elem = *elem_it;
			a->makeOutline(elem);
		}
	};

	vtkSmartPointer<vtkCallbackCommand> callback1 = vtkSmartPointer<vtkCallbackCommand>::New();
	callback1->SetClientData(this);
	callback1->SetCallback(transformCallback);

	vtkSmartPointer<vtkCallbackCommand> callback2 = vtkSmartPointer<vtkCallbackCommand>::New();
	callback2->SetClientData(this);
	callback2->SetCallback(sourceCallback);

	actualElem->transformFilter->AddObserver(vtkCommand::ModifiedEvent, callback1);
	actualElem->source->AddObserver(vtkCommand::ModifiedEvent, callback2);



	renderer->AddActor(actualElem->actor);	// Maybe should check if already exist?
}
//------------------------------------------------------------------------
void aperio::removeElem(weak_ptr<MyElem> it)
{
	auto actualElem = it.lock();

	auto todelete = find_if(myelems.begin(), myelems.end(), [=](shared_ptr<MyElem> &c) { return c == actualElem; });

	if (todelete != myelems.end())	// Exists
	{
		// Erase from renderer first
		renderer->RemoveActor(actualElem->actor);
		removeOutline(actualElem);

		// Then erase from vector
		myelems.erase(todelete);
	}
}
//------------------------------------------------------------------------
void aperio::clearElems()
{
	// Clear vector and renderer of elems
	for (auto &elem : myelems)
		renderer->RemoveActor(elem->actor);

	myelems.clear();
}
//----------------------------------------------------------------------------------------------
void aperio::explodeSlide(int value, int leafvalue)
{
	if (selectedMeshes.empty())
		return;

	//int index = 0;

	std::map< shared_ptr<MyElem>, int> sizes;
	std::map< shared_ptr<MyElem>, int> indices;
	sizes.clear();
	indices.clear();

	for (auto &selectedMesh : selectedMeshes)
	{
		auto actualMesh = selectedMesh.lock();
		auto elem = actualMesh->elem.lock();

		if (elem != nullptr)
			sizes[elem] = sizes[elem] + 1;
	}

	for (auto &selectedMesh : selectedMeshes)
	{
		auto actualMesh = selectedMesh.lock();
		auto elem = actualMesh->elem.lock();

		if (elem != nullptr)
		{
			explodeSlideInternal(value, actualMesh, leafvalue, indices[elem], sizes[elem]);
			indices[elem] = indices[elem] + 1;
		}
		
		//index++;
	}
}
//----------------------------------------------------------------------------------------------
void aperio::explodeSlideInternal(int value, weak_ptr<CustomMesh> selectedMesh_wk, int leafvalue, int index, int size)
{
	auto selectedMesh = selectedMesh_wk.lock();
	
	// Make sure elem still exists (not deleted already)
	auto elem = selectedMesh->elem.lock();
	if (!elem)	
		return;

	// If leafing, and currently ring, don't do anything
	if (leafvalue != NO_LEAFING && elem->toolType == RING)	
		return;

	// Make sure path generated for selected mesh
	auto path = elem->path;
	if (!path)	
		return;

	auto pathType = selectedMesh->pathType;

	double percent = (value / 100.0);

	if (elem->toolType == RING)	// slide around 85% of ring
	{
		percent = (value / 100.0) * RING_PERCENT;
	}

	if (index != -1 && ui.chkSpread->checkState() == Qt::Checked)	// Using order to compute Spread
	{
		percent = ((index + 1) / (double)size) * (value / 100.0);
	}

	int path_numpts = path->GetNumberOfPoints();
	int idx = selectedMesh->path_pt;

	vtkSmartPointer<vtkFloatArray> pathNormals = vtkFloatArray::SafeDownCast(path->GetPointData()->GetNormals());

	double pt[3];
	path->GetPoint(idx, pt);
	double nm[3];
	pathNormals->GetTuple(idx, nm);

	int nextIdx = percent * path_numpts;
	if (leafvalue != NO_LEAFING)		// If leafing, don't translate (set new index = old index/position)
		nextIdx = idx;
		 
	if (nextIdx >= path_numpts)
	{
		if (pathType == RING)	// Rings have start == end
			nextIdx = 0;
		else if (pathType == ROD)
			nextIdx = path_numpts - 1;
	}

	double nextpt[3];
	path->GetPoint(nextIdx, nextpt);
	double nextnm[3];
	pathNormals->GetTuple(nextIdx, nextnm);

	double init_pt[3];
	path->GetPoint(0, init_pt);

	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply();
	transform->Translate(-init_pt[0], -init_pt[1], -init_pt[2]);

	// Rotations
	if (leafvalue == NO_LEAFING)
	{
		if (selectedMesh->pathType == RING)
		{
			// Find the angle to change by
			vtkVector3f nm_vec = vtkVector3f(nm[0], nm[1], nm[2]);
			nm_vec.Normalize();
			vtkVector3f nextnm_vec = vtkVector3f(nextnm[0], nextnm[1], nextnm[2]);
			nextnm_vec.Normalize();

			vtkVector3f tangent_vec = vtkVector3f(nextpt[0] - pt[0], nextpt[1] - pt[1], nextpt[2] - pt[2]);
			tangent_vec.Normalize();

			vtkVector3f bitangent_vec = nm_vec.Cross(tangent_vec);
			bitangent_vec.Normalize();

			//float anglechange = vtkMath::DegreesFromRadians(acosf(nm_vec.Dot(nextnm_vec)));

			// Figure out angle given ratio of path completed
			double newangle = (nextIdx / (double)path_numpts) * 360.0;

			if (nextIdx < idx)
				newangle = -newangle;

			cout << newangle << "\n";
			cout << idx << "," << nextIdx << "\n";
			cout << bitangent_vec.GetX() << "\n";

			if (idx == nextIdx)	// Same index, use last angle and bitangent
			{
				transform->RotateWXYZ(selectedMesh->path_angle, selectedMesh->lastBitangent.GetData());
			}
			else	// Different index, update angle and bitangent and rotate with new values
			{
				selectedMesh->path_angle = newangle;
				selectedMesh->lastBitangent = bitangent_vec;

				transform->RotateWXYZ(selectedMesh->path_angle, bitangent_vec.GetData());
			}
				
		
		}
	}
	else	// Leafing
	{
		float leafangle = (leafvalue / 100.0) * 270.0f;

		// percent
		if (ui.chkSpread->checkState() == Qt::Checked)	// Using order to compute Spread
		{
			float leafpercent = ((index + 1) / (double)size) * (leafvalue / 100.0);

			// Specific Leafing/Fanning Parameter
			leafangle = -0.4 * leafpercent * (leafvalue / 100.0) * 360.0f;

			cout << leafpercent << "\n";
		}

		if (selectedMesh->pathType == ROD)
			transform->RotateWXYZ(leafangle, nm);

		selectedMesh->path_leafangle = leafangle;
	}
	transform->Translate(init_pt[0], init_pt[1], init_pt[2]);
	transform->Translate(nextpt[0] - init_pt[0], nextpt[1] - init_pt[1], nextpt[2] - init_pt[2]);

	if (nextIdx == 0 && leafvalue == NO_LEAFING)	// Reset at origin (only if not leafing)
		transform->SetMatrix(vtkSmartPointer<vtkMatrix4x4>::New());

	// Reset at end (only if its a ring since start == end) but NOT for rods
	if (selectedMesh->pathType == RING && nextIdx == path_numpts)
		transform->SetMatrix(vtkSmartPointer<vtkMatrix4x4>::New());

	selectedMesh->actor->SetUserTransform(transform);

	// Finally, update path_pt index to nextIdx (for next time)
	selectedMesh->path_pt = nextIdx;
}
//------------------------------------------------------------------------
void aperio::setCursor(bool release)
{
	if (release)
		qv->setCursor(QCursor(QPixmap(":aperio/hand2.png")));
	else
		qv->setCursor(QCursor(QPixmap(":aperio/hand.png")));

	// If not holding anything
	//a->qv->setCursor(Qt::ClosedHandCursor);
	//qv->setCursor(Qt::OpenHandCursor);
}
//------------------------------------------------------------------------
void aperio::updateButtons(ToolType type)
{
	QString orig_style = "QPushButton{ background: rgba(86, 80, 72, 150); " + style_button_default;
	QString highlighted_style = "QPushButton{ background: rgba(126,150,175,200); " + style_button_default;

	ui.btnCookie->setStyleSheet(orig_style);
	ui.btnHinge->setStyleSheet(orig_style);
	ui.btnKnife->setStyleSheet(orig_style);
	ui.btnRing->setStyleSheet(orig_style);
	ui.btnRod->setStyleSheet(orig_style);

	if (type == CUTTER)
		ui.btnCookie->setStyleSheet(highlighted_style);
	else if (type == HINGE)
		ui.btnHinge->setStyleSheet(highlighted_style);
	else if (type == KNIFE)
		ui.btnKnife->setStyleSheet(highlighted_style);
	else if (type == RING)
		ui.btnRing->setStyleSheet(highlighted_style);
	else if (type == ROD)
		ui.btnRod->setStyleSheet(highlighted_style);
}
//----------------------------------------------------------------------------------------------
void aperio::createPath()
{
	if (selectedMeshes.empty())	// return if no selectedMeshes
		return;

	for (auto &selectedMesh_wk : selectedMeshes)
	{
		auto selectedMesh = selectedMesh_wk.lock();

		createPathInternal(selectedMesh);
		// Path_IDs already set
	}

	// Sort by ID (index in path polydata)
	std::sort(selectedMeshes.begin(), selectedMeshes.end(), [](const weak_ptr<CustomMesh>& lhs, const weak_ptr<CustomMesh>& rhs)
	{
		return lhs.lock()->path_id < rhs.lock()->path_id;
	});

	//showSelected();	
}
//----------------------------------------------------------------------------------------------
void aperio::createPathInternal(weak_ptr<CustomMesh> selectedMesh_wk)
{
	// Always translate mesh to position 0 (at start, since intersection must
	// be done with mesh at reset/rest position for visualization to be preserved
	// a->selectedMesh->actor->SetUserTransform(vtkSmartPointer<vtkTransform>::New());
	// (Actually might not need to) since UserMatrix doesn't affect point positions for polyintersections

	//int i = a->myelems.size() - 1;
	//MyElem &elem = a->myelems.at(i);

	auto toolTip = this->toolTip.lock();

	MyElem *elem = toolTip.get();

	if (elem->source == nullptr)
		return;

	auto selectedMesh = selectedMesh_wk.lock();

	// Make a path (polydata with lines) from points and normals
	auto makePath = [](vtkSmartPointer<vtkPoints> points,
		vtkSmartPointer<vtkFloatArray> points_normals) -> vtkSmartPointer < vtkPolyData > {

		vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
		poly->SetPoints(points);
		poly->SetLines(Utility::generateLinesFromPoints(points));
		poly->GetPointData()->SetNormals(points_normals);

		return poly;
	};

	// Copy a point at index (with normals) from poly into newPoints and newNormals
	auto copyPoint = [](vtkSmartPointer<vtkPolyData> poly, int index,
		vtkSmartPointer<vtkPoints> newPoints, vtkSmartPointer<vtkFloatArray> newNormals)
	{
		double pt[3];
		poly->GetPoints()->GetPoint(index, pt);

		double nm[3];
		poly->GetPointData()->GetNormals()->GetTuple(index, nm);

		newPoints->InsertNextPoint(pt);
		newNormals->InsertNextTuple(nm);
	};

	auto createID = [](vtkSmartPointer<vtkPolyData> path, shared_ptr<CustomMesh> selectedMesh)
	{
		// Create IDs
		vtkSmartPointer<vtkOctreePointLocator> octree = vtkSmartPointer<vtkOctreePointLocator>::New();
		octree->SetDataSet(path);
		octree->BuildLocator();
		octree->Update();

		selectedMesh->path_id = octree->FindClosestPoint(selectedMesh->center);
	};

	// --------------------------------------------------------------- ROD ---------------------------
	if (toolTip->toolType == ROD)
	{

		// Update up vector
		elem->transformFilter->SetTransform(makeCompositeTransformFromSinglePoint(*elem));
		elem->transformFilter->Update();

		// Need to transform up vector by tilt
		auto getRodTransform = [](weak_ptr<MyElem> elem_wk) -> vtkSmartPointer<vtkTransform> {
			auto elem = elem_wk.lock();

			vtkVector3f right = elem->right;
			vtkVector3f up = elem->up;
			vtkVector3f forward = elem->forward;

			float s = sin(elem->tilt);
			float c = cos(elem->tilt);
			float oc = 1.0 - c;

			double elementsTilt[16] = {
				oc * right.GetX() * right.GetX() + c, oc * right.GetX() * right.GetY() - right.GetZ() * s, oc * right.GetZ() * right.GetX() + right.GetY() * s, 0,
				oc * right.GetX() * right.GetY() + right.GetZ() * s, oc * right.GetY() * right.GetY() + c, oc * right.GetY() * right.GetZ() - right.GetX() * s, 0,
				oc * right.GetZ() * right.GetX() - right.GetY() * s, oc * right.GetY() * right.GetZ() + right.GetX() * s, oc * right.GetZ() * right.GetZ() + c, 0,
				0, 0, 0, 1
			};
			vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
			transform->SetMatrix(elementsTilt);
			transform->Update();

			return transform;
		};

		// Total vector is norm. up vector (new one) multiplied by superquad's length; 0.5 is default vtkSuperquadric scale 
		auto superquadlength = elem->scale.GetY();
		vtkSmartPointer<vtkMatrix4x4> rodTransform = getRodTransform(toolTip)->GetMatrix();

		double newup[4] = { elem->up.GetX(), elem->up.GetY(), elem->up.GetZ(), 1};
		rodTransform->MultiplyPoint(newup, newup);
		vtkVector3f newup_vec = vtkVector3f(newup[0], newup[1], newup[2]);

		vtkVector3f total = newup_vec * superquadlength * 0.5;

		/*Utility::plotSphere(this, elem->p1.point.GetX(), elem->p1.point.GetY(), elem->p1.point.GetZ(), 0.05);
		Utility::plotSphere(this, elem->p1.point.GetX() + total.GetX(),
		elem->p1.point.GetY() + total.GetY(),
		elem->p1.point.GetZ() + total.GetZ(),
		0.05);*/

		// Generate 360 points for line (and normals, same as Ring Code)
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

		vtkSmartPointer<vtkFloatArray> points_normals = vtkSmartPointer<vtkFloatArray>::New();
		points_normals->SetNumberOfComponents(3);

		int path_numpts = 360;

		for (int i = 0; i < path_numpts; i++)
		{
			float percent = (i / (float)path_numpts);

			vtkVector3f newPos = elem->p1.point + (total * percent);
			vtkVector3f newNorm = newup_vec;

			points->InsertNextPoint(newPos.GetData());
			points_normals->InsertNextTuple(newNorm.GetData());
		}

		auto firstPath = makePath(points, points_normals);

		elem->path = firstPath;

		createID(elem->path, selectedMesh);

		// Get intersection
		vtkSmartPointer<vtkOBBTree> tree = vtkSmartPointer<vtkOBBTree>::New();
		tree->SetDataSet(vtkPolyData::SafeDownCast(selectedMesh->actor->GetMapper()->GetInput()));
		tree->BuildLocator();

		// Intersect the locator with the line
		vtkVector3f endline = elem->p1.point + total;

		double lineP0[3] = { elem->p1.point.GetX(), elem->p1.point.GetY(), elem->p1.point.GetZ() };
		double lineP1[3] = { endline.GetX(), endline.GetY(), endline.GetZ() };

		vtkSmartPointer<vtkPoints> intersectPoints = vtkSmartPointer<vtkPoints>::New();
		tree->IntersectWithLine(lineP0, lineP1, intersectPoints, nullptr);		

		double center[3];
		vtkIdType id;

		if (intersectPoints->GetNumberOfPoints() > 0)	// Intersection
		{
			std::cout << "intersects";

			intersectPoints->GetPoint(0, center);

			// Find nearest point to center of mass on path
			vtkSmartPointer<vtkOctreePointLocator> octree = vtkSmartPointer<vtkOctreePointLocator>::New();
			octree->SetDataSet(elem->path);
			octree->BuildLocator();

			id = octree->FindClosestPoint(center);

			// Found initial point point (id), Generate new points array with reordered points 
			// from previous point data (normals and points in previous poly)

			// (Transfer every point in poly AFTER id first, THEN transfer ones BEFORE id) 
			// This means id is first point in new points array
			vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
			vtkSmartPointer<vtkFloatArray> newPoints_normals = vtkSmartPointer<vtkFloatArray>::New();
			newPoints_normals->SetNumberOfComponents(3);

			for (int i = id; i < points->GetNumberOfPoints(); i++)
				copyPoint(elem->path, i, newPoints, newPoints_normals);
			for (int i = 0; i < id; i++)
				copyPoint(elem->path, i, newPoints, newPoints_normals);

			//for (int i = 0; i < points->GetNumberOfPoints(); i++)
//				copyPoint(elem->path, i, newPoints, newPoints_normals);

			// Regenerate path (using new points) and Update elem's path and create actor 
			//elem->path = makePath(newPoints, newPoints_normals);
			elem->pathActor = Utility::sourceToActor(this, elem->path, 1, 1, 1, 1);

			//renderer->AddActor(elem->pathActor);
		}
		else	// No intersection
		{			
		}
		//selectedMesh->path = elem->path;	// Reference/connect selectedMesh to path defined by elem
		selectedMesh->path_pt = 0;

		selectedMesh->path_initpt_exact = vtkVector3f(center[0], center[1], center[2]);
		selectedMesh->path_angle = 0;
		selectedMesh->pathType = ROD;

		selectedMesh->elem = toolTip;
		selectedMesh->elem.lock()->path = elem->path;
	}
	// --------------------------------------------------------------- RING ---------------------------
	// TODO
	else if (toolTip->toolType == RING)
	{
		float pi = vtkMath::Pi();
		float step = pi / 180.0;

		double dims[3] = { 0.5, 0.5, 0.5 };

		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

		vtkSmartPointer<vtkFloatArray> points_normals = vtkSmartPointer<vtkFloatArray>::New();
		points_normals->SetNumberOfComponents(3);

		float t = -pi;
		
		if (elem->inverse)
			t = pi;

		for (int i = 0; i < 360; i++)
		{
			if (elem->inverse)
				t -= step;
			else
				t += step;

			double pt[3];
			double nm[3];

			evalSuperquadric(t, 0.0, 0.0, 0.0, elem->source->GetThetaRoundness(),
				elem->source->GetPhiRoundness(), dims, 0.0, pt, nm);

			// Axis of symmetry is Y (evalsuperquad gives it in Z, so must swap Y and Z):
			// Getting Y-Axis Of Symmetry
			double tmp = pt[1];
			pt[1] = pt[2];
			pt[2] = tmp;
			pt[0] = -pt[0];

			tmp = nm[1];
			nm[1] = nm[2];
			nm[2] = tmp;
			nm[0] = -nm[0];
			
			points->InsertNextPoint(pt[0], pt[1], pt[2]);
			points_normals->InsertNextTuple(nm);
		}

		auto firstPath = makePath(points, points_normals);

		// Make composite transform
		vtkSmartPointer<vtkTransform> newtransform;
		
		if (snapToBBOX)
		{
			newtransform = makeCompositeTransformFromSinglePoint(*elem, selectedMesh);

			elem->transformFilter->SetTransform(newtransform);
			elem->transformFilter->Update();	// Update elem's transform
		}
		else
		{
			newtransform = makeCompositeTransformFromSinglePoint(*elem);
		}		
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter->SetInputData(firstPath);
		transformFilter->SetTransform(newtransform);
		transformFilter->Update();

		elem->path = transformFilter->GetOutput();
		createID(elem->path, selectedMesh);

		// Make tube version of path (simplified path for intersection test)
		vtkSmartPointer<vtkTubeFilter> pathTube = vtkSmartPointer<vtkTubeFilter>::New();
		pathTube->SetInputData(elem->path);
		pathTube->SetRadius(0.01); //default is .5
		pathTube->SetNumberOfSides(3);
		pathTube->Update();

		// Get intersection
		vtkSmartPointer<vtkTriangleFilter> firstitem = vtkSmartPointer<vtkTriangleFilter>::New();
		//firstitem->SetInputData(vtkPolyData::SafeDownCast(selectedMesh->actor->GetMapper()->GetInput()));
		firstitem->SetInputData(vtkPolyData::SafeDownCast(selectedMesh->actorOBB->GetMapper()->GetInput()));
		firstitem->Update();

		vtkSmartPointer<vtkTriangleFilter> seconditem = vtkSmartPointer<vtkTriangleFilter>::New();
		seconditem->SetInputData(vtkPolyData::SafeDownCast(pathTube->GetOutput()));
		//seconditem->SetInputData(vtkPolyData::SafeDownCast(elem->transformFilter->GetOutput()));
		seconditem->Update();

		vtkSmartPointer<vtkIntersectionPolyDataFilter> inters = vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
		inters->SetSplitFirstOutput(false);
		inters->SetSplitSecondOutput(false);
		inters->GlobalWarningDisplayOff();	// jacobi error (suppressing)
		inters->SetInputData(0, firstitem->GetOutput());
		inters->SetInputData(1, seconditem->GetOutput());
		inters->Update();
		inters->GlobalWarningDisplayOn();

		vtkSmartPointer<vtkPolyData> intersection = inters->GetOutput();

		double center[3] = {0, 0, 0};
		vtkIdType id;

		if (intersection->GetNumberOfPoints() > 0)	// Intersection
		{
			vtkSmartPointer<vtkCenterOfMass> centerOfMassFilter = vtkSmartPointer<vtkCenterOfMass>::New();
			centerOfMassFilter->SetInputData(inters->GetOutput());
			centerOfMassFilter->SetUseScalarsAsWeights(false);
			centerOfMassFilter->Update();

			centerOfMassFilter->GetCenter(center);

			// Find nearest point to center of mass on path
			vtkSmartPointer<vtkOctreePointLocator> octree = vtkSmartPointer<vtkOctreePointLocator>::New();
			octree->SetDataSet(elem->path);
			octree->BuildLocator();

			id = octree->FindClosestPoint(center);

			// Found initial point point (id), Generate new points array with reordered points 
			// from previous point data (normals and points in previous poly)

			// (Transfer every point in poly AFTER id first, THEN transfer ones BEFORE id) 
			// This means id is first point in new points array
			vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
			vtkSmartPointer<vtkFloatArray> newPoints_normals = vtkSmartPointer<vtkFloatArray>::New();
			newPoints_normals->SetNumberOfComponents(3);

			for (int i = id; i < points->GetNumberOfPoints(); i++)
				copyPoint(elem->path, i, newPoints, newPoints_normals);
			for (int i = 0; i < id; i++)
				copyPoint(elem->path, i, newPoints, newPoints_normals);

			// Regenerate path (using new points) and Update elem's path and create actor 
			elem->path = makePath(newPoints, newPoints_normals);
			elem->pathActor = Utility::sourceToActor(this, elem->path, 1, 1, 1, 1);

			//renderer->AddActor(elem->pathActor);
			cout << "intersects";
		}
		else	// No intersection
		{
			cout << "No intersection\n";

			// Find nearest point to (center of OBB?) on path
			vtkSmartPointer<vtkOctreePointLocator> octree = vtkSmartPointer<vtkOctreePointLocator>::New();
			octree->SetDataSet(elem->path);
			octree->BuildLocator();

			center[0] = selectedMesh->center[0];
			center[1] = selectedMesh->center[1];
			center[2] = selectedMesh->center[2];

			id = octree->FindClosestPoint(center);

			// Found initial point point (id), Generate new points array with reordered points 
			// from previous point data (normals and points in previous poly)

			// (Transfer every point in poly AFTER id first, THEN transfer ones BEFORE id) 
			// This means id is first point in new points array
			vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
			vtkSmartPointer<vtkFloatArray> newPoints_normals = vtkSmartPointer<vtkFloatArray>::New();
			newPoints_normals->SetNumberOfComponents(3);

			for (int i = id; i < points->GetNumberOfPoints(); i++)
				copyPoint(elem->path, i, newPoints, newPoints_normals);
			for (int i = 0; i < id; i++)
				copyPoint(elem->path, i, newPoints, newPoints_normals);

			// Regenerate path (using new points) and Update elem's path and create actor 
			elem->path = makePath(newPoints, newPoints_normals);
			elem->pathActor = Utility::sourceToActor(this, elem->path, 1, 1, 1, 1);
			//selectedMesh->path_initpt_exact = vtkVector3f()
		}

		selectedMesh->path_pt = 0;
		
		// Find nearest point to center of mass on path
		//vtkSmartPointer<vtkOctreePointLocator> octree = vtkSmartPointer<vtkOctreePointLocator>::New();
		//octree->SetDataSet(elem->path);
		//octree->BuildLocator();

		//id = octree->FindClosestPoint(center);
	

		selectedMesh->path_initpt_exact = vtkVector3f(center[0], center[1], center[2]);
		selectedMesh->path_angle = 0;
		selectedMesh->pathType = RING;

		selectedMesh->elem = toolTip;
		selectedMesh->elem.lock()->path = elem->path;
	}
}
//-----------------------------------------------------------------------------------
void aperio::setWidgetSelectMode(bool mode)
{
	// Set to mode
	widgetSelectMode = mode;

	// Update buttons
	QString orig_style = "QPushButton{ background: rgba(86, 80, 72, 150); " + style_button_default;
	QString highlighted_style = "QPushButton{ background: rgba(120,120,175,200); " + style_button_default;

	ui.btnPickUp->setStyleSheet(orig_style);

	if (widgetSelectMode)
	{
		ui.btnPickUp->setStyleSheet(highlighted_style);

		for (auto &elem : myelems)		// Make elems pickable
			elem->actor->PickableOn();
	}
	else
	{
		for (auto &elem : myelems)		// Make elems NOT pickable
			elem->actor->PickableOff();
	}
}
//-------------------------------------------------------------------------------------------
/*void aperio::makeOutline(weak_ptr<MyElem> elem_wk, bool update)
{
	// Make a path (polydata with lines) from points and normals
	auto makePath = [](vtkSmartPointer<vtkPoints> points) -> vtkSmartPointer < vtkPolyData > {

		vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
		poly->SetPoints(points);
		poly->SetLines(Utility::generateLinesFromPoints(points));

		return poly;
	};

	auto elem = elem_wk.lock();

	float pi = vtkMath::Pi();
	float step = pi / 180.0;

	double dims[3] = { 0.5, 0.5, 0.5 };

	if (elem->toolType != CUTTER && elem->toolType != KNIFE)
		return;

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	float t = -pi;
	for (int i = 0; i < 360; i++)
	{
		t += step;

		double pt[3];
		double nm[3];

		evalSuperquadric(t, 0.0, 0.0, 0.0, elem->source->GetThetaRoundness(),
			elem->source->GetPhiRoundness(), dims, 0.0, pt, nm);

		// Axis of symmetry is Y (evalsuperquad gives it in Z, so must swap Y and Z):
		// Getting Y-Axis Of Symmetry
		double tmp = pt[1];
		pt[1] = pt[2];
		pt[2] = tmp;
		pt[0] = -pt[0];

		points->InsertNextPoint(pt[0], pt[1], pt[2]);
	}

	auto firstPath = makePath(points);

	// Make composite transform
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(firstPath);

	if (elem->toolType == CUTTER)
		transformFilter->SetTransform(makeCompositeTransformFromSinglePoint(*elem, true,
			elem->scale.GetX(), 0, elem->scale.GetZ()));
	else if (elem->toolType == KNIFE)
		transformFilter->SetTransform(makeCompositeTransform(*elem, true,
		elem->scale.GetX(), 0, elem->scale.GetZ()));


	transformFilter->Update();

	//auto actor = elem->actor;

	//if (update)
	//{
	if (elem->outline == nullptr)
	{
		elem->outline = Utility::sourceToActor(this, transformFilter->GetOutput());
		renderer->AddActor(elem->outline);
	}
	else
		elem->outline->GetMapper()->SetInputDataObject(transformFilter->GetOutput());
	//}
	//else
	//{
		

	//}

	// Add outline key so shader knows
	vtkSmartPointer<vtkInformation> information = vtkSmartPointer<vtkInformation>::New();
	information->Set(vtkMyBasePass::OUTLINEKEY(), 0);	// dummy value
	elem->outline->SetPropertyKeys(information);
	elem->outline->GetProperty()->SetLineWidth(2.5);



	//elem->outline = elem->actor;
}*/

//-------------------------------------------------------------------------------------------
void aperio::makeOutline(weak_ptr<MyElem> elem_wk, bool update)
{
	auto elem = elem_wk.lock();

	if (elem->toolType != CUTTER && elem->toolType != KNIFE)
		return;

	vtkSmartPointer<MySuperquadricSource> source = vtkSmartPointer<MySuperquadricSource>::New();
	source->SetToroidal(true);
	source->SetThickness(0.01);
	source->SetThetaResolution(elem->source->GetThetaResolution());
	source->SetPhiResolution(elem->source->GetPhiResolution());
	source->SetThetaRoundness(elem->source->GetThetaRoundness());
	source->SetPhiRoundness(elem->source->GetPhiRoundness());

	if (elem->toolType == KNIFE || elem->toolType == CUTTER)
		source->SetTaper(elem->source->GetTaper());
	
	source->Update();

	// Make composite transform
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(source->GetOutput());

	float Y = 15.0;

	if (elem->toolType == CUTTER)
		transformFilter->SetTransform(makeCompositeTransformFromSinglePoint(*elem, nullptr, true,
		elem->scale.GetX(), Y, elem->scale.GetZ()));
	else if (elem->toolType == KNIFE)
		transformFilter->SetTransform(makeCompositeTransform(*elem, true,
		elem->scale.GetX(), Y, elem->scale.GetZ()));

	transformFilter->Update();

	if (elem->outline == nullptr)
	{
		elem->outline = Utility::sourceToActor(this, transformFilter->GetOutput());
		elem->outline->PickableOff();
		renderer->AddActor(elem->outline);
	}
	else
		elem->outline->GetMapper()->SetInputDataObject(transformFilter->GetOutput());

	// Add outline key so shader knows
	vtkSmartPointer<vtkInformation> information = vtkSmartPointer<vtkInformation>::New();
	information->Set(vtkMyBasePass::OUTLINEKEY(), 0);	// dummy value
	elem->outline->SetPropertyKeys(information);
	elem->outline->GetProperty()->SetLineWidth(2.5);
}
//-------------------------------------------------------------------------------------------
void aperio::removeOutline(weak_ptr<MyElem> elem_wk)
{
	// Delete from renderer and set elem's outline pointer to nullptr
	auto elem = elem_wk.lock();

	if (elem->outline)
	{
		renderer->RemoveActor(elem->outline);
		elem->outline = nullptr;
	}
}
//---------------------------------------------------------------------------------
void aperio::showSelected()	// Debug
{
	cout << "\n--- Selected ---\n\n";
	for (auto & selectedMesh : selectedMeshes)
	{
		auto actualSelectedMesh = selectedMesh.lock();

		if (actualSelectedMesh != nullptr)
			cout << actualSelectedMesh->name << "\n";
	}
}


