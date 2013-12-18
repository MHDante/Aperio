#include "stdafx.h"

// @author David Tran
// Wrote this .obj loader (supports 2D textures, normals, vertices, faces, groups) 
// NOTE: faces max 5 indices [change MAX_INDICES constant in header]
//
// 3D textures not supported (Search for 3D Texture - converted to 2D u,v coordinates)

#include "obj_parser.h"

#include <iostream>
#include <vector>
#include <time.h>
#include <windows.h>
#include <string.h>

using namespace std;
using namespace obj;

// Constructor
obj_parser::obj_parser()
{
	currentMeshIndex = -1;

	vertices.clear();
	normals.clear();
	tcoords.clear();

	themeshes.clear();
}

void obj_parser::addFace(Face f)
{
	if (themeshes.size() == 0)
	{
		// Trying to add a face to a non existant group, make a default group
		addGroup("default");
	}
	themeshes[currentMeshIndex].polys.push_back(f);
}

void obj_parser::addGroup(string name)
{
	currentMeshIndex++;

	TheMesh m;
	strcpy_s(m.name, name.c_str());
	themeshes.push_back(m);
}

void obj_parser::addVertex(float x, float y, float z)
{
	Vertex v = { x, y, z };
	vertices.push_back(v);
}

void obj_parser::addTCoord(float u, float v)
{
	Tex t = { u, v };
	tcoords.push_back(t);
}

void obj_parser::addNormal(float x, float y, float z)
{
	Vertex v = { x, y, z };
	normals.push_back(v);
}

bool obj_parser::parse(string fileName)
{
	FILE *in = fopen(fileName.c_str(), "r");
	if (in == NULL)
	{
		cout << "File " << fileName << " not found";
		return false;
	}

	bool everything_ok = true;
	const int MAX_LINE = 1024;
	char rawLine[MAX_LINE];
	float xyz[3];

	int lineNr = 0;
	while (everything_ok && fgets(rawLine, MAX_LINE, in) != NULL)
	{
		lineNr++;
		char *pLine = rawLine;
		char *pEnd = rawLine + strlen(rawLine);

		// find the first non-whitespace character
		while (isspace(*pLine) && pLine < pEnd) { pLine++; }

		// this first non-whitespace is the command
		const char *cmd = pLine;

		// skip over non-whitespace
		while (!isspace(*pLine) && pLine < pEnd) { pLine++; }

		// terminate command
		if (pLine < pEnd)
		{
			*pLine = '\0';
			pLine++;
		}

		// in the OBJ format the first characters determine how to interpret the line:
		if (strcmp(cmd, "v") == 0)
		{
			// this is a vertex definition, expect three floats, separated by whitespace:
			if (sscanf(pLine, "%f %f %f", xyz, xyz + 1, xyz + 2) == 3)
			{
				addVertex(xyz[0], xyz[1], xyz[2]);
			}
			else
			{
				cout << "Error reading 'v' at line " << lineNr;
				everything_ok = false;
			}
		}
		else if (strcmp(cmd, "vt") == 0)
		{
			int num = sscanf(pLine, "%f %f %f", xyz, xyz + 1, xyz + 2);
			addTCoord(xyz[0], xyz[1]);
		}
		else if (strcmp(cmd, "vn") == 0)
		{
			// this is a normal, expect three floats, separated by whitespace:
			if (sscanf(pLine, "%f %f %f", xyz, xyz + 1, xyz + 2) == 3)
			{
				addNormal(xyz[0], xyz[1], xyz[2]);
			}
			else
			{
				cout << "Error reading 'vn' at line " << lineNr;
				everything_ok = false;
			}
		}
		else if (strcmp(cmd, "f") == 0)
		{
			// this is a face definition, consisting of 1-based indices separated by whitespace and /
			Face f;
			f.numIndices = 0;
			f.haveNormals = false;
			f.haveTextures = false;
			int currentIndex = 0;

			while (everything_ok && pLine < pEnd)
			{
				// find the first non-whitespace character
				while (isspace(*pLine) && pLine < pEnd) { pLine++; }

				if (pLine < pEnd)         // there is still data left on this line
				{
					int iVert, iTCoord, iNormal;
					if (sscanf(pLine, "%d/%d/%d", &iVert, &iTCoord, &iNormal) == 3)
					{
						f.indices[currentIndex] = iVert - 1;
						f.indicesN[currentIndex] = iNormal - 1;
						f.indicesT[currentIndex] = iTCoord - 1;
						f.haveNormals = true;
						f.haveTextures = true;
						f.numIndices++;

						currentIndex++;
					}
					else if (sscanf(pLine, "%d//%d", &iVert, &iNormal) == 2)
					{
						f.indices[currentIndex] = iVert - 1;
						f.indicesN[currentIndex] = iNormal - 1;
						f.haveNormals = true;
						f.haveTextures = false;
						f.numIndices++;

						currentIndex++;
					}
					else if (sscanf(pLine, "%d/%d", &iVert, &iTCoord) == 2)
					{
						f.indices[currentIndex] = iVert - 1;
						f.indicesT[currentIndex] = iTCoord - 1;
						f.haveNormals = false;
						f.haveTextures = true;
						f.numIndices++;

						currentIndex++;
					}
					else if (sscanf(pLine, "%d", &iVert) == 1)
					{
						f.indices[currentIndex] = iVert - 1;
						f.haveNormals = false;
						f.haveTextures = false;
						f.numIndices++;

						currentIndex++;
					}
					else if (strcmp(pLine, "\\\n") == 0)
					{
						// handle backslash-newline continuation
						if (fgets(rawLine, MAX_LINE, in) != NULL)
						{
							lineNr++;
							pLine = rawLine;
							pEnd = rawLine + strlen(rawLine);
							continue;
						}
						else
						{
							cout << "Error reading continuation line at line " << lineNr;
							everything_ok = false;
						}
					}
					else
					{
						cout << "Error reading 'f' at line " << lineNr;
						everything_ok = false;
					}
					// skip over what we just read
					// (find the first whitespace character)
					while (!isspace(*pLine) && pLine < pEnd) { pLine++; }
				}
			}
			addFace(f);
		}
		else if (strcmp(cmd, "g") == 0)
		{
			char label[MAX_LINE];
			// this is the start of a mesh object
			if (sscanf(pLine, "%s\n", label) == 1)
			{
				strcpy(label, pLine);	// to get full groupname with spaces and without newline
				if (label[strlen(label) - 1] == '\n')
					label[strlen(label) - 1] = '\0';

				addGroup(label);
			}
		}
	}
	fclose(in);

	// After reading all vertices, tcoords, normals into giant arrays, separate them for each group (only ones they're using)
	separateVerticesIntoGroups();
	return everything_ok;
}

