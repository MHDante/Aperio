#include "stdafx.h"		// Precompiled header files
#include "illustrator.h"

// QT Includes
#include <QLayout>
#include <QDesktopWidget>

// VTK includes
#include <vtkSphereSource.h>

// Custom
#include "tiny_obj_loader.h"
#include "CarveConnector.h"

//#include "CustomDepthPeelingPass.h"
#include "MyInteractorStyle.h"
#include "vtkMyShaderPass.h"
#include "vtkMyProcessingPass.h"
//#include "vtkMyOutlinePass.h"

// More VTK
#include <vtkLightsPass.h>
#include <vtkCameraPass.h>
#include <vtkDefaultPass.h>
#include <vtkSequencePass.h>
#include <vtkRenderPassCollection.h>
#include <vtkGaussianBlurPass.h>
#include <vtkDepthPeelingPass.h>
#include <vtkSobelGradientMagnitudePass.h>

#include <vtkOpaquePass.h>
#include <vtkTranslucentPass.h>
#include <vtkClearZPass.h>

#include <vtkLight.h>

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

using namespace std;
#include "illustrator.h"

//-------------------------------------------------------------------------------------------------------------
illustrator::illustrator(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	QTimer::singleShot(0, this, SLOT(slot_afterShowWindow()));
}
//-------------------------------------------------------------------------------------------------------------
illustrator::~illustrator()
{
}
//-------------------------------------------------------------------------------------------------------------
void illustrator::update_orig_size()
{
	_orig_size.setX(ui.mainWidget->geometry().x());
	_orig_size.setY(ui.mainWidget->geometry().y());
	_orig_size.setWidth(ui.mainWidget->geometry().width());
	_orig_size.setHeight(ui.mainWidget->geometry().height());
}
///---------------------------------------------------------------------------------------
void illustrator::slot_afterShowWindow()
{
	update_orig_size();

	firsttime = true;

	// Set up instance variables
	fps = 50.0;
	std::string fname = "hearttest.obj";
	//std::string fname = "organs brain 250K.obj";

	// QT Variables
	pause = false;
	preview = false;

	// Uniforms
	wiggle = true;
	shadingnum = 0;		// Toon/normal, etc.
	peerInside = 0;
	brushDivide = 15.0;
	myexp = 2;

	difftrans = 1.0;
	shininess = ui.verticalSlider->value();
	darkness = 1.2;

	QApplication::processEvents();

	// Setup other instance variables
	status_label = new QLabel("Ready", this);
	status_label->setStyleSheet("background-color: rgba(0,0,0,0);");
	ui.statusBar->addWidget(status_label);

	QTimer* timer;
	timer = new QTimer(this);
	timer->setInterval(1000.0 / fps);
	timer->setTimerType(Qt::TimerType::PreciseTimer);
	timer->start();

	QTimer* timer2;				// few times a second
	timer2 = new QTimer(this);
	timer2->setInterval(1000.0 / 7.5);
	timer2->start();

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
	renderer->GetActiveCamera()->SetClippingRange(0.01, 1000);

	//float bgcolor[3] = { 39, 50, 67 };
	//float bgcolor[3] = { 109, 92, 73 };
	float bgcolor[3] = { 235, 235, 235 };
	float factor = 0.7;
	bgcolor[0] *= factor; bgcolor[1] *= factor; bgcolor[2] *= factor;
	renderer->SetBackground(bgcolor[0] / 255.0, bgcolor[1] / 255.0, bgcolor[2] / 255.0);

	renderWindow->AddRenderer(renderer);

	// Dynamically create a new QVTKWidget (and set render window to it)
	qv = new QVTKWidget(this);
	qv->SetRenderWindow(renderWindow);

	// Add a grid layout to mainWidget on-form and add QVTKWidget to it
	ui.mainWidget->setLayout(new QGridLayout(ui.mainWidget));
	ui.mainWidget->layout()->setMargin(2);
	ui.mainWidget->layout()->addWidget(qv);

	// Prepare all the rendering passes for vtkMyShaderPass
	vtkSmartPointer<vtkCameraPass> cameraP = vtkSmartPointer<vtkCameraPass>::New();

	vtkSmartPointer<vtkSequencePass> seq = vtkSmartPointer<vtkSequencePass>::New();

	vtkSmartPointer<vtkMyShaderPass> opaqueP = vtkSmartPointer<vtkMyShaderPass>::New();
	opaqueP->initialize(this, ShaderPassType::PASS_OPAQUE);

	vtkSmartPointer<vtkMyShaderPass> transP = vtkSmartPointer<vtkMyShaderPass>::New();
	transP->initialize(this, ShaderPassType::PASS_TRANSLUCENT);

	//vtkSmartPointer<vtkOpaquePass> opaqueP = vtkSmartPointer<vtkOpaquePass>::New();
	//vtkSmartPointer<vtkTranslucentPass> transP = vtkSmartPointer<vtkTranslucentPass>::New();

	//vtkSmartPointer<CustomDepthPeelingPass> peelP = vtkSmartPointer<CustomDepthPeelingPass>::New();
	//peelP->SetMaximumNumberOfPeels(4);
	//peelP->SetOcclusionRatio(0.5);
	//peelP->SetTranslucentPass(transP);	//Peeling pass needs translucent pass

	// Put all passes into a collection then into a sequence
	vtkSmartPointer<vtkRenderPassCollection> passes = vtkSmartPointer<vtkRenderPassCollection>::New();
	//passes->AddItem(vtkSmartPointer<vtkClearZPass>::New());
	//passes->AddItem(vtkSmartPointer<vtkLightsPass>::New());
	passes->AddItem(opaqueP);
	//passes->AddItem(peelP);
	passes->AddItem(transP);
	//passes->AddItem(vtkSmartPointer<vtkDefaultPass>::New());

	seq->SetPasses(passes);
	cameraP->SetDelegatePass(seq);

	// Requires Depth from camera pass
	//vtkSmartPointer<vtkMyProcessingPass> dofP = vtkSmartPointer<vtkMyProcessingPass>::New();
	//dofP->setShaderFile("shader_dof.frag", true);
	//dofP->SetDelegatePass(cameraP);

	// Requires Depth from camera pass
	vtkSmartPointer<vtkMyProcessingPass> ssaoP = vtkSmartPointer<vtkMyProcessingPass>::New();
	ssaoP->setShaderFile("shader_ssao.frag", true);
	ssaoP->SetDelegatePass(cameraP);

	//vtkSmartPointer<vtkMyProcessingPass> dotP = vtkSmartPointer<vtkMyProcessingPass>::New();
	//dotP->setShaderFile("shader_dot.frag", true);
	//dotP->SetDelegatePass(ssaoP);

	vtkSmartPointer<vtkMyProcessingPass> fxaaP = vtkSmartPointer<vtkMyProcessingPass>::New();
	//fxaaP->setShaderFile("shader_fxaa.vert", false);
	fxaaP->setShaderFile("shader_fxaa.frag", true);
	fxaaP->SetDelegatePass(ssaoP);

	vtkSmartPointer<vtkMyProcessingPass> bloomP = vtkSmartPointer<vtkMyProcessingPass>::New();
	bloomP->setShaderFile("shader_bloom.frag", true);
	bloomP->SetDelegatePass(fxaaP);

	vtkOpenGLRenderer::SafeDownCast(renderer.GetPointer())->SetPass(bloomP);

	// Render window interactor
	vtkSmartPointer<QVTKInteractor> renderWindowInteractor = vtkSmartPointer<QVTKInteractor>::New();

	interactorstyle = vtkSmartPointer<MyInteractorStyle>::New();
	interactorstyle->initialize(this);

	renderWindowInteractor->SetInteractorStyle(interactorstyle);
	renderWindowInteractor->Initialize();

	qv->GetRenderWindow()->SetInteractor(renderWindowInteractor);

	QApplication::processEvents();

	// Connect all signals to slots
	connect(ui.btnHello, &QPushButton::clicked, this, &illustrator::slot_buttonclicked);
	connect(ui.btnColor, &QPushButton::clicked, this, &illustrator::slot_colorclicked);

	connect(colorDialog, &QColorDialog::currentColorChanged, this, &illustrator::slot_colorchanged);

	connect(timer, &QTimer::timeout, this, &illustrator::slot_timeout);
	connect(timer2, &QTimer::timeout, this, &illustrator::slot_timeout2);
	connect(ui.actionOpen, &QAction::triggered, this, &illustrator::slot_open);
	connect(ui.actionExit, &QAction::triggered, this, &illustrator::slot_exit);
	
	connect(ui.actionAbout, &QAction::triggered, this, &illustrator::slot_about);
	connect(ui.actionPreview, &QAction::triggered, this, &illustrator::slot_preview);
	connect(ui.actionFull_Screen, &QAction::triggered, this, &illustrator::slot_fullScreen);

	connect(ui.menuFile, &QMenu::aboutToShow, this, &illustrator::slot_menuclick);
	connect(ui.menuHelp, &QMenu::aboutToShow, this, &illustrator::slot_menuclick);

	connect(ui.horizontalSlider, &QSlider::valueChanged, this, &illustrator::slot_valueChanged);
	connect(ui.verticalSlider, &QSlider::valueChanged, this, &illustrator::slot_valueChangedV);
	connect(ui.verticalSlider_2, &QSlider::valueChanged, this, &illustrator::slot_valueChangedV2);

	connect(ui.listWidget, &QListWidget::itemEntered, this, &illustrator::slot_listitementered);
	connect(ui.listWidget, &QListWidget::currentRowChanged, this, &illustrator::slot_listitemclicked);

	readFile(fname);
}
// ------------------------------------------------------------------------
void illustrator::resizeInternal(const QSize &newWindowSize, bool using_preview)
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
void illustrator::resizeEvent(QResizeEvent * event)
{
	resizeInternal(event->size(), false);
}
///---------------------------------------------------------------------------------------
void illustrator::mouseMoveEvent(QMouseEvent *)
{
}
///---------------------------------------------------------------------------------------
void illustrator::readFile(std::string filename)
{
	//vtkObject::GlobalWarningDisplayOff();	// dangerous (keep on for most part)

	// Reset values for new file
	toon = 0;
	hoveredIndex = 0;

	renderer->RemoveAllViewProps();	// Remove from renderer, clear listwidget, clear vectors
	ui.listWidget->clear();
	myelems.clear();
	meshes.clear();

	qDebug() << " - reading file - \n";

	//read jpeg, instant
	vtkSmartPointer<vtkJPEGReader> jpgReader = vtkSmartPointer<vtkJPEGReader>::New();
	jpgReader->SetFileName("metal.jpg");
	jpgReader->Update();

	colorTexture = vtkSmartPointer<vtkTexture>::New();
	colorTexture->SetInputConnection(jpgReader->GetOutputPort());
	colorTexture->InterpolateOn();

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

	// VTK : Create a directional light (default)
	renderer->AutomaticLightCreationOff();
	renderer->RemoveAllLights();

	vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
	light->SetLightTypeToSceneLight();		// Headlights are located at camera position (CameraLights do not have to be)	

	float amb = 0.2; //0.2
	float diff = 0.6; //0.6
	float spec = 1.0; //1.0

	light->SetAmbientColor(amb, amb, amb);
	light->SetDiffuseColor(diff, diff, diff);
	light->SetSpecularColor(spec, spec, spec);

	light->SetPosition(0, 0, 0);	// Light centered at camera origin (view space)
	renderer->AddLight(light);

	//srand(time(nullptr));		//Random Seed

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
		nextMesh->BuildCells();
		nextMesh->BuildLinks();

		std::string groupname = shapes.at(z).name;

		// Remove darkfactor when customizable colours implemented and saveable (1.4f currently in fragment shader)

		// Default random colours
		if (QString::compare(QString(groupname.c_str()), QString("c_pericardium")) == 0 && firsttime)
		{
			r = 0.448148;
			g = 0.514215;
			b = 0.811111;
		}
		if (QString::compare(QString(groupname.c_str()), QString("c_arteriae_coronari")) == 0 && firsttime)
		{
			r = 0.803704;
			g = 0.307407;
			b = 0.744444;
		}
		if (QString::compare(QString(groupname.c_str()), QString("c_venae_coronari")) == 0 && firsttime)
		{
			r = 0.344444;
			g = 0.637037;
			b = 0.844444;
		}
		if (QString::compare(QString(groupname.c_str()), QString("c_venDex")) == 0 && firsttime)
		{
			r = 0.922222;
			g = 0.337037;
			b = 0.618519;
		}
		if (QString::compare(QString(groupname.c_str()), QString("c_vc_venSin")) == 0 && firsttime)
		{
			r = 0.770370;
			g = 0.807407;
			b = 0.762963;
		}
		if (QString::compare(QString(groupname.c_str()), QString("c_atrDex")) == 0 && firsttime)
		{
			r = 0.418519;
			g = 0.662963;
			b = 0.622222;
		}

		if (tinyobj::info::matFileExists)
		{			
			if (z == 0)	// Only print this once
				std::cout << "Material file found. Using colours from .mtl file.\n";

			r = shapes.at(z).material.diffuse[0];
			g = shapes.at(z).material.diffuse[1];
			b = shapes.at(z).material.diffuse[2];
		}

		//printf("c_atrDex %f %f %f\n", r, g, b);
		//vtkSmartPointer<vtkOBBTree> objectOBBTree = vtkSmartPointer<vtkOBBTree>::New();
		//objectOBBTree->SetDataSet(nextMesh);
		//objectOBBTree->BuildLocator();
		//CommonData::objectOBBTrees.push_back(objectOBBTree);

		// Compute normals/toggle on/off
		vtkSmartPointer<vtkPolyDataNormals> dataset = vtkSmartPointer<vtkPolyDataNormals>::New();
		dataset->SetInputData(nextMesh);
		dataset->ComputePointNormalsOn();
		dataset->ComputeCellNormalsOff();
		dataset->SplittingOn();
		dataset->SetFeatureAngle(60);
		dataset->Update();

		//vtkSmartPointer<vtkDepthSortPolyData> depthSort = vtkSmartPointer<vtkDepthSortPolyData>::New();
		//depthSort->SetInputData(dataset->GetOutput());
		//depthSort->SetDirectionToBackToFront();
		//depthSort->SetVector(1, 1, 1);
		//depthSort->SetCamera(renderer->GetActiveCamera());
		//depthSort->SortScalarsOff();
		//depthSort->Update();

		vtkColor3f c(r, g, b);

		Utility::addMesh(this, dataset->GetOutput(), z, groupname, c, 1.0);

		// Make default shaders
		//shaderProgram = Utility::buildShader(renderer->GetRenderWindow(), "shader.vert", "shader.frag");
		shaderProgram = Utility::buildShader(renderer->GetRenderWindow(), "shader_water.vert", "shader.frag");

		// Default uniform variables
		mouse[0] = 1;
		mouse[1] = 1;
		mouse[2] = 1;

		// add actor to renderer
		renderer->AddActor(meshes[z].actor);
		renderer->ResetCamera();
		qv->GetRenderWindow()->Render();
	}

	// Added meshes, now set selectedMesh to last one (Might have to update this for future selections)
	selectedMesh = meshes.end();	// Reset selectedMesh to nothing

	if (firsttime)
		firsttime = false;

	// add mesh groupnames to listbox
	for (int i = 0; i < meshes.size(); i++)
	{
		QListWidgetItem* item = new QListWidgetItem(meshes[i].name.c_str(), ui.listWidget);

		item->setCheckState(Qt::Checked);
		ui.listWidget->addItem(item);
		//ui.listWidget->itemAt(0, i)->setCheckState(Qt::Checked);
	}
}
//-------------------------------------------------------------------------------------
void illustrator::slot_listitemclicked(int i)
{
	if (i == -1)
		return;

	for (int k = 0; k < meshes.size(); k++)
		meshes[k].selected = false;	// Reset all selectedMesh's selection parameter to false

	std::string itemString = ui.listWidget->item(i)->text().toStdString();	// get new selectedMesh string from list
	selectedMesh = getMeshByName(itemString);

	selectedMesh->selected = true;		// Set new selectedMesh's selection parameter to true

	updateOpacitySliderAndList();
}
//-------------------------------------------------------------------------------------
void illustrator::updateOpacitySliderAndList()
{
	if (selectedMesh != meshes.end())
	{
		ui.horizontalSlider->setValue(selectedMesh->opacity * 100);

		ui.listWidget->clearSelection();	// unselect all previous selected items in list

		if (selectedMesh != meshes.end())
		{
			auto item = getListItemByName(selectedMesh->name);
			
			if (item)
			{
				item->setSelected(true);
				ui.listWidget->scrollToItem(item);

				vtkColor3f mycol = selectedMesh->color;
				colorDialog->setCurrentColor(QColor(mycol.GetRed() * 255, mycol.GetGreen() * 255, mycol.GetBlue() * 255));
			}
		}
	}
}