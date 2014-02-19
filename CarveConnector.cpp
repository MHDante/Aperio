#include "stdafx.h"
#include "CarveConnector.h"

//------------------------------------------------------------------------------------
CarveConnector::CarveConnector()
{
}
//------------------------------------------------------------------------------------
CarveConnector::~CarveConnector()
{
}
//----------------------------------------------------------------------------------------------------------------
unique_ptr<carve::mesh::MeshSet<3> > CarveConnector::makeCube(float size, const carve::math::Matrix &t)
{
	vector<carve::geom3d::Vector> vertices;

	vertices.push_back(t * carve::geom::VECTOR(+size, +size, +size));
	vertices.push_back(t * carve::geom::VECTOR(-size, +size, +size));
	vertices.push_back(t * carve::geom::VECTOR(-size, -size, +size));
	vertices.push_back(t * carve::geom::VECTOR(+size, -size, +size));
	vertices.push_back(t * carve::geom::VECTOR(+size, +size, -size));
	vertices.push_back(t * carve::geom::VECTOR(-size, +size, -size));
	vertices.push_back(t * carve::geom::VECTOR(-size, -size, -size));
	vertices.push_back(t * carve::geom::VECTOR(+size, -size, -size));

	vector<int> f;
	int numfaces = 0;

	f.push_back(4);
	f.push_back(0); f.push_back(1); f.push_back(2); f.push_back(3);
	numfaces++;
	f.push_back(4);
	f.push_back(7); f.push_back(6); f.push_back(5); f.push_back(4);
	numfaces++;
	f.push_back(4);
	f.push_back(0); f.push_back(4); f.push_back(5); f.push_back(1);
	numfaces++;
	f.push_back(4);
	f.push_back(1); f.push_back(5); f.push_back(6); f.push_back(2);
	numfaces++;
	f.push_back(4);
	f.push_back(2); f.push_back(6); f.push_back(7); f.push_back(3);
	numfaces++;
	f.push_back(4);
	f.push_back(3); f.push_back(7); f.push_back(4); f.push_back(0);
	numfaces++;

	unique_ptr<carve::mesh::MeshSet<3> > poly(new carve::mesh::MeshSet<3>(vertices, numfaces, f));
	return poly;
}
//-------------------------------------------------------------------------------------------------
unique_ptr<carve::mesh::MeshSet<3> > CarveConnector::performDifference(unique_ptr<carve::mesh::MeshSet<3> > &a, unique_ptr<carve::mesh::MeshSet<3> > &b)
{
	carve::csg::CSG csg;
	csg.hooks.registerHook(new carve::csg::CarveTriangulator, carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT); // slow but accurate

	unique_ptr<carve::mesh::MeshSet<3> > c(csg.compute(a.get(), b.get(), carve::csg::CSG::A_MINUS_B, nullptr, carve::csg::CSG::CLASSIFY_EDGE));
	return c;
}
//----------------------------------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> CarveConnector::meshSetToVTKPolyData(unique_ptr<carve::mesh::MeshSet<3> > &c)
{
	// Create points
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetNumberOfPoints(c->vertex_storage.size());	// allocate memory

	boost::unordered_map<carve::mesh::MeshSet<3>::vertex_t*, uint> vertexToIndex_map;	// vertex index map
	auto iter = begin(c->vertex_storage);
	for (int k = 0; iter != end(c->vertex_storage); ++k, ++iter) {

		carve::mesh::MeshSet<3>::vertex_t *vertex = &(*iter);

		points->SetPoint(k, vertex->v.x, vertex->v.y, vertex->v.z);
		vertexToIndex_map[vertex] = k;
	}
	// Create polygons (faces)
	vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();

	for (auto face_iter = c->faceBegin(); face_iter != c->faceEnd(); ++face_iter) {
		carve::mesh::MeshSet<3>::face_t *f = *face_iter;
		polygon->GetPointIds()->SetNumberOfIds(f->nVertices());

		// nVertices = nEdges since half edge
		int j = 0;
		for (auto e_iter = f->begin(); e_iter != f->end(); ++e_iter) {
			polygon->GetPointIds()->SetId(j++, vertexToIndex_map.at(e_iter->vert));
		}
		polygons->InsertNextCell(polygon);
	}
	// Create vtkPolyData
	vtkSmartPointer<vtkPolyData> polygonPolyData = vtkSmartPointer<vtkPolyData>::New();
	polygonPolyData->SetPoints(points);
	polygonPolyData->SetPolys(polygons);

	return polygonPolyData;
}
//----------------------------------------------------------------------------------------------------
unique_ptr<carve::mesh::MeshSet<3> > CarveConnector::vtkPolyDataToMeshSet(vtkSmartPointer<vtkPolyData> &thepolydata)
{
	// First make MeshSet's points
	std::vector<carve::geom3d::Vector> vertices;
	vertices.resize(thepolydata->GetNumberOfPoints());

	for (vtkIdType i = 0; i < thepolydata->GetNumberOfPoints(); i++)
	{
		double p[3];
		thepolydata->GetPoint(i, p);

		vertices[i] = carve::geom::VECTOR(p[0], p[1], p[2]);
	}

	// Then make MeshSet Faces 
	vtkSmartPointer<vtkCellArray> polys = thepolydata->GetPolys();
	polys->InitTraversal();

	std::vector<int> f;
	int numfaces = 0;
	int t = 0;

	f.resize(polys->GetSize());	// exact size: n1, id1, id2, id3, n2, id1, id2..etc
	for (int i = 0; i < polys->GetNumberOfCells(); i++)
	{
		vtkIdType n_pts;
		vtkIdType *pts = nullptr;
		polys->GetNextCell(n_pts, pts);

		f[t++] = n_pts;
		f[t++] = pts[0];
		f[t++] = pts[1];
		f[t++] = pts[2];

		if (n_pts > 3)
			f[t++] = pts[3];
		numfaces++;
	}
	// Construct MeshSet from vertices and faces
	unique_ptr<carve::mesh::MeshSet<3> > first(new carve::mesh::MeshSet<3>(vertices, numfaces, f));
	return first;
}


