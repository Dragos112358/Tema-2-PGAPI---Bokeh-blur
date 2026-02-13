#include "lab_m2/Tema1/Tema1.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m2;
#include "stb/stb_image.h"



//rotesc televizorul cu tastele 1 si 2
//cu tastele 3 si 4, schimb modul de vizualizare (vedere cu poligoane full vs vedere doar structura)



/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */
struct Particle
{
    glm::vec4 position;
    glm::vec4 speed;
    glm::vec4 initialPos;
    glm::vec4 initialSpeed;
    float delay;
    float initialDelay;
    float lifetime;
    float initialLifetime;

    Particle() {}

    Particle(const glm::vec4& pos, const glm::vec4& speed)
    {
        SetInitial(pos, speed);
    }

    void SetInitial(const glm::vec4& pos, const glm::vec4& speed,
        float delay = 0, float lifetime = 0)
    {
        position = pos;
        initialPos = pos;

        this->speed = speed;
        initialSpeed = speed;

        this->delay = delay;
        initialDelay = delay;

        this->lifetime = lifetime;
        initialLifetime = lifetime;
    }
};


ParticleEffect<Particle>* particleEffect2;
Tema1::Tema1()
{
    angle = 0;
    light_position = glm::vec3(0, 0, 0);
    light_direction = glm::normalize(glm::vec3(-1, -1, -1));
    light_space_near_plane = 0.1f;
    light_space_far_plane = 100.0f;
    draw_framebuffer_textures = true;
    framebuffer_object = 0;
    color_texture = 0;
    depth_texture = 0;
    depth_texture2 = 1;
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    light_position = glm::vec3(7, 7, 7);
    light_direction = glm::normalize(glm::vec3(-1, -1, -1));
    x_direction = 0.f;
    y_direction = 0.f;
    z_direction = 1.f;
    angle2 = 0;
    light_space_near_plane = 0.1f;
    light_space_far_plane = 30.0f;

    draw_framebuffer_textures = true;

    framebuffer_object = 0;
    framebuffer_object2 = 0;
    color_texture = 0;
    color_texture2 = 0;
    depth_texture = 0;
    depth_texture2 = 1;
    auto camera = GetSceneCamera();
    camera->SetPositionAndRotation(glm::vec3(0, 8, 8), glm::quat(glm::vec3(-40 * TO_RADIANS, 0, 0)));
    camera->Update();
    std::string texturePath = PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES, "cube");
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("cube");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("bunny");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "animals"), "bunny.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("archer");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "characters", "archer"), "Archer.fbx");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("bamboo");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "vegetation", "bamboo"), "bamboo.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }

    ToggleGroundPlane();
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "wood.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "vase.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "gray.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "textured_gray.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "textured_white.png");
    bool ok = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "gray.png");
    //TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "ground.jpg");
    if (!ok) {
        std::cout << "Eroare la incarcarea texturii!\n";
    }
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "particle2.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "ground.jpg");


    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS,
        "characters", "archer", "Akai_E_Espiritu.fbm"), "akai_diffuse.png");


    // Create a shader program for surface generation
    {
        Shader* shader = new Shader("SurfaceGeneration");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "GeometryShader.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader2 = new Shader("Fireworks");
        shader2->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Particle_fireworks.VS.glsl"), GL_VERTEX_SHADER);
        shader2->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Particle.GS.glsl"), GL_GEOMETRY_SHADER);
        shader2->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Particle_simple.FS.glsl"), GL_FRAGMENT_SHADER);
        shader2->CreateAndLink();
        shaders[shader2->GetName()] = shader2;
    }

    {
        Shader* shader3 = new Shader("Framebuffer");
        shader3->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Framebuffer.VS.glsl"), GL_VERTEX_SHADER);
        shader3->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Framebuffer.GS.glsl"), GL_GEOMETRY_SHADER);
        shader3->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Framebuffer.FS.glsl"), GL_FRAGMENT_SHADER);
        shader3->CreateAndLink();
        shaders[shader3->GetName()] = shader3;
    }

    {
        Shader* shader4 = new Shader("ShaderNormal");
        shader4->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Normal.VS.glsl"), GL_VERTEX_SHADER);
        shader4->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Normal.FS.glsl"), GL_FRAGMENT_SHADER);
        shader4->CreateAndLink();
        shaders[shader4->GetName()] = shader4;
    }
    {
        Shader* shader = new Shader("Screen");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Screen.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Screen.GS.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Screen.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("Rabbit");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Screen.VS.glsl"), GL_VERTEX_SHADER);
        //shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Screen.GS.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Screen.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    LoadShader("ShadowMappingPassOne");
    LoadShader("ShadowMappingPassTwo");
    /* {
        Shader* shader3 = new Shader("Screen");
        shader3->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Screen.VS.glsl"), GL_VERTEX_SHADER);
        shader3->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Screen.GS.glsl"), GL_GEOMETRY_SHADER);
        shader3->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Screen.FS.glsl"), GL_FRAGMENT_SHADER);
        shader3->CreateAndLink();
        shaders[shader3->GetName()] = shader3;
    }*/

    ResetParticlesFireworks2(20, 20, 20);
    // Parameters related to surface generation
    no_of_generated_points = 25;            // number of points on a Bezier curve
    no_of_instances = 25;                    // number of instances (number of curves that contain the surface)
    max_translate = 12.0f;                   // for the translation surface, it's the distance between the first and the last curve
    max_rotate = glm::radians(720.0f);      // for the rotation surface, it's the angle between the first and the last curve

    // Define control points
    /*control_p0 = glm::vec3(-4.0, -2.5, 1.0);
    control_p1 = glm::vec3(-2.5, 1.5, 1.0);
    control_p2 = glm::vec3(-1.5, 3.0, 1.0);
    control_p3 = glm::vec3(-4.0, 5.5, 1.0);*/
    // Picioare mai subțiri, înalte
    control_p0 = glm::vec3(-0.5f, 0.0f, 0.0f);  // baza jos stânga
    control_p1 = glm::vec3(-0.5f, 2.0f, 0.0f);
    control_p2 = glm::vec3(-0.5f, 4.0f, 0.0f);
    control_p3 = glm::vec3(-0.5f, 6.0f, 0.0f);  // vârful piciorului


    // Create a bogus mesh with 2 points (a line)
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(control_p0, glm::vec3(0, 1, 1)),
            VertexFormat(control_p3, glm::vec3(0, 1, 0))
        };

        vector<unsigned int> indices =
        {
            0, 1
        };

        meshes["surface"] = new Mesh("generated initial surface points");
        meshes["surface"]->InitFromData(vertices, indices);
        meshes["surface"]->SetDrawMode(GL_LINES);
    }
    generator_position2 = glm::vec3(6.75f, 7.4f, 3.2f);
    offset2 = 0.05;

    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "characters", "archer", "Akai_E_Espiritu.fbm"), "akai_diffuse.png");

    cubeMapTextureID = UploadCubeMapTexture(
        PATH_JOIN(texturePath, "pos_x.png"),
        PATH_JOIN(texturePath, "pos_y.png"),
        PATH_JOIN(texturePath, "pos_z.png"),
        PATH_JOIN(texturePath, "neg_x.png"),
        PATH_JOIN(texturePath, "neg_y.png"),
        PATH_JOIN(texturePath, "neg_z.png"));
    CreateFramebuffer(1024, 1024);
    CreateShadowFramebuffer(1024, 1024);
    //CreateFramebuffer(1280, 720);

    glGenFramebuffers(1, &fireworksFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, fireworksFBO);

    // Creează textura pentru FBO

    glGenTextures(1, &fireworksTexture);
    glBindTexture(GL_TEXTURE_2D, fireworksTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fireworksTexture, 0);

    // Check FBO
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "FBO for fireworks is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Tema1::DrawScene(Shader* shader)
{
    for (int i = 0; i < 16; i++)
    {
        float rotateAngle = (angle + i) * ((i % 2) * 2 - 1);
        glm::vec3 position = glm::vec3(-4 + (i % 4) * 2.5, 2, -2 + (i / 4) * 2.5);

        glm::mat4 modelMatrix = glm::translate(glm::mat4(1), position);
        modelMatrix = glm::rotate(modelMatrix, rotateAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

        RenderSimpleMesh(meshes["bamboo"], shader, modelMatrix, TextureManager::GetTexture("bamboo.png"));
    }

    glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(0.5f));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 2.0f, 0.0f)); // mutare pe OY cu +2
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f)); // scala 0.5 pe toate axele

    RenderSimpleMesh(meshes["plane"], shader, modelMatrix, TextureManager::GetTexture("ground.jpg"));
}





