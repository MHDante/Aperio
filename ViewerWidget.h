#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

using namespace osg;

class additive;

class ViewerWidget : public QWidget, public osgViewer::CompositeViewer
{
public:
	// Custom variables
	additive * _a;
	ref_ptr<osg::StateSet> stateOne;
	
	// Custom constructor and methods
	ViewerWidget(additive* a, osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::CompositeViewer::SingleThreaded);
	
	QWidget* addViewWidget(osgQt::GraphicsWindowQt* gw, osg::Node* scene);
	osgQt::GraphicsWindowQt* createGraphicsWindow(int x, int y, int w, int h, const std::string& name = "", bool windowDecoration = false);

	void keyboard();
	void loadData(std::string filename);

	ref_ptr<osg::Camera>  createHUD();
	ref_ptr<osg::Geode>		  createHUDTextGeode(std::string text, float x, float y, float size);
	ref_ptr<osg::Geode>		  createHUDImageGeode(std::string filename, float x, float y, float w, float h);
	
	virtual void ViewerWidget::paintEvent(QPaintEvent* event)
	{
		frame();
		keyboard();
	}
protected:
	QTimer _timer;
};

#endif