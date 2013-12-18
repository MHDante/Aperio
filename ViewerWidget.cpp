#include "stdafx.h"

#include "ViewerWidget.h"
#include "additive.h"

#include "Utility.h"
//#include "tiny_obj_loader.h"

#include "obj_parser.h"

//#include "carve/geometry.hpp"
#include <osgFX/Outline>

using namespace osg;

unique_ptr<carve::mesh::MeshSet<3> > geomToMeshSet(ref_ptr<osg::Geometry> & geom)
{
	Utility::start_clock('d');

	carve::input::PolyhedronData data;

	osg::Vec3Array *vertices = static_cast<osg::Vec3Array*> (geom->getVertexArray());	
	data.reserveVertices(vertices->size());

	for (int i = 0; i < vertices->size(); i++)
	{
		osg::Vec3 v = vertices->at(i);
		data.addVertex(carve::geom::VECTOR(v.x(), v.y(), v.z()));
	}

	osg::Geometry::PrimitiveSetList setList = geom->getPrimitiveSetList();
	
	data.reserveFaces(setList.size(), 5);
	for (int i = 0; i < setList.size(); i++)
	{
		osg::PrimitiveSet * ps = setList.at(i);

		if (ps->getNumIndices() == 4)
			data.addFace(ps->index(0), ps->index(1), ps->index(2), ps->index(3));
		else if (ps->getNumIndices() == 3)
			data.addFace(ps->index(0), ps->index(1), ps->index(2));
	}
	unique_ptr<carve::mesh::MeshSet<3> > poly(new carve::mesh::MeshSet<3>(data.points, data.getFaceCount(), data.faceIndices));

	Utility::end_clock('d');
	std::cout << "done geom to mesh\n";

	return poly;
}

ref_ptr<osg::Geode> meshSetToGeode(unique_ptr<carve::mesh::MeshSet<3> > &c)
{
	Utility::start_clock('e');

	// Create geometry object
	ref_ptr<osg::Geometry> shapeGeometry = new osg::Geometry();

	ref_ptr<osg::Vec3Array> shapeArray = new osg::Vec3Array;
	shapeArray->reserve(c->vertex_storage.size());
	//ref_ptr<osg::Vec3Array> shapeArrayN = new osg::Vec3Array;
	//ref_ptr<osg::Vec2Array> shapeArrayT = new osg::Vec2Array;

	boost::unordered_map<carve::mesh::MeshSet<3>::vertex_t*, uint> vertexToIndex_map;	// vertex index map

	auto iter = begin(c->vertex_storage);
	for (int k = 0; iter != end(c->vertex_storage); ++k, ++iter) {

		carve::mesh::MeshSet<3>::vertex_t *vertex = &(*iter);
		shapeArray->push_back(osg::Vec3(vertex->v.x, vertex->v.y, vertex->v.z));
		vertexToIndex_map[vertex] = k;
	}
	shapeGeometry->setVertexArray(shapeArray);

	shapeGeometry->getPrimitiveSetList().reserve(c->faceEnd() - c->faceBegin());	
	
	//adding faces
	for (auto face_iter = c->faceBegin(); face_iter != c->faceEnd(); ++face_iter) {
		carve::mesh::MeshSet<3>::face_t *f = *face_iter;
		ref_ptr<osg::DrawElementsUInt> shapeElement = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
		shapeElement->reserve(f->nVertices());
		
		for (auto e_iter = f->begin(); e_iter != f->end(); ++e_iter) {
			shapeElement->push_back(vertexToIndex_map.at(e_iter->vert));
		}
		shapeGeometry->addPrimitiveSet(shapeElement);
	}
	osgUtil::SmoothingVisitor::smooth(*shapeGeometry, osg::DegreesToRadians(180.0));
	
	ref_ptr<osg::Geode> shapeGeode = new osg::Geode();
	shapeGeode->addDrawable(shapeGeometry);

	Utility::end_clock('e');
	std::cout << "done mesh to geom\n";

	return shapeGeode;
}
/// ---------------------------------------------------------------------------
/// <summary> Returns a cube in Carve CSG format (MeshSet)
/// </summary>
/// <param name="size">Size of cube</param>
/// <param name="t">A transformation matrix </param>
/// <returns>Cube in Carve's MeshSet format</returns>
unique_ptr<carve::mesh::MeshSet<3> > makeCube(float size,
	const carve::math::Matrix &transform) {
	carve::input::PolyhedronData data; 

	data.addVertex(transform * carve::geom::VECTOR(+size, +size, +size));
	data.addVertex(transform * carve::geom::VECTOR(-size, +size, +size));
	data.addVertex(transform * carve::geom::VECTOR(-size, -size, +size));
	data.addVertex(transform * carve::geom::VECTOR(+size, -size, +size));
	data.addVertex(transform * carve::geom::VECTOR(+size, +size, -size));
	data.addVertex(transform * carve::geom::VECTOR(-size, +size, -size));
	data.addVertex(transform * carve::geom::VECTOR(-size, -size, -size));
	data.addVertex(transform * carve::geom::VECTOR(+size, -size, -size));

	data.addFace(0, 1, 2, 3);
	data.addFace(7, 6, 5, 4);
	data.addFace(0, 4, 5, 1);
	data.addFace(1, 5, 6, 2);
	data.addFace(2, 6, 7, 3);
	data.addFace(3, 7, 4, 0);

	unique_ptr<carve::mesh::MeshSet<3> > poly(new carve::mesh::MeshSet<3>(data.points, data.getFaceCount(), data.faceIndices));
	return poly;
}