void Tema1::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    // Set shader uniforms for light properties
    GLint loc_light_position = glGetUniformLocation(shader->program, "light_position");
    glUniform3f(loc_light_position, light_position.x, light_position.y, light_position.z);

    GLint loc_light_direction = glGetUniformLocation(shader->program, "light_direction");
    glUniform3f(loc_light_direction, light_direction.x, light_direction.y, light_direction.z);

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    GLint eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set model matrix uniform
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Set view matrix uniform
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Set projection matrix uniform
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Set light space view matrix uniform
    GLint loc_light_space_view = glGetUniformLocation(shader->program, "light_space_view");
    glUniformMatrix4fv(loc_light_space_view, 1, GL_FALSE, glm::value_ptr(light_space_view));

    // Set light space projection matrix uniform
    GLint loc_light_space_projection = glGetUniformLocation(shader->program, "light_space_projection");
    glUniformMatrix4fv(loc_light_space_projection, 1, GL_FALSE, glm::value_ptr(light_space_projection));

    // Set uniform for the far plane
    GLint loc_light_space_far_plane = glGetUniformLocation(shader->program, "light_space_far_plane");
    glUniform1f(loc_light_space_far_plane, light_space_far_plane);

    // ========================================
    // FOLOSEȘTE TEXTURE UNITS 2 și 3 în loc de 0 și 1
    // pentru a evita conflictul cu cubemap-urile
    // ========================================

    // Texture unit 2 pentru textura obiectului

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // unbind cubemap
    glBindTexture(GL_TEXTURE_2D, 0);       // unbind orice 2D texture

    if (texture)
    {
        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 2); // ← SCHIMBAT din 0 în 2
    }

    // Texture unit 3 pentru shadow map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // unbind cubemap
    glBindTexture(GL_TEXTURE_2D, 0);       // unbind orice 2D texture
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glUniform1i(glGetUniformLocation(shader->program, "depth_texture"), 3); // ← SCHIMBAT din 1 în 3

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);

    // Cleanup: unbind după desenare
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Revin la texture unit 0 (pentru alte operații)
    glActiveTexture(GL_TEXTURE0);
}

unsigned int Tema1::UploadCubeMapTexture(const std::string& pos_x, const std::string& pos_y, const std::string& pos_z, const std::string& neg_x, const std::string& neg_y, const std::string& neg_z)
{
    int width, height, chn;

    unsigned char* data_pos_x = stbi_load(pos_x.c_str(), &width, &height, &chn, 0);
    unsigned char* data_pos_y = stbi_load(pos_y.c_str(), &width, &height, &chn, 0);
    unsigned char* data_pos_z = stbi_load(pos_z.c_str(), &width, &height, &chn, 0);
    unsigned char* data_neg_x = stbi_load(neg_x.c_str(), &width, &height, &chn, 0);
    unsigned char* data_neg_y = stbi_load(neg_y.c_str(), &width, &height, &chn, 0);
    unsigned char* data_neg_z = stbi_load(neg_z.c_str(), &width, &height, &chn, 0);

    unsigned int textureID = 0;
    // TODO(student): Create the texture
    GLuint texture;
    glGenTextures(1, &texture);

    // 2. Leagă textura ca un cubemap
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);


    // TODO(student): Bind the texture

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (GLEW_EXT_texture_filter_anisotropic) {
        float maxAnisotropy;

        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // TODO(student): Load texture information for each face
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pos_x);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pos_y);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pos_z);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_neg_x);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_neg_y);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_neg_z);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    if (GetOpenGLError() == GL_INVALID_OPERATION)
    {
        cout << "\t[NOTE] : For students : DON'T PANIC! This error should go away when completing the tasks." << std::endl;
    }
    if (!data_pos_x) cout << "Failed to load" << "\n";

    // Free memory
    SAFE_FREE(data_pos_x);
    SAFE_FREE(data_pos_y);
    SAFE_FREE(data_pos_z);
    SAFE_FREE(data_neg_x);
    SAFE_FREE(data_neg_y);
    SAFE_FREE(data_neg_z);

    return texture;
}
void Tema1::CreateShadowFramebuffer(int width, int height)
{
    // 1. Generează framebuffer-ul
    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

    // 2. Generează depth texture
    glGenTextures(1, &shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
        width, height, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // 3. Atașează depth texture la framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);

    // Nu avem color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // 4. Verifică dacă framebuffer-ul e complet
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: Shadow framebuffer not complete!" << std::endl;
    else
        std::cout << "Shadow framebuffer created: " << width << "x" << height << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Tema1::CreateFramebuffer(int width, int height)
{
    // TODO(student): In this method, use the attributes
    // 'framebuffer_object', 'color_texture'
    // declared in lab6.h

    // TODO(student): Generate and bind the framebuffer

    glGenFramebuffers(1, &framebuffer_object);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);
    // TODO(student): Generate and bind the color texture
    glGenTextures(1, &color_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, color_texture);
    // TODO(student): Initialize the color textures

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);



    if (color_texture) {
        //cubemap params
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (GLEW_EXT_texture_filter_anisotropic) {
            float maxAnisotropy;

            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Bind the color textures to the framebuffer as a color attachments
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        std::vector<GLenum> draw_textures;
        draw_textures.push_back(GL_COLOR_ATTACHMENT0);
        glDrawBuffers(draw_textures.size(), &draw_textures[0]);

    }

    // TODO(student): Generate and bind the depth texture
    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_texture);


    // TODO(student): Initialize the depth textures

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);



    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

    // Check if the framebuffer is complete


    glCheckFramebufferStatus(GL_FRAMEBUFFER);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    //cout << resolution.x << " " << resolution.y << "\n";
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::ResetParticlesFireworks2(int xSize, int ySize, int zSize)
{
    unsigned int nrParticles = 1000;

    particleEffect2 = new ParticleEffect<Particle>();
    particleEffect2->Generate(nrParticles, true);



    auto particleSSBO = particleEffect2->GetParticleBuffer();
    Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

    glm::vec3 generatorPos = cubeposition;
    float radius = 2.0f;
    /*generatorPos.x = generator_position2.x + radius * cos(-tvrotationangle);
    generatorPos.y = 9.65f;
    generatorPos.z = generator_position2.z + radius * sin(-tvrotationangle);*/
    cout << "Explozie = " << generatorPos << "\n";
    for (unsigned int i = 0; i < nrParticles; i++)
    {
        glm::vec4 pos(generatorPos + glm::vec3(0.0f, -4.0f, 10.0f), 1.0f);

        // direcție aleatorie
        float theta = ((float)(rand() % 360)) * TO_RADIANS; // unghi orizontal
        float phi = ((float)(rand() % 180)) * TO_RADIANS;   // unghi vertical
        //float speedMag = ((rand() % 10) / 10.0f) + 1.5f;    // viteza totală
        float speedMag = 1.0f;
        glm::vec4 speed;
        speed.x = speedMag * sin(phi) * cos(theta) * 3;
        speed.y = speedMag * cos(phi);
        speed.z = speedMag * sin(phi) * sin(theta);
        speed.w = 0.0f;

        data[i].SetInitial(pos, speed, 0.0f, 10.0f); // lifetime 3 secunde
    }

    particleSSBO->SetBufferData(data);
}
void Tema1::RenderMeshInstanced(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int instances, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object instanced
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElementsInstanced(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, (void*)0, instances);
}