/// <summary>
/// All the vertices, tcoords and normals are read into one giant vector, this function
/// separates the vertices, tcoords and normals individually for each group
/// in their respective [mesh.vertices], [mesh.tcoords], [mesh.normals] (by reading face indices
/// and adding only vertices the group uses)
/// </summary>
//void obj_parser::separateVerticesIntoGroups2()
//{
//	// need enough slots in array to map oldindex->newindex 
//	int MAX_VERTICES = vertices.size() * 2;
//
//	// Remember to deallocate this memory at end (already done check below)
//	int * vertexIndexMap = new int[MAX_VERTICES];
//	int * vertexIndexMapN = new int[MAX_VERTICES];
//	int * vertexIndexMapT = new int[MAX_VERTICES];
//
//	// per group
//	for (int i = 0; i < themeshes.size(); i++)
//	{
//		// for each group reset map to -1 (same as memset but cleaner)
//		std::fill(vertexIndexMap, vertexIndexMap + MAX_VERTICES, -1);
//		std::fill(vertexIndexMapN, vertexIndexMapN + MAX_VERTICES, -1);
//		std::fill(vertexIndexMapT, vertexIndexMapT + MAX_VERTICES, -1);
//
//		themeshes[i].vertices.reserve(themeshes[i].polys.size() * 4);
//		themeshes[i].tcoords.reserve(themeshes[i].polys.size() * 4);
//		themeshes[i].normals.reserve(themeshes[i].polys.size() * 4);
//
//		// per face
//		for (int j = 0; j < themeshes[i].polys.size(); j++)
//		{
//			// per index in face
//			for (int k = 0; k < themeshes[i].polys[j].numIndices; k++)
//			{
//				int index = themeshes[i].polys[j].indices[k];
//
//				if (vertexIndexMap[index] == -1)	// not exist, add it into vertices and new index
//				{
//					Vertex v = { vertices[index].p[0], vertices[index].p[1], vertices[index].p[2] };
//					themeshes[i].vertices.push_back(v);
//
//					int newindex = themeshes[i].vertices.size() - 1;
//
//					// reassign new index
//					themeshes[i].polys[j].indices[k] = newindex;
//
//					// map oldindex->newindex 
//					vertexIndexMap[index] = newindex;
//				}
//				else	// already exist, just reassign new index to one in the map
//				{
//					int newindex = vertexIndexMap[index];
//					themeshes[i].polys[j].indices[k] = newindex;
//				}
//
//				if (themeshes[i].polys[j].haveNormals)
//				{
//					int index = themeshes[i].polys[j].indicesN[k];
//
//					if (vertexIndexMapN[index] == -1)	// not exist, add it into vertices and new index
//					{
//						Vertex v = { normals[index].p[0], normals[index].p[1], normals[index].p[2] };
//						themeshes[i].normals.push_back(v);
//
//						int newindex = themeshes[i].normals.size() - 1;
//
//						// reassign new index
//						themeshes[i].polys[j].indicesN[k] = newindex;
//
//						// map oldindex->newindex 
//						vertexIndexMapN[index] = newindex;
//					}
//					else	// already exist, just reassign new index to one in the map
//					{
//						int newindex = vertexIndexMapN[index];
//						themeshes[i].polys[j].indicesN[k] = newindex;
//					}
//				}
//
//				if (themeshes[i].polys[j].haveTextures)
//				{
//					int index = themeshes[i].polys[j].indicesT[k];
//
//					if (vertexIndexMapT[index] == -1)	// not exist, add it into vertices and new index
//					{
//						Tex t = { tcoords[index].p[0], tcoords[index].p[1] };
//						themeshes[i].tcoords.push_back(t);
//
//						int newindex = themeshes[i].tcoords.size() - 1;
//
//						// reassign new index
//						themeshes[i].polys[j].indicesT[k] = newindex;
//
//						// map oldindex->newindex 
//						vertexIndexMapT[index] = newindex;
//					}
//					else	// already exist, just reassign new index to one in the map
//					{
//						int newindex = vertexIndexMapT[index];
//						themeshes[i].polys[j].indicesT[k] = newindex;
//					}
//				}
//			}
//		}
//	}
//	// Deallocated map memory
//	delete[] vertexIndexMap;
//	delete[] vertexIndexMapN;
//	delete[] vertexIndexMapT;
//}

