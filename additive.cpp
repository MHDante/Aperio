#include "stdafx.h"		// Precompiled header files
#include "additive.h"

#include "CustomDepthPeelingPass.h"

// Additional includes
#include <vtkSphereSource.h>
#include <vtkPointPicker.h>
#include <vtkPolyDataNormals.h>
#include <vtkFloatArray.h>
#include <vtkPainterDeviceAdapter.h>
#include <vtkShaderDeviceAdapter2.h>
#include <vtkGLSLShaderDeviceAdapter2.h>

// Custom
#include "MyInteractorStyle.h"
#include "vtkMyShaderPass.h"

#include <QtGui/QLayout>

#include "vtkAppendPolyData.h"
#include "vtkCleanPolyData.h"
#include "vtkDepthSortPolyData.h"

#include "vtkPropAssembly.h"
#include "vtkOpenGLRenderer.h"

#include "vtkShader2.h"
#include "vtkShader2Collection.h"
#include "vtkShaderProgram2.h"

#include "vtkLightsPass.h"
#include "vtkCameraPass.h"
#include "vtkDefaultPass.h"
#include "vtkSequencePass.h"
#include "vtkRenderPassCollection.h"
#include "vtkGaussianBlurPass.h"
#include "vtkDepthPeelingPass.h"

#include "vtkOpaquePass.h"
#include "vtkTranslucentPass.h"

#include "vtkOutlineFilter.h"
#include "vtkClearZPass.h"

#include "vtkLightCollection.h"
#include "vtkLight.h"

#include "vtkSobelGradientMagnitudePass.h"

#include "vtkInformation.h"
#include "vtkUniformVariables.h"
#include "vtkFieldData.h"

#include "vtkContourFilter.h"
#include "vtkWindowToImageFilter.h"
#include "vtkScenePicker.h"

#include "vtkCellLocator.h"
#include "vtkOBBTree.h"
#include "vtkKdTree.h"
#include "vtkModifiedBSPTree.h"

//#include "vtkPolyDataSilhouette.h"

#include "vtkMyProcessingPass.h"

#include <vtkOpenGLRenderWindow.h>

#include <vtkSmoothPolyDataFilter.h>
#include <vtkOpenGLProperty.h>

#include "obj_parser.h"
#include "CarveConnector.h"

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

using namespace std;