void Tema1::DrawScreen(float x, float y, float z)
{
    auto camera = GetSceneCamera();
    Shader* shader = shaders["Screen"];
    shader->Use();

    // IMPORTANT: Bind cubemap texture ÎNAINTE de a seta alte uniforms
    if (color_texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, color_texture);
        glUniform1i(shader->GetUniformLocation("texture_cubemap"), 0);
    }
    else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
        glUniform1i(shader->GetUniformLocation("texture_cubemap"), 0);
    }

    // Bind textura pentru material (dacă geometry shader-ul o folosește)
    glActiveTexture(GL_TEXTURE1);
    TextureManager::GetTexture("textured_gray.png")->BindToTextureUnit(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader->program, "myTexture"), 1);


    glm::vec3 tvCenter = glm::vec3(x - 4.5f, y + 3.5f, z + 1.8f);

    // Pornim de la identitate
    glm::mat4 tvModel = glm::mat4(1.0f);

    // 1. Mutăm centrul la origine
    tvModel = glm::translate(tvModel, tvCenter);

    // 2. Aplicăm rotația
    tvModel = glm::rotate(tvModel, tvrotationangle, glm::vec3(0, 1, 0));

    // 3. Readucem centrul înapoi la poziția dorită
    tvModel = glm::translate(tvModel, -tvCenter);

    // 4. Aplicăm scala finală
    tvModel = glm::scale(tvModel, glm::vec3(0.7f, 0.7f, 0.7f));
    glm::mat4 modelMatrix3 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x + 0.5, y - 0.5, z)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    //RenderMeshInstanced(mesh, shader, modelMatrix3, no_of_instances);

    surface_type = 2;

    /*control_p0 = glm::vec3(6.0f, 0.0f, 0.0f);  // baza jos stânga
    control_p1 = glm::vec3(6.0f, 2.0f, 0.0f);
    control_p2 = glm::vec3(6.0f, 4.0f, 0.0f);
    control_p3 = glm::vec3(6.0f, 6.0f, 0.0f);  // vârful piciorului
    max_rotate = glm::radians(180.0f);
    no_of_generated_points = 5;
    surface_type = 4;
    no_of_instances = 100;*/
    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
    glUniform1i(glGetUniformLocation(shader->program, "direction"), 1);
    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix3));
    control_p3 = glm::vec3(-0.5f, 6.0f, 0.0f);  // vârful piciorului
    /*cout << control_p0.x << " " << control_p0.y << " " << control_p0.z << "\n";
    cout << control_p1.x << " " << control_p1.y << " " << control_p1.z << "\n";
    cout << control_p2.x << " " << control_p2.y << " " << control_p2.z << "\n";
    cout << control_p3.x << " " << control_p3.y << " " << control_p3.z << "\n";*/

    glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);
    glUniform1i(shader->GetUniformLocation("type"), type);
    glm::ivec2 resolution = window->GetResolution();
    float aspectCorrection = (float)resolution.x / (float)resolution.y;
    glUniform1f(shader->GetUniformLocation("aspect_correction"), aspectCorrection);
    //glUniform1i(shader->GetUniformLocation("direction"), 0);

    no_of_generated_points = 25;
    no_of_instances = 25;
    control_p0 = glm::vec3(-0.5f, 0.0f, 0.0f);  // baza jos stânga
    control_p1 = glm::vec3(-0.5f, 2.0f, 0.0f);
    control_p2 = glm::vec3(-0.5f, 4.0f, 0.0f);
    control_p3 = glm::vec3(-0.5f, 6.0f, 0.0f);  // vârful piciorului
    max_rotate = glm::radians(540.0f);

    auto cameraPosition = camera->m_transform->GetWorldPosition();
    int loc_camera = shader->GetUniformLocation("camera_position");
    glUniform3f(loc_camera, cameraPosition.x, cameraPosition.y, cameraPosition.z);


    //asta e extra
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fireworksTexture);
    glUniform1i(shader->GetUniformLocation("fireworks_texture"), 1);



    Mesh* mesh = meshes["surface"];
    RenderMeshInstanced(mesh, shader, modelMatrix3, no_of_instances);


}



void Tema1::DrawRoom(float x, float y, float z)
{
    if (on_screen == false) {
        float size = 20.0f;
        Shader* shader = shaders["SurfaceGeneration"];
        shader->Use();
        glUniform1f(glGetUniformLocation(shader->program, "time"), glfwGetTime());
        glUniform1i(glGetUniformLocation(shader->program, "myTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        TextureManager::GetTexture("textured_gray.png")->BindToTextureUnit(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
        // Send uniforms to shaders
        glm::vec3 tvCenter = glm::vec3(x - 4.5f, y + 3.5f, z + 1.8f);

        // Pornim de la identitate
        glm::mat4 tvModel = glm::mat4(1.0f);

        // 1. Mutăm centrul la origine
        tvModel = glm::translate(tvModel, tvCenter);

        // 2. Aplicăm rotația
        tvModel = glm::rotate(tvModel, 0.0f, glm::vec3(0, 1, 0));

        // 3. Readucem centrul înapoi la poziția dorită
        tvModel = glm::translate(tvModel, -tvCenter);

        // 4. Aplicăm scala finală
        tvModel = glm::scale(tvModel, glm::vec3(0.7f * size, 0.7f * size, 0.7f * size));
        surface_type = 2;
        glm::mat4 modelMatrix2 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));



        glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
        glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix2));
        glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
        glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
        glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);

        Mesh* mesh = meshes["surface"];

        // Draw the object instanced
        RenderMeshInstanced(mesh, shader, modelMatrix2, no_of_instances);

        glm::mat4 modelMatrix3 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x + 0.45, y - 0.5, z)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 0, 1)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
        glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix3));
        glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
        glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
        glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);

        RenderMeshInstanced(mesh, shader, modelMatrix3, no_of_instances);

        glm::mat4 modelMatrix4 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x - 6.0f, y - 0.5, z - 0.5f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.75f));
        glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
        glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix4));
        glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
        glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
        glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);

        RenderMeshInstanced(mesh, shader, modelMatrix4, no_of_instances);

        glm::mat4 modelMatrix5 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x - 6.0f, y - 0.5, z + 7.5f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.75f));
        glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
        glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix5));
        glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
        glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
        glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);

        RenderMeshInstanced(mesh, shader, modelMatrix5, no_of_instances);

        glm::mat4 modelMatrix6 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x - 5.5, y - 0.5, z)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 0, 1)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
        glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix6));
        glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
        glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
        glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);
        RenderMeshInstanced(mesh, shader, modelMatrix6, no_of_instances);

        glm::mat4 modelMatrix7 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x, y + 6.0f, z)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
        glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix7));
        glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
        glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
        glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);
        RenderMeshInstanced(mesh, shader, modelMatrix7, no_of_instances);

        surface_type = 6;
        max_rotate = glm::radians(180.0f);
    }
    else
    {

    }


}