ViewerWidget::ViewerWidget(additive* a, osgViewer::ViewerBase::ThreadingModel threadingModel) : QWidget()
{
	this->_a = a;
	setThreadingModel(threadingModel);

	// disable the default setting of viewer.done() by pressing Escape.
	setKeyEventSetsDone(0);

	// Set up signals and slots for QT
	connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));
	float fps = 50.0;
	_timer.start(1000.0 / fps);

	QGridLayout* grid = new QGridLayout;
	grid->setMargin(2);
	setLayout(grid);
	
	// 640
	QWidget* widget = addViewWidget(createGraphicsWindow(0, 0, this->width(), this->height(), ""), nullptr);
	grid->addWidget(widget, 0, 0);
}

QWidget* ViewerWidget::addViewWidget(osgQt::GraphicsWindowQt* gw, osg::Node* scene)
{
	ref_ptr<osgViewer::View> view = new osgViewer::View;
	this->addView(view);
	
	osg::Camera* camera = view->getCamera();
	camera->setGraphicsContext(gw);

	const osg::GraphicsContext::Traits* traits = gw->getTraits();

	float bgcolor[3] = { 28, 100, 160 };
	camera->setClearColor(osg::Vec4(bgcolor[0] / 255.0, bgcolor[1] / 255.0, bgcolor[2] / 255.0, 1.0));
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0f, 10000.0f);

	view->addEventHandler(new osgViewer::StatsHandler);
	view->setCameraManipulator(new osgGA::TrackballManipulator);

	return gw->getGLWidget(); 
}

osgQt::GraphicsWindowQt* ViewerWidget::createGraphicsWindow(int x, int y, int w, int h, const std::string& name, bool windowDecoration)
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowName = name;
	traits->windowDecoration = windowDecoration;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = 2;// ds->getNumMultiSamples();	// smoother
	
	return new osgQt::GraphicsWindowQt(traits.get());
}


void ViewerWidget::keyboard()
{
	if (GetAsyncKeyState(VK_LEFT))
		;// stateOne->getUniform("mouse")->set(osg::Vec3(1, 0, 0));

	if (GetAsyncKeyState(VK_ESCAPE))
		exit(0);
}

