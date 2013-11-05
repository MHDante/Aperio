#include "stdafx.h"		// Precompiled header files
#include "additive.h"

// Additional includes 
#include <vtkSphereSource.h>
#include <vtkPointPicker.h>
#include <vtkPolyDataNormals.h> 

// Custom 
#include "myOBJReader.h"
#include "myOBJReader_Exp.h"
#include "MyInteractor.h"
#include "vtkMyShaderPass.h"

#include "CheckForMemoryLeaks.h"	// MUST be Last include  

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

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

using namespace std;

/// ---------------------------------------------------------------------------
/// <summary> Returns a cube in Carve CSG format (MeshSet)
/// </summary>
/// <param name="size">Size of cube</param>
/// <param name="t">A transformation matrix </param>
/// <returns>Cube in Carve's MeshSet format</returns>
unique_ptr<carve::mesh::MeshSet<3> > makeCube
	(float size, const carve::math::Matrix &t = carve::math::Matrix()) 
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

/// ---------------------------------------------------------------------------
/// <summary> The constructor (Fires a timer that starts slot_afterShowWindow which has all initialization code)
/// </summary>
additive::additive(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	QTimer::singleShot(0, this, SLOT(slot_afterShowWindow()));	
}

/// ---------------------------------------------------------------------------------------
/// <summary> Finalizes an instance of the <see cref="additive" /> class.
/// </summary>
additive::~additive()
{
}

void additive::slot_timeout2()
{
	
	return;

}