void Tema1::LoadShader(const std::string& name)
{
    std::string shaderPath = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders");

    // Create a shader program
    {
        Shader* shader = new Shader(name);
        shader->AddShader(PATH_JOIN(shaderPath, name + ".VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, name + ".FS.glsl"), GL_FRAGMENT_SHADER);

        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
}
void Tema1::DrawTV(float x, float y, float z)
{
    /*if (render_to_cubemap == 1) {
        no_of_instances = 10;
        no_of_generated_points = 10;
    }*/
    Shader* shader = shaders["SurfaceGeneration"];
    shader->Use();
    glUniform1f(glGetUniformLocation(shader->program, "time"), glfwGetTime());
    glUniform1i(glGetUniformLocation(shader->program, "myTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    TextureManager::GetTexture("textured_gray.png")->BindToTextureUnit(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    // Send uniforms to shaders
    glm::vec3 tvCenter = glm::vec3(x - 4.5f, y + 3.5f, z + 1.8f);

    // Pornim de la identitate
    glm::mat4 tvModel = glm::mat4(1.0f);

    // 1. Mutăm centrul la origine
    tvModel = glm::translate(tvModel, tvCenter);

    // 2. Aplicăm rotația
    tvModel = glm::rotate(tvModel, tvrotationangle, glm::vec3(0, 1, 0));

    // 3. Readucem centrul înapoi la poziția dorită
    tvModel = glm::translate(tvModel, -tvCenter);

    // 4. Aplicăm scala finală
    tvModel = glm::scale(tvModel, glm::vec3(0.7f, 0.7f, 0.7f));
    surface_type = 2;
    glm::mat4 modelMatrix2 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));



    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix2));
    glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);

    // TODO(student): Send to the shaders the number of points that approximate
    // a curve (no_of_generated_points), as well as the characteristics for
    // creating the translation/rotation surfaces (max_translate, max_rotate).
    // NOTE: If you're feeling lost and need a frame of reference while doing
    // this lab, go to `FrameEnd()` and activate `DrawCoordinateSystem()`.

    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);

    Mesh* mesh = meshes["surface"];

    // Draw the object instanced
    RenderMeshInstanced(mesh, shader, modelMatrix2, no_of_instances);

    glm::mat4 modelMatrix3 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x + 0.45, y - 0.5, z)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    RenderMeshInstanced(mesh, shader, modelMatrix3, no_of_instances);

    glm::mat4 modelMatrix4 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x - 6.0f, y - 0.5, z - 0.5f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.75f));

    RenderMeshInstanced(mesh, shader, modelMatrix4, no_of_instances);

    glm::mat4 modelMatrix5 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x - 6.0f, y - 0.5, z + 7.5f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.75f));

    RenderMeshInstanced(mesh, shader, modelMatrix5, no_of_instances);

    glm::mat4 modelMatrix6 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x - 5.5, y - 0.5, z)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    RenderMeshInstanced(mesh, shader, modelMatrix6, no_of_instances);

    glm::mat4 modelMatrix7 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x, y + 6.0f, z)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    RenderMeshInstanced(mesh, shader, modelMatrix7, no_of_instances);


    glm::mat4 modelMatrix8 = tvModel * glm::translate(glm::mat4(1.0f), glm::vec3(x + 0.8f, y - 0.5, z)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0, 1, 0)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 3.0f));
    RenderMeshInstanced(mesh, shader, modelMatrix6, no_of_instances);
    surface_type = 6;
    max_rotate = glm::radians(180.0f);

    control_p0 = glm::vec3(-0.2, 0.0, -0.2);
    control_p1 = glm::vec3(-1.0, 1.5, -1.0);
    control_p2 = glm::vec3(-1.0, 4.5, -1.0);
    control_p3 = glm::vec3(-0.2, 6.0, -0.2);
    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix8));
    glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
    control_p0 = glm::vec3(-0.5f, 0.0f, 0.0f);  // baza jos stânga
    control_p1 = glm::vec3(-0.5f, 2.0f, 0.0f);
    control_p2 = glm::vec3(-0.5f, 4.0f, 0.0f);
    control_p3 = glm::vec3(-0.5f, 6.0f, 0.0f);  // vârful piciorului
    // TODO(student): Send to the shaders the number of points that approximate
    // a curve (no_of_generated_points), as well as the characteristics for
    // creating the translation/rotation surfaces (max_translate, max_rotate).
    // NOTE: If you're feeling lost and need a frame of reference while doing
    // this lab, go to `FrameEnd()` and activate `DrawCoordinateSystem()`.

    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);
    max_rotate = glm::radians(540.0f);

    Mesh* mesh5 = meshes["surface"];

    // Draw the object instanced
    //RenderMeshInstanced(mesh5, shader, modelMatrix8, no_of_instances);
}

