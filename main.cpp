#include "stdafx.h"
#include "additive.h"

#include <iostream>

int main(int argc, char *argv[])
{
	//osgViewer::Viewer * viewer = new osgViewer::Viewer();
	//viewer->setUpViewInWindow(100, 100, 640, 480);
	//viewer->run(); 

	QApplication a(argc, argv);
	QApplication::setStyle("cleanlooks");

	additive w;
	w.show();

	w.move(QPoint(500, 80));
		
	return a.exec(); 
}
