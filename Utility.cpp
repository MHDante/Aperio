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

#include <vtkTextureUnitManager.h>

// Utility Variables -------------------------------------------------------------------------
namespace Utility
{
	/// <summary> Map of "clockname->clock_t" pairs used for profiling (for timing chunks of code)
	/// </summary>
	std::map<char, clock_t> clocks;
}
///---------------------------------------------------------------------------------------------
void Utility::start_clock(char clockname)
{
	clocks[clockname] = clock();
	std::cout << "--- Started Clock: " << clockname << " --- " << std::endl;
}
///---------------------------------------------------------------------------------------------
void Utility::end_clock(char clockname)
{
	std::cout << "--- Ended Clock: " << clockname << ", (elapsed: " << float(clock() - clocks[clockname]) / CLOCKS_PER_SEC << ") ---" << std::endl;
}
///---------------------------------------------------------------------------------------------
void Utility::messagebox(std::string text)
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
vtkSmartPointer<vtkShaderProgram2> Utility::makeShader(vtkRenderWindow *context, std::string vert, std::string frag)
{
	vtkSmartPointer<vtkShaderProgram2> shaderProgram = vtkSmartPointer<vtkShaderProgram2>::New();

	vtkSmartPointer<vtkShader2> shader = vtkSmartPointer<vtkShader2>::New();
	vtkSmartPointer<vtkShader2> shader2 = vtkSmartPointer<vtkShader2>::New();

	if (vert != "")
	{
		std::ifstream file1(vert);
		std::stringstream buffer1;
		buffer1 << file1.rdbuf();

		shader->SetSourceCode(buffer1.str().c_str());
		shader->SetType(VTK_SHADER_TYPE_VERTEX);
		shaderProgram->GetShaders()->AddItem(shader);
	}
	if (frag != "")
	{
		std::ifstream file2(frag);
		std::stringstream buffer2;
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
		
		std::cout << "Shader has errors.\n" << shaderProgram->GetLastLinkLog() << "\n";
	}	
	return shaderProgram;
}
//-------------------------------------------------------------------------------------------------------------------
void Utility::updateShader(vtkShaderProgram2* shaderProgram, std::string vert, std::string frag)
{
	if (shaderProgram == nullptr)
		return;

	if (vert != "")
	{
		std::ifstream file(vert);
		std::stringstream buffer;
		buffer << file.rdbuf();
		vtkShader2::SafeDownCast(shaderProgram->GetShaders()->GetItemAsObject(0))->SetSourceCode(buffer.str().c_str());
	}
	if (frag != "")
	{
		std::ifstream file(frag);
		std::stringstream buffer;
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
CustomMesh& Utility::addMesh(aperio *a, vtkSmartPointer<vtkPolyData> source, int z, std::string groupname, vtkColor3f color, float opacity)
{
	// Add mesh to custom meshes vector
	a->meshes.push_back(CustomMesh());
	a->meshes[z].opacity = opacity;	// myopacity

	a->meshes[z].color.Set(color.GetRed(), color.GetGreen(), color.GetBlue());
	a->meshes[z].name = groupname;
	a->meshes[z].selected = false;

	// Add cell locator for mesh to cellpicker and to mesh
	a->meshes[z].cellLocator = vtkSmartPointer<vtkCellLocator>::New();
	a->meshes[z].cellLocator->SetDataSet(source);
	a->meshes[z].cellLocator->BuildLocator();
	a->meshes[z].cellLocator->LazyEvaluationOn();

	a->interactorstyle->cellPicker->AddLocator(a->meshes[z].cellLocator);

	// Make mapper and actors
	a->meshes[z].actor = Utility::sourceToActor(a, source, color.GetRed(), color.GetGreen(), color.GetBlue(), opacity);

	a->meshes[z].generated = false;

	return a->meshes[z];
}
//-----------------------------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Utility::computeNormals(vtkSmartPointer<vtkPolyData> source)
{
	vtkSmartPointer<vtkPolyDataNormals> dataset = vtkSmartPointer<vtkPolyDataNormals>::New();
	dataset->SetInputData(source);
	dataset->ComputePointNormalsOn();
	dataset->ComputeCellNormalsOff();
	dataset->SplittingOn();
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
///-------------------------------------------------------------------------------------------------
/// <summary> Compute smooth normals for a vtkPolyData source (usually after computeNormals)
/// </summary>
//-------------------------------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Utility::smoothNormals(vtkSmartPointer<vtkPolyData> source)
{
	// Compute smooth normals for extra smooth lighting :)
	vtkSmartPointer<vtkPolyData> dataset = source;
	dataset->BuildCells();
	dataset->BuildLinks();

	// Smooth Normals Calculation (Interpolate normals from the point's neighbouring faces/triangles)
	vtkSmartPointer<vtkDoubleArray> points_normal = vtkSmartPointer<vtkDoubleArray>::New();
	points_normal->SetNumberOfComponents(3); //3d normals (ie x,y,z)
	points_normal->SetNumberOfTuples(dataset->GetNumberOfPoints());

	std::vector<vtkVector3f>* normal_buffer = new std::vector<vtkVector3f>[dataset->GetNumberOfPoints()];

	vtkSmartPointer<vtkCellArray> polys = dataset->GetPolys();
	polys->InitTraversal();

	for (int i = 0; i < dataset->GetPolys()->GetNumberOfCells(); i++)
	{
		vtkIdType n_pts;
		vtkIdType *pts = nullptr;
		polys->GetNextCell(n_pts, pts);

		vtkVector3f p1 = vtkVector3f(dataset->GetPoint(pts[0])[0], dataset->GetPoint(pts[0])[1], dataset->GetPoint(pts[0])[2]);
		vtkVector3f p2 = vtkVector3f(dataset->GetPoint(pts[1])[0], dataset->GetPoint(pts[1])[1], dataset->GetPoint(pts[1])[2]);
		vtkVector3f p3 = vtkVector3f(dataset->GetPoint(pts[2])[0], dataset->GetPoint(pts[2])[1], dataset->GetPoint(pts[2])[2]);

		vtkVector3f v1 = vtkVector3f(p2.GetX() - p1.GetX(), p2.GetY() - p1.GetY(), p2.GetZ() - p1.GetZ());
		vtkVector3f v2 = vtkVector3f(p3.GetX() - p1.GetX(), p3.GetY() - p1.GetY(), p3.GetZ() - p1.GetZ());

		vtkVector3f normal = v1.Cross(v2);
		normal.Normalize();

		normal_buffer[pts[0]].push_back(normal);
		normal_buffer[pts[1]].push_back(normal);
		normal_buffer[pts[2]].push_back(normal);
	}

	// Now loop through each vertex vector, and average out all the normals stored.
	for (int i = 0; i < dataset->GetNumberOfPoints(); ++i)
	{
		vtkVector3f sum = vtkVector3f(0, 0, 0);

		for (int j = 0; j < normal_buffer[i].size(); ++j)
		{
			sum = vtkVector3f(sum.GetX() + normal_buffer[i][j].GetX(),
				sum.GetY() + normal_buffer[i][j].GetY(),
				sum.GetZ() + normal_buffer[i][j].GetZ());
		}

		sum = vtkVector3f(sum.GetX() / normal_buffer[i].size(),
			sum.GetY() / normal_buffer[i].size(),
			sum.GetZ() / normal_buffer[i].size());


		points_normal->SetTuple3(i, sum.GetX(), sum.GetY(), sum.GetZ());
	}
	dataset->GetPointData()->SetNormals(points_normal);

	//cleanup
	delete[] normal_buffer;


	return dataset;
}