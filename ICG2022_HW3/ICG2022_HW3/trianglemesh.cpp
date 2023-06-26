#include "trianglemesh.h"
#include "material.h"
#include "imagetexture.h"
using namespace std;
SubMesh sub;

// Constructor of a triangle mesh.
TriangleMesh::TriangleMesh()
{
	numVertices = 0;
	numTriangles = 0;
	numIndex = 0;
	objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	objExtent = glm::vec3(0.0f, 0.0f, 0.0f);
	vector<PhongMaterial*> dicMat;
	// ---------------------------------------------------------------------------
	// Feel free to add codes.
	// ---------------------------------------------------------------------------
}

// Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh()
{
	vertices.clear();
	vertexIndices.clear();
	subMeshes.clear();
	/*for (int i = 0 ; dicMat.size(); i++) {
		delete dicMat[i];
		dicMat[i] = nullptr;
	}*/
	dicMat.clear();
	// ---------------------------------------------------------------------------
	// Feel free to add codes.
	// ---------------------------------------------------------------------------
}

void TriangleMesh::SavePoint(int i, vector<string>& NPoint) {
	//將三角形三個點依序放入，如有01234 五個點，依序存入012-023-034
	string s1;
	int a, j;
	VertexPTN tmp;
	//SubMesh tmpSubMesh;
	map<string, int>::iterator iter;
	s1 = NPoint[0];
	iter = Vertex_Index_Map.find(s1);

	//check same vertices//
	if (iter != Vertex_Index_Map.end()) { //attribute exist
		vertexIndices.push_back(Vertex_Index_Map[s1]);
		sub.vertexIndices.push_back(Vertex_Index_Map[s1]);
	}
	else {
		a = 0;
		Vertex_Index_Map[s1] = numIndex;
		for (j = 0; s1[j] != '/'; j++) {
			if (s1[j] != '-')	a = a * 10 + (s1[j] - 48);
		}
		tmp.position = vertexp[a - 1];
		vertexIndices.push_back(numIndex);
		sub.vertexIndices.push_back(numIndex++);
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
	//三角形後面兩個頂點
	for (int v = i - 1; v <= i; v++) {
		s1 = NPoint[v];
		iter = Vertex_Index_Map.find(s1);
		//check same vertices//
		if (iter != Vertex_Index_Map.end()) {//attribute exist
			vertexIndices.push_back(Vertex_Index_Map[s1]);
			sub.vertexIndices.push_back(Vertex_Index_Map[s1]);
		}
		else {
			a = 0;
			Vertex_Index_Map[s1] = numIndex;
			for (j = 0; s1[j] != '/'; j++) {
				if (s1[j] != '-')	a = a * 10 + (s1[j] - 48);
			}
			tmp.position = vertexp[a - 1];
			vertexIndices.push_back(numIndex);
			sub.vertexIndices.push_back(numIndex++);
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
	cout << "filePath：" << filePath << endl;
	ifstream infile(filePath.c_str()); //打開指定文件
	string sline = "";//每一行
	string fline = "";
	int hasNormalize = 0;
	string s1, mtlType, usemtl;
	GLfloat f2, f3, f4;
	int flag = 0;
	int lastf = 0;
	GLfloat min_x = FLT_MAX, min_y = FLT_MAX, min_z = FLT_MAX, max_x = FLT_MIN, max_y = FLT_MIN, max_z = FLT_MIN;
	GLfloat normalized_x, normalized_y, normalized_z, normal;
	PhongMaterial* mat = nullptr;
	
	while (getline(infile, sline)) //從指定文件逐行讀取
	{
		VertexPTN tmp;

		glm::vec2 tmpt;
		glm::vec3 tmpn;
		glm::vec3 tmpp;


		string PointType, mtlPath;

		if (sline[0] == 'm') { //進入mtl檔案開始讀取
			istringstream sin(sline);
			sin >> PointType >> mtlPath;
			cout << "mtlPath: " << mtlPath << endl;
			string mtlLine = "";
			ifstream inmtlfile(mtlPath.c_str());//打開mtl檔案
			
			while (getline(inmtlfile, mtlLine)) {
				
				istringstream sinMtl(mtlLine);//用空格做切割
				string newMtl, mtl;
				sinMtl >> newMtl >> mtl;

				if (newMtl != "newmtl") {
					continue;//會從上面迴圈開始跑
				}
				mat = new PhongMaterial();
				mat->SetName(mtl);

				GLfloat NsVal, c1, c2, c3;
				string cofficient, map_Kd, mapKdPath;
				
				for (int i = 0; i < 4; i++) {//save material cofficient
					mtlLine = "";
					getline(inmtlfile, mtlLine);
					istringstream sinNs(mtlLine);//用空格做切割
					sinNs >> cofficient;
					if (cofficient == "Ns") {
						sinNs >> NsVal;
						mat->SetNs(NsVal);
					}
					else if (cofficient == "Ka") {
						sinNs >> c1 >> c2 >> c3;
						mat->SetKa(vec3(c1, c2, c3));
					}
					else if (cofficient == "Kd") {
						sinNs >> c1 >> c2 >> c3;
						mat->SetKd(vec3(c1, c2, c3));
					}
					else if (cofficient == "Ks") {
						sinNs >> c1 >> c2 >> c3;
						mat->SetKs(vec3(c1, c2, c3));
					}
				}

				
				mtlLine = "";
				getline(inmtlfile, mtlLine);
				istringstream sinNs(mtlLine);//用空格做切割
				sinNs >> map_Kd;
				cout << "map_kd :" << map_Kd << endl;
				if (map_Kd != "map_Kd") {
					mat->SetHaveMapKd(0);
					continue;
				}
				else {
					sinNs >> mapKdPath;
					cout << "have map_Kd path : " << mapKdPath << endl;
					ImageTexture* imgtex = new ImageTexture(mapKdPath);
					mat->SetMapKd(imgtex);
					mat->SetHaveMapKd(1);
					getline(inmtlfile, mtlLine);//讀空行
				}
				dicMat.push_back(mat);
			}
			/*cout << "list all of the Material" << endl;
			for (int i = 0; i < dicMat.size(); i++) {
				cout << "matName()" << dicMat[i]->GetName() << endl;
				cout << "matNs()" << dicMat[i]->GetNs() << endl;
				cout << "matKa()" << dicMat[i]->GetKa().x << "," << dicMat[i]->GetKa().y << "," << dicMat[i]->GetKa().z << endl;
				cout << "matKd()" << dicMat[i]->GetKd().x << "," << dicMat[i]->GetKd().y << "," << dicMat[i]->GetKd().z << endl;
				cout << "matKs()" << dicMat[i]->GetKs().x << "," << dicMat[i]->GetKs().y << "," << dicMat[i]->GetKs().z << endl << endl;
			}*/
			lastf = 0;
			inmtlfile.close();
		}

		else if (sline[0] == 'v')//how many vertices
		{
			if (lastf == 1) {
				subMeshes.push_back(sub);
				sub.vertexIndices.clear();
			}
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
			lastf = 0;
		}
		else if (sline[0] == 'u') {
			istringstream sin(sline);
			sin >> usemtl >> mtlType;
			if (lastf == 1) {
				subMeshes.push_back(sub);
				sub.vertexIndices.clear();
			}

			lastf = 0;
			flag = 0;
		}
		else if (sline[0] == 'f') {
			int test = 0;
			vector<string> NPoint;
			//cout << "sline " << test++ << endl << sline << endl;
			istringstream in(sline);
			in >> s1;//去掉f
			while (in >> s1) NPoint.push_back(s1);
			numTriangles += NPoint.size() - 2;
			//cout << "f Npoint " << NPoint.size() << endl;
			int tmp = 1;
			if (flag == 0) {
				for (int i = 0; i < dicMat.size(); i++) {
					if (dicMat[i]->GetName() == mtlType) {
						sub.material = dicMat[i];
						break;
					}
					else
					{
						tmp++;
					}
				}
			}
			for (int i = 2; i < NPoint.size(); i++) {
				SavePoint(i, NPoint);
			}
			flag = 1;
			lastf = 1;
		}
		
	}
	infile.close();
	//if (mat != nullptr) {
	//	delete mat;
	//	mat = nullptr;
	//}
	if (lastf == 1) {
		subMeshes.push_back(sub);
		sub.vertexIndices.clear();
	}
	cout << "SubMeshes size：" << subMeshes.size() << endl;
	/*for (int i = 0; i < subMeshes.size(); i++) {
		cout << "SubMeshes " << i << " " << subMeshes[i].material->GetName() << endl;
		cout << "vertexIndices size：" << subMeshes[i].vertexIndices.size() << endl;
		cout << subMeshes[i].material->GetName() << endl;
		for (int j = 0; j < subMeshes[i].vertexIndices.size(); j++) {
			cout << subMeshes[i].vertexIndices[j] << " ";
		}
		cout << endl << endl;
	}*/
	objCenter /= numVertices;
	normal = max_x - min_x;
	if (max_y - min_y > normal)	normal = max_y - min_y;
	if (max_z - min_z > normal)	normal = max_z - min_z;
	objExtent.x = max_x - min_x;
	objExtent.y = max_y - min_y;
	objExtent.z = max_z - min_z;
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].position -= objCenter;
		vertices[i].position /= normal;
	}
	return true;
}

// Apply transform to vertices using GPU.
/*
void TriangleMesh::ApplyTransformCPU(const glm::mat4x4& mvpMatrix)
{
	for (int i = 0; i < GetnumVertices(); ++i) {
		glm::vec4 p = mvpMatrix * glm::vec4(vertices[i].position, 1.0f);
		if (p.w != 0.0f) {
			float inv = 1.0f / p.w;
			vertices[i].position.x = p.x * inv;
			vertices[i].position.y = p.y * inv;
			vertices[i].position.z = p.z * inv;
		}
	}
}*/

void TriangleMesh::CreateBuffers()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPTN) * GetnumVertices(), GetFirstMemPosi_Of_Vertices(), GL_STATIC_DRAW);

	for (int i = 0; i < subMeshes.size(); i++) {
		glGenBuffers(1, &subMeshes[i].iboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMeshes[i].iboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * subMeshes[i].GetnumVertexIndices(), GetMemPosi_Of_SubMesh_VertexIndices(i), GL_STATIC_DRAW);
	}
}

void TriangleMesh::Draw(const SubMesh& obj) {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, (const GLvoid*)12);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, (const GLvoid*)24);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.iboId);
	glDrawElements(GL_TRIANGLES, obj.vertexIndices.size(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

// Show model information.
void TriangleMesh::ShowInfo()
{
	cout << "# Vertices: " << numVertices << std::endl;
	cout << "# Triangles: " << numTriangles << std::endl;
	std::cout << "Total " << subMeshes.size() << " subMeshes loaded" << std::endl;
	for (unsigned int i = 0; i < subMeshes.size(); ++i) {
		const SubMesh& g = subMeshes[i];
		std::cout << "SubMesh " << i << " with material: " << g.material->GetName() << std::endl;
		std::cout << "Num. triangles in the subMesh: " << g.vertexIndices.size() / 3 << std::endl;
	}
	cout << "Model Center: " << objCenter.x << ", " << objCenter.y << ", " << objCenter.z << std::endl;
	cout << "Model Extent: " << objExtent.x << " x " << objExtent.y << " x " << objExtent.z << std::endl;
	//cout << "Vertex_Index_Map.size(): " << Vertex_Index_Map.size() << endl;
	cout << endl;
}

