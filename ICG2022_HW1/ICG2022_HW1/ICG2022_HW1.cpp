// OpenGL and FreeGlut headers.
#include <Windows.h>

// OpenGL and FreeGlut headers.
#include <glew.h>
#include <freeglut.h>

// GLM.
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

// C++ STL headers.
#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <chrono>

// My headers.
#include "trianglemesh.h"

// Global variables.
#define MAX_BUFFER_SIZE     1024
#define LOAD_MODEL_EVENT       1

// Global variables.
const int screenWidth = 600;
const int screenHeight = 600;
TriangleMesh* mesh = nullptr;
GLuint vbo;
GLuint ibo;

// Function prototypes.
void CreatePopUpMenu();
void SetupRenderState();
void SetupScene(const std::string&);
void ReleaseResources();
std::string GetOpenModelFilePath();
void RenderSceneCB();
void ReshapeCB(int, int);
void ProcessSpecialKeysCB(int, int, int);
void ProcessKeysCB(unsigned char, int, int);
void ProcessMenuEvents(int);


// Callback function for glutDisplayFunc.
void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render the triangle mesh.
    // ---------------------------------------------------------------------------
    // Add your implementation.
    // ---------------------------------------------------------------------------
    if (mesh != nullptr) {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_TRIANGLES, mesh->GetnumVertexIndices(), GL_UNSIGNED_INT, 0);
        glDisableVertexAttribArray(0);
    }
    
    glutSwapBuffers();
}

// Callback function for glutSpecialFunc.
void ProcessSpecialKeysCB(int key, int x, int y)
{
    // Handle special (functional) keyboard inputs such as F1, spacebar, page up, etc. 
    switch (key) {
    case GLUT_KEY_F1:
        // Render with point mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case GLUT_KEY_F2:
        // Render with line mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case GLUT_KEY_F3:
        // Render with fill mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    default:
        break;
    }
}

// Callback function for glutKeyboardFunc.
void ProcessKeysCB(unsigned char key, int x, int y)
{
    // Handle other keyboard inputs those are not defined as special keys.
    if (key == 27) {
        // Release memory allocation if needed.
        ReleaseResources();
        exit(0);
    }
}

void ProcessMenuEvents(int option)
{
    switch (option) {
    case LOAD_MODEL_EVENT:
        std::string filePath = GetOpenModelFilePath();
        cout << "file path : " << filePath << endl;
        SetupScene(filePath);
        break;
    }
}

void CreatePopUpMenu()
{
    int menu = glutCreateMenu(ProcessMenuEvents);
    glutAddMenuEntry("Load 3D Model", LOAD_MODEL_EVENT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void ReleaseResources()
{
    // Release dynamically allocated resources.
    // ---------------------------------------------------------------------------
    // Add your implementation.
    // ---------------------------------------------------------------------------
    if (mesh != nullptr) {
        delete mesh;
        mesh = nullptr;
    }
}

string GetOpenModelFilePath()
{
    OPENFILENAME ofn;       // common dialog box structure.
    wchar_t szFile[260];    // buffer for file name.
    HWND hwnd;              // owner window.
    HANDLE hf;              // file handle.

    hwnd = GetForegroundWindow();

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"Model\0*.obj\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box. 
    if (GetOpenFileName(&ofn) == TRUE) {

        std::wstring ws(szFile);
        std::string str(ws.begin(), ws.end());
        return str;
    }
    return "";
}

void SetupRenderState()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::vec4 clearColor = glm::vec4(0.44f, 0.57f, 0.55f, 1.00f);
    glClearColor(
        (GLclampf)(clearColor.r), 
        (GLclampf)(clearColor.g), 
        (GLclampf)(clearColor.b), 
        (GLclampf)(clearColor.a)
    );
}

void SetupScene(const std::string& modelPath)
{
    if (modelPath.empty())
        return;

    // Release previous mesh data.
    if (mesh != nullptr) {
        delete mesh;
        mesh = nullptr;
    }

    // Load a model from obj file.
    mesh = new TriangleMesh();
    mesh->LoadFromFile(modelPath);
    mesh->ShowInfo();
    
    // Build transformation matrices.
    glm::mat4x4 M(1.0f);

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.5f, 2.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4x4 V = glm::lookAt(cameraPos, cameraTarget, cameraUp);

    float fov = 40.0f;
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    float zNear = 0.1f;
    float zFar = 100.0f;
    glm::mat4x4 P = glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);

    // Apply CPU transformation.
    glm::mat4x4 MVP = P * V * M;
    
    mesh->ApplyTransformCPU(MVP);
    
    // Create vertex and index buffer.
    // ---------------------------------------------------------------------------
    // Add your implementation.
    // ---------------------------------------------------------------------------
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPTN) * mesh->GetnumVertices(), mesh->GetFirstMemPosi_Of_Vertices(), GL_STATIC_DRAW);

    //cout << "sizeof(Vertices) : " << sizeof(glm::vec3) * mesh->GetnumVertices() << endl;

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->GetnumVertexIndices(), mesh->GetFirstMemPosi_Of_VertexIndices(), GL_STATIC_DRAW);
    //cout << "sizeof(VertexIndices) : " << sizeof(unsigned int) * mesh->GetnumVertexIndices() << endl;
    cout << "mesh->GetnumVertexIndices(): " << mesh->GetnumVertexIndices() << endl;
}

int main(int argc, char** argv)
{
    // Setting window properties.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("HW1: OBJ Loader");

    // Initialize GLEW.
    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        std::cerr << "GLEW initialization error: " 
                  << glewGetErrorString(res) << std::endl;
        return 1;
    }

    // Initialization.
    SetupRenderState();
    SetupScene("");

    CreatePopUpMenu();
    // Register callback functions.
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);
    glutSpecialFunc(ProcessSpecialKeysCB);
    glutKeyboardFunc(ProcessKeysCB);

    // Start rendering loop.
    glutMainLoop();

    return 0;
}
