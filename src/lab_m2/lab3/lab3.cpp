#include "lab_m2/lab3/lab3.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m2;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab3::Lab3()
{
}


Lab3::~Lab3()
{
}


void Lab3::Init()
{
    nrInstances = 0;
    maxInstances = 50;
    shrink = 0;
    scaleFactor = 1.0f;

    normalLength = 0.3f;

    showNormals = true;

    auto camera = GetSceneCamera();
    camera->SetPositionAndRotation(glm::vec3(0, 5, 4), glm::quat(glm::vec3(-30 * TO_RADIANS, 0, 0)));
    camera->Update();

    // Load a mesh from file into GPU memory
    {
        Mesh* mesh = new Mesh("bamboo");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "vegetation", "bamboo"), "bamboo.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for rendering to texture
    {
        Shader *shader = new Shader("Instances");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "lab3", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "lab3", "shaders", "GeometryShader.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "lab3", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Create a shader program for rendering normals

    {
        Shader* shader = new Shader("Normals");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "lab3", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "lab3", "shaders", "AuxGeometryShader.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "lab3", "shaders", "FragmentShader2.glsl"), GL_FRAGMENT_SHADER);

        shader->CreateAndLink();

        shaders[shader->GetName()] = shader;

    }
}


void Lab3::FrameStart()
{
}


void Lab3::Update(float deltaTimeSeconds)
{
    ClearScreen();

    {
        auto shader = shaders["Instances"];

        shader->Use();

        int loc_instances = shader->GetUniformLocation("instances");
        float loc_scalefactor = shader->GetUniformLocation("scaleFactor");
        glUniform1i(loc_instances, nrInstances);
        glUniform1f(loc_scalefactor, scaleFactor);

        // TODO(student): Add a shrinking parameter for scaling each
        // triangle in the geometry shader

        // Note that we only render a single mesh!
        RenderMesh(meshes["bamboo"], shaders["Instances"], glm::vec3(-3.3f, 0.5f, 0), glm::vec3(0.1f));
    }
    // Render normals (dacă sunt activate)
    if (showNormals)
    {
        auto shader = shaders["Normals"];
        shader->Use();
        float loc_normal_length = shader->GetUniformLocation("normalLength");
        glUniform1f(loc_normal_length, normalLength);
        RenderMesh(meshes["bamboo"], shaders["Normals"], glm::vec3(-3.3f, 0.5f, 0), glm::vec3(0.1f));

    }



}


void Lab3::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab3::OnInputUpdate(float deltaTime, int mods)
{
    float factor = 1.5f; // baza multiplicării exponențiale

    if (window->KeyHold(GLFW_KEY_UP)) {
        scaleFactor *= pow(factor, deltaTime); // creștere exponențială lentă
        if (scaleFactor > 200.0f) scaleFactor = 200.0f;
    }

    if (window->KeyHold(GLFW_KEY_DOWN)) {
        scaleFactor /= pow(factor, deltaTime); // descreștere exponențială lentă
        if (scaleFactor < 0.1f) scaleFactor = 0.1f;
    }
}




void Lab3::OnKeyPress(int key, int mods)
{
    // Add key press event

    if (key == GLFW_KEY_EQUAL)
    {
        nrInstances++;
        nrInstances %= maxInstances;
        cout << "Trece pe egal";
    }

    if (key == GLFW_KEY_MINUS)
    {
        nrInstances--;
        nrInstances %= maxInstances;
        cout << "Trece pe minus";
    }
}


void Lab3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab3::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