void Tema1::DrawTable(float x, float y, float z)
{
    if (on_screen == 1)
        Shader* shader200 = shaders["SurfaceGeneration"];
    Shader* shader = shaders["SurfaceGeneration"];
    shader->Use();
    glUniform1f(glGetUniformLocation(shader->program, "time"), glfwGetTime());
    glUniform1i(glGetUniformLocation(shader->program, "myTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    TextureManager::GetTexture("wood.png")->BindToTextureUnit(GL_TEXTURE0);
    // Send uniforms to shaders
    surface_type = 2;
    glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.45f, 1.45f, 1.45f));


    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    if (render_to_cubemap == 1) {
        max_rotate = glm::radians(540.f);
    }
    //cout << no_of_generated_points << "\n";

    glUniform1f(glGetUniformLocation(shader->program, "x_direction"), x_direction);
    glUniform1f(glGetUniformLocation(shader->program, "y_direction"), y_direction);
    glUniform1f(glGetUniformLocation(shader->program, "z_direction"), z_direction);
    //cout << x_direction << y_direction << z_direction << "\n";
    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix2));
    glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);

    Mesh* mesh = meshes["surface"];

    // Draw the object instanced
    RenderMeshInstanced(mesh, shader, modelMatrix2, no_of_instances);

    surface_type = 2;
    glm::mat4 modelMatrix3 = glm::translate(glm::mat4(1.0f), glm::vec3(x, y + 0.4f, z)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.45f, 1.45f, 1.45f));



    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix2));
    glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);

    // TODO(student): Send to the shaders the number of points that approximate
    // a curve (no_of_generated_points), as well as the characteristics for
    // creating the translation/rotation surfaces (max_translate, max_rotate).
    // NOTE: If you're feeling lost and need a frame of reference while doing
    // this lab, go to `FrameEnd()` and activate `DrawCoordinateSystem()`.

    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);

    Mesh* mesh4 = meshes["surface"];

    // Draw the object instanced
    RenderMeshInstanced(mesh4, shader, modelMatrix3, no_of_instances);

    std::vector<glm::vec3> offsets = {
    glm::vec3(x - 8.0f, y - 6.5f, z + 0.5f),
    glm::vec3(x - 0.7f, y - 6.5f, z + 0.5f),
    glm::vec3(x - 8.0f, y - 6.5f, z + 11.0f),
    glm::vec3(x - 0.7f, y - 6.5f, z + 11.0f)
    };

    Mesh* mesh2 = meshes["surface"];


    float marginThickness = 0.8f; // grosimea laterala
    float marginHeight = 0.07f;    // grosimea blatului
    float tableLength = 1.0f;     // lungimea mesei
    float tableWidth = 1.45f;    // latimea mesei

    // Față și spate (lungimea mesei, grosimea laterala)
    glm::vec3 frontOffset = glm::vec3(x, y - marginHeight / 2 - 0.7f, z);
    glm::vec3 backOffset = glm::vec3(x, y + marginHeight / 2, z);

    glm::mat4 frontMargin =
        glm::translate(glm::mat4(1.0f), frontOffset + glm::vec3(-8.7f, 0.f, -0.5f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(tableLength, marginHeight, marginThickness + 0.2875));

    RenderMeshInstanced(mesh, shader, frontMargin, no_of_instances);

    glm::mat4 backMargin =
        glm::translate(glm::mat4(1.0f), frontOffset + glm::vec3(-8.7f, 0.f, 11.1f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(tableLength, marginHeight, marginThickness + 0.2875));
    RenderMeshInstanced(mesh, shader, backMargin, no_of_instances);

    // Stânga și dreapta (latimea mesei, grosimea laterala)
    glm::vec3 leftOffset = glm::vec3(x - tableLength / 2 - 7.8f, y - marginHeight / 2 - 0.7f, z);
    glm::vec3 rightOffset = glm::vec3(x + tableLength / 2 - 0.1f, y - marginHeight / 2 - 0.7f, z);

    glm::mat4 leftMargin = glm::translate(glm::mat4(1.0f), leftOffset) *
        glm::scale(glm::mat4(1.0f), glm::vec3(marginThickness, marginHeight, tableWidth));
    RenderMeshInstanced(mesh, shader, leftMargin, no_of_instances);

    glm::mat4 rightMargin = glm::translate(glm::mat4(1.0f), rightOffset) *
        glm::scale(glm::mat4(1.0f), glm::vec3(marginThickness, marginHeight, tableWidth));
    RenderMeshInstanced(mesh, shader, rightMargin, no_of_instances);


    for (auto& offset : offsets) {
        surface_type = 0;
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), offset);
        glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
        glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);

        // TODO(student): Send to the shaders the number of points that approximate
        // a curve (no_of_generated_points), as well as the characteristics for
        // creating the translation/rotation surfaces (max_translate, max_rotate).
        // NOTE: If you're feeling lost and need a frame of reference while doing
        // this lab, go to `FrameEnd()` and activate `DrawCoordinateSystem()`.

        glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
        glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
        max_rotate = glm::radians(540.0f);
        glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);
        RenderMeshInstanced(mesh2, shader, modelMatrix, no_of_instances);
    }
    max_rotate = glm::radians(720.0f);

    //pentru televizor
    surface_type = 3; // sau tipul pentru ecranul TV (plane surface)

    glm::mat4 tvModel =
        glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 6.0f, 0.0f)) *   // poziție pe masă
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));         // un TV subțire

    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(tvModel));

    glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);

    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);
    // Desenează televizorul
    RenderMeshInstanced(mesh, shader, tvModel, no_of_instances);
}
void Tema1::DrawLamp(float x, float y, float z)
{
    Shader* shader = shaders["SurfaceGeneration"];
    shader->Use();
    // Texture 1
    glActiveTexture(GL_TEXTURE0);
    TextureManager::GetTexture("wood.png")->BindToTextureUnit(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->program, "myTexture"), 0);
    glUniform1f(glGetUniformLocation(shader->program, "time"), glfwGetTime());

    /*if (render_to_cubemap == 1) {
        no_of_instances = 10;
        no_of_generated_points = 10;
    }*/
    surface_type = 5;
    max_rotate = glm::radians(1080.0f);
    glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(0.35f, 0.35f, 0.35f));
    control_p0 = glm::vec3(1.75, 0.0, 1.5);
    control_p1 = glm::vec3(1.0, 2.5, 1.0);
    control_p2 = glm::vec3(0.85, 4.0, 0.75);
    control_p3 = glm::vec3(0.45, 10.0, 0.45);
    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix2));
    glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    control_p0 = glm::vec3(-0.5f, 0.0f, 0.0f);  // baza jos stânga
    control_p1 = glm::vec3(-0.5f, 2.0f, 0.0f);
    control_p2 = glm::vec3(-0.5f, 4.0f, 0.0f);
    control_p3 = glm::vec3(-0.5f, 6.0f, 0.0f);  // vârful piciorului
    max_rotate = glm::radians(720.0f);
    //cout << max_rotate << "\n";
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);
    Mesh* mesh = meshes["surface"];

    // Draw the object instanced
    RenderMeshInstanced(mesh, shader, modelMatrix2, no_of_instances);

    surface_type = 5;
    glm::mat4 modelMatrix3 =
        glm::translate(glm::mat4(1.0f), glm::vec3(x - 1.6f, y + 2.6f, z))
        * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0))
        * glm::rotate(glm::mat4(1.0f), glm::radians(65.0f), glm::vec3(0, 0, 1))
        * glm::scale(glm::mat4(1.0f), glm::vec3(0.35f, 0.35f, 0.35f)
        );
    control_p0 = glm::vec3(2.5, 0.0, 2.5);
    control_p1 = glm::vec3(1.5, 1.5, 1.5);
    control_p2 = glm::vec3(1.0, 5.5, 1.0);
    control_p3 = glm::vec3(0.0, 6.0, 0.0);

    glActiveTexture(GL_TEXTURE0);
    TextureManager::GetTexture("textured_white.png")->BindToTextureUnit(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->program, "myTexture"), 0);
    glUniform1f(glGetUniformLocation(shader->program, "time"), glfwGetTime());
    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix3));
    glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
    control_p0 = glm::vec3(-0.5f, 0.0f, 0.0f);  // baza jos stânga
    control_p1 = glm::vec3(-0.5f, 2.0f, 0.0f);
    control_p2 = glm::vec3(-0.5f, 4.0f, 0.0f);
    control_p3 = glm::vec3(-0.5f, 6.0f, 0.0f);  // vârful piciorului
    max_rotate = glm::radians(720.0f);
    //cout << max_rotate << "\n";
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);
    //Mesh* mesh2 = meshes["surface"];

    // Draw the object instanced
    RenderMeshInstanced(mesh, shader, modelMatrix3, no_of_instances);
}


