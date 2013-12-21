#ifndef ADDITIVE_H
#define ADDITIVE_H

#include <QtGui/QMainWindow>
#include "ui_additive.h"

#include <QtGui/QPixmap>

// Custom includes
#include "ViewerWidget.h" 

class additive : public QMainWindow
{
	Q_OBJECT

public:
	additive(QWidget *parent = 0, Qt::WFlags flags = 0);
	~additive();

	// Custom variables
	QLabel * _status_label;
	QListWidget* _listWidget;
	ViewerWidget* _viewWidget;
	QRect _orig_size;
	bool preview;
	QString path;

	// Custom methods
	void additive::resizeEvent(QResizeEvent * event);
	void additive::resizeInternal(const QSize &newWindowSize, bool use_orig_size);
	void additive::update_orig_size();

	// ------------------------------------------------------------------------
	/// <summary> Changes label in status bar to a new message
	/// </summary>
	/// <param name="text">The message.</param>
	void print_statusbar(std::string text)
	{
		_status_label->setText(text.c_str());
		QApplication::processEvents();
	}

	// ************* PUBLIC SLOTS *******************************************************************************
	public slots:
	void slot_afterShowWindow();
	
	// ------------------------------------------------------------------------
	/// <summary> Slot called when File->Exit clicked
	/// </summary>
	void slot_exit()
	{
		//QMessageBox::QPushButton reply;
		QMessageBox mb(this);
		mb.setStyleSheet("color: white; background: black; selection-color: black;");
		mb.setWindowOpacity(0.9);
		mb.setWindowTitle(this->windowTitle());
		mb.setText("Are you sure you want to quit?");
		mb.setIcon(QMessageBox::Warning);

		QPushButton *yesButton = mb.addButton(tr("Yes"), QMessageBox::ActionRole);
		QPushButton *noButton = mb.addButton(tr("No"), QMessageBox::ActionRole);
		//noButton->setFocus();

		yesButton->setStyleSheet(this->styleSheet());
		noButton->setStyleSheet(this->styleSheet());

		mb.exec();

		if (mb.clickedButton() == yesButton)
			QApplication::quit();
	}
	// ------------------------------------------------------------------------
	/// <summary> Slot called when fullscreen menu item clicked
	/// </summary>
	void slot_fullScreen()
	{
		if (this->isFullScreen())
			this->showNormal();
		else
			this->showFullScreen();
	}

	// ------------------------------------------------------------------------
	/// <summary> Slot called when File->Open clicked
	/// </summary>
	void slot_open()
	{
		if (path.isEmpty())
			path = QDir::currentPath();

		//pause = true;

		QString selectedFilter;
		QFileDialog::Options options;
		
		QFileDialog dialog;

		QString fileName = dialog.getOpenFileName(this,
			"Select a file.", QString(path),
			"OBJ Files (*.obj);;All Files (*)",
			&selectedFilter,
			options);

		//pause = false;

		if (!fileName.isEmpty())
		{
			path = QFileInfo(fileName).path(); // store path for next time

			_viewWidget->loadData(fileName.toStdString());
		}
		else
			;
	}

	// ------------------------------------------------------------------------
	/// <summary> Slot called when opacity slider value changed
	/// </summary>
	/// <param name="i">new opacity value</param>
	void slot_valueChanged(int i)
	{
		//setDiffuseAndOpacity();

		// opacity is perceived opacity, i / 100.0f is actual opacity;
		float actualopacity = i / 100.0f;
		float opacity;

		opacity = actualopacity;// *0.4f;
		if (i >= 100)
			opacity = 1;

		std::cout << i << std::endl;
		// if opacity = 0, disabled
		// if opacity > 0, enabled

		//meshes[selectedIndex].opacity = actualopacity;

		//meshes[selectedIndex].actor->GetProperty()->SetOpacity(opacity);
		//meshes[selectedIndex].actor->GetProperty()->SetDiffuseColor()

		//meshes[selectedIndex].actor->GetProperty()->AddShaderVariable("translucency",  meshes[selectedIndex].opacity);
		//meshes[selectedIndex].actor->GetProperty()->AddShaderVariable("translucency", opacity);

		//for (int z = 0; z < meshes.size(); z++)
		//{
			//if (z != selectedIndex)
			//meshes[z].actor->GetProperty()->AddShaderVariable("translucency",  1.0);
		//}
		//renderer->ResetCameraClippingRange();
		//ResetCameraClippingRange
		//updateDisplay();
	}

	// ------------------------------------------------------------------------
	/// <summary> Slot called when the Translucency button clicked
	/// </summary>
	void slot_buttonclicked()
	{
		ui.btnHello->setText("Toggled");
		print_statusbar("Depth peeling toggled!");

		//if (!edgevisible)
		//actor->GetProperty()->EdgeVisibilityOn();
		//else
		//actor->GetProperty()->EdgeVisibilityOff();

		//edgevisible = !edgevisible;
	}

	// ------------------------------------------------------------------------
	/// <summary> Slot called when File->New Window menu item clicked
	/// </summary>
	void slot_Preview()
	{
		ui.btnHello->setText("Changed");
		print_statusbar("New window clicked!");

		resizeInternal(this->size(), true);
	}

	// ------------------------------------------------------------------------
	/// <summary> Slot called when Help->About menu item clicked
	/// </summary>
	void slot_about()
	{
		QMessageBox messageBox(this);
		
		messageBox.setIconPixmap(QPixmap("about.png"));

		messageBox.setWindowTitle("About OpenSlider");
		messageBox.setText("OpenSlider ");
		messageBox.setInformativeText("1.0.0.1 (Released 12-17-2013) \n\nCopyright © 2011-2013 David Tran\n");
		messageBox.setWindowOpacity(.85);
		messageBox.setStyleSheet("background: rgba(0, 0, 0, 255); color: white; selection-color: black;");

		QPushButton *okButton = messageBox.addButton(tr("Ok"), QMessageBox::ActionRole);
		
		okButton->setStyleSheet(this->styleSheet());
		messageBox.exec();
	}
private:
	Ui::additiveClass ui;
};

#endif // ADDITIVE_H
