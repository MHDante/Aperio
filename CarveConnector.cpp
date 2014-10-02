#include "stdafx.h"
#include "CarveConnector.h"

#include "Utility.h"

using namespace carve::mesh;

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
//----------------------------------------------------------------------------------------------------------------------------------------
class HoleResolver : public carve::csg::CarveHoleResolver {
	void removeDuplicatedFaces(std::vector<MeshSet<3>::face_t *> &faces) {
		std::vector<MeshSet<3>::face_t *> out_faces;
		std::vector<MeshSet<3>::face_t *> duplicated_faces;

		for (size_t face_index = 0; face_index < faces.size(); ++face_index) {
			carve::mesh::MeshSet<3>::face_t *face = faces[face_index];
			face->canonicalize();
		}

		for (size_t i = 0; i < faces.size(); ++i) {
			carve::mesh::MeshSet<3>::face_t *face = faces[i];

			bool found = false;
			for (size_t j = i + 1; j < faces.size() && found == false; ++j) {
				MeshSet<3>::face_t *cur_face = faces[j];
				if (cur_face->nEdges() == face->nEdges() &&
					cur_face->edge->vert == face->edge->vert)
				{
					MeshSet<3>::edge_t *cur_edge = cur_face->edge,
						*forward_edge = face->edge,
						*backward_edge = face->edge;
					bool forward_matches = true, backward_matches = true;

					for (int a = 0; a < cur_face->nEdges(); ++a) {
						if (forward_edge->vert != cur_edge->vert) {
							forward_matches = false;
							if (backward_matches == false) {
								break;
							}
						}

						if (backward_edge->vert != cur_edge->vert) {
							backward_matches = false;
							if (forward_matches == false) {
								break;
							}
						}

						cur_edge = cur_edge->next;
						forward_edge = forward_edge->next;
						backward_edge = backward_edge->prev;
					}

					if (forward_matches || backward_matches) {
						found = true;
						break;
					}
				}
			}

			if (found) {
				duplicated_faces.push_back(face);
			}
			else {
				out_faces.push_back(face);
			}
		}

		for (int i = 0; i < duplicated_faces.size(); ++i) {
			delete duplicated_faces[i];
		}

		std::swap(faces, out_faces);
	}

public:
	virtual ~HoleResolver() {
	}

	virtual void processOutputFace(std::vector<MeshSet<3>::face_t *> &faces,
		const MeshSet<3>::face_t *orig,
		bool flipped) {
		carve::csg::CarveHoleResolver::processOutputFace(faces, orig, flipped);
		if (faces.size() > 1) {
			removeDuplicatedFaces(faces);
		}
	}
};
//-------------------------------------------------------------------------------------------------
unique_ptr<carve::mesh::MeshSet<3> > CarveConnector::perform(unique_ptr<carve::mesh::MeshSet<3> > &a, unique_ptr<carve::mesh::MeshSet<3> > &b, carve::csg::CSG::OP op)
{
	carve::csg::CSG csg;

	//csg.hooks.registerHook(new GLUTriangulator, carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT);
	//csg.hooks.registerHook(new carve::csg::CarveTriangulationImprover, carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT);

	//csg.hooks.registerHook(new carve::csg::CarveTriangulatorWithImprovement, carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT);
	csg.hooks.registerHook(new carve::csg::CarveTriangulator, carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT);

	//csg.hooks.registerHook(new carve::csg::CarveHoleResolver, carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT);
	//csg.hooks.registerHook(new HoleResolver, carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT);

	carve::csg::CSG::CLASSIFY_TYPE type = carve::csg::CSG::CLASSIFY_NORMAL;
	unique_ptr<carve::mesh::MeshSet<3> > c(csg.compute(a.get(), b.get(), op, NULL, type));

	return c;
}
//----------------------------------------------------------------------------------------------------------------------------------------
static bool Carve_checkDegeneratedFace(boost::unordered_map<MeshSet<3>::vertex_t*, uint> *vertexToIndex_map, MeshSet<3>::face_t *face)
{
	/* only tris for now */
	if (face->n_edges == 3) {
		uint v1, v2, v3;

		v1 = vertexToIndex_map->find(face->edge->prev->vert)->second;
		v2 = vertexToIndex_map->find(face->edge->vert)->second;
		v3 = vertexToIndex_map->find(face->edge->next->vert)->second;

		if (v1 == v2 || v2 == v3 || v1 == v3)
			return true;
	}
	return false;
}
//----------------------------------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> CarveConnector::meshSetToVTKPolyData(unique_ptr<carve::mesh::MeshSet<3> > &c)
{
	// Create points
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetNumberOfPoints(c->vertex_storage.size());	// allocate memory

	boost::unordered_map<MeshSet<3>::vertex_t*, uint> vertexToIndex_map;	// vertex index map
	auto iter = c->vertex_storage.begin();
	for (int k = 0; iter != c->vertex_storage.end(); ++k, ++iter) {
		MeshSet<3>::vertex_t *vertex = &(*iter);

		//points->SetPoint(k, vertex->v.x, vertex->v.y, vertex->v.z);
		vertexToIndex_map[vertex] = k;
	}

	for (int k = 0; k < c->vertex_storage.size(); k++)
	{
		points->SetPoint(k, c->vertex_storage[k].v[0], c->vertex_storage[k].v[1], c->vertex_storage[k].v[2]);
	}
	// Create polygons (faces)
	vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();

	for (auto face_iter = c->faceBegin(); face_iter != c->faceEnd(); ++face_iter) {
		carve::mesh::MeshSet<3>::face_t *f = *face_iter;
		polygon->GetPointIds()->SetNumberOfIds(f->nVertices());

		if (Carve_checkDegeneratedFace(&vertexToIndex_map, f))
			continue;

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

		for (int j = 0; j < n_pts; j++)
			f[t++] = pts[j];

		//f[t++] = pts[0];
		//f[t++] = pts[1];
		//f[t++] = pts[2];

		//if (n_pts > 3)
		//f[t++] = pts[3];
		numfaces++;
	}
	// Construct MeshSet from vertices and faces
	unique_ptr<MeshSet<3> > first(new MeshSet<3>(vertices, numfaces, f));
	return first;
}
//---------------------------------------------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> CarveConnector::cleanVtkPolyData(vtkSmartPointer<vtkPolyData> &thepolydata, bool triangulate)
{
	thepolydata->GetPointData()->SetTCoords(nullptr);
	thepolydata->GetPointData()->SetNormals(nullptr);

	vtkSmartPointer<vtkPolyData> processed = thepolydata;

	if (triangulate)
	{
		vtkSmartPointer<vtkTriangleFilter> filter = vtkSmartPointer<vtkTriangleFilter>::New();
		filter->SetInputData(thepolydata);
		filter->PassLinesOff();
		filter->PassVertsOff();
		filter->Update();

		processed = filter->GetOutput();
	}

	vtkSmartPointer<vtkCleanPolyData> clean = vtkSmartPointer<vtkCleanPolyData>::New();
	clean->SetInputData(processed);
	//clean->SetPieceInvariant(false);
	//clean->SetConvertLinesToPoints(false);
	//clean->SetConvertPolysToLines(false);
	//clean->SetConvertStripsToPolys(false);
	//clean->PieceInvariantOn();
	//clean->ConvertPolysToLinesOn);
	//clean->ConvertLinesToPointsOn();
	//clean->ConvertStripsToPolysOn();
	clean->PointMergingOn();
	clean->Update();

	return vtkSmartPointer<vtkPolyData>(clean->GetOutput());
}