void ViewerWidget::loadData(std::string filename)
{
	// Set up status label

	_a->print_statusbar("Loading file...please be patient!");

	//osg::ref_ptr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options();
	//options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_NONE);
	//options->setBuildKdTreesHint(osgDB::ReaderWriter::Options::BUILD_KDTREES);	// good for picking
	//options->setPrecisionHint(osgDB::ReaderWriter::Options::DOUBLE_PRECISION_ALL); // for accurate preprocessing
	//options->setReadFileCallback(NULL);

	//osg::ref_ptr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options();
	//options->setOptionString("noTesselateLargePolygons noTriStripPolygons noRotation");
	//options->setOptionString("noTriStripPolygons noTesselateLargePolygons");	// faster loading, slower rendering
	
	_a->_listWidget->clear();

	// Create HUD Cam that draws HUDTexture on-screen (We use HUDTexture for render-to-texture, FBO)
	myHUDCam = createHUDCamera();

	// Create a HUD texture that attaches to FBO (HUDTexture is drawn to screen)
	HUDTexture = new osg::Texture2D();
	HUDTexture->setTextureSize(this->width(), this->height());
	HUDTexture->setInternalFormat(GL_RGBA);
	HUDTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
	HUDTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

	ref_ptr<Geode> myHUDImage = createHUDImageGeode(HUDTexture, 0, 0, this->width(), this->height());
	myHUDCam->addChild(myHUDImage);

	// Create a RenderCamera - where our scene is, attaching HUDTexture to FBO (so we can render-to-texture instead of screen)
	// and then draw that texture on screen in HUD Cam
	renderCam = new osg::Camera();
	renderCam->setViewport(0, 0, this->width(), this->height());
	float bgcolor[3] = { 28, 100, 160 };
	renderCam->setClearColor(osg::Vec4(bgcolor[0] / 255.0, bgcolor[1] / 255.0, bgcolor[2] / 255.0, 1));
	renderCam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderCam->setRenderOrder(osg::Camera::PRE_RENDER);
	renderCam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);	
	renderCam->attach(osg::Camera::COLOR_BUFFER, HUDTexture.get(), 0, 0, false, 1, 1);

	// Create group containing both cams
	ref_ptr<osg::Group> myGroup = new osg::Group();
	myGroup->addChild(myHUDCam);
	myGroup->addChild(renderCam);

	// Start the parser
	unique_ptr<obj::obj_parser> parser(new obj::obj_parser());
	parser->parse(filename);
	
	//ref_ptr<osg::Group> mainroot = new osg::Group();
	ref_ptr<osg::Group> root = new osg::Group();
	//osg::Node *sub_model = osgDB::readNodeFile("lz.osg");

	ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
	transform->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(90.0), osg::Vec3(1, 0, 0)));
	transform->addChild(root);

	//ref_ptr<osgFX::Outline> outline = new osgFX::Outline();
	//float rgb[4] = { 255, 170, 64, 255};
	//outline->setColor(osg::Vec4(rgb[0] / 255.0, rgb[1] / 255.0, rgb[2] / 255.0, rgb[3] / 255.0));
	//outline->setWidth(1.75);
	//outline->setEnabled(false);
	//outline->addChild(transform);
	
	// Add Light
	ref_ptr<osg::Light> pLight = new osg::Light();
	pLight->setLightNum(0);	
	pLight->setAmbient(osg::Vec4(0.2, 0.2, 0.2, 1.0));
	pLight->setDiffuse(osg::Vec4(0.6, 0.6, 0.6, 1.0));
	pLight->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	
	pLight->setPosition(osg::Vec4(0, 1, 0, 0));        // last param    w = 0.0 directional light (direction), w = 1.0 point light (position)

	ref_ptr<osg::LightSource> pLightSource = new osg::LightSource();
	pLightSource->setLight(pLight);
	root->addChild(pLightSource);

	auto toGeom = [](obj::TheMesh & currentmesh) -> ref_ptr<osg::Geometry> {

		// Create geometry object
		ref_ptr<osg::Geometry> shapeGeometry = new osg::Geometry();

		ref_ptr<osg::Vec3Array> shapeArray = new osg::Vec3Array;
		ref_ptr<osg::Vec3Array> shapeArrayN = new osg::Vec3Array;
		ref_ptr<osg::Vec2Array> shapeArrayT = new osg::Vec2Array;

		int index = 0;
		bool computeNormals = false;
		shapeArray->reserve(currentmesh.vertices.size());
		shapeArrayT->reserve(currentmesh.vertices.size());
		shapeArrayN->reserve(currentmesh.vertices.size());

		// vertex indices
		for (int j = 0; j < currentmesh.vertices.size(); j++)
		{
			obj::Vertex v = currentmesh.vertices.at(j);
			shapeArray->push_back(osg::Vec3(v.p[0], v.p[1], v.p[2]));
		}

		shapeGeometry->getPrimitiveSetList().reserve(currentmesh.polys.size());

		for (int j = 0; j < currentmesh.polys.size(); j++)
		{
			// For every face (polygon)
			obj::Face currentF = currentmesh.polys.at(j);

			//ref_ptr<osg::DrawElementsUInt> shapeElement = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);

			//for (int k = 0; k < currentF.numIndices; k++)	// For each index in the face
			//{
			//	shapeArray->push_back(osg::Vec3(currentmesh.vertices.at(currentF.indices[k]).p[0],
			//		currentmesh.vertices.at(currentF.indices[k]).p[1],
			//		currentmesh.vertices.at(currentF.indices[k]).p[2]));

			//	shapeElement->push_back(index++);
			//}
			//shapeGeometry->addPrimitiveSet(shapeElement);

			ref_ptr<osg::DrawElementsUInt> shapeElement = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
			shapeElement->reserve(currentF.numIndices);

			for (int k = 0; k < currentF.numIndices; k++)	// For each index in the face
			{
				shapeElement->push_back(currentF.indices[k]);
			}
			shapeGeometry->addPrimitiveSet(shapeElement);


			for (int k = 0; k < currentF.numIndices; k++)	// For each index in the face
			{
				if (currentF.haveTextures)
				{
					shapeArrayT->push_back(osg::Vec2(currentmesh.tcoords.at(currentF.indicesT[k]).p[0],
						currentmesh.tcoords.at(currentF.indicesT[k]).p[1]));
				}
				else
					shapeArrayT->push_back(osg::Vec2(0, 0));
			}

			for (int k = 0; k < currentF.numIndices; k++)	// For each index in the face
			{
				if (currentF.haveNormals)
				{
					shapeArrayN->push_back(osg::Vec3(currentmesh.normals.at(currentF.indicesN[k]).p[0],
						currentmesh.normals.at(currentF.indicesN[k]).p[1],
						currentmesh.normals.at(currentF.indicesN[k]).p[2]));
				}
				else
				{
					shapeArrayN->push_back(osg::Vec3(0, 0, 0));
					computeNormals = true;
				}
			}
		}
		shapeGeometry->setVertexArray(shapeArray);
		//shapeGeometry->setTexCoordArray(0, shapeArrayT, Array::BIND_PER_VERTEX);
		//shapeGeometry->setNormalArray(shapeArrayN, Array::BIND_PER_VERTEX);

		//if (computeNormals)
			osgUtil::SmoothingVisitor::smooth(*shapeGeometry, osg::DegreesToRadians(180.0));

		//cout << shapeGeometry->getVertexArray()->getNumElements();

		return shapeGeometry;
	};

	Utility::start_clock('a');

	float r, g, b;
	r = 1;
	g = 1;
	b = 1;

	ref_ptr<osg::Geometry> myGeom = nullptr;

	for (int i = 0; i < parser->themeshes.size(); i++)
	{
		// For each mesh
		obj::TheMesh currentmesh = parser->themeshes.at(i);

		ref_ptr<osg::Geometry> shapeGeometry = toGeom(currentmesh);
		ref_ptr<osg::Geode> shapeGeode = new osg::Geode();
		shapeGeode->addDrawable(shapeGeometry);

		if (i == 0)
			myGeom = shapeGeometry;
		
		// Modify colour for each Geode
		if (parser->themeshes.size() > 1) // assign a random colour if there's more than one object
		{
			r = ((rand() % 177 + 80) / 270.0);
			g = ((rand() % 177 + 80) / 270.0);
			b = ((rand() % 177 + 80) / 270.0);
		}
		ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
		colorArray->push_back(osg::Vec4(r, g, b, 1.0));
		shapeGeometry->setColorArray(colorArray, Array::BIND_OVERALL);

		//shapeGeometry->setUseVertexBufferObjects(true);
		//shapeGeometry->setUseDisplayList(false);

		root->addChild(shapeGeode);

		// Add to listbox
		_a->_listWidget->addItem(currentmesh.name);
	}
	Utility::end_clock('a');

	//	for (int j = 0; j < shapes.at(i).mesh.positions.size() / 3; j++) {
	//		
	//		shapeVertices->push_back(
	//			osg::Vec3(shapes[i].mesh.positions[3 * j + 0], shapes[i].mesh.positions[3 * j + 1], 
	//					  shapes[i].mesh.positions[3 * j + 2])
	//		);
	//	}
	//	shapeGeometry->setVertexArray(shapeVertices);

	//	// per face
	//	for (int j = 0; j < themeshes[i].polys.size(); j++)
	//	{
	//		// per index in face
	//		for (int k = 0; k < themeshes[i].polys[j].numIndices; k++)
	//		{
	//			int index = themeshes[i].polys[j].indices[k];

	//			if (vertexIndexMap[index] == -1)	// not exist, add it into vertices and new index
	//			{
	//				Vertex v = { vertices[index].p[0], vertices[index].p[1], vertices[index].p[2] };
	//				themeshes[i].vertices.push_back(v);

	//				int newindex = themeshes[i].vertices.size() - 1;

	//				// reassign new index
	//				themeshes[i].polys[j].indices[k] = newindex;

	//				// map oldindex->newindex 
	//				vertexIndexMap[index] = newindex;
	//			}
	//			else	// already exist, just reassign new index to one in the map
	//			{
	//				int newindex = vertexIndexMap[index];
	//				themeshes[i].polys[j].indices[k] = newindex;
	//			}

	//			if (themeshes[i].polys[j].haveNormals)
	//			{
	//				int index = themeshes[i].polys[j].indicesN[k];

	//				if (vertexIndexMapN[index] == -1)	// not exist, add it into vertices and new index
	//				{
	//					Vertex v = { normals[index].p[0], normals[index].p[1], normals[index].p[2] };
	//					themeshes[i].normals.push_back(v);

	//					int newindex = themeshes[i].normals.size() - 1;

	//					// reassign new index
	//					themeshes[i].polys[j].indicesN[k] = newindex;

	//					// map oldindex->newindex 
	//					vertexIndexMapN[index] = newindex;
	//				}
	//				else	// already exist, just reassign new index to one in the map
	//				{
	//					int newindex = vertexIndexMapN[index];
	//					themeshes[i].polys[j].indicesN[k] = newindex;
	//				}
	//			}

	//			if (themeshes[i].polys[j].haveTextures)
	//			{
	//				int index = themeshes[i].polys[j].indicesT[k];

	//				if (vertexIndexMapT[index] == -1)	// not exist, add it into vertices and new index
	//				{
	//					Tex t = { tcoords[index].p[0], tcoords[index].p[1] };
	//					themeshes[i].tcoords.push_back(t);

	//					int newindex = themeshes[i].tcoords.size() - 1;

	//					// reassign new index
	//					themeshes[i].polys[j].indicesT[k] = newindex;

	//					// map oldindex->newindex 
	//					vertexIndexMapT[index] = newindex;
	//				}
	//				else	// already exist, just reassign new index to one in the map
	//				{
	//					int newindex = vertexIndexMapT[index];
	//					themeshes[i].polys[j].indicesT[k] = newindex;
	//				}
	//			}
	//		}
	//	}
	//}


	//std::vector<tinyobj::shape_t> shapes;
	//std::string err = tinyobj::LoadObj(shapes, "heart.obj");

	//if (!err.empty()) {
	//	std::cerr << err << std::endl;
	//	return;
	//}

	//ref_ptr<osg::Group> root = new osg::Group();

	//for (int i = 0; i < shapes.size(); i++)
	//{
	//	// Create geode (geometry node), assign geometry to geode 
	//	ref_ptr<osg::Geode> shapeGeode = new osg::Geode();
	//	ref_ptr<osg::Geometry> shapeGeometry = new osg::Geometry();

	//	shapeGeode->addDrawable(shapeGeometry);
	//	root->addChild(shapeGeode);

	//	// Add vertices to geometry
	//	ref_ptr<osg::Vec3Array> shapeVertices = new osg::Vec3Array;

	//	for (int j = 0; j < shapes.at(i).mesh.positions.size() / 3; j++) {
	//		
	//		shapeVertices->push_back(
	//			osg::Vec3(shapes[i].mesh.positions[3 * j + 0], shapes[i].mesh.positions[3 * j + 1], 
	//					  shapes[i].mesh.positions[3 * j + 2])
	//		);
	//	}
	//	shapeGeometry->setVertexArray(shapeVertices);

	//	// Add normals to geometry
	//	//ref_ptr<osg::Vec3Array> shapeNormals = new osg::Vec3Array;

	//	//for (int j = 0; j < shapes.at(i).mesh.normals.size() / 3; j++) {

	//	//	shapeNormals->push_back(
	//	//		osg::Vec3(shapes[i].mesh.normals[3 * j + 0], shapes[i].mesh.normals[3 * j + 1],
	//	//		shapes[i].mesh.normals[3 * j + 2])
	//	//		);
	//	//}
	//	//shapeGeometry->setNormalArray(shapeNormals);

	//	// Add Texture coords to geometry
	//	ref_ptr<osg::Vec3Array> shapeTex = new osg::Vec3Array;

	//	for (int j = 0; j < shapes.at(i).mesh.texcoords.size() / 3; j++) {

	//		shapeTex->push_back(
	//			osg::Vec3(shapes[i].mesh.texcoords[3 * j + 0], shapes[i].mesh.texcoords[3 * j + 1],
	//			shapes[i].mesh.texcoords[3 * j + 2])
	//			);
	//	}
	//	shapeGeometry->setTexCoordArray(0, shapeTex);

	//	// Add indices (draw elements containing indices) to geometry
	//	for (int j = 0; j < shapes.at(i).mesh.indices.size(); j += 3) {
	//		ref_ptr<osg::DrawElementsUInt> shapeElement = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_FAN, 0);
	//		shapeElement->push_back(shapes.at(i).mesh.indices.at(j + 0));
	//		shapeElement->push_back(shapes.at(i).mesh.indices.at(j + 1));
	//		shapeElement->push_back(shapes.at(i).mesh.indices.at(j + 2));

	//		shapeGeometry->addPrimitiveSet(shapeElement);
	//	}
	//}

	ref_ptr<osg::Texture2D> myTexture = new osg::Texture2D;
	myTexture->setDataVariance(Object::DYNAMIC);
	
	ref_ptr<osg::Image> myTextureImage = osgDB::readImageFile("luigi.tga");
	if (!myTextureImage)
	{
		_a->print_statusbar("Error reading in texture.");
		return;
	}
	myTexture->setImage(myTextureImage);

	// Create a new StateSet with default settings: 
	stateOne = transform->getOrCreateStateSet();

	//osg::Fog* pFog = new osg::Fog();
	//pFog->setMode(osg::Fog::EXP2);
	//pFog->setColor(osg::Vec4(1, 1, 1, 1));
	//pFog->setDensity(0.05f);
	//stateOne->setAttribute(pFog, osg::StateAttribute::ON);
	//stateOne->setMode(GL_FOG, osg::StateAttribute::ON);

	stateOne->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateOne->setTextureAttributeAndModes(0, myTexture, osg::StateAttribute::ON);

	ref_ptr<osg::Material> mat = new osg::Material();
	mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 1.0, 1.0, 1));
	mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.8, 0.8, 0.8, 1));
	mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.15, 0.15, 0.15, 1));
	mat->setShininess(osg::Material::FRONT_AND_BACK, 15);
	stateOne->setAttributeAndModes(mat, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	//ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	//stateOne->setAttributeAndModes(bf);
	//stateOne->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//stateOne->setRenderBinDetails(10, "DepthSortedBin");

	stateOne->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	stateOne->setMode(GL_LIGHT0, osg::StateAttribute::ON);

	//stateOne->setAttribute(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));

	//ref_ptr<osg::ShadeModel> shadeModel = new osg::ShadeModel();
	//shadeModel->setMode(osg::ShadeModel::FLAT);
	//stateOne->setAttributeAndModes(shadeModel);
	
	//ref_ptr<osg::CullFace> cullFace = new osg::CullFace();
	//cullFace->setMode(osg::CullFace::FRONT);
	//stateOne->setAttributeAndModes(cullFace);

//	transform->setStateSet(stateOne);

	ref_ptr<osg::Shader> vertShader = new osg::Shader(osg::Shader::VERTEX);
	vertShader->loadShaderSourceFromFile("shader.glsl");
	//ref_ptr<osg::Shader> geomShader = new osg::Shader(osg::Shader::GEOMETRY);
	//geomShader->loadShaderSourceFromFile("shaderGeom.glsl");
	ref_ptr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT);
	fragShader->loadShaderSourceFromFile("shaderFrag.glsl");

	ref_ptr<osg::Program> program = new osg::Program();
	program->addShader(vertShader);
	//program->addShader(geomShader);
	program->addShader(fragShader);

	//program->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
	//program->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
	//program->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);

	stateOne->setAttributeAndModes(program);

	ref_ptr<osg::Uniform> uniform = new osg::Uniform("x", 0.5f);
	ref_ptr<osg::Uniform> uniformMouse = new osg::Uniform("mouse", osg::Vec3(0,0,0));
	
	stateOne->addUniform(uniform);
	stateOne->addUniform(uniformMouse);

	//ref_ptr<osg::Camera> projection2D = createHUD();
	//mainroot->addChild(projection2D);
	//this->getView(0)->setSceneData(projection2D);
	
	transform->setStateSet(stateOne);

	//// Perform CSG
	//unique_ptr<carve::mesh::MeshSet<3> > first = geomToMeshSet(myGeom);
	//unique_ptr<carve::mesh::MeshSet<3> > second = makeCube(0.00, carve::math::Matrix::TRANS(0, 0, 0));
	//
	//Utility::start_clock('c');

	//carve::csg::CSG csg;
	////csg.hooks.registerHook(new carve::csg::CarveTriangulator, carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT); // slow but accurate triangulator
	//unique_ptr<carve::mesh::MeshSet<3> > c(csg.compute(first.get(), second.get(), carve::csg::CSG::A_MINUS_B, nullptr, carve::csg::CSG::CLASSIFY_EDGE));
	//
	//Utility::end_clock('c');
	//std::cout << "done csg difference\n";

	//// Converting c back to points and faces
	//ref_ptr<osg::Geode> resultGeode = meshSetToGeode(c);
	//root->addChild(resultGeode);
	//
	//osgGA::TrackballManipulator* manip = dynamic_cast<osgGA::TrackballManipulator*>(this->getView(0)->getCameraManipulator());
	//manip->home(0);

	/*osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("lz.osg");
	osg::ref_ptr<osg::Node> sub_model = osgDB::readNodeFile("glider.osg");

	int tex_width = this->width();
	int tex_height = this->height();

	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
	texture->setTextureSize(tex_width, tex_height);
	texture->setInternalFormat(GL_RGBA);
	texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
	texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

	FindTextureVisitor ftv(texture.get());
	model->accept(ftv);

	ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setViewport(0, 0, tex_width, tex_height);
	camera->setClearColor(osg::Vec4(1, 1, 1, 0));
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->setRenderOrder(osg::Camera::PRE_RENDER);
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	camera->attach(osg::Camera::COLOR_BUFFER, texture.get());
	camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	camera->addChild(sub_model.get());

	camera->setViewMatrixAsLookAt(osg::Vec3(0, 1, 0), osg::Vec3(0, 0, 0), osg::Vec3(0, 1, 0));

	this->getView(0)->setSceneData(sub_model);
	*/
	//osg::ref_ptr<osg::Node> sub_model = osgDB::readNodeFile("glider.osg");

	ref_ptr<osg::StateSet> stateTwo = myHUDCam->getOrCreateStateSet();
	
	stateTwo->setTextureAttributeAndModes(0, HUDTexture, osg::StateAttribute::ON);

	ref_ptr<osg::Shader> vertShader2 = new osg::Shader(osg::Shader::VERTEX);
	vertShader->loadShaderSourceFromFile("shader2.glsl");
	ref_ptr<osg::Shader> fragShader2 = new osg::Shader(osg::Shader::FRAGMENT);
	fragShader->loadShaderSourceFromFile("shaderFrag2.glsl");

	ref_ptr<osg::Program> program2 = new osg::Program();
	program2->addShader(vertShader2);
	program2->addShader(fragShader2);



	renderCam->addChild(transform);
	this->getView(0)->setSceneData(myGroup);

	_a->print_statusbar("Ready.");
}

