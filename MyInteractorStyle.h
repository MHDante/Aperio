// *******************************************************************************
// Interactor Style - Interactor for QVTKWidget, includes keypresses, mouse
//					  drags, any interaction on-screen (potentially multi-touch)
//					  May split .cpp file
// *******************************************************************************

#ifndef __MYINTERACTORSTYLE_H
#define __MYINTERACTORSTYLE_H

// Inherits from
#include "vtkInteractorStyleTrackballCamera.h"

class aperio;	// Forward declarations

//-----------------------------------------------------------------------------------------
/// <summary> Class MyInteractorStyle, contains direct key press, mouse events etc.
/// </summary>
class MyInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	/// <summary> Pointer to the mainwindow class so we can access instance variables, etc. </summary>
	aperio * a;
	vtkSmartPointer<vtkCellPicker> cellPicker;

private:
	unsigned int NumberOfClicks;	// For double clicks
	unsigned int NumberOfClicksR;	// For double clicks (right-button)
	int PreviousPosition[2];
	int PreviousPositionR[2];		// For right-button
	int ResetPixelDistance;

	clock_t lastClickTimeR; 


	double mouseNorms[5][3];
	int mouseNormI = 0;

	bool dragging;
	bool creation;

	void avgNormal(double *currentN, double *averageN);

public:

	//--------------------------------------------------------------------------------------------------
	/// <summary> Creates a new instance (Factory creator declaration) - definition is outside of class
	/// </summary>
	static MyInteractorStyle* New();

	MyInteractorStyle();

	//--------------------------------------------------------------------------------------------------
	/// <summary> Used to initialize a pointer to the main QT window
	/// </summary>
	void initialize(aperio *window);

	//--------------------------------------------------------------------------------------------------
	/// <summary> Get Outline Actor
	/// </summary>
	vtkSmartPointer<vtkActor> GetOutlineActor();

	//--------------------------------------------------------------------------------------------------
	/// <summary> Get Outline
	/// </summary>
	vtkOutlineSource* GetOutline()
	{
		return Outline;
	}

	//--------------------------------------------------------------------------------------------------
	void setPickList(int z);
	vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleTrackballCamera);

	/// ---------------------------------------------------------------------------------------------
	/// <summary> Called on key press (handles QT key events)
	/// </summary>
	virtual void OnKeyPress() override;

	//----------------------------------------------------------------------------
	/// <summary>
	/// Called when [mouse move].
	/// </summary>
	/// ----------------------------------------------------------------------------
	virtual void OnMouseMove()  override;

	//----------------------------------------------------------------------------
	/// <summary>
	/// Called when [mouse left button click].
	/// </summary>
	/// ----------------------------------------------------------------------------
	virtual void OnLeftButtonDown() override;
	////----------------------------------------------------------------------------
	/// <summary> Called when [left button up].
	/// </summary>
	virtual void OnLeftButtonUp() override;
	/// <summary> Called when [mouse right button click].
	/// </summary>
	virtual void OnRightButtonDown() override;
	//----------------------------------------------------------------------------
	virtual void OnMouseWheelForward()  override;
	//----------------------------------------------------------------------------
	virtual void OnMouseWheelBackward() override;
	//----------------------------------------------------------------------------
	virtual void OnChar() override;
	//----------------------------------------------------------------------------

	///<summary> Override the following with same code but removing Render() call to limit framerate
	///</summary>
	virtual void Rotate() override;
	//----------------------------------------------------------------------------
	virtual void Dolly(double factor) override;
	//----------------------------------------------------------------------------
	virtual void Pan() override;
	//----------------------------------------------------------------------------
	virtual void Spin() override;

	//----------------------------------------------------------------------------

	// Custom Methods
	void forwardLeftButtonDown();
};
#endif