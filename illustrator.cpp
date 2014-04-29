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
	fps = 1000;
	std::string fname = "hearttest.obj";

	// QT Variables
	pause = false;
	preview = false;

	// Uniforms
	shadingnum = 0;		// Toon/normal, etc.
	peerInside = 0;
	brushDivide = 15.0;
	myexp = 2;

	difftrans = 1.0;
	shininess = 128;
	darkness = 1.2;

	QApplication::processEvents();

	// Setup other instance variables
	status_label = new QLabel("Ready", this);
	status_label->setStyleSheet("background-color: rgba(0,0,0,0);");
	ui.statusBar->addWidget(status_label);

	QTimer* timer;
	timer = new QTimer(this);
	timer->setInterval(0);// 1000.0 / fps);
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
	renderer->GetActiveCamera()->SetClippingRange(1.0, 1000);

	//float bgcolor[3] = { 39, 50, 67 };
	float bgcolor[3] = { 109, 92, 73 };
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

	//vtkSmartPointer<vtkMyOutlinePass> outlineP = vtkSmartPointer<vtkMyOutlinePass>::New();	
	//outlineP->setShaders();
	//outlineP->SetDelegatePass(cameraP);

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
	interactorstyle->initialize(this);

	renderWindowInteractor->SetInteractorStyle(interactorstyle);
	renderWindowInteractor->Initialize();

	qv->GetRenderWindow()->SetInteractor(renderWindowInteractor);

	QApplication::processEvents();

	// Connect all signals to slots
	connect(ui.btnHello, SIGNAL(clicked()), this, SLOT(slot_buttonclicked()));
	connect(ui.btnColor, SIGNAL(clicked()), this, SLOT(slot_colorclicked()));

	connect(colorDialog, SIGNAL(currentColorChanged(const QColor &)), this, SLOT(slot_colorchanged(const QColor &)));

	connect(timer, SIGNAL(timeout()), this, SLOT(slot_timeout()));
	connect(timer2, SIGNAL(timeout()), this, SLOT(slot_timeout2()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(slot_open()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(slot_exit()));
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(slot_about()));
	connect(ui.actionPreview, SIGNAL(triggered()), this, SLOT(slot_preview()));

	connect(ui.menuFile, SIGNAL(aboutToShow()), this, SLOT(slot_menuclick()));
	connect(ui.menuHelp, SIGNAL(aboutToShow()), this, SLOT(slot_menuclick()));

	connect(ui.horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(slot_valueChanged(int)));
	connect(ui.verticalSlider, SIGNAL(valueChanged(int)), this, SLOT(slot_valueChangedV(int)));
	connect(ui.verticalSlider_2, SIGNAL(valueChanged(int)), this, SLOT(slot_valueChangedV2(int)));

	connect(ui.listWidget, SIGNAL(itemEntered(QListWidgetItem *)), this, SLOT(slot_listitementered(QListWidgetItem *)));
	connect(ui.listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(slot_listitemclicked(int)));
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

	selectedIndex = 0;	// Get rid of these eventually
	hoveredIndex = 0;

	renderer->RemoveAllViewProps();	// Remove from renderer, clear listwidget, clear vectors
	ui.listWidget->clear();
	myelems.clear();
	meshes.clear();

	qDebug() << " - reading file - \n";

	//read jpeg, instant
	vtkSmartPointer<vtkJPEGReader> jpgReader = vtkSmartPointer<vtkJPEGReader>::New();
	jpgReader->SetFileName("luigi.jpg");
	jpgReader->Update();

	colorTexture = vtkSmartPointer<vtkTexture>::New();
	colorTexture->SetInputConnection(jpgReader->GetOutputPort());
	colorTexture->InterpolateOn();

	// Tiny OBJ reader
	std::vector<tinyobj::shape_t> shapes;
	std::string err = tinyobj::LoadObj(shapes, filename.c_str());

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

		// Add mesh to custom meshes vector
		meshes.push_back(CustomMesh());
		//system("pause");
		meshes[z].opacity = 1.0;	// myopacity

		meshes[z].color.r = r;
		meshes[z].color.g = g;
		meshes[z].color.b = b;
		meshes[z].name = groupname;
		meshes[z].selected = 0;

		// Add cell locator for mesh to cellpicker and to mesh
		meshes[z].cellLocator = vtkSmartPointer<vtkCellLocator>::New();
		meshes[z].cellLocator->SetDataSet(dataset->GetOutput());
		meshes[z].cellLocator->BuildLocator();
		meshes[z].cellLocator->LazyEvaluationOn();

		interactorstyle->cellPicker->AddLocator(meshes[z].cellLocator);

		// Make mapper and actors
		meshes[z].actor = Utility::sourceToActor(dataset->GetOutput(), r, g, b, 1.0);
		//meshes[z].actor->GetProperty()->SetTexture(0, colorTexture);

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
	if (selectedIndex > -1 && selectedIndex < meshes.size())	// Reset Mesh's previous selected index object to not selected
		meshes[selectedIndex].selected = 0;

	selectedIndex = i;

	if (selectedIndex > -1 && selectedIndex < meshes.size())
		meshes[selectedIndex].selected = 1;						// New object selected

	updateOpacitySliderAndList();
}
//-------------------------------------------------------------------------------------
void illustrator::updateOpacitySliderAndList()
{
	if (selectedIndex < meshes.size())
	{
		ui.horizontalSlider->setValue(meshes[selectedIndex].opacity * 100);

		if (ui.listWidget->selectedItems().size() > 0)
			ui.listWidget->selectedItems().at(0)->setSelected(false);

		ui.listWidget->item(selectedIndex)->setSelected(true);
		ui.listWidget->scrollToItem(ui.listWidget->selectedItems().at(0));

		Utility::myColor mycol = meshes[selectedIndex].color;
		colorDialog->setCurrentColor(QColor(mycol.r * 255, mycol.g * 255, mycol.b * 255));
	}
}