#include "stdafx.h"		// Precompiled header files
#include "Utility.h"

// Standard
#include <iostream>
#include <map>
#include <time.h>

#include <vtkPainterPolyDataMapper.h>
#include <vtkOpenGLProperty.h>

#include "vtkMyShaderPass.h"
#include <vtkShader2.h>
#include <vtkShaderProgram2.h>
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
//		RemoveAllVertexAttributeMappings();

	//vtkPainterPolyDataMapper::SafeDownCast(mapper)->
		//MapDataArrayToVertexAttribute("weights", "weights", vtkDataObject::FIELD_ASSOCIATION_POINTS, -1);

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	// Set actor properties
	actor->GetProperty()->BackfaceCullingOff();
	actor->GetProperty()->SetInterpolationToPhong();

	//actor->GetProperty()->SetAmbientColor(r, g, b);
	actor->GetProperty()->SetAmbientColor(r /1.0, g /1.0, b / 1.4);
	actor->GetProperty()->SetAmbient(1.0);
	//actor->GetProperty()->SetColor(r, g, b);	// sets gl_Color? (Both ambient and diffuse colours are affected) - set to white for textures

	actor->GetProperty()->SetDiffuseColor(r, g, b);
	actor->GetProperty()->SetDiffuse(1.0);

	actor->GetProperty()->SetSpecular(0.30);
	actor->GetProperty()->SetSpecularPower(128);

	actor->GetProperty()->SetOpacity(a);	// myopacity	// TODO: REMEMBER TO CHANGE THIS TO CORRECT FACTOR
	
	

	//actor->GetProperty()->SetEdgeVisibility(true);
	//actor->GetProperty()->SetEdgeColor(125, 125, 125);
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

	//vtkSmartPointer<vtkTextureMapToCylinder> map = vtkSmartPointer<vtkTextureMapToCylinder>::New();
	//map->SetInputData(source);
	//map->AutomaticCylinderGenerationOn();
	//map->Update();
	
	//source->GetPointData()->SetTCoords(map->GetOutput()->GetPointData()->GetTCoords());
	//source->GetPointData()->SetTCoords(map->GetOutput());
	//vtkPolyData * p = vtkPolyData::SafeDownCast(map->GetOutput());
	
	//source.Take(p);
}

