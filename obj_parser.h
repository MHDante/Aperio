#ifndef OBJ_OBJ_PARSER_HPP_INCLUDED
#define OBJ_OBJ_PARSER_HPP_INCLUDED

#include <vector>
#include <string.h>

using namespace std;

namespace obj {

	const int MAX_INDICES = 10;	// Max indices per face

	struct Vertex
	{
		float p[3];
	};
	struct Tex
	{
		float p[2];
	};
	struct Face
	{
		bool haveNormals;
		bool haveTextures;

		int numIndices;
		int indices[MAX_INDICES];		// 5 indices per face MAX (increase if needed)
		int indicesN[MAX_INDICES];	// Normals
		int indicesT[MAX_INDICES];	// Textures
	};
	struct TheMesh
	{
		char name[256];
		vector<Face> polys;

		vector<Vertex> vertices;
		vector<Tex> tcoords;
		vector<Vertex> normals;
	};

	// Class obj_parser
	class obj_parser
	{
	public:
		obj_parser();						// constructor
		bool parse(string fileName);

		// Access all mesh data here
		vector <TheMesh> themeshes;

	private:
		// Part of implementation (no need to access from outside)
		vector <Vertex> vertices;
		vector <Vertex> normals;
		vector <Tex> tcoords;

		void addFace(Face f);
		void addGroup(string name);

		void addVertex(float x, float y, float z);
		void addTCoord(float u, float v);
		void addNormal(float x, float y, float z);

		void obj_parser::separateVerticesIntoGroups();
		void obj_parser::separateVerticesIntoGroups2();
		int currentMeshIndex;
	};

} // namespace obj

#endif // OBJ_OBJ_PARSER_HPP_INCLUDED
