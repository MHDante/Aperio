#include "stdafx.h"		// Precompiled header files
#include "Utility.h"

// Standard
#include <iostream>
#include <map>
#include <time.h>

// VTK Includes
//#include <vtkShader2Collection.h>

#include "aperio.h"
#include "MyInteractorStyle.h"

#include <vtk/vtkTextureUnitManager.h>

// Utility Variables -------------------------------------------------------------------------
namespace Utility
{
	/// <summary> Map of "clockname->clock_t" pairs used for profiling (for timing chunks of code)
	/// </summary>
	map<char, clock_t> clocks;
}
///---------------------------------------------------------------------------------------------
void Utility::start_clock(char clockname)
{
	clocks[clockname] = clock();
	cout << "--- Started Clock: " << clockname << " --- " << endl;
}
///---------------------------------------------------------------------------------------------
void Utility::end_clock(char clockname)
{
	cout << "--- Ended Clock: " << clockname << ", (elapsed: " << float(clock() - clocks[clockname]) / CLOCKS_PER_SEC << ") ---" << endl;
}
///---------------------------------------------------------------------------------------------
void Utility::messagebox(string text)
{
	MessageBoxA(nullptr, text.c_str(), "", MB_ICONINFORMATION);
}
///---------------------------------------------------------------------------------------------
vtkSmartPointer<vtkActor> Utility::sourceToActor(aperio *ap, vtkSmartPointer<vtkPolyData> source, float r, float g, float b, float a)
{
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(source);
	mapper->Update();

	//vtkPainterPolyDataMapper::SafeDownCast(mapper)->
	//MapDataArrayToVertexAttribute("weights", "weights", vtkDataObject::FIELD_ASSOCIATION_POINTS, -1);

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	// Set actor properties
	actor->GetProperty()->BackfaceCullingOff();
	actor->GetProperty()->SetInterpolationToPhong();

	actor->GetProperty()->SetAmbientColor(r, g, b);
	actor->GetProperty()->SetAmbient(1.0);
	//actor->GetProperty()->SetColor(r, g, b);	// sets gl_Color? (Both ambient and diffuse colours are affected) - set to white for textures

	actor->GetProperty()->SetDiffuseColor(r, g, b);
	actor->GetProperty()->SetDiffuse(1.0);

	actor->GetProperty()->SetSpecular(0.30);
	actor->GetProperty()->SetSpecularPower(128);

	actor->GetProperty()->SetOpacity(a);	// myopacity	// TODO: REMEMBER TO CHANGE THIS TO CORRECT FACTOR (in k call)

	// Create main shader (does not build since no main function, just funcPropVS)
	//vtkSmartPointer<vtkShaderProgram2> pgm = Utility::makeShader(ap->renderer->GetRenderWindow(), "shader_water.vert", "shader.frag");
	//vtkSmartPointer<vtkOpenGLProperty> openGLproperty = static_cast<vtkOpenGLProperty*>(actor->GetProperty());
	//openGLproperty->SetPropProgram(pgm);
	//openGLproperty->ShadingOn();

	return actor;
}
///---------------------------------------------------------------------------------------------
void Utility::generateTexCoords(vtkSmartPointer<vtkPolyData> source)
{
	vtkSmartPointer<vtkFloatArray> textureCoordinates = vtkSmartPointer<vtkFloatArray>::New();
	textureCoordinates->SetNumberOfComponents(2);
	textureCoordinates->Allocate(2 * source->GetNumberOfPoints());

	float tuple[2] = { 0.0, 0.0 };
	for (int i = 0; i < source->GetNumberOfPoints(); i++)
	{
		if ((i % 2) == 0)
		{
			tuple[0] = 0; tuple[1] = 0;
		}
		else if ((i % 3) == 0)
		{
			tuple[0] = 1; tuple[1] = 0;
		}
		else
		{
			tuple[0] = 0; tuple[1] = 1;
		}

		textureCoordinates->InsertNextTuple(tuple);
	}
	source->GetPointData()->SetTCoords(textureCoordinates);
}

