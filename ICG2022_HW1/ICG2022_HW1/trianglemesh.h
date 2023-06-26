#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

// OpenGL and FreeGlut headers.
#include <glew.h>
#include <freeglut.h>

// GLM.
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

// C++ STL headers.
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace std;
using namespace glm;

// VertexPTN Declarations.
struct VertexPTN
{
	VertexPTN() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		normal = glm::vec3(0.0f, 1.0f, 0.0f);
		texcoord = glm::vec2(0.0f, 0.0f);
	}
	VertexPTN(glm::vec3 p, glm::vec3 n, glm::vec2 uv) {
		position = p;
		normal = n;
		texcoord = uv;
	}
	glm::vec3 position;		// Vertex position.
	glm::vec3 normal;		// Vertex normal.
	glm::vec2 texcoord;		// Vertex texture coordinate.
};

// TriangleMesh Declarations.
class TriangleMesh
{
public:
	// TriangleMesh Public Methods.
	TriangleMesh();
	~TriangleMesh();
	
	// Load the model from an *.OBJ file.
	bool LoadFromFile(const std::string& filePath);
	
	// Apply transform on CPU.
	void ApplyTransformCPU(const glm::mat4x4& mvpMatrix);

	// Show model information.
	void ShowInfo();

	//Save f attribute to vertices.
	void SavePoint(int i, vector<string>& NPoint);

	//Get size of vertices. 
	int GetnumVertices() { return vertices.size();}

	//Get size of vertexIndices.
	int GetnumVertexIndices() { return vertexIndices.size();}

	//Get vertices start position.
	VertexPTN *GetFirstMemPosi_Of_Vertices() { return &vertices[0];}

	//Get vertexIndices start position.
	unsigned int * GetFirstMemPosi_Of_VertexIndices() { return &vertexIndices[0];}

private:
	// TriangleMesh Private Data.
	std::vector<VertexPTN> vertices;
	std::vector<unsigned int> vertexIndices;
	vector<glm::vec3> vertexp;
	vector<glm::vec3> vertexn;
	vector<glm::vec2> vertext;
	map <string, int> Vertex_Index_Map;
	int numVertices;
	int numTriangles;
	int numIndex;
	glm::vec3 objCenter;//normalization後應該要在(0, 0)
	glm::vec3 objExtent;//model範圍(normalization後應該要在(-0.5,0.5)之間)
};

#endif