void obj_parser::separateVerticesIntoGroups()
{
	// need enough slots in array to map oldindex->newindex 
	int MAX_VERTICES = vertices.size() * 2;

	// Remember to deallocate this memory at end (already done check below)
	int * vertexIndexMap = new int[MAX_VERTICES];
	int * vertexIndexMapN = new int[MAX_VERTICES];
	int * vertexIndexMapT = new int[MAX_VERTICES];

	// per group
	for (int i = 0; i < themeshes.size(); i++)
	{
		// for each group reset map to -1 (same as memset but cleaner)
		std::fill(vertexIndexMap, vertexIndexMap + MAX_VERTICES, -1);
		std::fill(vertexIndexMapN, vertexIndexMapN + MAX_VERTICES, -1);
		std::fill(vertexIndexMapT, vertexIndexMapT + MAX_VERTICES, -1);

		themeshes[i].vertices.reserve(themeshes[i].polys.size() * MAX_INDICES);
		themeshes[i].tcoords.reserve(themeshes[i].polys.size() * MAX_INDICES);
		themeshes[i].normals.reserve(themeshes[i].polys.size() * MAX_INDICES);

		// per face
		for (int j = 0; j < themeshes[i].polys.size(); j++)
		{
			// per index in face
			for (int k = 0; k < themeshes[i].polys[j].numIndices; k++)
			{
				int index = themeshes[i].polys[j].indices[k];

				if (vertexIndexMap[index] == -1)	// not exist, add it into vertices and new index
				{
					Vertex v = { vertices[index].p[0], vertices[index].p[1], vertices[index].p[2] };
					themeshes[i].vertices.push_back(v);

					int newindex = themeshes[i].vertices.size() - 1;

					// reassign new index
					themeshes[i].polys[j].indices[k] = newindex;

					// map oldindex->newindex 
					vertexIndexMap[index] = newindex;
				}
				else	// already exist, just reassign new index to one in the map
				{
					int newindex = vertexIndexMap[index];
					themeshes[i].polys[j].indices[k] = newindex;
				}

				if (themeshes[i].polys[j].haveNormals)
				{
					int index = themeshes[i].polys[j].indicesN[k];

					if (vertexIndexMapN[index] == -1)	// not exist, add it into vertices and new index
					{
						Vertex v = { normals[index].p[0], normals[index].p[1], normals[index].p[2] };
						themeshes[i].normals.push_back(v);

						int newindex = themeshes[i].normals.size() - 1;

						// reassign new index
						themeshes[i].polys[j].indicesN[k] = newindex;

						// map oldindex->newindex 
						vertexIndexMapN[index] = newindex;
					}
					else	// already exist, just reassign new index to one in the map
					{
						int newindex = vertexIndexMapN[index];
						themeshes[i].polys[j].indicesN[k] = newindex;
					}
				}

				if (themeshes[i].polys[j].haveTextures)
				{
					int index = themeshes[i].polys[j].indicesT[k];

					if (vertexIndexMapT[index] == -1)	// not exist, add it into vertices and new index
					{
						Tex t = { tcoords[index].p[0], tcoords[index].p[1] };
						themeshes[i].tcoords.push_back(t);

						int newindex = themeshes[i].tcoords.size() - 1;

						// reassign new index
						themeshes[i].polys[j].indicesT[k] = newindex;

						// map oldindex->newindex 
						vertexIndexMapT[index] = newindex;
					}
					else	// already exist, just reassign new index to one in the map
					{
						int newindex = vertexIndexMapT[index];
						themeshes[i].polys[j].indicesT[k] = newindex;
					}
				}
			}
		}
	}
	// Deallocated map memory
	delete[] vertexIndexMap;
	delete[] vertexIndexMapN;
	delete[] vertexIndexMapT;
}