//---------------------------------------------------------------------------------------------------
vtkSmartPointer<vtkShaderProgram2> Utility::makeShader(vtkRenderWindow *context, string vert, string frag)
{
	vtkSmartPointer<vtkShaderProgram2> shaderProgram = vtkSmartPointer<vtkShaderProgram2>::New();

	vtkSmartPointer<vtkShader2> shader = vtkSmartPointer<vtkShader2>::New();
	vtkSmartPointer<vtkShader2> shader2 = vtkSmartPointer<vtkShader2>::New();

	if (vert != "")
	{
		ifstream file1(vert);
		stringstream buffer1;
		buffer1 << file1.rdbuf();

		shader->SetSourceCode(buffer1.str().c_str());
		shader->SetType(VTK_SHADER_TYPE_VERTEX);
		shaderProgram->GetShaders()->AddItem(shader);
	}
	if (frag != "")
	{
		ifstream file2(frag);
		stringstream buffer2;
		buffer2 << file2.rdbuf();

		shader2->SetSourceCode(buffer2.str().c_str());
		shader2->SetType(VTK_SHADER_TYPE_FRAGMENT);
		shaderProgram->GetShaders()->AddItem(shader2);
	}
	shaderProgram->SetContext(context);

	shader->SetContext(shaderProgram->GetContext());
	shader2->SetContext(shaderProgram->GetContext());

	shaderProgram->Build();

	if (shaderProgram->GetLastBuildStatus() == VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
	{
	}
	else
	{
		cout << "Shader has errors.\n" << shaderProgram->GetLastLinkLog() << "\n";
	}
	return shaderProgram;
}
//-------------------------------------------------------------------------------------------------------------------
void Utility::updateShader(vtkShaderProgram2* shaderProgram, string vert, string frag)
{
	if (shaderProgram == nullptr)
		return;

	if (vert != "")
	{
		ifstream file(vert);
		stringstream buffer;
		buffer << file.rdbuf();
		vtkShader2::SafeDownCast(shaderProgram->GetShaders()->GetItemAsObject(0))->SetSourceCode(buffer.str().c_str());
	}
	if (frag != "")
	{
		ifstream file(frag);
		stringstream buffer;
		buffer << file.rdbuf();
		vtkShader2::SafeDownCast(shaderProgram->GetShaders()->GetLastShader())->SetSourceCode(buffer.str().c_str());
	}
	shaderProgram->Build();
}

//--------------------------------------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Utility::assimpOBJToVtkPolyData(aiMesh *mesh)
{
	//Setup point coordinates
	vtkSmartPointer<vtkPolyData> mypolydata = vtkSmartPointer<vtkPolyData>::New();

	// Create points
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetNumberOfPoints(mesh->mNumVertices);	// allocate memory

	for (int j = 0; j < mesh->mNumVertices; j++)
	{
		points->SetPoint(j, mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
	}

	// Create polygons (faces)
	vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();

	for (int j = 0; j < mesh->mNumFaces; j++)
	{
		polygon->GetPointIds()->SetNumberOfIds(mesh->mFaces[j].mNumIndices);

		for (int k = 0; k < mesh->mFaces[j].mNumIndices; k++)
			polygon->GetPointIds()->SetId(k, mesh->mFaces[j].mIndices[k]);

		polygons->InsertNextCell(polygon);
	}

	// Successful copy
	if (points->GetNumberOfPoints() > 0)
	{
		mypolydata->SetPoints(points);
		mypolydata->SetPolys(polygons);

		// Create texture coordinates
		//Utility::generateTexCoords(polydata);
		//vtkSmartPointer<vtkTextureMapToSphere> map = vtkSmartPointer<vtkTextureMapToSphere>::New();
		//map->SetAutomaticSphereGeneration(true);
		//map->SetInputData(mypolydata);
		//map->Update();
		//mypolydata = map->GetPolyDataOutput();
	}
	return mypolydata;
}
//--------------------------------------------------------------------------------------------------------
weak_ptr<CustomMesh> Utility::addMesh(aperio *a, vtkSmartPointer<vtkPolyData> source, string groupname, vtkColor3f color, float opacity, shared_ptr<CustomMesh> parentMesh, shared_ptr<CustomMesh> oldMesh)
{
	// Add mesh to custom meshes vector
	a->meshes.push_back(make_shared<CustomMesh>());
	auto customMesh = a->meshes.back();

	customMesh->color.Set(color.GetRed(), color.GetGreen(), color.GetBlue());
	customMesh->name = groupname;
	customMesh->selected = false;

	// Add cell locator for mesh to cellpicker and to mesh
	customMesh->cellLocator = vtkSmartPointer<vtkCellLocator>::New();
	customMesh->cellLocator->SetDataSet(source);
	customMesh->cellLocator->BuildLocator();
	customMesh->cellLocator->LazyEvaluationOn();

	a->interactorstyle->cellPicker->AddLocator(customMesh->cellLocator);

	// ------ Make OBB Tree
	double corner[3], max[3], mid[3], min[3], size[3];
	vtkSmartPointer<vtkOBBTree> objectOBBTree = vtkSmartPointer<vtkOBBTree>::New();
	objectOBBTree->SetDataSet(source);
	objectOBBTree->ComputeOBB(source->GetPoints(), corner, max, mid, min, size);
	vtkVector3f meshSize = vtkVector3f(size[0], size[1], size[2]);

	vtkSmartPointer<vtkPolyData> polydataOBB = vtkSmartPointer<vtkPolyData>::New();
	objectOBBTree->BuildLocator();
	objectOBBTree->GenerateRepresentation(0, polydataOBB);
	vtkSmartPointer<vtkActor> actorOBB = Utility::sourceToActor(a, polydataOBB);

	customMesh->cornerOBB = vtkVector3f(corner[0], corner[1], corner[2]);
	customMesh->axesOBB[0] = vtkVector3f(max[0], max[1], max[2]); customMesh->axesOBB[0].Normalize();
	customMesh->axesOBB[1] = vtkVector3f(mid[0], mid[1], mid[2]); customMesh->axesOBB[1].Normalize();
	customMesh->axesOBB[2] = vtkVector3f(min[0], min[1], min[2]); customMesh->axesOBB[2].Normalize();
	customMesh->sizeOBB = vtkVector3f(size[0], size[1], size[2]);
	customMesh->actorOBB = actorOBB;
	customMesh->actorOBB->VisibilityOff();
	customMesh->actorOBB->PickableOff();
	//a->renderer->AddActor(customMesh->actorOBB);


	customMesh->size[0] = size[0];
	customMesh->size[1] = size[1];
	customMesh->size[2] = size[2];

	// Compute OBB's Center of Mass
	vtkSmartPointer<vtkCenterOfMass> centerOfMassFilter = vtkSmartPointer<vtkCenterOfMass>::New();
	centerOfMassFilter->SetInputData(polydataOBB);
	centerOfMassFilter->SetUseScalarsAsWeights(false);
	centerOfMassFilter->Update();
	double center[3];
	centerOfMassFilter->GetCenter(center);

	customMesh->center[0] = center[0];
	customMesh->center[1] = center[1];
	customMesh->center[2] = center[2];

	// ----- If there is an oldMesh available (inputted), copy its path properties
	if (oldMesh != nullptr)
	{
		customMesh->elem = oldMesh->elem.lock();
		customMesh->path_angle = oldMesh->path_angle;
		customMesh->path_initpt_exact = oldMesh->path_initpt_exact;
		customMesh->path_leafangle = oldMesh->path_leafangle;
		customMesh->path_pt = oldMesh->path_pt;
		customMesh->pathType = oldMesh->pathType;
	}
	// ----- Make mapper and actors
	customMesh->actor = Utility::sourceToActor(a, source, color.GetRed(), color.GetGreen(), color.GetBlue(), opacity);

	setMeshOpacity(a, customMesh, opacity);

	customMesh->generated = false;

	customMesh->parentMesh = parentMesh;

	// Add actor to renderer and to list
	a->renderer->AddActor(customMesh->actor);
	a->addToList(customMesh->name);

	//a->clearSelectedMeshes();

	return customMesh;
}
//-----------------------------------------------------------------------------------------------
void Utility::removeMesh(aperio *a, weak_ptr<CustomMesh> mesh)
{
	auto it = a->getMeshIterator(mesh);

	if (it != a->meshes.end())
	{
		auto actualMesh = *it;

		// Remove from list
		a->RemoveFromList(actualMesh->name);

		// Remove from renderer
		a->renderer->RemoveActor(actualMesh->actor);

		// Remove from meshes vector
		a->meshes.erase(it);
	}
}
//-----------------------------------------------------------------------------------------------
void Utility::setMeshOpacity(aperio *a, weak_ptr<CustomMesh> mesh, float opacity)
{
	setMeshOpacitySeparate(a, mesh, opacity, opacity);
}
//-----------------------------------------------------------------------------------------------
void Utility::setMeshOpacitySeparate(aperio *a, weak_ptr<CustomMesh> mesh, float opacity, float actorOpacity)
{
	auto actualMesh = mesh.lock();

	if (actualMesh)
	{
		// Set opacity field
		actualMesh->opacity = opacity;

		auto actor = actualMesh->actor;

		// Set actor opacity
		if (actor)
			actor->GetProperty()->SetOpacity(actorOpacity);
	}
}
//-----------------------------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Utility::computeNormals(vtkSmartPointer<vtkPolyData> source)
{
	vtkSmartPointer<vtkPolyDataNormals> dataset = vtkSmartPointer<vtkPolyDataNormals>::New();
	dataset->SetInputData(source);
	dataset->ComputePointNormalsOn();
	dataset->ComputeCellNormalsOff();
	dataset->SplittingOn();
	dataset->SetConsistency(true);
	dataset->SetFeatureAngle(60);
	dataset->Update();

	return dataset->GetOutput();
}
//-------------------------------------------------------------------------------------------------------------------------
#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

void Utility::get_bounding_box_for_node(const aiScene* scene, const aiNode* nd, aiVector3D* min, aiVector3D* max)
{
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {
			aiVector3D tmp = mesh->mVertices[t];

			min->x = aisgl_min(min->x, tmp.x);
			min->y = aisgl_min(min->y, tmp.y);
			min->z = aisgl_min(min->z, tmp.z);

			max->x = aisgl_max(max->x, tmp.x);
			max->y = aisgl_max(max->y, tmp.y);
			max->z = aisgl_max(max->z, tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(scene, nd->mChildren[n], min, max);
	}
}
//----------------------------------------------------------------------------------------------
void Utility::get_bounding_box(const aiScene* scene, aiVector3D* min, aiVector3D* max)
{
	min->x = min->y = min->z = 1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene, scene->mRootNode, min, max);
}
//-----------------------------------------------------------------------------------------------
vtkSmartPointer<vtkActor> Utility::plotLine(aperio *a, vtkVector3f p1, vtkVector3f p2, float radius, float r, float g, float b)
{
	vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
	pts->InsertNextPoint(p1.GetData());
	pts->InsertNextPoint(p2.GetData());

	vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
	int num_pts = 2;
	polyLine->GetPointIds()->SetNumberOfIds(num_pts);
	for (unsigned int i = 0; i < num_pts; i++)
		polyLine->GetPointIds()->SetId(i, i);

	// Create a cell array to store the lines in and add the lines to it
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(polyLine);

	// Create a polydata to store everything in
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(pts);
	polyData->SetLines(cells);

	// Create tube filter 
	vtkSmartPointer<vtkTubeFilter> tubefilter = vtkSmartPointer<vtkTubeFilter>::New();
	tubefilter->SetInputData(polyData);
	tubefilter->SetNumberOfSides(3);
	tubefilter->SetRadius(radius);
	tubefilter->Update();

	auto actor = Utility::sourceToActor(a, tubefilter->GetOutput(), r, g, b, 1);
	a->renderer->AddActor(actor);

	return actor;
}
//-----------------------------------------------------------------------------------------------
vtkSmartPointer<vtkActor> Utility::plotSphere(aperio *a, float x, float y, float z, float size, float r, float g, float b)
{
	vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
	sphere->SetRadius(size);
	sphere->Update();

	vtkSmartPointer<vtkActor> actor = Utility::sourceToActor(a, sphere->GetOutput(), 1, 1, 1, 1);
	actor->GetProperty()->SetColor(r, g, b);
	actor->SetPosition(x, y, z);

	a->renderer->AddActor(actor);

	return actor;
}
//---------------------------------------------------------------------------------------------------
vtkSmartPointer<vtkCellArray> Utility::generateLinesFromPoints(vtkSmartPointer<vtkPoints> points)
{
	// Generate lines from points
	int num_pts = points->GetNumberOfPoints();

	vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
	polyLine->GetPointIds()->SetNumberOfIds(num_pts);
	for (unsigned int i = 0; i < num_pts; i++)
		polyLine->GetPointIds()->SetId(i, i);

	// Create a cell array to store the lines in and add the lines to it
	vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
	lines->InsertNextCell(polyLine);

	return lines;
}
//---------------------------------------------------------------------------------
vtkSmartPointer<vtkImageData> Utility::getImageData(string filename)
{
	vtkSmartPointer<vtkImageData> imageData;

	// Transforms (in-place) filename to lowercase
	std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

	if (filename.substr(filename.length() - 4) == ".jpg")
	{
		vtkSmartPointer<vtkJPEGReader> jPEGReader = vtkSmartPointer<vtkJPEGReader>::New();
		jPEGReader->SetFileName(filename.c_str());
		jPEGReader->Update();
		imageData = jPEGReader->GetOutput();
	}
	else	// Otherwise .PNG
	{
		vtkSmartPointer<vtkPNGReader> pngReader = vtkSmartPointer<vtkPNGReader>::New();
		pngReader->SetFileName(filename.c_str());
		pngReader->Update();
		imageData = pngReader->GetOutput();
	}
	return imageData;
}
