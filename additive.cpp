#include "stdafx.h"		// Precompiled header files
#include "additive.h"

// Additional includes
#include <vtkSphereSource.h>
#include <vtkPointPicker.h>
#include <vtkPolyDataNormals.h>

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

#include "vtkPolyDataSilhouette.h"

#include "vtkMyProcessingPass.h"

#include <vtkOpenGLRenderWindow.h>

#include <vtkSmoothPolyDataFilter.h>

#include "obj_parser.h"

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

using namespace std;

//-------------------------------------------------------------------------------------------------------------
/// <summary> Returns a cube in Carve CSG format (MeshSet)
/// </summary>
/// <param name="size">Size of cube</param>
/// <param name="t">A transformation matrix </param>
/// <returns>Cube in Carve's MeshSet format</returns>
unique_ptr<carve::mesh::MeshSet<3> > makeCube(float size, const carve::math::Matrix &t = carve::math::Matrix())
{
	vector<carve::geom3d::Vector> vertices;

	vertices.push_back(t * carve::geom::VECTOR(+size, +size, +size));
	vertices.push_back(t * carve::geom::VECTOR(-size, +size, +size));
	vertices.push_back(t * carve::geom::VECTOR(-size, -size, +size));
	vertices.push_back(t * carve::geom::VECTOR(+size, -size, +size));
	vertices.push_back(t * carve::geom::VECTOR(+size, +size, -size));
	vertices.push_back(t * carve::geom::VECTOR(-size, +size, -size));
	vertices.push_back(t * carve::geom::VECTOR(-size, -size, -size));
	vertices.push_back(t * carve::geom::VECTOR(+size, -size, -size));

	vector<int> f;
	int numfaces = 0;

	f.push_back(4);
	f.push_back(0); f.push_back(1); f.push_back(2); f.push_back(3);
	numfaces++;
	f.push_back(4);
	f.push_back(7); f.push_back(6); f.push_back(5); f.push_back(4);
	numfaces++;
	f.push_back(4);
	f.push_back(0); f.push_back(4); f.push_back(5); f.push_back(1);
	numfaces++;
	f.push_back(4);
	f.push_back(1); f.push_back(5); f.push_back(6); f.push_back(2);
	numfaces++;
	f.push_back(4);
	f.push_back(2); f.push_back(6); f.push_back(7); f.push_back(3);
	numfaces++;
	f.push_back(4);
	f.push_back(3); f.push_back(7); f.push_back(4); f.push_back(0);
	numfaces++;

	unique_ptr<carve::mesh::MeshSet<3> > poly(new carve::mesh::MeshSet<3>(vertices, numfaces, f));
	return poly;
}
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
	fps = 50.0f;
	fname = "heart 256k.obj";
	preview = false;

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
	renderWindow->SetAlphaBitPlanes(1);
	renderWindow->SetMultiSamples(4);

	renderer = vtkSmartPointer<vtkRenderer>::New();
	float bgcolor[3] = { 39, 50, 67 };
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

	vtkSmartPointer<vtkLightsPass> lightsP = vtkSmartPointer<vtkLightsPass>::New();

	vtkSmartPointer<vtkMyShaderPass> opaqueP = vtkSmartPointer<vtkMyShaderPass>::New();
	opaqueP->initialize(this, ShaderPassType::PASS_OPAQUE);

	vtkSmartPointer<vtkMyShaderPass> transP = vtkSmartPointer<vtkMyShaderPass>::New();
	transP->initialize(this, ShaderPassType::PASS_TRANSLUCENT);

	//vtkSmartPointer<vtkOpaquePass> opaqueP = vtkSmartPointer<vtkOpaquePass>::New();
	//vtkSmartPointer<vtkTranslucentPass> transP = vtkSmartPointer<vtkTranslucentPass>::New();

	//vtkSmartPointer<vtkMyShaderPass> silhP = vtkSmartPointer<vtkMyShaderPass>::New();
	//silhP->initialize(this, ShaderPassType::PASS_SILHOUETTE);

	//vtkSmartPointer<vtkDepthPeelingPass> peelP = vtkSmartPointer<vtkDepthPeelingPass>::New();
	//peelP->SetMaximumNumberOfPeels(4);
	//peelP->SetOcclusionRatio(0.1);
	//peelP->SetTranslucentPass(transP);	//Peeling pass needs translucent pass

	// Put all passes into a collection then into a sequence
	vtkSmartPointer<vtkRenderPassCollection> passes = vtkSmartPointer<vtkRenderPassCollection>::New();
	passes->AddItem(lightsP);
	//passes->AddItem(silhP);
	passes->AddItem(opaqueP);
	//passes->AddItem(peelP);
	passes->AddItem(transP);
	//passes->AddItem(vtkDefaultPass::New());

	seq->SetPasses(passes);
	cameraP->SetDelegatePass(seq);

	//vtkSmartPointer<vtkSobelGradientMagnitudePass> sobelP = vtkSmartPointer<vtkSobelGradientMagnitudePass>::New();
	//sobelP->SetDelegatePass(cameraP);
	//vtkGaussianBlurPass* blurP = vtkGaussianBlurPass::New();
	//blurP->SetDelegatePass(cameraP);

	vtkSmartPointer<vtkMyProcessingPass> myProcessingPass = vtkSmartPointer<vtkMyProcessingPass>::New();
	myProcessingPass->SetDelegatePass(cameraP);

	vtkOpenGLRenderer::SafeDownCast(renderer)->SetPass(cameraP);

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

	vtkSmartPointer<vtkTexture> colorTexture = vtkSmartPointer<vtkTexture>::New();
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

			objectMeshCollection->AddItem(polydata);
		}
	}

	// Create a directional light (default)
	renderer->AutomaticLightCreationOff();
	renderer->RemoveAllLights();

	vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
	//light->SetLightTypeToHeadlight();		// Headlights are located at camera position (CameraLights do not have to be)	
	//light->SetLightTypeToSceneLight();	
	light->SetLightTypeToHeadlight();

	float amb = 0.2; //0.2
	float diff = 0.6; //0.6
	float spec = 1.0; //1.0
	
	light->SetAmbientColor(amb, amb, amb);
	light->SetDiffuseColor(diff, diff, diff);
	light->SetSpecularColor(spec, spec, spec);

	light->SetPosition(0, 0, 0);	// Light centered at camera origin (view space)
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
		vtkSmartPointer<vtkPolyDataMapper> const mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputData(dataset->GetOutput());

		meshes[z].actor = vtkSmartPointer<vtkActor>::New();
		meshes[z].actor->SetMapper(mapper);
		
		// Set actor properties
		meshes[z].actor->GetProperty()->BackfaceCullingOff();
		meshes[z].actor->GetProperty()->SetInterpolationToPhong(); 

		meshes[z].actor->GetProperty()->SetAmbientColor(r, g, b);
		meshes[z].actor->GetProperty()->SetAmbient(1.0);
		//meshes[z].actor->GetProperty()->SetColor(r, g, b);	// sets gl_Color? (Both ambient and diffuse colours are affected) - set to white for textures

		meshes[z].actor->GetProperty()->SetDiffuseColor(r, g, b);
		meshes[z].actor->GetProperty()->SetDiffuse(1.0);

		meshes[z].actor->GetProperty()->SetSpecular(0.15);
		meshes[z].actor->GetProperty()->SetSpecularPower(15);

		meshes[z].actor->GetProperty()->SetOpacity(1.0);	// myopacity
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

	Utility::start_clock('z');
	qDebug() << "Start processing data" << endl;

	//// vertices = 0.007 seconds
	//Utility::start_clock('a');

	//qDebug() << "start poly vertices # " << thepolydata->GetNumberOfPoints() << "\n";

	//std::vector<carve::geom3d::Vector> vertices;
	//vertices.resize(thepolydata->GetNumberOfPoints());

	//for(vtkIdType i = 0; i < thepolydata->GetNumberOfPoints(); i++)
	//{
	//	double p[3];
	//	thepolydata->GetPoint(i,p);

	//	vertices[i] = carve::geom::VECTOR(p[0], p[1], p[2]);
	//}
	//qDebug() << "end poly vertices\n";
	//Utility::end_clock('a');

	//// faces - 0.006 seconds
	//Utility::start_clock('a');

	//qDebug() << "start faces #" << thepolydata->GetPolys()->GetNumberOfCells() << "\n";

	//vtkSmartPointer<vtkCellArray> polys = thepolydata->GetPolys();
	//polys->InitTraversal();

	//std::vector<int> f;
	//int numfaces = 0;
	//int t = 0;

	//f.resize(polys->GetSize());	// exact size: n1, id1, id2, id3, n2, id1, id2..etc
	//for (int i = 0; i < polys->GetNumberOfCells(); i++)
	//{
	//	vtkIdType n_pts;
	//	vtkIdType *pts = nullptr;

	//	polys->GetNextCell(n_pts, pts);

	//	f[t++] = n_pts;
	//	f[t++] = pts[0];
	//	f[t++] = pts[1];
	//	f[t++] = pts[2];

	//	if (n_pts > 3)
	//		f[t++] = pts[3];
	//	numfaces++;
	//}
	//qDebug() << "end faces\n";

	//Utility::end_clock('a');

	//Utility::start_clock('a');
	//qDebug() << "start create \n";		// 2 seconds (under debug, 0.7 release)
	//unique_ptr<carve::mesh::MeshSet<3> > first(new carve::mesh::MeshSet<3> (vertices, numfaces, f));
	//qDebug() << "end create\n";
	//Utility::end_clock('a');

	//auto makecubef = [](float size, const carve::math::Matrix &t) -> unique_ptr<carve::mesh::MeshSet<3> > {
	//	vector<carve::geom3d::Vector> vertices;

	//	vertices.push_back(carve::geom::VECTOR(+size, +size, +size));
	//	vertices.push_back(carve::geom::VECTOR(-size, +size, +size));
	//	vertices.push_back(carve::geom::VECTOR(-size, -size, +size));
	//	vertices.push_back(carve::geom::VECTOR(+size, -size, +size));
	//	vertices.push_back(carve::geom::VECTOR(+size, +size, -size));
	//	vertices.push_back(carve::geom::VECTOR(-size, +size, -size));
	//	vertices.push_back(carve::geom::VECTOR(-size, -size, -size));
	//	vertices.push_back(carve::geom::VECTOR(+size, -size, -size));

	//	vector<int> f;
	//	int numfaces = 0;

	//	f.push_back(4);
	//	f.push_back(0); f.push_back(1); f.push_back(2); f.push_back(3);
	//	numfaces++;
	//	f.push_back(4);
	//	f.push_back(7); f.push_back(6); f.push_back(5); f.push_back(4);
	//	numfaces++;
	//	f.push_back(4);
	//	f.push_back(0); f.push_back(4); f.push_back(5); f.push_back(1);
	//	numfaces++;
	//	f.push_back(4);
	//	f.push_back(1); f.push_back(5); f.push_back(6); f.push_back(2);
	//	numfaces++;
	//	f.push_back(4);
	//	f.push_back(2); f.push_back(6); f.push_back(7); f.push_back(3);
	//	numfaces++;
	//	f.push_back(4);
	//	f.push_back(3); f.push_back(7); f.push_back(4); f.push_back(0);
	//	numfaces++;

	//	unique_ptr<carve::mesh::MeshSet<3> > poly(new carve::mesh::MeshSet<3>(vertices, numfaces, f));
	//	//unique_ptr<carve::mesh::MeshSet<3> > poly(new carve::mesh::MeshSet<3>(vertices, numfaces, f));

	//	return poly;
	//};

	//qDebug() << "start cube \n";
	//unique_ptr<carve::mesh::MeshSet<3> > second = makecubef(1.2, carve::math::Matrix::ROT(0.0, .2, .3, .4));
	//qDebug() << "end cube\n";
	//
	//Utility::start_clock('a');
	//qDebug() << "start compute\n";		// 11 secs (on luigi in Debug, 0.8 release)
	//carve::csg::CSG csg;
	//csg.hooks.registerHook(new carve::csg::CarveTriangulator, carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT); // slow but accurate
	//unique_ptr<carve::mesh::MeshSet<3> > c(csg.compute(first.get(), second.get(), carve::csg::CSG::A_MINUS_B, nullptr, carve::csg::CSG::CLASSIFY_EDGE));
	//qDebug() << "end compute\n";
	//Utility::end_clock('a');
	//
	//
	//Utility::start_clock('a');
	//qDebug() << "start adding points # " << c->vertex_storage.size() << "\n";	// 0.016 seconds

	///// Converting c back to vtk points and faces
	//vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	//points->SetNumberOfPoints(c->vertex_storage.size());	// allocate memory

	//boost::unordered_map<carve::mesh::MeshSet<3>::vertex_t*, uint> vertexToIndex_map;	// vertex index map

	//auto iter = begin(c->vertex_storage);
	//for (int k = 0; iter != end(c->vertex_storage); ++k, ++iter) {
	//
	//	carve::mesh::MeshSet<3>::vertex_t *vertex = &(*iter);

	//	points->SetPoint(k, vertex->v.x, vertex->v.y, vertex->v.z);
	//	vertexToIndex_map[vertex] = k;
	//}
	//cout<<" points done";
	//qDebug() << "end adding points\n";
	//Utility::end_clock('a');

	//Utility::start_clock('a');

	//qDebug() << "start adding faces";	// 0.37 seconds see if you can change insertnextcell to setcell?

	//vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();
	//vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();

	//for (auto face_iter = c->faceBegin(); face_iter != c->faceEnd(); ++face_iter) {
	//	carve::mesh::MeshSet<3>::face_t *f = *face_iter;
	//	polygon->GetPointIds()->SetNumberOfIds(f->nVertices());

	//	// nVertices = nEdges since half edge
	//	int j = 0;
	//	for (auto e_iter = f->begin(); e_iter != f->end(); ++e_iter) {
	//		polygon->GetPointIds()->SetId (j++, vertexToIndex_map.at(e_iter->vert) );
	//	}
	//	polygons->InsertNextCell(polygon);
	//
	//}
	//cout << polygons->GetNumberOfCells() << endl;

	//qDebug() << "end adding faces\n";

	//Utility::end_clock('a');

	//Utility::start_clock('a');
	//qDebug() << "start vtkPolyData\n";
	//vtkSmartPointer<vtkPolyData> polygonPolyData = vtkSmartPointer<vtkPolyData>::New();
	//polygonPolyData->SetPoints(points);
	//polygonPolyData->SetPolys(polygons);
	//qDebug() << "end vtkPolyData \n";
	//Utility::end_clock('a');

	//// Create a mapper and actor
	//vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	//mapper->SetInput(polygonPolyData);

	//vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	//actor->SetMapper(mapper);

	//// Now replace mesh too
	//renderer->GetViewProps()->ReplaceItem(myIndex, actor);
	//meshes[myIndex].actor = actor;

	qDebug() << "end processing data \n";
	Utility::end_clock('z');
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