void obj_parser::separateVerticesIntoGroups2()
{
	// need enough slots in array to map oldindex->newindex 
	//int MAX_VERTICES = vertices.size();

	// Remember to deallocate this memory at end (already done check below)
	//int * vertexIndexMap = new int[MAX_VERTICES];
	//int * vertexIndexMapN = new int[MAX_VERTICES];
	//int * vertexIndexMapT = new int[MAX_VERTICES];

	// per group
	for (int i = 0; i < themeshes.size(); i++)
	{
		// for each group reset map to -1 (same as memset but cleaner)
		//std::fill(vertexIndexMap, vertexIndexMap + MAX_VERTICES, -1);
		//std::fill(vertexIndexMapN, vertexIndexMapN + MAX_VERTICES, -1);
		//std::fill(vertexIndexMapT, vertexIndexMapT + MAX_VERTICES, -1);

		themeshes[i].vertices.reserve(themeshes[i].polys.size() * MAX_INDICES);
		themeshes[i].tcoords.reserve(themeshes[i].polys.size() * MAX_INDICES);
		themeshes[i].normals.reserve(themeshes[i].polys.size() * MAX_INDICES);

		//int currentIndex;
		//currentIndex = 0;

		// per face
		for (int j = 0; j < themeshes[i].polys.size(); j++)
		{
			// per index in face
			for (int k = 0; k < themeshes[i].polys[j].numIndices; k++)
			{
				int index = themeshes[i].polys[j].indices[k];

				// Add vertex from large array to local-mesh vertex array
				Vertex v = { vertices[index].p[0], vertices[index].p[1], vertices[index].p[2] };
				themeshes[i].vertices.push_back(v);

				// Update index to point to last element in local-mesh array 
				themeshes[i].polys[j].indices[k] = themeshes[i].vertices.size() - 1;

				if (themeshes[i].polys[j].haveNormals)
				{
					int index = themeshes[i].polys[j].indicesN[k];
					// Add normal from large array to local-mesh normal array
					Vertex v = { normals[index].p[0], normals[index].p[1], normals[index].p[2] };
					themeshes[i].normals.push_back(v);

					// Update index to point to last element in local-mesh array 
					themeshes[i].polys[j].indicesN[k] = themeshes[i].normals.size() - 1;
				}

				if (themeshes[i].polys[j].haveTextures)
				{
					int index = themeshes[i].polys[j].indicesT[k];
					// Add tcoord from large array to local-mesh tcoord array
					Tex v = { tcoords[index].p[0], tcoords[index].p[1] };
					themeshes[i].tcoords.push_back(v);

					// Update index to point to last element in local-mesh array 
					themeshes[i].polys[j].indicesT[k] = themeshes[i].tcoords.size() - 1;
				}

			}
		}
	}

	//std::cout << themeshes[0].vertices.size();
}