#include "stdafx.h"		// Precompiled header files
#include "Utility.h"

// Standard
#include <iostream>
#include <map>
#include <time.h>

// VTK Includes
#include <vtkShader2Collection.h>

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
vtkSmartPointer<vtkActor> Utility::sourceToActor(vtkSmartPointer<vtkPolyData> source, float r, float g, float b, float a)
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
vtkSmartPointer<vtkShaderProgram2> Utility::buildShader(vtkRenderWindow *context, std::string vert, std::string frag)
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
		std::cout << "Shader has errors.\n";
	}
	return shaderProgram;
}
//-------------------------------------------------------------------------------------------------------------------
void Utility::updateShader(vtkShaderProgram2* shaderProgram, std::string vert, std::string frag)
{

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
vtkSmartPointer<vtkPolyData> Utility::objToVtkPolyData(tinyobj::shape_t &shape)
{
	// Create points
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetNumberOfPoints(shape.mesh.positions.size());

	for (size_t v = 0; v < shape.mesh.positions.size() / 3; v++)
	{
		points->SetPoint(v, shape.mesh.positions[3 * v + 0], shape.mesh.positions[3 * v + 1], shape.mesh.positions[3 * v + 2]);
	}
	// Create faces
	vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();

	vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
	polygon->GetPointIds()->SetNumberOfIds(3);

	for (size_t f = 0; f < shape.mesh.indices.size(); f += 3)
	{
		polygon->GetPointIds()->SetId(0, shape.mesh.indices[f + 0]);
		polygon->GetPointIds()->SetId(1, shape.mesh.indices[f + 1]);
		polygon->GetPointIds()->SetId(2, shape.mesh.indices[f + 2]);

		polygons->InsertNextCell(polygon);
	}
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();

	// Make polygon data
	if (points->GetNumberOfPoints() > 0)
	{
		polydata->SetPoints(points);
		polydata->SetPolys(polygons);

		// Create texture coordinates
		//Utility::generateTexCoords(polydata);
	}
	return polydata;
}