void Tema1::DrawVase(float x, float y, float z) {
    Shader* shader = shaders["SurfaceGeneration"];
    shader->Use();
    // Texture 1
    glActiveTexture(GL_TEXTURE0);
    TextureManager::GetTexture("vase.png")->BindToTextureUnit(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->program, "myTexture"), 0);

    /*glActiveTexture(GL_TEXTURE0 + 1);
    TextureManager::GetTexture("gray.png")->BindToTextureUnit(GL_TEXTURE0 + 1);
    glUniform1i(glGetUniformLocation(shader->program, "myTexture2"), 0);
    glActiveTexture(GL_TEXTURE0);*/

    // Time
    if (render_to_cubemap == 1) {
        max_rotate = glm::radians(20.f);
    }
    glUniform1f(glGetUniformLocation(shader->program, "time"), glfwGetTime());
    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    surface_type = 4;
    glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(0.6f, 0.6f, 0.6f));



    control_p0 = glm::vec3(-0.2, 0.0, 1.0);
    control_p1 = glm::vec3(-1.0, 1.5, 1.0);
    control_p2 = glm::vec3(-2.0, 4.0, 1.0);
    control_p3 = glm::vec3(-0.2, 5.5, 1.0);
    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix2));
    glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);

    control_p0 = glm::vec3(-0.5f, 0.0f, 0.0f);  // baza jos stânga
    control_p1 = glm::vec3(-0.5f, 2.0f, 0.0f);
    control_p2 = glm::vec3(-0.5f, 4.0f, 0.0f);
    control_p3 = glm::vec3(-0.5f, 6.0f, 0.0f);  // vârful piciorului

    // TODO(student): Send to the shaders the number of points that approximate
    // a curve (no_of_generated_points), as well as the characteristics for
    // creating the translation/rotation surfaces (max_translate, max_rotate).
    // NOTE: If you're feeling lost and need a frame of reference while doing
    // this lab, go to `FrameEnd()` and activate `DrawCoordinateSystem()`.
    max_rotate = glm::radians(720.0f);
    //cout << max_rotate << "\n";
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);
    Mesh* mesh = meshes["surface"];

    // Draw the object instanced
    RenderMeshInstanced(mesh, shader, modelMatrix2, no_of_instances);
}
void Tema1::DrawFireworks(float x, float y, float z, float deltaTimeSeconds)
{
    fireworkTimer += deltaTimeSeconds;
    glm::vec3 generator_position3;
    glm::vec3 generator_position4;
    float radius = 2.0f;
    generator_position3.x = generator_position2.x + radius * cos(-tvrotationangle);
    generator_position3.y = generator_position2.y;
    generator_position3.z = generator_position2.z + radius * sin(-tvrotationangle);

    generator_position4.x = generator_position2.x + radius * cos(tvrotationangle);
    generator_position4.y = generator_position2.y;
    generator_position4.z = generator_position2.z + radius * sin(tvrotationangle);
    float speed2 = 0.5f;
    currentPosition.y += deltaTimeSeconds * speed2;
    glm::vec3 activeColor = fireworkColors[current_color];
    glm::mat4 model2 = glm::translate(glm::mat4(1), generator_position3 + currentPosition);
    cubeposition = generator_position3 + currentPosition;
    //cout << "CubePosition = " << cubeposition << "\n";

    model2 = glm::scale(model2, glm::vec3(0.2f));




    // Desenează particulele
    auto shader15 = shaders["Fireworks"];
    if (shader15 && shader15->GetProgramID())
    {
        shader15->Use();
        glm::vec3 generator_position6 = generator_position2 + glm::vec3(5.0f, 1.1f, 0);
        //glm::vec3 newpos = generator_position3-glm::vec3(-1.f, 3.0f, 0.5f);
        TextureManager::GetTexture("particle2.png")->BindToTextureUnit(GL_TEXTURE0);
        glUniform3fv(glGetUniformLocation(shader15->program, "generator_position"), 1, glm::value_ptr(generator_position6));
        //glUniform3fv(glGetUniformLocation(shader15->program, "generator_position"), 1, glm::value_ptr(generator_position3));
        //glUniform3fv(glGetUniformLocation(shader15->program, "generator_position"), 1, glm::value_ptr(newpos));
        glUniform1f(glGetUniformLocation(shader15->program, "deltaTime"), deltaTimeSeconds);
        glUniform1f(glGetUniformLocation(shader15->program, "offset"), offset2);
        glUniform3fv(glGetUniformLocation(shader15->program, "color"), 1, glm::value_ptr(activeColor));
        //glUniform1i(glGetUniformLocation(shader->program, "is_cube"), 1);

        glm::mat4 viewMatrices[6];

        // Calculează cele 6 view matrices pentru fețele cubemap-ului

        glm::mat4 viewMatrices2[6] = {
            glm::lookAt(ScreenPos, ScreenPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +X
            glm::lookAt(ScreenPos, ScreenPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -X
            glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),  // +Y
            glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f,-1.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f)), // -Y
            glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +Z
            glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f, 0.0f,-1.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -Z
        };


        // Trimite array-ul de matrici
        glUniformMatrix4fv(shader15->GetUniformLocation("viewMatrices"), 6, GL_FALSE, glm::value_ptr(viewMatrices2[0]));


        //particleEffect2->Render(GetSceneCamera(), shader15);
    }
    // Reset fireworks
    if (fireworkTimer >= fireworkInterval)
    {
        fireworkTimer = 0.0f;
        current_color++;

        // 1. Resetezi pozitia INAINTE sa actualizezi cubeposition
        currentPosition = initialPosition;
        currentPosition2 = initialPosition;

        // 2. RECALCULEZI generator_position3 DUPĂ resetarea pozitie
        float radius = 2.0f;
        generator_position3.x = generator_position2.x + radius * cos(-tvrotationangle);
        generator_position3.y = generator_position2.y;
        generator_position3.z = generator_position2.z + radius * sin(-tvrotationangle);

        // 3. ACUM cubeposition este corect
        cubeposition = generator_position3 + currentPosition;

        // 4. Abia ACUM apelăm resetarea particulelor
        ResetParticlesFireworks2(20, 20, 20);

        if (current_color >= 10)
            current_color = 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fireworksFBO);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 0); // fundal transparent
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // folosește shader-ul de particule pentru artificii
    Shader* shader20 = shaders["Fireworks"];
    shader20->Use();
    // trimite poziția și culoarea particulelor
    particleEffect2->Render(GetSceneCamera(), shader20);

    Shader* shader21 = shaders["Framebuffer"];
    if (shader21 && shader21->GetProgramID()) {
        glm::vec3 generator_position5 = generator_position2 + glm::vec3(-1.0f, 0.0f, 0.65f);
        auto camera = GetSceneCamera();
        shader21->Use();
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);

        // Desenează cubemap-ul de fundal în framebuffer
        {
            glm::mat4 model2 = glm::translate(glm::mat4(1), generator_position5 + currentPosition2);
            //glm::mat4 model2 = glm::translate(glm::mat4(1), generator_position3);
            currentPosition2.x += 1.5 * deltaTimeSeconds;
            model2 = glm::scale(model2, glm::vec3(0.25f, 0.1f, 0.1f));
            glm::mat4 view = glm::mat4(1.0f);
            //glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(0, -100.5, -1.0f));

            glUniformMatrix4fv(shader21->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model2));
            glUniformMatrix4fv(shader21->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            //glUniformMatrix4fv(shader21->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(shader21->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));

            glm::mat4 cubeView[6] = {
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +X
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -X
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),  // +Y
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f,-1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)), // -Y
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +Z
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f, 0.0f,-1.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -Z
            };
            glUniformMatrix4fv(glGetUniformLocation(shader21->GetProgramID(), "viewMatrices"), 6, GL_FALSE, glm::value_ptr(cubeView[0]));
            glUniformMatrix4fv(shader21->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, TextureManager::GetTexture("akai_diffuse.png")->GetTextureID());
            glUniform1i(glGetUniformLocation(shader21->program, "texture_1"), 0);

            glUniform1i(glGetUniformLocation(shader21->program, "cube_draw"), 0);
            glUniform1i(glGetUniformLocation(shader21->program, "cube_firework"), 0);
            glUniform1i(glGetUniformLocation(shader21->program, "is_cube"), 1);
            glUniform3fv(glGetUniformLocation(shader21->program, "color2"), 1, glm::value_ptr(activeColor));

            glDisable(GL_DEPTH_TEST);
            meshes["box"]->Render();
            glEnable(GL_DEPTH_TEST);
        }
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0); // revine la framebuffer-ul principal

}
void Tema1::Update(float deltaTimeSeconds)
{
    //Aici e partea de lab1
    angle += 0.5f * deltaTimeSeconds;
    auto camera20 = GetSceneCamera();

    glm::vec3 camPosition = camera20->m_transform->GetWorldPosition();
    glm::quat camRotation = camera20->m_transform->GetWorldRotation();
    auto projectionInfo = camera20->GetProjectionInfo();

    // PASUL 1: Render the scene from the perspective of the spot light
    // source. It uses a perspective projection centered in the
    // position of the light source with the view direction in
    // the direction of illumination of the source.
    {
        camera20->SetPosition(light_position);
        camera20->SetRotation(glm::quatLookAt(light_direction, glm::vec3(0, 1, 0)));

        // The spot light source has an angle opening of 90
        // degrees, so a perspective projection is used with
        // a viewing angle of 90 degrees both vertically and
        // horizontally. For this reason, an aspect ratio of
        // 1 is used.
        camera20->SetPerspective(90 /* vert fov */, 1 /* aspect ratio */,
            light_space_near_plane, light_space_far_plane);

        // Save the view and projection matrix
        light_space_view = camera20->GetViewMatrix();
        light_space_projection = camera20->GetProjectionMatrix();

        // Bind framebuffer-ul custom și clear texturile
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setează viewport-ul la rezoluția texturilor din framebuffer (1024x1024)
        glViewport(0, 0, 1024, 1024);

        // Redarea scenei cu shader-ul pentru pasul 1
        Shader* shader21 = shaders["ShadowMappingPassOne"];
        shader21->Use();

        DrawScene(shaders["ShadowMappingPassOne"]);

    }

    // PASUL 2: Render the scene with shadows
    {
        camera20->SetPosition(camPosition);
        camera20->SetRotation(camRotation);
        camera20->SetProjection(projectionInfo);

        // Bind framebuffer-ul default (0 = ecranul)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setează viewport-ul la rezoluția ferestrei
        glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
        glActiveTexture(GL_TEXTURE0);
        Shader* shader21 = shaders["ShadowMappingPassTwo"];
        shader21->Use();
        DrawScene(shaders["ShadowMappingPassTwo"]);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);

        // Unbind toate texturile pentru a evita conflicte
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Revenire la GL_TEXTURE0
        glActiveTexture(GL_TEXTURE0);
    }

    // Render the point light in the scene
    {
        glm::mat4 modelMatrix10 = glm::mat4(1);
        modelMatrix10 = glm::translate(modelMatrix10, light_position);
        modelMatrix10 = glm::scale(modelMatrix10, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix10);
    }

    /*
    Aici e tema in continuare*/
    angle2 += 0.5f * deltaTimeSeconds;
    //ClearScreen(glm::vec3(0.12121, 0.168, 0.372));

    if (view_structure == true) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    auto camera = GetSceneCamera();

    // ============================================
    // PASUL 1: RENDERARE ÎN FRAMEBUFFER (pentru cubemap cu reflexii)
    // ============================================
    if (framebuffer_object)
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);
        glViewport(0, 0, 1024, 1024);
        //glViewport(0, 0, 1280, 720);
        //glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader* shader = shaders["Framebuffer"];
        if (shader && shader->GetProgramID()) {
            shader->Use();
            glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
            {
                glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(150));
                glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
                glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
                glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));

                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
                glUniform1i(glGetUniformLocation(shader->program, "texture_cubemap"), 1);
                glUniform1i(glGetUniformLocation(shader->program, "cube_draw"), 1);
                glUniform1i(glGetUniformLocation(shader->program, "cube_firework"), 0);
                //glUniform1i(glGetUniformLocation(shader->program, "randare_normala"), 1);
                //glDepthMask(GL_FALSE);
                //glDisable(GL_DEPTH_TEST);
                meshes["cube"]->Render();
                //glEnable(GL_DEPTH_TEST);
                //glDepthMask(GL_TRUE);
            }
            for (int i = 0; i < 5; i++)
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix *= glm::rotate(glm::mat4(1), angle2 + i * glm::radians(360.0f) / 5, glm::vec3(0, 1, 0));
                modelMatrix *= glm::translate(glm::mat4(1), glm::vec3(3, -1, 0));
                modelMatrix *= glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
                modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.01f));

                glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
                glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
                glUniform1i(glGetUniformLocation(shader->program, "is_cube"), 0);
                glUniform1i(glGetUniformLocation(shader->program, "cube_firework"), 0);

                glm::mat4 cubeView[6] = {
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +X
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -X
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),  // +Y
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f,-1.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f)), // -Y
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +Z
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f, 0.0f,-1.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -Z
                };

                glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "viewMatrices"), 6, GL_FALSE, glm::value_ptr(cubeView[0]));
                glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, TextureManager::GetTexture("akai_diffuse.png")->GetTextureID());
                glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

                glUniform1i(glGetUniformLocation(shader->program, "cube_draw"), 0);

                meshes["archer"]->Render();
            }

            // Setează view matrices pentru cubemap (6 fețe)
            glm::vec3 ScreenPos2 = glm::vec3(6.75f, 9.4f, 3.2f);
            glm::mat4 cubeView[6] = {
                glm::lookAt(ScreenPos2, ScreenPos2 + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +X
                glm::lookAt(ScreenPos2, ScreenPos2 + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -X
                glm::lookAt(ScreenPos2, ScreenPos2 + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),  // +Y
                glm::lookAt(ScreenPos2, ScreenPos2 + glm::vec3(0.0f,-1.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f)), // -Y
                glm::lookAt(ScreenPos2, ScreenPos2 + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +Z
                glm::lookAt(ScreenPos2, ScreenPos2 + glm::vec3(0.0f, 0.0f,-1.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -Z
            };

            glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "viewMatrices"), 6, GL_FALSE, glm::value_ptr(cubeView[0]));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(glGetUniformLocation(shader->program, "cube_draw"), 0);

            // DESENEAZĂ OBIECTELE ÎN FRAMEBUFFER (cu geometry shader)
            // NOTĂ: DrawTable, DrawTV, etc. folosesc deja shader-ul "SurfaceGeneration" cu geometry shader
            // Trebuie să adaptezi aceste funcții să folosească shader-ul "Framebuffer" când desenezi în framebuffer
            // SAU să creezi funcții separate, SAU să pasezi shader-ul ca parametru

            // Pentru moment, le desenăm direct (vor folosi propriul lor shader)
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);

            shader = shaders["SurfaceGeneration"];
            shader->Use();
            render_to_cubemap = 1;
            glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "viewMatrices"), 6, GL_FALSE, glm::value_ptr(cubeView[0]));
            glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
            no_of_generated_points = 15;
            no_of_instances = 15;
            //glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
            glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);  // ← ADAUGĂ
            glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);                // ← ADAUGĂ

            //GLint loc = glGetUniformLocation(shader->program, "render_to_cubemap");
            //glUniform1i(loc, 0);
            //GLint value;
            //glGetUniformiv(shader->program, loc, &value);
            //std::cout << "În FRAMEBUFFER - render_to_cubemap = " << value << std::endl;
            on_screen = true;
            DrawTable(7.5f, 6.5f, -0.5f);
            //glGetUniformiv(shader->program, loc, &value);
            //std::cout << "DUPĂ DrawTable în framebuffer: render_to_cubemap = " << value << std::endl;
            DrawTable(22.5f, 6.5f, -0.5f);
            DrawVase(3.75f, 6.2f, 0.1f);
            DrawVase(3.75f, 6.2f, 10.8f);
            DrawRoom(5.0f, 1.0f, -2.0f);
            //DrawTV(0.0f, 0.0f, 0.0f);
           // DrawTV(6.75f, 9.4f, 3.2f);
            //DrawScreen(ScreenPos.x, ScreenPos.y, ScreenPos.z);
            DrawLamp(17.5, 6.2f, 5.0f);
            render_to_cubemap = 0;
            no_of_generated_points = 25;
            no_of_instances = 25;
            DrawFireworks(0.0f, 0.0f, 0.0f, deltaTimeSeconds);
            on_screen = false;


            // Generează mipmap-uri pentru cubemap-ul creat
            glBindTexture(GL_TEXTURE_CUBE_MAP, color_texture);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

            // **RESTAUREAZĂ framebuffer-ul și viewport-ul**
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        }
    }

    // ============================================
    // PASUL 2: RENDERARE PE ECRAN (ce vezi tu)
    // ============================================
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);

    // Desenează cubemap-ul de fundal pe ecran (dacă ai shader pentru asta)
    // Acest pas este opțional, depinde dacă vrei să vezi cubemap-ul ca skybox
    {
        Shader* shader = shaders["ShaderNormal"]; // sau alt shader pentru cubemap
        if (shader && shader->GetProgramID()) {
            shader->Use();

            glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(30));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
            glUniform1i(glGetUniformLocation(shader->program, "texture_cubemap"), 0);
            glUniform1f(glGetUniformLocation(shader->program, "time"), glfwGetTime());

            //glDepthMask(GL_FALSE);
            glDisable(GL_DEPTH_TEST);
            if (meshes.find("cube") != meshes.end()) {
                meshes["cube"]->Render();
            }
            glEnable(GL_DEPTH_TEST);
            //glDepthMask(GL_TRUE);

        }
    }
    for (int i = 0; i < 5; i++)
    {
        Shader* shader = shaders["Simple"];
        shader->Use();

        //glm::mat4 modelMatrix = glm::mat4(1);
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1), glm::vec3(3.75f, 0.0f, 5.75f));
        modelMatrix *= glm::rotate(glm::mat4(1), angle2 + i * glm::radians(360.0f) / 5, glm::vec3(0, 1, 0));
        modelMatrix *= glm::translate(glm::mat4(1), glm::vec3(3, -1, 0));
        modelMatrix *= glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.01f));

        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
        glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureManager::GetTexture("akai_diffuse.png")->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

        meshes["archer"]->Render();
    }

    // Desenează obiectele pe ecran (cu geometry shader-ul lor normal)
    Shader* shader = shaders["SurfaceGeneration"];
    shader->Use();
    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);
    DrawTable(7.5f, 6.5f, -0.5f);
    DrawTable(22.5f, 6.5f, -0.5f);
    DrawVase(3.75f, 6.2f, 0.1f);
    DrawVase(3.75f, 6.2f, 10.8f);
    DrawTV(6.75f, 9.4f, 3.2f);
    //DrawTV(0.0f, 0.0f, 0.0f);
    DrawRoom(5.0f, 0.5f, -2.0f);
    DrawScreen(ScreenPos.x, ScreenPos.y, ScreenPos.z);
    DrawLamp(17.5, 6.2f, 5.0f);
    //DrawFireworks(0.0f,0.0f,0.0f, deltaTimeSeconds);
    // ============================================
    // PARTICULE ȘI ANIMAȚII
    // ============================================*/


    //aici adaug iepurele, care va fi de fapt ecranul
    {
        Shader* shader = shaders["Rabbit"];
        shader->Use();

        //glm::vec3 bunnyPos = glm::vec3(5.5f, 7.5f, 3.75f);
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, bunnyPos);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

        // Folosește view-ul centrat pe iepure pentru cubemap/reflexii
        glm::mat4 view = glm::lookAt(bunnyPos, bunnyPos + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
        surface_type = 1;


        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
        glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

        auto cameraPosition = camera->m_transform->GetWorldPosition();

        if (!color_texture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
            int loc_texture = shader->GetUniformLocation("texture_cubemap");
            glUniform1i(loc_texture, 0);
        }

        if (color_texture) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, color_texture);
            int loc_texture2 = shader->GetUniformLocation("texture_cubemap");
            glUniform1i(loc_texture2, 1);
        }


        int loc_camera = shader->GetUniformLocation("camera_position");
        glUniform3f(loc_camera, cameraPosition.x, cameraPosition.y, cameraPosition.z);

        //type = 0;
        glUniform1i(shader->GetUniformLocation("type"), type);
        glUniform1i(shader->GetUniformLocation("iepure"), 1);

        meshes["bunny"]->Render();
        //DrawScreen(ScreenPos.x, ScreenPos.y, ScreenPos.z);
    }
}