ref_ptr<Camera> ViewerWidget::createHUDCamera()
{
	ref_ptr<osg::Camera> hudCamera = new osg::Camera;
	hudCamera->setClearMask(0);
	hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
	hudCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);

	hudCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, this->width(), 0, this->height()));
	hudCamera->setViewMatrix(osg::Matrix::identity());

	// Set modelview state sets
	hudCamera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	hudCamera->getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");

	hudCamera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	hudCamera->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	hudCamera->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

	return hudCamera;
}
ref_ptr<Camera> ViewerWidget::createHUD()
{
	ref_ptr<osg::Camera> hudCamera = createHUDCamera();

	// This geode contains the text
	ref_ptr<osg::Geode> geodeText = createHUDTextGeode("Console", 0, .3 * height(), .02 * width());

	// This geode contains the image

	// read in image/texture
	ref_ptr<osg::Texture2D> HUDTexture = new osg::Texture2D();
	HUDTexture->setDataVariance(osg::Object::DYNAMIC);
	ref_ptr<osg::Image> hudImage;
	hudImage = osgDB::readImageFile("luigi.tga");
	HUDTexture->setImage(hudImage);

	ref_ptr<osg::Geode> geodeImage = createHUDImageGeode(HUDTexture, 0, 0, .2 * width(), .3 * height());

	// Add the geodes to modelview
	hudCamera->addChild(geodeText);
	hudCamera->addChild(geodeImage);

	ref_ptr<osg::Material> mat = new osg::Material();
	mat->setAlpha(osg::Material::FRONT_AND_BACK, .5);
	hudCamera->getOrCreateStateSet()->setAttributeAndModes(mat, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	// Return the root (projection)
	return hudCamera;
}

ref_ptr<osg::Geode> ViewerWidget::createHUDTextGeode(std::string text, float x, float y, float size)
{
	ref_ptr<osgText::Text> hudText = new  osgText::Text;
	hudText->setDataVariance(osg::Object::DYNAMIC);
	//_hudText->setFont(timesFont);
	hudText->setPosition(osg::Vec3(x, y, 0));
	hudText->setText(text);
	hudText->setColor(osg::Vec4(1, 1, 1, 1.0));
	hudText->setCharacterSize(size);
	
	ref_ptr<osg::Geode> geodeText = new osg::Geode();
	geodeText->addDrawable(hudText);

	return geodeText;
}

ref_ptr<osg::Geode> ViewerWidget::createHUDImageGeode(ref_ptr<osg::Texture2D> &HUDTexture, float x, float y, float w, float h)
{
	// create background geometry
	ref_ptr<osg::Geometry> HUDBackgroundGeometry = new osg::Geometry();

	ref_ptr<osg::Vec3Array> HUDBackgroundVertices = new osg::Vec3Array;
	HUDBackgroundVertices->push_back(osg::Vec3(x, y, -1));
	HUDBackgroundVertices->push_back(osg::Vec3(x + w, y, -1));
	HUDBackgroundVertices->push_back(osg::Vec3(x + w, y + h, -1));
	HUDBackgroundVertices->push_back(osg::Vec3(x, y + h, -1));

	ref_ptr<osg::DrawElementsUInt> HUDBackgroundIndices = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
	HUDBackgroundIndices->push_back(0);
	HUDBackgroundIndices->push_back(1);
	HUDBackgroundIndices->push_back(2);
	HUDBackgroundIndices->push_back(3);

	ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array(4);
	(*texcoords)[0].set(0.0f, 0.0f);
	(*texcoords)[1].set(1.0f, 0.0f);
	(*texcoords)[2].set(1.0f, 1.0f);
	(*texcoords)[3].set(0.0f, 1.0f);

	HUDBackgroundGeometry->setVertexArray(HUDBackgroundVertices);
	HUDBackgroundGeometry->addPrimitiveSet(HUDBackgroundIndices);
	HUDBackgroundGeometry->setTexCoordArray(0, texcoords);

	// attach texture to geode (but first make the geode and attach geometry to geode)
	ref_ptr<osg::Geode> geodeImage = new osg::Geode();
	geodeImage->addDrawable(HUDBackgroundGeometry);

	geodeImage->getOrCreateStateSet()->setTextureAttributeAndModes(0, HUDTexture, osg::StateAttribute::ON);

	return geodeImage;
}

