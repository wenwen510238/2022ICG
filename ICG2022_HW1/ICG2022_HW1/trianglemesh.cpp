#include "trianglemesh.h"
using namespace std;



// Constructor of a triangle mesh.
TriangleMesh::TriangleMesh()
{
	numVertices = 0;
	numTriangles = 0;
	numIndex = 0;
	objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	objExtent = glm::vec3(0.0f, 0.0f, 0.0f);
	// ---------------------------------------------------------------------------
    // Feel free to add codes.
    // ---------------------------------------------------------------------------
}

// Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh()
{
	vertices.clear();
	vertexIndices.clear();
	// ---------------------------------------------------------------------------
    // Feel free to add codes.
    // ---------------------------------------------------------------------------
}

void TriangleMesh::SavePoint(int i , vector<string> &NPoint) {
	string s1 ;
	int a,j;
	VertexPTN tmp;
	map<string, int>::iterator iter;
	s1 = NPoint[0];
	iter = Vertex_Index_Map.find(s1);
	//check same vertices//
	if (iter != Vertex_Index_Map.end()) { //attribute exist
		vertexIndices.push_back(Vertex_Index_Map[s1]);
	}
	else {
		a = 0;
		Vertex_Index_Map[s1] = numIndex;
		for (j = 0; s1[j] != '/'; j++) {
			if (s1[j] != '-')	a = a * 10 + (s1[j] - 48);
		}
		tmp.position = vertexp[a - 1];
		vertexIndices.push_back(numIndex++);
		a = 0;
		for (j = j + 1; s1[j] != '/'; j++) {
			if (s1[j] != '-')	a = a * 10 + (s1[j] - 48);
		}
		tmp.texcoord = vertext[a - 1];
		a = 0;
		for (j = j + 1; j < s1.size(); j++) {
			if (s1[j] != '-')	a = a * 10 + (s1[j] - 48);
		}
		tmp.normal = vertexn[a - 1];
		vertices.push_back(tmp);
	}
	for (int v = i - 1; v <= i; v++) {
		s1 = NPoint[v];
		iter = Vertex_Index_Map.find(s1);
		//check same vertices//
		if (iter != Vertex_Index_Map.end()) {//attribute exist
			vertexIndices.push_back(Vertex_Index_Map[s1]);
		}
		else {
			a = 0;
			Vertex_Index_Map[s1] = numIndex;
			for (j = 0; s1[j] != '/'; j++) {
				if (s1[j] != '-')	a = a * 10 + (s1[j] - 48);
			}
			tmp.position = vertexp[a - 1];
			vertexIndices.push_back(numIndex++);
			a = 0;
			for (j = j + 1; s1[j] != '/'; j++) {
				if (s1[j] != '-')	a = a * 10 + (s1[j] - 48);
			}
			tmp.texcoord = vertext[a - 1];
			a = 0;
			for (j = j + 1; j < s1.size(); j++) {
				if (s1[j] != '-')	a = a * 10 + (s1[j] - 48);
			}
			tmp.normal = vertexn[a - 1];
			vertices.push_back(tmp);
		}
	}
}

// Load the geometry and material data from an OBJ file.
bool TriangleMesh::LoadFromFile(const std::string& filePath)
{	
	ifstream infile(filePath.c_str()); //打開指定文件
	string sline="";//每一行
	string s1;
	GLfloat f2, f3, f4;
	int  flag=0;
	GLfloat min_x = FLT_MAX , min_y = FLT_MAX, min_z = FLT_MAX, max_x = FLT_MIN, max_y = FLT_MIN, max_z = FLT_MIN;
	GLfloat normalized_x, normalized_y, normalized_z, normal;
	while (getline(infile, sline)) //從指定文件逐行讀取
	{
		VertexPTN tmp;
		glm::vec2 tmpt;
		glm::vec3 tmpn;
		glm::vec3 tmpp;
		vector<string> NPoint;

		if (sline[0] == 'v')//how many vertices
		{
			istringstream sin(sline);//用空格做切割
			if (sline[1] == 't') { //vertex texture
				sin >> s1 >> f2 >> f3;
				tmpt = vec2(f2, f3);
				vertext.push_back(tmpt);
			}
			else if (sline[1] == 'n') { //vertex normal
				sin >> s1 >> f2 >> f3 >> f4;
				tmpn = vec3(f2, f3, f4);
				vertexn.push_back(tmpn);
			}
			else {//vertex position
				numVertices++;
				sin >> s1 >> f2 >> f3 >> f4;
				tmpp = vec3(f2, f3, f4);
				vertexp.push_back(tmpp);
				objCenter += tmpp;
				if (f2 < min_x)	min_x = f2;
				if (f3 < min_y)	min_y = f3;
				if (f4 < min_z)	min_z = f4;
				if (f2 > max_x)	max_x = f2;
				if (f3 > max_y)	max_y = f3;
				if (f4 > max_z)	max_z = f4;
			}
		}
		else if (sline[0] == 'f')
		{
			///normalized///
			if (!flag) {
				objCenter /= numVertices;
				normal = max_x - min_x;
				if (max_y - min_y > normal)	normal = max_y - min_y;
				if (max_z - min_z > normal)	normal = max_z - min_z;
				objExtent.x = max_x - min_x;
				objExtent.y = max_y - min_y;
				objExtent.z = max_z - min_z;
				for (int i = 0; i < numVertices; i++) {
					vertexp[i] -= objCenter;
					vertexp[i] /= normal;
				}
				flag = 1;
			}
			
			istringstream in(sline);
			in >> s1;//去掉f
			while (in >> s1) NPoint.push_back(s1);
			numTriangles += NPoint.size()-2;

			for (int i = 2; i < NPoint.size() ; i++) {
				SavePoint(i, NPoint);
			}
		}
	}
	return true;
}

// Apply transform to vertices using GPU.
void TriangleMesh::ApplyTransformCPU(const glm::mat4x4& mvpMatrix)
{
	for (int i = 0 ; i < GetnumVertices(); ++i) {
        glm::vec4 p = mvpMatrix * glm::vec4(vertices[i].position, 1.0f);
        if (p.w != 0.0f) {
            float inv = 1.0f / p.w; 
            vertices[i].position.x = p.x * inv;
            vertices[i].position.y = p.y * inv;
            vertices[i].position.z = p.z * inv;
        }
    }
}

// Show model information.
void TriangleMesh::ShowInfo()
{
	cout << "# Vertices: " << numVertices << std::endl;
	cout << "# Triangles: " << numTriangles << std::endl;
	cout << "Model Center: " << objCenter.x << ", " << objCenter.y << ", " << objCenter.z << std::endl;
	cout << "Model Extent: " << objExtent.x << " x " << objExtent.y << " x " << objExtent.z << std::endl;
	//cout << "Vertex_Index_Map.size(): " << Vertex_Index_Map.size() << endl;
	cout << endl;
}