void Tema1::FrameEnd()
{
    DrawCoordinateSystem();

}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input

    // You can move the control points around by using the dedicated key,
    // in combination with Ctrl, Shift, or both.
    /*float delta = deltaTime;
    auto keyMaps = std::vector<std::pair<glm::vec3&, uint32_t>>
    {
        { control_p0, GLFW_KEY_1 },
        { control_p1, GLFW_KEY_2 },
        { control_p2, GLFW_KEY_3 },
        { control_p3, GLFW_KEY_4 }
    };

    for (const auto& k : keyMaps)
    {
        if (window->KeyHold(k.second))
        {
            if (mods & GLFW_MOD_SHIFT && mods & GLFW_MOD_CONTROL)
            {
                k.first.y -= delta;
            }
            else if (mods & GLFW_MOD_CONTROL)
            {
                k.first.y += delta;
            }
            else if (mods & GLFW_MOD_SHIFT)
            {
                k.first.x -= delta;
            }
            else
            {
                k.first.x += delta;
            }

            //std::cout << glm::vec2(control_p0) << glm::vec2(control_p1) << glm::vec2(control_p2) << glm::vec2(control_p3) << "\n";
        }
    }*/
    if (window->KeyHold(GLFW_KEY_1))
    {
        tvrotationangle += deltaTime; // rotește spre dreapta
    }
    if (window->KeyHold(GLFW_KEY_2))
    {
        tvrotationangle -= deltaTime; // rotește spre stânga
    }

    if (window->KeyHold(GLFW_KEY_Z))
    {
        x_direction += deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_X))
    {
        x_direction -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_C))
    {
        y_direction += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_V))
    {
        y_direction -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_B))
    {
        z_direction += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_N))
    {
        z_direction -= deltaTime;
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
    // TODO(student): Use keys to change the number of instances and the
    // number of generated points. Avoid the camera keys, and avoid the
    // the keys from `OnInputUpdate`.
    if (key == GLFW_KEY_Z)
    {
        no_of_instances++;
    }
    if (key == GLFW_KEY_X)
    {
        if (no_of_instances > 0) {
            no_of_instances--;
        }
    }
    if (key == GLFW_KEY_V)
    {
        no_of_generated_points++;
    }
    if (key == GLFW_KEY_B)
    {
        if (no_of_generated_points > 0) {
            no_of_generated_points--;
        }
    }
    if (key == GLFW_KEY_3) //vad structura de pornire
    {
        view_structure = true;
    }
    if (key == GLFW_KEY_4) //vad scena reala
    {
        view_structure = false;
    }
    if (key == GLFW_KEY_5) //vad scena reala
    {
        type = 0;
    }
    if (key == GLFW_KEY_6) //vad scena reala
    {
        type = 1;
    }



}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Tema1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