/// ---------------------------------------------------------------------------------------
/// <summary>
/// Slot called after the window is fully loaded (contains all initialization code)
/// </summary>
void additive::slot_afterShowWindow()
{
	strcpy(fname, "heartsmall.obj");
	QApplication::processEvents();

	brushDivide = 15.0; 
	peerInside = 0;

	// set up variables

	myexp = 2;

	status_label = new QLabel("Ready", this); 
	status_label->setStyleSheet("background-color: rgba(0,0,0,0);");
	ui.statusBar->addWidget(status_label);
	 
	QTimer *timer = new QTimer(this);
	timer->setInterval(1);
	timer->start();

	QTimer *timer2 = new QTimer(this);
	timer2->setInterval(100);
	timer2->start();

	pause = false;

	// Dynamically create a new QVTKWidget (and add it to a centralWidget2->gridLayout
	qv = new QVTKWidget();

	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderer = vtkSmartPointer<vtkRenderer>::New();
	//renderer->SetBackground(1.0,1.0,1.0);
	renderer->SetBackground(.37,.63,.80);
	//renderer->SetBackground(0.3203, 0.3398, 0.4297);

	renderWindow->AddRenderer(renderer);
	qv->SetRenderWindow(renderWindow);

	// Add QVTKWidget to centralWidget->gridLayout
	ui.centralWidget2->setLayout(ui.gridLayout);

	ui.centralWidget2->layout()->setMargin(2);
	ui.centralWidget2->layout()->addWidget(qv);

	// Required for Depth peeling (if used)
	renderer->GetRenderWindow()->SetAlphaBitPlanes(1);
	renderer->GetRenderWindow()->SetMultiSamples(0);

	// Prepare all the rendering passes for vtkMyShaderPass	
	vtkCameraPass *cameraP=vtkCameraPass::New();

	vtkSequencePass *seq=vtkSequencePass::New();
	
	vtkLightsPass *lightsP=vtkLightsPass::New();

	vtkMyShaderPass *opaqueP = vtkMyShaderPass::New();
	opaqueP->initialize(this, ShaderPassType::PASS_OPAQUE);
	
	vtkMyShaderPass *transP = vtkMyShaderPass::New();
	transP->initialize(this, ShaderPassType::PASS_TRANSLUCENT);


	//vtkOpaquePass *opaqueP = vtkOpaquePass::New();
	//vtkTranslucentPass *transP = vtkTranslucentPass::New();


	//vtkMyShaderPass *silhP = vtkMyShaderPass::New();
	//silhP->initialize(this, ShaderPassType::PASS_SILHOUETTE);
	//transP->SettranslucentPass(true);

	vtkDepthPeelingPass *peelP = vtkDepthPeelingPass::New();
	peelP->SetMaximumNumberOfPeels(10);
	peelP->SetOcclusionRatio(0.05);	
	peelP->SetTranslucentPass(transP);	//Peeling pass needs translucent pass

	// Put all passes into a collection then into a sequence.
	vtkRenderPassCollection *passes=vtkRenderPassCollection::New();
	passes->AddItem(lightsP);
	//passes->AddItem(silhP);
	passes->AddItem(opaqueP);
	passes->AddItem(peelP);
	//passes->AddItem(transP);
	//passes->AddItem(vtkDefaultPass::New());

	seq->SetPasses(passes);
	cameraP->SetDelegatePass(seq);

	//vtkSmartPointer<vtkSobelGradientMagnitudePass> sobelP = vtkSmartPointer<vtkSobelGradientMagnitudePass>::New();
	//sobelP->SetDelegatePass(cameraP);
	//vtkGaussianBlurPass* blurP = vtkGaussianBlurPass::New();
	//blurP->SetDelegatePass(cameraP);

	( static_cast<vtkOpenGLRenderer *>(renderer.GetPointer()) )->SetPass(cameraP);

	qv->show();
	qv->update();
	renderWindow->Render();

	vtkSmartPointer<QVTKInteractor> renderWindowInteractor = vtkSmartPointer<QVTKInteractor>::New();
	renderWindowInteractor->SetRenderWindow(qv->GetRenderWindow());

	estyle =  vtkSmartPointer<MouseInteractorStylePP>::New();
	renderWindowInteractor->SetInteractorStyle( estyle );
	estyle->initialize(this);

	renderWindowInteractor->Initialize();

	// Connect all signals to slots 
	connect(ui.btnHello, SIGNAL(clicked()), this, SLOT(slot_buttonclicked()));
	connect(ui.actionNew_Window, SIGNAL(triggered()), this, SLOT(slot_newwindow()));
	connect(timer, SIGNAL(timeout()), this, SLOT(slot_timeout()));
	connect(timer2, SIGNAL(timeout()), this, SLOT(slot_timeout2()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(slot_open()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(slot_exit()));

	connect( ui.horizontalSlider, SIGNAL(valueChanged(int)),  this, SLOT(slot_valueChanged(int)) );

	connect( ui.listWidget, SIGNAL(itemEntered(QListWidgetItem *)), this, SLOT(slot_listitementered(QListWidgetItem *)));
	connect( ui.listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(slot_listitemclicked(int)));
	//throw std::exception("The method or operation is not implemented.");
	readFile(fname);  

	//vtkScenePicker * picker = vtkScenePicker::New();
	//picker->SetRenderer(renderer);
	//picker->

	// Write some stuff for interactive stuff.
	//TestScenePickerCommand * command = TestScenePickerCommand::New();
	//command->m_Picker = picker;
	//command->SetActorDescription( meshes[0].actor, "Head"   );
	//command->SetActorDescription( meshes[1].actor, "Sphere" );
	
	//renderer->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::MouseMoveEvent, command);

	//picker->EnableVertexPickingOff();

	//renderer->GetRenderWindow()->SetStencilCapable(1);
	renderer->GetRenderWindow()->Render();
}
/// ---------------------------------------------------------------------------------------
/// <summary> Event called when window resized (resizes qvtkwidget)
/// </summary>
/// <param name="event">Event object (Qt-based)</param>
void additive::resizeEvent( QResizeEvent * event) 
{
	QSize newSize = event->size(); 

	int margin = 20;
	int extra = 2;	// extra border-width margin of qframe

	QRect central = ui.centralWidget2->geometry();

	QRect newRect;
	newRect.setCoords(central.topLeft().x(), central.topLeft().y(), newSize.width() - margin, newSize.height() - margin * 3);

	ui.centralWidget2->setGeometry(newRect); 
}

/// ---------------------------------------------------------------------------------------
/// <summary> Event called when mouse moves over window
/// </summary>
/// <param name="">Event object (Qt-based)</param>
void additive::mouseMoveEvent( QMouseEvent * )
{

	//for (int j = 0; j < meshes.size(); j++)
	//{
		//cout << "hi";
		//meshes[selectedIndex].actor->GetProperty()->AddShaderVariable("lightDirs",  0.0, 0.0, 10.0);
		//meshes[j].actor->GetProperty()->EdgeVisibilityOff();
		//meshes[j].actor->GetProperty()->SetOpacity(meshes[j].opacity);
	//}
}

/// ---------------------------------------------------------------------------------------
/// <summary> Reads an .obj file and loads into meshes vector
/// </summary>
/// <param name="filename">filename.</param>
void additive::readFile(char * filename)
{
	toon = 0;

	qDebug() << "started\n";
	
	selectedIndex = 0;
	hoveredIndex = 0;

	renderer->RemoveAllViewProps();
	ui.listWidget->clear();
	meshes.clear();
	
	Utility::start_clock('a');
	qDebug() << "reading file\n";


	// the readers
	vtkSmartPointer<myOBJReader> s = vtkSmartPointer<myOBJReader>::New();	
	//vtkSmartPointer<myOBJReader_Exp> s = vtkSmartPointer<myOBJReader_Exp>::New();	
	s->SetFileName(filename);
	s->Update();
	Utility::end_clock('a');

	//s->GetOutput()->GlobalReleaseDataFlagOn();

	//read jpeg, instant
	vtkSmartPointer<vtkJPEGReader> jpgReader = vtkSmartPointer<vtkJPEGReader>::New(); 
	jpgReader->SetFileName("luigi.jpg"); 
	jpgReader->Update(); 

	vtkSmartPointer<vtkTexture> colorTexture = vtkSmartPointer<vtkTexture>::New(); 
	colorTexture->SetInputConnection(jpgReader->GetOutputPort());
	colorTexture->InterpolateOn();

	qDebug() << "Init read meshes\n";
	
	vtkSmartPointer<vtkPolyDataCollection> objectMeshCollection = s->getPolyDataCollection();
	objectMeshCollection->InitTraversal();
	int numMeshes = objectMeshCollection->GetNumberOfItems();
	
	// lighting colors
	Utility::myColor mambient= {0.2, 0.2, 0.2};
	Utility::myColor mdiffuse= {0.6, 0.5, 0.5};
	Utility::myColor mspecular= {1.0, 1.0, 1.0};

	Utility::myColor lambient= {0.2, 0.2, 0.2};
	Utility::myColor ldiffuse= {0.6, 0.5, 0.5};
	Utility::myColor lspecular= {1.0, 1.0, 1.0};

	float lightPos[3] = {0, 0, 0};
	float shininess = 1;

	float r, g, b;
	srand (time(nullptr));

	for(int z=0; z<numMeshes; z++)
	{
		if(numMeshes > 1) // assign a random colour if there's more than one object
		{
			r = ( (rand() % 177 + 80) / 270.0) ;
			g = ( (rand() % 177 + 80) / 270.0) ;
			b = ( (rand() % 177 + 80) / 270.0) ;
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
		
		//std::cout << "..." << std::endl;
		//std::cout << nextMesh->GetNumberOfPoints() << std::endl;

		vtkPolyDataNormals *dataset = vtkPolyDataNormals::New();
		dataset->SetInputData(nextMesh);		
		dataset->ComputePointNormalsOff();	
		dataset->ComputeCellNormalsOff();	
		//dataset->FlipNormalsOff();
		dataset->SplittingOn();
		dataset->SetFeatureAngle(60);
		dataset->Update();

		//std::cout << dataset->GetOutput()->GetNumberOfPoints() << std::endl;

		//if (z == 0)
		//{
		//	//Compute the silhouette
		//	vtkSmartPointer<vtkPolyDataSilhouette> silhouette = vtkSmartPointer<vtkPolyDataSilhouette>::New();
		//	silhouette->SetInputData(dataset->GetOutput());
		//	silhouette->SetCamera(renderer->GetActiveCamera());
		//	//silhouette->SetDirectionToSpecifiedVector();
		//	//silhouette->SetVector(renderer->GetActiveCamera()->GetDirectionOfProjection());
		//	silhouette->SetEnableFeatureAngle(0);

		//	vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
		//	mapper2->SetInputConnection(silhouette->GetOutputPort());

		//	vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
		//	actor2->SetMapper(mapper2);
		//	actor2->GetProperty()->SetColor(1.0, 0.3882, 0.2784); // tomato
		//	actor2->GetProperty()->SetLineWidth(2);
		//	renderer->AddActor(actor2);

		//}

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);

		//contour = tfilter1->GetOutput();

		//vtkSmartPointer<vtkDepthSortPolyData> depthSort = vtkSmartPointer<vtkDepthSortPolyData>::New();  
		//depthSort->SetInputData(dataset->GetOutput());
		//depthSort->SetDirectionToBackToFront();
		//depthSort->SetVector(1, 1, 1);
		//depthSort->SetCamera(renderer->GetActiveCamera());
		//depthSort->SortScalarsOff();
		//depthSort->Update();

		// Add mesh to meshes vector
		meshes.push_back(CustomMesh());

		meshes[z].cellLocator = vtkSmartPointer<vtkCellLocator>::New();
		meshes[z].cellLocator ->SetDataSet(dataset->GetOutput());
		meshes[z].cellLocator ->BuildLocator();
		meshes[z].cellLocator ->LazyEvaluationOn();

		//estyle->worldPicker->AddLocator(meshes[z].cellLocator);
		estyle->cellPicker->AddLocator(meshes[z].cellLocator);
		

		vtkSmartPointer<vtkPolyDataMapper> const mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		//mapper->SetInputData(nextMesh);
		mapper->SetInputData(dataset->GetOutput());
		//mapper->ImmediateModeRenderingOff();

		//vtkSmartPointer<vtkOutlineFilter> outline = vtkSmartPointer<vtkOutlineFilter>::New();
		//outline->SetInputData(dataset->GetOutput());
		//outline->Update();
		//vtkSmartPointer<vtkPolyDataMapper> outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		//outlineMapper->SetInputConnection(outline->GetOutputPort());
		//vtkSmartPointer<vtkActor> outlineActor = vtkSmartPointer<vtkActor>::New();
		//outlineActor->SetMapper(outlineMapper);
		//renderer->AddActor(outlineActor);

		meshes[z].opacity = 1.0;	// myopacity

		meshes[z].color.r = r;
		meshes[z].color.g = g;
		meshes[z].color.b = b;

		strcpy(meshes[z].name , s->getGroupNames()[z].c_str());

		meshes[z].actor = vtkSmartPointer<vtkActor>::New();
		meshes[z].actor->SetMapper(mapper);
		meshes[z].actor->GetProperty()->SetInterpolationToPhong();

		meshes[z].actor->GetProperty()->BackfaceCullingOff();
		
		//meshes[z].actor->GetProperty()->EdgeVisibilityOn();
		meshes[z].actor->GetProperty()->SetSpecular(.3);
		meshes[z].actor->GetProperty()->SetSpecularColor(0.5, 0.5, 0.5);
		//meshes[z].actor->GetProperty()->SetSpecularPowerle(80);

		//meshes[z].actor->GetProperty()->SetAmbientColor(1.0, 1.0, 1.0);
		//meshes[z].actor->GetProperty()->SetAmbient(0.1);
		
		meshes[z].actor->GetProperty()->SetOpacity(1.0);	// myopacity
		meshes[z].actor->GetProperty()->SetDiffuseColor(r, g, b);
		//meshes[z].actor->GetProperty()->SetDiffuse(1.0);
		//meshes[z].actor->GetProperty()->FrontfaceCullingOn();	// culling on
		//meshes[z].actor->GetProperty()->SetTexture(0, colorTexture); 

		//vtkObject::GlobalWarningDisplayOff();	// dangerous (keep on for most part)

		mouse[0] = 1;
		mouse[1] = 1;
		mouse[2] = 1;

		//print_statusbar(meshes[z].actor->GetProperty()->GetInterpolationAsString());
		//int index = 0;
		//float elems [16];

		//for (int i = 0; i < 4; i++)
		//	for (int j = 0; j < 4; j++)
		//	{
		//		 //elems[index] = (float) mat->GetElement(i, j);
		//		elems[index] = 1.0f;
		//		index++;
		//	} 
		
//		meshes[z].actor->GetProperty()->
		meshes[z].actor->GetProperty()->AddShaderVariable("mouse",  mouse[0], mouse[1], mouse[2]);
		meshes[z].actor->GetProperty()->AddShaderVariable("translucency",  meshes[z].opacity);
		meshes[z].actor->GetProperty()->AddShaderVariable("peerInside",  peerInside);

		//meshes[z].actor->GetProperty()->AddShaderVariable("mat", 1, elems);

		//meshes[z].actor->GetProperty()->AddShaderVariable()

		meshes[z].actor->GetProperty()->AddShaderVariable("toon",  toon);
		meshes[z].actor->GetProperty()->AddShaderVariable("selected",  0);
		meshes[z].actor->GetProperty()->AddShaderVariable("mouseSize",  mouseSize);
		
		meshes[z].selected = 0;

		//((vtkPolydata*)meshes[z].actor->GetMapper()->GetInput())->SetFieldData(fieldData);
		//( (vtkPolyData *) (meshes[z].actor->GetMapper()->GetInput()) )->SetFieldData(fieldData);
		//meshes[z].actor->GetProperty()->AddShaderVariable("mat", 4, elems);
		
		updateMat(z);

		//updateMouseShader();

		//float spotDir[] = {0, 0, -1};
		//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);

		//meshes[z].actor->GetProperty()->SetLighting

		//meshes[z].actor->GetProperty()->AddShaderVariable("myColor",  meshes[z].color.r, meshes[z].color.g, meshes[z].color.b);
		//meshes[z].actor->GetProperty()->AddShaderVariable("lightDirs",  0, 0, 0);

		//if (z == 0)
		//{

		//	}

		//meshes[z].actor->GetProperty()->AddShaderVariable("mambient",  mambient.r, mambient.g, mambient.b);
		//meshes[z].actor->GetProperty()->AddShaderVariable("mdiffuse",  mdiffuse.r, mdiffuse.g, mdiffuse.b);
		//meshes[z].actor->GetProperty()->AddShaderVariable("mspecular",   mspecular.r, mspecular.g, mspecular.b);

		//meshes[z].actor->GetProperty()->AddShaderVariable("lambient",  lambient.r, lambient.g, lambient.b);
		//meshes[z].actor->GetProperty()->AddShaderVariable("ldiffuse",  ldiffuse.r, ldiffuse.g, ldiffuse.b);
		//meshes[z].actor->GetProperty()->AddShaderVariable("lspecular", lspecular.r, lspecular.g, lspecular.b);

		//meshes[z].actor->GetProperty()->AddShaderVariable("shininess", shininess);
		//meshes[z].actor->GetProperty()->AddShaderVariable("lightPos", lightPos[0], lightPos[1], lightPos[2]);
		//meshes[z].actor->GetProperty()->AddShaderVariable("width", 1.0);

		//
		////float lightDirs[] = {1.0f, 1.0f, 1.0f}; 
		//meshes[z].actor->GetProperty()->AddShaderVariable("image", 0);

		// Read vector stuff (and new obj person)

		//vtkSmartPointer<vtkOBJReader> reader2 = vtkSmartPointer<vtkOBJReader>::New();
		//reader2->SetFileName(fname);
		//reader2->Update();

		//// Visualize
		//vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
		//mapper2->SetInputConnection(reader2->GetOutputPort());

		//vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
		//actor2->SetMapper(mapper2);
		//actor2->GetProperty()->SetTexture(0, colorTexture);

		//vtkPolyData * thepolydata = vtkPolyData::SafeDownCast (meshes[z].actor->GetMapper()->GetInput());
		////vertices.resize(thepolydata->GetNumberOfPoints());
		//
		//static int aa = 0;
		//cout << "verts" << thepolydata->GetNumberOfPoints() << endl;
		//aa +=thepolydata->GetNumberOfPoints();
		//for(vtkIdType i = 0; i < thepolydata->GetNumberOfPoints(); i++)
		//{
		//	double p[3];
		//	thepolydata->GetPoint(i,p);
		//	
		//	//cout << p[0] << "," << p[1] << ", " << p[2] << endl;
		//	
		//}
		//cout  << aa << "hehe";
		////cout <<" seh" << endl;

		//vtkSmartPointer<vtkCellArray> polys = thepolydata->GetPolys();
		//
		//polys->InitTraversal();

		//for (int i = 0; i < polys->GetNumberOfCells(); i++)
		//{
		//	vtkIdType n_pts;
		//	vtkIdType *pts = nullptr;

		//	polys->GetNextCell(n_pts, pts);

		//	for (int j = 0; j < n_pts; j++ )
		//	{
		//		//cout << pts[j];
		//		//cout << ",";
		//	}
		//	//cout << endl;
		//}

		//vtkSmartPointer<vtkFloatArray> polysn = vtkFloatArray::SafeDownCast(thepolydata->GetPointData()->GetNormals());

		//cout << polysn->GetNumberOfTuples() << endl;

		renderer->AddActor(meshes[z].actor);
		renderer->ResetCamera();
		qv->GetRenderWindow()->Render();
	}

	//vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
	//windowToImageFilter->SetInput(renderer->GetRenderWindow());
	//windowToImageFilter->SetMagnification(2); //image quality
	//windowToImageFilter->Update();

	//vtkSmartPointer<vtkContourFilter> contourFilter = vtkSmartPointer<vtkContourFilter>::New();
	//contourFilter->SetInputData(windowToImageFilter->GetOutput());
	//contourFilter->SetInputData( (vtkPolyData*) ( (vtkPolyDataMapper*)(meshes[0].actor->GetMapper)->GetInput() ) );
	//contourFilter->SetValue(0,255);
	//contourFilter->Update();

	////renderer->RemoveAllViewProps();

	//vtkSmartPointer<vtkTransform>transform1 = vtkSmartPointer<vtkTransform>::New();
	//transform1->Scale( 10, 10, 1.);

	//vtkSmartPointer<vtkTransformPolyDataFilter> tfilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	//tfilter1->SetInputData(contourFilter->GetOutput());
	//tfilter1->SetTransform(transform1);
	//tfilter1->Update();

	//vtkSmartPointer<vtkPolyDataMapper> mappp = vtkSmartPointer<vtkPolyDataMapper>::New();
	//mappp->SetInputData(tfilter1->GetOutput());
	//mappp->Update();

	//vtkSmartPointer<vtkActor> actorrr = vtkSmartPointer<vtkActor>::New();
	//actorrr->SetMapper(mappp);
	//
	//renderer->AddActor(actorrr);

	
	// get all stuff
	renderer->AutomaticLightCreationOff();
	renderer->RemoveAllLights();

	vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
	//light->SetLightTypeToSceneLight();
	light->SetLightTypeToCameraLight();
	//light->SetLightTypeToCameraLight();
	light->SetPosition(0, 0, 1);
	
	light->SetPositional(true);
	//light->SetIntensity(5);
	light->SetFocalPoint(0, 0, 0);
	///light->setlight
	//light->SetPosition(0, 0, 0);
	//light->SetPositional(true); // required for vtkLightActor below
	//light->SetConeAngle(10);
	//light->SetFocalPoint(lightFocalPoint[0], lightFocalPoint[1], lightFocalPoint[2]);
	//light->SetDiffuseColor(.5,.5,.5);
	//light->SetAmbientColor(0.8,0.8,0.0);
	//light->SetIntensity(.5);
	//light->SetSpecularColor(1,1,0);
	renderer->AddLight(light);
	//renderer->GetLights()->InitTraversal();
	//vtkLight * x = renderer->GetLights()->GetNextItem();
	//x->SetPosition(100, 0, 0);
	

	//vtkSmartPointer<vtkCleanPolyData> cleanFilter = vtkSmartPointer<vtkCleanPolyData>::New();
	//cleanFilter->SetInputConnection(app->GetOutputPort());
	//cleanFilter->Update();

	//vtkSmartPointer<vtkPolyDataMapper> mappers = vtkSmartPointer<vtkPolyDataMapper>::New();
	//mappers->SetInputData(cleanFilter->GetOutput());
	
	//vtkSmartPointer<vtkActor> actors = vtkSmartPointer<vtkActor>::New();
	//actors->SetMapper(mappers);

	//actors->GetProperty()->LoadMaterial("Phong.xml");
//		actors->GetProperty()->ShadingOn();

	//renderer->AddActor(actors);
	//renderer->ResetCamera();


	cout << "end" << endl;

	//vtkSmartPointer<vtkPointPicker> worldPicker = vtkSmartPointer<vtkPointPicker>::New();
	//vtkSmartPointer<vtkWorldPointPicker> worldPicker = vtkSmartPointer<vtkWorldPointPicker>::New();
	//worldPicker = vtkSmartPointer<vtkCellPicker>::New();
	//cellPicker->SetTolerance(0.00);

	//QApplication::exec();
	//renderWindowInteractor->Start();


	// add meshes groupnames to listbox
	for (int i = 0; i < meshes.size(); i++)
	{
		QListWidgetItem* item = new QListWidgetItem(meshes[i].name, ui.listWidget);
		item->setCheckState(Qt::Checked);
		ui.listWidget->addItem(item);
		//ui.listWidget->itemAt(0, i)->setCheckState(Qt::Checked);
	}

	selectedIndex = 0;
	hoveredIndex = 0;

	//ui.listWidget->item(selectedIndex)->setSelected(true);
	//CommonData::increment = CommonData::currentRenderer->GetActiveCamera()->GetDistance() / 2000;
	//System::Windows::Forms::MessageBox::Show("Increment is " + CommonData::increment);

	//// Transformation
	//vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
	//translation->Scale(.5, .5, .5);
	//translation->Translate(1.0, -56.5, 0.0);

	//vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	//transformFilter->SetInputConnection(reader->GetOutputPort());
	//transformFilter->SetTransform(translation);
	//transformFilter->Update();

	// Triangulation
	////vtkSmartPointer<vtkTriangleFilter> triangleFilter1 = vtkSmartPointer<vtkTriangleFilter>::New();
	//triangleFilter1->SetInputConnection(transformFilter->GetOutputPort());
	//triangleFilter1->Update();

	// UI
	//opacity = ui.horizontalSlider->value() / 100.0;

	//// VTK CSG
	//vtkSmartPointer<vtkBooleanOperationPolyDataFilter> booleanOperation = vtkSmartPointer<vtkBooleanOperationPolyDataFilter>::New();
	//booleanOperation->SetOperationToIntersection();
	//booleanOperation->SetInputConnection(0, polydata1->GetProducerPort());
	//booleanOperation->SetInputConnection(1, polydata2->GetProducerPort());
	//clock_t start_time = clock();	///////////
	Utility::start_clock('z');
	cout << "Start processing data" << endl;

	//int myIndex = 0; // trouble
	//
	//vtkPolyData * thepolydata = static_cast<vtkPolyData *> (meshes.at(myIndex).actor->GetMapper()->GetInput());
	//thepolydata->BuildLinks();

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

	//meshes[myIndex].actor->GetProperty()->SetDiffuseColor(meshes[myIndex].color.r, meshes[myIndex].color.g, meshes[myIndex].color.b);

	//meshes[myIndex].actor->GetProperty()->LoadMaterial("GLSLTwisted.xml");
	//meshes[myIndex].actor->GetProperty()->ShadingOn();

	//meshes[myIndex].actor->GetProperty()->AddShaderVariable("mouse",  mouse[0], mouse[1], mouse[2]);
	//meshes[myIndex].actor->GetProperty()->AddShaderVariable("translucency",  meshes[myIndex].opacity);
	//meshes[myIndex].actor->GetProperty()->AddShaderVariable("peerInside",  peerInside);

	//meshes[myIndex].actor->GetProperty()->AddShaderVariable("toon",  toon);

	//actor->GetProperty()->SetTexture(0, colorTexture);

	//actor->GetProperty()->LoadMaterial("GLSLTwisted.xml");
	//actor->GetProperty()->ShadingOn();

	//actor->GetProperty()->AddShaderVariable("bgl_RenderedTexture", 0);

	//actor->GetProperty()->AddShaderVariable("mambient",  mambient.r, mambient.g, mambient.b);
	//actor->GetProperty()->AddShaderVariable("mdiffuse",  meshes[myIndex].color.r, meshes[myIndex].color.g, meshes[myIndex].color.b);
	//actor->GetProperty()->AddShaderVariable("mspecular",   mspecular.r, mspecular.g, mspecular.b);

	//actor->GetProperty()->AddShaderVariable("lambient",  lambient.r, lambient.g, lambient.b);
	//actor->GetProperty()->AddShaderVariable("ldiffuse",  ldiffuse.r, ldiffuse.g, ldiffuse.b);
	//actor->GetProperty()->AddShaderVariable("lspecular", lspecular.r, lspecular.g, lspecular.b);

	//actor->GetProperty()->AddShaderVariable("shininess", shininess);
	//actor->GetProperty()->AddShaderVariable("lightPos", lightPos[0], lightPos[1], lightPos[2]);
	qDebug() << "replaced \n";
	Utility::end_clock('z');
}



//------------------

void additive::slot_listitemclicked(int i )
{
	//estyle->setPickList(i);

	//estyle->setPickList(i);
	setDiffuseAndOpacity();

	if (selectedIndex > -1 && selectedIndex < meshes.size())
	{
		meshes[selectedIndex].actor->GetProperty()->AddShaderVariable("selected",  0);
		meshes[selectedIndex].selected = 0;
		//((vtkUniformVariables *) meshes[selectedIndex].actor->GetPropertyKeys()->Get(vtkMyShaderPass::UNIFORMS()))->SetUniformi("selected", 1, 0);

	}

	selectedIndex = i;

	if (selectedIndex > -1 && selectedIndex < meshes.size())
	{
		meshes[selectedIndex].actor->GetProperty()->AddShaderVariable("selected",  1);
		meshes[selectedIndex].selected = 1;
	}
	updateOpacitySliderAndList();

	updateDisplay();
}

void additive::updateOpacitySliderAndList()
{
	if (selectedIndex < meshes.size())
	{
		//meshes[selectedIndex].actor->GetProperty()->SetDiffuseColor(1, 0, 0);
		ui.horizontalSlider->setValue(meshes[selectedIndex].opacity * 100);

		if (ui.listWidget->selectedItems().size() > 0)
			ui.listWidget->selectedItems().at(0)->setSelected(false);

		ui.listWidget->item(selectedIndex)->setSelected(true);
		ui.listWidget->scrollToItem(ui.listWidget->selectedItems().at(0));
	}
}
