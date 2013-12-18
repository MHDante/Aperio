#include "stdafx.h"
#include "additive.h"

// Custom includes
#include "ViewerWidget.h"

additive::additive(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	_listWidget = ui.listWidget;
	_viewWidget = nullptr;

	// Connect slots
	QTimer::singleShot(0, this, SLOT(slot_afterShowWindow()));
	
	connect(ui.btnHello, SIGNAL(clicked()), this, SLOT(slot_buttonclicked()));
	connect(ui.actionPreview, SIGNAL(triggered()), this, SLOT(slot_Preview()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(slot_open()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(slot_exit()));
	connect(ui.actionFullscreen, SIGNAL(triggered()), this, SLOT(slot_fullScreen()));
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(slot_about()));

	connect(ui.horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(slot_valueChanged(int)));

	//connect(ui.listWidget, SIGNAL(itemEntered(QListWidgetItem *)), this, SLOT(slot_listitementered(QListWidgetItem *)));
	//connect(ui.listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(slot_listitemclicked(int)));
}

additive::~additive()
{

}

/// ---------------------------------------------------------------------------------------
/// <summary> Event called when window resized (resizes qvtkwidget)
/// </summary>
/// <param name="event">Event object (Qt-based)</param>
void additive::resizeEvent(QResizeEvent * event)
{
	resizeInternal(event->size(), false);
}
// ------------------------------------------------------------------------
void additive::resizeInternal(const QSize &newWindowSize, bool using_preview)
{
	int margin = 20;
	int extra = 2;	// extra border-width margin of qframe

	QRect central = ui.centralWidget2->geometry();
	QRect newRect;

	// Previewing resize
	if (using_preview)
	{
		// Check state of preview (true or false and draw appropriately)
		if (!preview)
		{
			preview = true;
			newRect.setCoords(margin, 0, newWindowSize.width() - margin, newWindowSize.height() - margin * 3);		
			ui.centralWidget2->raise();
		}		
		else
		{
			preview = false;
			newRect.setCoords(_orig_size.x(), _orig_size.y(), newWindowSize.width() - margin, newWindowSize.height() - margin * 3);
			ui.centralWidget2->lower();
		}
	}
	else  // Regular resize
	{	
		newRect.setCoords(central.topLeft().x(), central.topLeft().y(), newWindowSize.width() - margin, newWindowSize.height() - margin * 3);
	}
	ui.centralWidget2->setGeometry(newRect);

	if (_viewWidget)
	{
		if (_viewWidget->HUDTexture)
		{
			_viewWidget->myHUDCam->setViewport(0, 0, _viewWidget->width(), _viewWidget->height());
			_viewWidget->renderCam->setViewport(0, 0, _viewWidget->width(), _viewWidget->height());
			
			_viewWidget->HUDTexture->setTextureSize(_viewWidget->width(), _viewWidget->height());
			_viewWidget->HUDTexture->dirtyTextureObject();
			
			_viewWidget->renderCam->setRenderingCache(0);
			_viewWidget->myHUDCam->setRenderingCache(0);
		}
	}
}

// ------------------------------------------------------------------------
void additive::update_orig_size()
{
	_orig_size.setX(ui.centralWidget2->geometry().x());
	_orig_size.setY(ui.centralWidget2->geometry().y());
	_orig_size.setWidth(ui.centralWidget2->geometry().width());
	_orig_size.setHeight(ui.centralWidget2->geometry().height());
}

// ------------------ Slots --------------------------------------------

void additive::slot_afterShowWindow()
{
	QApplication::processEvents();	

	// set up original size of viewer
	update_orig_size();

	// Set translucencies for all menus
	float menu_opacity = 0.8f;

	ui.menuFile->setAttribute(Qt::WA_TranslucentBackground);
	ui.menuFile->setWindowOpacity(menu_opacity);
	ui.menuHelp->setAttribute(Qt::WA_TranslucentBackground);
	ui.menuHelp->setWindowOpacity(menu_opacity);

	// Create and add an OSG ViewWidget to a gridLayout on the form
	_viewWidget = new ViewerWidget(this, osgViewer::ViewerBase::ThreadingModel::SingleThreaded);	// ! Enable this for GLIntercept
	//_viewWidget = new ViewerWidget(this, osgViewer::ViewerBase::ThreadingModel::AutomaticSelection);
	
	ui.centralWidget2->layout()->addWidget(_viewWidget);	// I already added a gridLayout to centralWidget2, add the viewWidget to it

	// Create and add status_label to status bar
	_status_label = new QLabel("", this);
	_status_label->setStyleSheet("background: none;");
	this->statusBar()->addWidget(_status_label);

	QApplication::processEvents();	// Process again to make sure viewer shows up before loading data

	// Finally, Load viewer data
	_viewWidget->loadData("heart.obj");
}