//-------------------------------------------------------------------------------------------------------------
additive::additive(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	QTimer::singleShot(0, this, SLOT(slot_afterShowWindow()));
}
//-------------------------------------------------------------------------------------------------------------
additive::~additive()
{
}
//-------------------------------------------------------------------------------------------------------------
void additive::update_orig_size()
{
	_orig_size.setX(ui.mainWidget->geometry().x());
	_orig_size.setY(ui.mainWidget->geometry().y());
	_orig_size.setWidth(ui.mainWidget->geometry().width());
	_orig_size.setHeight(ui.mainWidget->geometry().height());
}
///---------------------------------------------------------------------------------------
void additive::slot_afterShowWindow()
{
	update_orig_size();

	// Set up instance variables
	fps = 50;
	//fname = "organs brain 250k.obj";
	fname = "heart 256k.obj";
	
	preview = false;
	shadingnum = 0;

	pause = false;

	brushDivide = 15.0;
	peerInside = 0;
	myexp = 2;

	QApplication::processEvents();

	status_label = new QLabel("Ready", this);
	status_label->setStyleSheet("background-color: rgba(0,0,0,0);");
	ui.statusBar->addWidget(status_label);

	QTimer* timer;
	timer = new QTimer(this);
	timer->setInterval(1000.0 / fps);
	timer->start();

	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AlphaBitPlanesOn();
	renderWindow->DoubleBufferOn();
	renderWindow->SetMultiSamples(0);
		
	renderer = vtkSmartPointer<vtkRenderer>::New();
	//float bgcolor[3] = { 39, 50, 67 };
	float bgcolor[3] = { 109, 92, 73 };
	//float bgcolor[3] = { 67, 118,145};
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

	opaqueP = vtkSmartPointer<vtkMyShaderPass>::New();
	opaqueP->initialize(this, ShaderPassType::PASS_OPAQUE);

	vtkSmartPointer<vtkMyShaderPass> transP = vtkSmartPointer<vtkMyShaderPass>::New();
	transP->initialize(this, ShaderPassType::PASS_TRANSLUCENT);

	//vtkSmartPointer<vtkOpaquePass> opaqueP = vtkSmartPointer<vtkOpaquePass>::New();
	//vtkSmartPointer<vtkTranslucentPass> transP = vtkSmartPointer<vtkTranslucentPass>::New();

	//vtkSmartPointer<vtkMyShaderPass> silhP = vtkSmartPointer<vtkMyShaderPass>::New();
	//silhP->initialize(this, ShaderPassType::PASS_SILHOUETTE);

	
	vtkSmartPointer<CustomDepthPeelingPass> peelP = vtkSmartPointer<CustomDepthPeelingPass>::New();
	peelP->SetMaximumNumberOfPeels(4);
	peelP->SetOcclusionRatio(0.5);
	peelP->SetTranslucentPass(transP);	//Peeling pass needs translucent pass

	// Put all passes into a collection then into a sequence
	vtkSmartPointer<vtkRenderPassCollection> passes = vtkSmartPointer<vtkRenderPassCollection>::New();
	//passes->AddItem(vtkSmartPointer<vtkCameraPass>::New());
	//passes->AddItem(vtkClearZPass::New());
	//passes->AddItem(vtkSmartPointer<vtkLightsPass>::New());
	////passes->AddItem(silhP);
	passes->AddItem(opaqueP);
	//passes->AddItem(peelP);
	passes->AddItem(transP);
	//passes->AddItem(vtkDefaultPass::New());

	seq->SetPasses(passes);
	cameraP->SetDelegatePass(seq);

	//vtkGaussianBlurPass* blurP = vtkGaussianBlurPass::New();
	//blurP->SetDelegatePass(cameraP);

	//vtkSmartPointer<vtkSobelGradientMagnitudePass> sobelP = vtkSmartPointer<vtkSobelGradientMagnitudePass>::New();
	//sobelP->SetDelegatePass(fxaaP);

	vtkSmartPointer<vtkMyProcessingPass> fxaaP = vtkSmartPointer<vtkMyProcessingPass>::New();
	fxaaP->setShaderFile("shader_fxaa.vert", false);
	fxaaP->setShaderFile("shader_fxaa.frag", true);
	fxaaP->SetDelegatePass(cameraP);

	vtkSmartPointer<vtkMyProcessingPass> bloomP = vtkSmartPointer<vtkMyProcessingPass>::New();
	bloomP->setShaderFile("shader_bloom.frag", true);
	bloomP->SetDelegatePass(fxaaP);

	vtkOpenGLRenderer::SafeDownCast(renderer)->SetPass(bloomP);

	vtkSmartPointer<QVTKInteractor> renderWindowInteractor = vtkSmartPointer<QVTKInteractor>::New();
	renderWindowInteractor->SetRenderWindow(qv->GetRenderWindow());

	interactorstyle = vtkSmartPointer<MyInteractorStyle>::New();
	renderWindowInteractor->SetInteractorStyle(interactorstyle);
	interactorstyle->initialize(this);

	renderWindowInteractor->Initialize();

	QApplication::processEvents();

	// Connect all signals to slots
	connect(ui.btnHello, SIGNAL(clicked()), this, SLOT(slot_buttonclicked()));
	connect(timer, SIGNAL(timeout()), this, SLOT(slot_timeout()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(slot_open()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(slot_exit()));
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(slot_about()));
	connect(ui.actionPreview, SIGNAL(triggered()), this, SLOT(slot_preview()));

	connect(ui.menuFile, SIGNAL(aboutToShow()), this, SLOT(slot_menuclick()));
	connect(ui.menuHelp, SIGNAL(aboutToShow()), this, SLOT(slot_menuclick()));

	connect(ui.horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(slot_valueChanged(int)));

	connect(ui.listWidget, SIGNAL(itemEntered(QListWidgetItem *)), this, SLOT(slot_listitementered(QListWidgetItem *)));
	connect(ui.listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(slot_listitemclicked(int)));
	readFile(fname);
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
///---------------------------------------------------------------------------------------
void additive::resizeEvent(QResizeEvent * event)
{
	resizeInternal(event->size(), false);
}
///---------------------------------------------------------------------------------------
void additive::mouseMoveEvent(QMouseEvent *)
{
}
///---------------------------------------------------------------------------------------
void additive::readFile(std::string filename)
{
	//vtkObject::GlobalWarningDisplayOff();	// dangerous (keep on for most part)

	toon = 0;

	selectedIndex = 0;
	hoveredIndex = 0;

	renderer->RemoveAllViewProps();
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

	// The reader
	obj::obj_parser parser;
	parser.parse(filename);

	vtkSmartPointer<vtkPolyDataCollection> objectMeshCollection = vtkSmartPointer<vtkPolyDataCollection>::New();

	// Add meshes to objectMeshCollection
	for (int i = 0; i < parser.themeshes.size(); i++)
	{
		// Create points
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		points->SetNumberOfPoints(parser.themeshes.at(i).vertices.size());

		for (int j = 0; j < parser.themeshes.at(i).vertices.size(); j++)
			points->SetPoint(j, parser.themeshes.at(i).vertices.at(j).p);

		// Create faces
		vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();

		for (int j = 0; j < parser.themeshes.at(i).polys.size(); j++)
		{
			vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
			polygon->GetPointIds()->SetNumberOfIds(parser.themeshes.at(i).polys.at(j).numIndices);

			for (int k = 0; k < parser.themeshes.at(i).polys.at(j).numIndices; k++)
				polygon->GetPointIds()->SetId(k, parser.themeshes.at(i).polys.at(j).indices[k]);

			polygons->InsertNextCell(polygon);
		}

		// Make polygon data
		if (points->GetNumberOfPoints() > 0)
		{
			vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
			polydata->SetPoints(points);
			polydata->SetPolys(polygons);

			// Create texture coordinates
			Utility::generateTexCoords(polydata);

			// Create attributes for vertices
			//vtkSmartPointer<vtkFloatArray> weights = vtkSmartPointer<vtkFloatArray>::New();
			//weights->SetNumberOfComponents(3);
			//weights->SetName("weights");

			//float tuple[3] = { 5, 5, 5};
			//for (int i = 0; i < polydata->GetNumberOfPoints(); i++)
			//	weights->InsertNextTuple(tuple);

			//polydata->GetPointData()->SetScalars(weights);
			
			
			objectMeshCollection->AddItem(polydata);
		}
	}

	// Create a directional light (default)
	renderer->AutomaticLightCreationOff();
	renderer->RemoveAllLights();

	vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
	light->SetLightTypeToHeadlight();		// Headlights are located at camera position (CameraLights do not have to be)	
	//light->SetPositional(true);
	//light->SetPosition(50, 50, 50);
	float amb = 0.2; //0.2
	float diff = 0.6; //0.6
	float spec = 1.0; //1.0

	light->SetAmbientColor(amb, amb, amb);
	light->SetDiffuseColor(diff, diff, diff);
	light->SetSpecularColor(spec, spec, spec);

	light->SetPosition(0, 0, 0);	// Light centered at camera origin (view space)

	//light->set
	renderer->AddLight(light);

	//srand(time(nullptr));

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

		//vtkSmartPointer<vtkOBBTree> objectOBBTree = vtkSmartPointer<vtkOBBTree>::New();
		//objectOBBTree->SetDataSet(nextMesh);
		//objectOBBTree->BuildLocator();
		//CommonData::objectOBBTrees.push_back(objectOBBTree);

		//vtkPolyData *s = new vtkPolyData(*nextMesh);

		// Compute normals/toggle on/off
		vtkSmartPointer<vtkPolyDataNormals> dataset = vtkSmartPointer<vtkPolyDataNormals>::New();
		dataset->SetInputData(nextMesh);
		dataset->ComputePointNormalsOn();
		dataset->ComputeCellNormalsOff();
		dataset->SplittingOn();
		dataset->SetFeatureAngle(60);
		dataset->Update();

		//if (z == 0)
		//{
		//	//Compute the silhouette
		//	vtkSmartPointer<vtkPolyDataSilhouette> silhouette = vtkSmartPointer<vtkPolyDataSilhouette>::New();
		//	silhouette->SetInputData(dataset->GetOutput());
		//	silhouette->SetCamera(renderer->GetActiveCamera());
		//	silhouette->SetDirectionToSpecifiedVector();
		//	silhouette->SetVector(renderer->GetActiveCamera()->GetDirectionOfProjection());
		//	silhouette->SetEnableFeatureAngle(0);
		//}

		//vtkSmartPointer<vtkDepthSortPolyData> depthSort = vtkSmartPointer<vtkDepthSortPolyData>::New();
		//depthSort->SetInputData(dataset->GetOutput());
		//depthSort->SetDirectionToBackToFront();
		//depthSort->SetVector(1, 1, 1);
		//depthSort->SetCamera(renderer->GetActiveCamera());
		//depthSort->SortScalarsOff();
		//depthSort->Update();

		// Add mesh to custom meshes vector
		meshes.push_back(CustomMesh());

		meshes[z].opacity = 1.0;	// myopacity

		meshes[z].color.r = r;
		meshes[z].color.g = g;
		meshes[z].color.b = b;
		meshes[z].name = parser.themeshes.at(z).name;

		// Add cell locator for mesh to cellpicker
		meshes[z].cellLocator = vtkSmartPointer<vtkCellLocator>::New();
		meshes[z].cellLocator->SetDataSet(dataset->GetOutput());
		meshes[z].cellLocator->BuildLocator();
		meshes[z].cellLocator->LazyEvaluationOn();

		interactorstyle->cellPicker->AddLocator(meshes[z].cellLocator);

		

		// Make mapper and actors
		//vtkSmartPointer<vtkPolyDataMapper> const mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		//mapper->SetInputData(dataset->GetOutput());

		//meshes[z].actor = vtkSmartPointer<vtkActor>::New();
		//meshes[z].actor->SetMapper(mapper);

		meshes[z].actor = Utility::sourceToActor(dataset->GetOutput(), r, g, b, 1.0);

		// Add shader
		shaderProgram = vtkSmartPointer<vtkShaderProgram2>::New();

		vtkSmartPointer<vtkShader2> shader = vtkSmartPointer<vtkShader2>::New();
		vtkSmartPointer<vtkShader2> shader2 = vtkSmartPointer<vtkShader2>::New();

		std::ifstream file1("shader.vert");
		std::stringstream buffer1;
		buffer1 << file1.rdbuf();

		shader->SetSourceCode(buffer1.str().c_str());

		std::ifstream file2("shader.frag");
		std::stringstream buffer2;
		buffer2 << file2.rdbuf();

		shader2->SetSourceCode(buffer2.str().c_str());

		shader->SetType(VTK_SHADER_TYPE_VERTEX);
		shader2->SetType(VTK_SHADER_TYPE_FRAGMENT);

		shaderProgram->GetShaders()->AddItem(shader);
		shaderProgram->GetShaders()->AddItem(shader2);

		//shaderProgram->SetContext(glContext);
		shader->SetContext(shaderProgram->GetContext());
		shader2->SetContext(shaderProgram->GetContext());

		shaderProgram->Build();

		//vtkSmartPointer<vtkGLSLShaderDeviceAdapter2> adapter = vtkSmartPointer<vtkGLSLShaderDeviceAdapter2>::New();
		//adapter->SetShaderProgram(shaderProgram);
		


		//std::cout << loc << "\n";
		//vtkSmartPointer<vtkGLSLShaderDeviceAdapter2> adapter = vtkSmartPointer<vtkGLSLShaderDeviceAdapter2>::New();
		//adapter->SetShaderProgram(shaderProgram);
		//adapter->
		//float att[3] = { 5, 5, 5 };
		//adapter->SendAttribute("weights", 3, VTK_FLOAT, att);

		//int loc = shaderProgram->GetAttributeLocation("weights");

		//float att[3] = {5, 5, 5};

		//vtkOpenGLRenderWindow::SafeDownCast(renderer->GetRenderWindow())->GetPainterDeviceAdapter()->SendAttribute(0, 3, VTK_FLOAT, att);
		//vtkOpenGLRenderer::SafeDownCast(renderer)->SetShaderProgram(shaderProgram);
		
		//vtkOpenGLRenderer::SafeDownCast(renderer)->SetShaderProgram(shaderProgram);

		//vtkOpenGLProperty::SafeDownCast(meshes[z].actor->GetProperty())->ShadingOff();
		//vtkOpenGLProperty::SafeDownCast(meshes[z].actor->GetProperty())->GlobalWarningDisplayOff();
		//vtkOpenGLProperty::SafeDownCast(meshes[z].actor->GetProperty())->SetPropProgram(shaderProgram);
		
		//meshes[z].actor->GetProperty()->ShadingOn();

		//meshes[z].actor->GetProperty()->SetTexture(0, colorTexture);



		mouse[0] = 1;
		mouse[1] = 1;
		mouse[2] = 1;

		meshes[z].selected = 0;

		// add actor to renderer
		renderer->AddActor(meshes[z].actor);
		renderer->ResetCamera();
		qv->GetRenderWindow()->Render();
	}

	// get all stuff

	//vtkSmartPointer<vtkPointPicker> worldPicker = vtkSmartPointer<vtkPointPicker>::New();
	//vtkSmartPointer<vtkWorldPointPicker> worldPicker = vtkSmartPointer<vtkWorldPointPicker>::New();
	//worldPicker = vtkSmartPointer<vtkCellPicker>::New();
	//cellPicker->SetTolerance(0.00);

	//QApplication::exec();
	//renderWindowInteractor->Start();

	// add meshes groupnames to listbox
	for (int i = 0; i < meshes.size(); i++)
	{
		QListWidgetItem* item = new QListWidgetItem(meshes[i].name.c_str(), ui.listWidget);

		item->setCheckState(Qt::Checked);
		ui.listWidget->addItem(item);
		//ui.listWidget->itemAt(0, i)->setCheckState(Qt::Checked);
	}
	// VTK CSG
	//vtkSmartPointer<vtkBooleanOperationPolyDataFilter> booleanOperation = vtkSmartPointer<vtkBooleanOperationPolyDataFilter>::New();
	//booleanOperation->SetOperationToIntersection();
	//booleanOperation->SetInputConnection(0, polydata1->GetProducerPort());
	//booleanOperation->SetInputConnection(1, polydata2->GetProducerPort());

	//CarveConnector connector;
	//vtkSmartPointer<vtkPolyData> thepolydata (vtkPolyData::SafeDownCast(meshes[0].actor->GetMapper()->GetInput()));
	//
	//// Make MeshSet from vtkPolyData
	//unique_ptr<carve::mesh::MeshSet<3> > first(CarveConnector::vtkPolyDataToMeshSet(thepolydata));
	//unique_ptr<carve::mesh::MeshSet<3> > second(CarveConnector::makeCube(10, carve::math::Matrix::IDENT()));
	//
	//unique_ptr<carve::mesh::MeshSet<3> > c(CarveConnector::performDifference(first, second));

	//vtkSmartPointer<vtkPolyData> c_poly (CarveConnector::meshSetToVTKPolyData(c));

	//// Create a mapper and actor
	//vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	//mapper->SetInputData(c_poly);

	//vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	//actor->GetProperty()->SetDiffuseColor(meshes[0].color.r, meshes[0].color.b, meshes[0].color.b);
	//actor->GetProperty()->SetAmbientColor(meshes[0].color.r, meshes[0].color.b, meshes[0].color.b);
	//actor->SetMapper(mapper);

	//// Now replace mesh too
	//renderer->GetViewProps()->ReplaceItem(0, actor);
	//meshes[0].actor = actor;
}
//-------------------------------------------------------------------------------------
void additive::slot_listitemclicked(int i)
{
	if (selectedIndex > -1 && selectedIndex < meshes.size())	// Reset previous selected index object to not selected
		meshes[selectedIndex].selected = 0;

	selectedIndex = i;

	if (selectedIndex > -1 && selectedIndex < meshes.size())
		meshes[selectedIndex].selected = 1;						// New object selected

	updateOpacitySliderAndList();
}
//-------------------------------------------------------------------------------------
void additive::updateOpacitySliderAndList()
{
	if (selectedIndex < meshes.size())
	{
		ui.horizontalSlider->setValue(meshes[selectedIndex].opacity * 100);

		if (ui.listWidget->selectedItems().size() > 0)
			ui.listWidget->selectedItems().at(0)->setSelected(false);

		ui.listWidget->item(selectedIndex)->setSelected(true);
		ui.listWidget->scrollToItem(ui.listWidget->selectedItems().at(0));
	}
}