#include "lab_m2/Tema1_bonus/Tema1_bonus.h"

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


ParticleEffect<Particle>* particleEffect3;
Tema1_bonus::Tema1_bonus()
{
}


Tema1_bonus::~Tema1_bonus()
{
}


void Tema1_bonus::Init()
{
    angle2 = 0;
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
    ToggleGroundPlane();
    cout << "Trece pe aici\n";
    bool vaseLoaded = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "vase.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "wood.png");
    if (!vaseLoaded) {
        std::cout << "ERROR: Failed to load vase.png!\n";
        std::cout << "Path: " << PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES) << std::endl;
    }


    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "gray.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "textured_gray.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "textured_white.png");


    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "white.png");


    bool ok = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "grey.jpg");
    if (!ok) {
        std::cout << "Eroare la incarcarea texturii!\n";
    }
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "particle2.png");


    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "characters", "archer", "Akai_E_Espiritu.fbm"), "akai_diffuse.png");


    // Create a shader program for surface generation
    {
        Shader* shader = new Shader("SurfaceGeneration");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "GeometryShader.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }


    {
        Shader* shader4 = new Shader("ShaderNormal");
        shader4->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Normal.VS.glsl"), GL_VERTEX_SHADER);
        shader4->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Normal.FS.glsl"), GL_FRAGMENT_SHADER);
        shader4->CreateAndLink();
        shaders[shader4->GetName()] = shader4;
    }

    {
        Shader* shader = new Shader("Shadow");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Shadow.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Shadow.GS.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Shadow.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }


    /*
    * TODO uncomment
    {
        Shader* shader2 = new Shader("Fireworks");
        shader2->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Particle_fireworks.VS.glsl"), GL_VERTEX_SHADER);
        shader2->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Particle.GS.glsl"), GL_GEOMETRY_SHADER);
        shader2->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Particle_simple.FS.glsl"), GL_FRAGMENT_SHADER);
        shader2->CreateAndLink();
        shaders[shader2->GetName()] = shader2;
    }

    {
        Shader* shader3 = new Shader("Framebuffer");
        shader3->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Framebuffer.VS.glsl"), GL_VERTEX_SHADER);
        shader3->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Framebuffer.GS.glsl"), GL_GEOMETRY_SHADER);
        shader3->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Framebuffer.FS.glsl"), GL_FRAGMENT_SHADER);
        shader3->CreateAndLink();
        shaders[shader3->GetName()] = shader3;
    }


    {
        Shader* shader = new Shader("Screen");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Screen.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Screen.GS.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_bonus", "shaders", "Screen.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }*/



    ResetParticlesFireworks2(20, 20, 20);
    // Parameters related to surface generation
    no_of_generated_points = 25;            // number of points on a Bezier curve
    no_of_instances = 25;                    // number of instances (number of curves that contain the surface)
    max_translate = 12.0f;                   // for the translation surface, it's the distance between the first and the last curve
    max_rotate = glm::radians(720.0f);      // for the rotation surface, it's the angle between the first and the last curve

    // Define control points
    control_p0 = glm::vec3(-0.5f, 0.0f, 0.0f);  // baza jos stânga
    control_p1 = glm::vec3(-0.5f, 2.0f, 0.0f);
    control_p2 = glm::vec3(-0.5f, 4.0f, 0.0f);
    control_p3 = glm::vec3(-0.5f, 6.0f, 0.0f);  // vârful piciorului


    uvScale = glm::vec2(1.0f);

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
    generator_position2 = glm::vec3(0, 0, 0);
    offset2 = 0.05;


    CreateShadowFramebuffer();  // Add this line

    // Initialize light position (above the lamp)
    lightPosition = glm::vec3(17.5f, 9.5f, 4.0f);  // At the lamp shade opening
    lightDirection = glm::normalize(glm::vec3(0.1f, -1.0f, -0.2f));  // Pointing down and slightly forward
    lightIntensity = 1.5f;  // Increase intensity
    spotAngle = 40.0f;  // Wider cone
    cout << "Linia 231\n";


    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "characters", "archer", "Akai_E_Espiritu.fbm"), "akai_diffuse.png");

    cubeMapTextureID = UploadCubeMapTexture(
        PATH_JOIN(texturePath, "pos_x.png"),
        PATH_JOIN(texturePath, "pos_y.png"),
        PATH_JOIN(texturePath, "pos_z.png"),
        PATH_JOIN(texturePath, "neg_x.png"),
        PATH_JOIN(texturePath, "neg_y.png"),
        PATH_JOIN(texturePath, "neg_z.png"));



    //CreateFramebuffer(1024, 1024);
}




unsigned int Tema1_bonus::UploadCubeMapTexture(const std::string& pos_x, const std::string& pos_y, const std::string& pos_z, const std::string& neg_x, const std::string& neg_y, const std::string& neg_z)
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

void Tema1_bonus::CreateFramebuffer(int width, int height)
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

void Tema1_bonus::CreateShadowFramebuffer()
{
    // Generate framebuffer
    glGenFramebuffers(1, &shadowFBO);

    // Generate depth texture
    glGenTextures(1, &shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
        shadowMapResolution, shadowMapResolution, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Shadow map texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Areas outside shadow map should be lit (white = no shadow)
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);

    // No color buffer needed for shadow map
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR: Shadow framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::cout << "Shadow framebuffer created: " << shadowMapResolution << "x" << shadowMapResolution << std::endl;
}


void Tema1_bonus::RenderShadowMap()
{
    // Bind shadow framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glViewport(0, 0, shadowMapResolution, shadowMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Use front-face culling to reduce shadow acne
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    Shader* shader = shaders["Shadow"];
    shader->Use();

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "LightSpaceMatrix"),
        1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    RenderSceneForShadow(shader);

    // RESTORE everything
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Tema1_bonus::RenderSceneForShadow(Shader* shader)
{
    Mesh* mesh = meshes["surface"];

    // Helper lambda to set uniforms and render
    auto renderObject = [&](const glm::mat4& modelMatrix, int surfType,
        const glm::vec3& p0, const glm::vec3& p1,
        const glm::vec3& p2, const glm::vec3& p3,
        float maxRot) {
            glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"),
                1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surfType);
            glUniform3fv(glGetUniformLocation(shader->program, "control_p0"), 1, glm::value_ptr(p0));
            glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 1, glm::value_ptr(p1));
            glUniform3fv(glGetUniformLocation(shader->program, "control_p2"), 1, glm::value_ptr(p2));
            glUniform3fv(glGetUniformLocation(shader->program, "control_p3"), 1, glm::value_ptr(p3));
            glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
            glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
            glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
            glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), maxRot);

            glBindVertexArray(mesh->GetBuffers()->m_VAO);
            glDrawElementsInstanced(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()),
                GL_UNSIGNED_INT, (void*)0, no_of_instances);
        };

    // Render tables
    // Table 1
    float tableLength = 25.0f;
    float tableWidth = 13.0f;
    float marginThickness = 1.0f;

    // Table at (-1, 5, -20)
    glm::vec3 tablePos1 = glm::vec3(-1.f, 5.f, -20.f);

    // Table top planes
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), tablePos1);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(tableLength, tableWidth, 1.0f));
    renderObject(modelMatrix, 2,
        glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(-0.5f, 2.0f, 0.0f),
        glm::vec3(-0.5f, 4.0f, 0.0f), glm::vec3(-0.5f, 6.0f, 0.0f),
        glm::radians(540.0f));

    // Table legs
    glm::vec3 legP0 = glm::vec3(0.4f, 0.0f, 0.0f);
    glm::vec3 legP1 = glm::vec3(0.25f, 2.0f, 0.0f);
    glm::vec3 legP2 = glm::vec3(0.25f, 4.0f, 0.0f);
    glm::vec3 legP3 = glm::vec3(0.4f, 6.0f, 0.0f);

    float legHeight = 5.0f;
    float legWidth = 2.0f;
    float cornerSize = 2.0f;

    std::vector<glm::vec3> legOffsets = {
        tablePos1 + glm::vec3(cornerSize, -legHeight, cornerSize),
        tablePos1 + glm::vec3(tableLength - cornerSize, -legHeight, cornerSize),
        tablePos1 + glm::vec3(tableLength - cornerSize, -legHeight, tableWidth - cornerSize),
        tablePos1 + glm::vec3(cornerSize, -legHeight, tableWidth - cornerSize)
    };

    for (auto& offset : legOffsets) {
        modelMatrix = glm::translate(glm::mat4(1.0f), offset);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(legWidth, legHeight, legWidth));
        renderObject(modelMatrix, 0, legP0, legP1, legP2, legP3, glm::radians(540.0f));
    }

    // Add more objects (table 2, vases, lamp base, etc.)
    // Similar pattern for each object...

    // Table 2 at (2, 5, -1)
    glm::vec3 tablePos2 = glm::vec3(2.f, 5.f, -1.f);
    modelMatrix = glm::translate(glm::mat4(1.0f), tablePos2);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(tableLength, tableWidth, 1.0f));
    renderObject(modelMatrix, 2,
        glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(-0.5f, 2.0f, 0.0f),
        glm::vec3(-0.5f, 4.0f, 0.0f), glm::vec3(-0.5f, 6.0f, 0.0f),
        glm::radians(540.0f));

    // Vases
    glm::vec3 vaseP0 = glm::vec3(0.4f, 0.0f, 0.0f);
    glm::vec3 vaseP1 = glm::vec3(0.55f, 1.0f, 0.0f);
    glm::vec3 vaseP2 = glm::vec3(1.5f, 4.5f, 0.0f);
    glm::vec3 vaseP3 = glm::vec3(0.5f, 6.0f, 0.0f);
    float vaseHeight = 3.5f;

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(4.35f, 6.f, -12.1f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, vaseHeight, 1.0f));
    renderObject(modelMatrix, 0, vaseP0, vaseP1, vaseP2, vaseP3, glm::radians(360.0f));

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(18.95f, 6.f, -14.8f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, vaseHeight, 1.0f));
    renderObject(modelMatrix, 0, vaseP0, vaseP1, vaseP2, vaseP3, glm::radians(360.0f));

    // Lamp base
    glm::vec3 lampP0 = glm::vec3(1.2f, 0.0f, 0.0f);
    glm::vec3 lampP1 = glm::vec3(1.4f, 1.0f, 0.0f);
    glm::vec3 lampP2 = glm::vec3(0.5f, 2.5f, 0.0f);
    glm::vec3 lampP3 = glm::vec3(0.5f, 4.0f, 0.0f);
    float lampHeight = 3.3f;
    float baseWidth = 0.6f;

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(17.5f, 6.f, 5.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(baseWidth, lampHeight, baseWidth));
    renderObject(modelMatrix, 0, lampP0, lampP1, lampP2, lampP3, glm::radians(360.0f));
}


void Tema1_bonus::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1_bonus::ResetParticlesFireworks2(int xSize, int ySize, int zSize)
{
    unsigned int nrParticles = 1000;

    particleEffect3 = new ParticleEffect<Particle>();
    particleEffect3->Generate(nrParticles, true);

    auto particleSSBO = particleEffect3->GetParticleBuffer();
    Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

    glm::vec3 generatorPos = glm::vec3(0.0f, 10.0f, 0.0f); // punctul de spawn

    for (unsigned int i = 0; i < nrParticles; i++)
    {
        glm::vec4 pos(generatorPos, 1.0f);

        // direcție aleatorie
        float theta = ((float)(rand() % 360)) * TO_RADIANS; // unghi orizontal
        float phi = ((float)(rand() % 180)) * TO_RADIANS;   // unghi vertical
        //float speedMag = ((rand() % 10) / 10.0f) + 1.5f;    // viteza totală
        float speedMag = 1.0f;
        glm::vec4 speed;
        speed.x = speedMag * sin(phi) * cos(theta);
        speed.y = speedMag * cos(phi);
        speed.z = speedMag * sin(phi) * sin(theta);
        speed.w = 0.0f;

        data[i].SetInitial(pos, speed, 0.0f, 2.0f); // lifetime 3 secunde
    }

    particleSSBO->SetBufferData(data);
}




void Tema1_bonus::RenderMeshInstanced(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int instances, const glm::vec3& color)
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

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "LightSpaceMatrix"),
        1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));


    glUniform3fv(glGetUniformLocation(shader->program, "light_position"), 1, glm::value_ptr(lightPosition));
    glUniform3fv(glGetUniformLocation(shader->program, "light_direction"), 1, glm::value_ptr(lightDirection));
    glUniform1f(glGetUniformLocation(shader->program, "light_intensity"), lightIntensity);
    glUniform1f(glGetUniformLocation(shader->program, "spot_angle"), spotAngle);

    auto cameraPos = GetSceneCamera()->m_transform->GetWorldPosition();
    glUniform3fv(glGetUniformLocation(shader->program, "camera_position"), 1, glm::value_ptr(cameraPos));

    // Bind shadow map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glUniform1i(glGetUniformLocation(shader->program, "shadowMap"), 1);



    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);
    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);

    glUniform1f(glGetUniformLocation(shader->program, "time"), glfwGetTime());
    glUniform1i(glGetUniformLocation(shader->program, "myTexture"), 0);


    glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);

    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);


    glUniform2f(glGetUniformLocation(shader->program, "uvScale"), uvScale.x, uvScale.y);


    // Draw the object instanced
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElementsInstanced(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, (void*)0, instances);
}

void Tema1_bonus::DrawScreen(float x, float y, float z)
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

    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
    glUniform1i(glGetUniformLocation(shader->program, "direction"), 1);
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
    glUniform1i(shader->GetUniformLocation("type"), type);
    //glUniform1i(shader->GetUniformLocation("direction"), 0);

    auto cameraPosition = camera->m_transform->GetWorldPosition();
    int loc_camera = shader->GetUniformLocation("camera_position");
    glUniform3f(loc_camera, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    Mesh* mesh = meshes["surface"];
    RenderMeshInstanced(mesh, shader, modelMatrix3, no_of_instances);
}

void Tema1_bonus::DrawTV(float x, float y, float z)
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


void Tema1_bonus::DrawTable(float x, float y, float z)
{
    Shader* shader = shaders["SurfaceGeneration"];
    shader->Use();

    Mesh* mesh = meshes["surface"];

    glm::mat4 modelMatrix;

    glActiveTexture(GL_TEXTURE0);
    TextureManager::GetTexture("wood.png")->BindToTextureUnit(GL_TEXTURE0);


    // TODO change
    if (render_to_cubemap == 1) {
        max_rotate = glm::radians(540.f);
    }

    float marginThickness = 1.f; // grosimea laterala
    //float marginHeight = 0.07f;    // grosimea blatului
    float tableLength = 25.0f;     // lungimea mesei
    float tableWidth = 13.0f;    // latimea mesei
    float legHeigth = 5.0f;
    float legWidth = 2.0f;
    float cornerSize = 2.0f;

    control_p0 = glm::vec3(-0.5f, 0.0f, 0.0f);
    control_p1 = glm::vec3(-0.5f, 2.0f, 0.0f);
    control_p2 = glm::vec3(-0.5f, 4.0f, 0.0f);
    control_p3 = glm::vec3(-0.5f, 6.0f, 0.0f);

    // draw the 2 planes of the table
    surface_type = 2;
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(tableLength, tableWidth, 1.0f));
    RenderMeshInstanced(mesh, shader, modelMatrix, no_of_instances);


    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y + marginThickness, z));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(tableLength, tableWidth, 1.0f));
    RenderMeshInstanced(mesh, shader, modelMatrix, no_of_instances);

    surface_type = 1;

    control_p0 = glm::vec3(0.0f, 0.0f, 0.0f);      // Bottom
    control_p1 = glm::vec3(0.0f, 0.1f, 0.2f);      // Curves outward
    control_p2 = glm::vec3(0.0f, 0.3f, 0.1f);
    control_p3 = glm::vec3(0.0f, 0.5f, 0.0f);



    // draw the edges of the table
    glm::vec3 pos = glm::vec3(x, y, z);

    uvScale = glm::vec2(1.0f, 5.0f);

    modelMatrix = glm::translate(glm::mat4(1.0f), pos);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(tableLength, marginThickness, 1.0f));
    RenderMeshInstanced(mesh, shader, modelMatrix, no_of_instances);

    modelMatrix = glm::translate(glm::mat4(1.0f), pos + glm::vec3(tableLength, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(tableWidth, marginThickness, 1.0f));
    RenderMeshInstanced(mesh, shader, modelMatrix, no_of_instances);


    modelMatrix = glm::translate(glm::mat4(1.0f), pos + glm::vec3(tableLength, 0.0f, tableWidth));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-180.0f), glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(tableLength, marginThickness, 1.0f));
    RenderMeshInstanced(mesh, shader, modelMatrix, no_of_instances);


    modelMatrix = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0.0f, 0.0f, tableWidth));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-270.0f), glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(tableWidth, marginThickness, 1.0f));
    RenderMeshInstanced(mesh, shader, modelMatrix, no_of_instances);


    uvScale = glm::vec2(1.0f);

    // draw the legs of the table
    std::vector<glm::vec3> offsets = {
        glm::vec3(x + cornerSize, y - legHeigth, z + cornerSize),
        glm::vec3(x + tableLength - cornerSize, y - legHeigth, z + cornerSize),
        glm::vec3(x + tableLength - cornerSize, y - legHeigth, z + tableWidth - cornerSize),
        glm::vec3(x + cornerSize, y - legHeigth, z + tableWidth - cornerSize)
    };

    // control points for the leg of the table
    control_p0 = glm::vec3(0.4f, 0.0f, 0.0f);
    control_p1 = glm::vec3(0.25f, 2.0f, 0.0f);
    control_p2 = glm::vec3(0.25f, 4.0f, 0.0f);
    control_p3 = glm::vec3(0.4f, 6.0f, 0.0f);

    max_rotate = glm::radians(540.0f);


    surface_type = 0;
    for (auto& offset : offsets) {

        modelMatrix = glm::translate(glm::mat4(1.0f), offset);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(legWidth, legHeigth, legWidth));

        RenderMeshInstanced(mesh, shader, modelMatrix, no_of_instances);
    }

}



void Tema1_bonus::DrawLamp(float x, float y, float z)
{
    Shader* shader = shaders["SurfaceGeneration"];
    shader->Use();

    Mesh* mesh = meshes["surface"];

    float lampHeight = 3.3f;
    float baseWidth = 0.6f;

    glActiveTexture(GL_TEXTURE0);
    TextureManager::GetTexture("grey.jpg")->BindToTextureUnit(GL_TEXTURE0);


    /*if (render_to_cubemap == 1) {
        no_of_instances = 10;
        no_of_generated_points = 10;
    }*/


    /*
    // base of the lamp
    surface_type = 2;  // PLANE
    control_p0 = glm::vec3(0.0f, 0.0f, 0.0f);
    control_p1 = glm::vec3(0.0f, 0.0f, 0.0f);
    control_p2 = glm::vec3(0.0f, 0.0f, 0.0f);
    control_p3 = glm::vec3(1.0f, 0.0f, 0.0f);

    glm::mat4 baseMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(baseWidth, 1.f, baseWidth));

    RenderMeshInstanced(mesh, shader, baseMatrix, no_of_instances);*/


    // curved shape of the bottom of the lamp
    max_rotate = glm::radians(360.0f);
    surface_type = 0;
    glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(baseWidth, lampHeight, baseWidth));




    control_p0 = glm::vec3(1.2f, 0.0f, 0.0f);   // Wide bottom base
    control_p1 = glm::vec3(1.4f, 1.0f, 0.0f);   // Bulges out even more
    control_p2 = glm::vec3(0.5f, 2.5f, 0.0f);   // Narrows to cylinder
    control_p3 = glm::vec3(0.5f, 4.0f, 0.0f);





    // Draw the object instanced
    RenderMeshInstanced(mesh, shader, modelMatrix2, no_of_instances);



    surface_type = 5;


    glm::mat4 modelMatrix3 =
        glm::translate(glm::mat4(1.0f), glm::vec3(x, y + lampHeight, z - 1.1f))
        * glm::rotate(glm::mat4(1.0f), glm::radians(100.0f), glm::vec3(0, 1, 0))
        * glm::rotate(glm::mat4(1.0f), glm::radians(65.0f), glm::vec3(0, 0, 1))
        * glm::scale(glm::mat4(1.0f), glm::vec3(baseWidth * 4.0f, baseWidth * 2.0f, baseWidth * 4.0f)
        );
    control_p0 = glm::vec3(2.5, 0.0, 0.0);  // Bottom radius = 2.5, Z = 0
    control_p1 = glm::vec3(1.5, 1.5, 0.0);  // Z = 0
    control_p2 = glm::vec3(1.0, 5.5, 0.0);  // Z = 0
    control_p3 = glm::vec3(0.0, 6.0, 0.0);

    glActiveTexture(GL_TEXTURE0);
    TextureManager::GetTexture("white.png")->BindToTextureUnit(GL_TEXTURE0);



    RenderMeshInstanced(mesh, shader, modelMatrix3, no_of_instances);
}


void Tema1_bonus::DrawVase(float x, float y, float z) {
    Shader* shader = shaders["SurfaceGeneration"];
    shader->Use();

    glActiveTexture(GL_TEXTURE0);
    TextureManager::GetTexture("vase.png")->BindToTextureUnit(GL_TEXTURE0);

    // SAVE original control points
    glm::vec3 original_p0 = control_p0;
    glm::vec3 original_p1 = control_p1;
    glm::vec3 original_p2 = control_p2;
    glm::vec3 original_p3 = control_p3;
    float original_max_rotate = max_rotate;

    surface_type = 0;
    uvScale = glm::vec2(5.0f, 6.0f);

    control_p0 = glm::vec3(0.4f, 0.0f, 0.0f);
    control_p1 = glm::vec3(0.55f, 1.0f, 0.0f);
    control_p2 = glm::vec3(1.5f, 4.5f, 0.0f);
    control_p3 = glm::vec3(0.5f, 6.0f, 0.0f);

    max_rotate = glm::radians(360.0f);

    float vaseHeight = 3.5f;
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, vaseHeight, 1.0f));

    Mesh* mesh = meshes["surface"];
    RenderMeshInstanced(mesh, shader, modelMatrix, no_of_instances);

    uvScale = glm::vec2(1.0f);
}


void Tema1_bonus::DrawSkybox()
{
    auto camera = GetSceneCamera();
    Shader* shader = shaders["ShaderNormal"];

    if (!shader || !shader->GetProgramID()) return;

    shader->Use();

    // CRITICAL: These settings for skybox
    glDepthMask(GL_FALSE);      // Don't write to depth
    glDepthFunc(GL_LEQUAL);     // Pass if equal (skybox at z=1)
    glDisable(GL_CULL_FACE);    // Draw all faces of cube

    glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE,
        glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "View"), 1, GL_FALSE,
        glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Projection"), 1, GL_FALSE,
        glm::value_ptr(camera->GetProjectionMatrix()));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
    glUniform1i(glGetUniformLocation(shader->program, "texture_cubemap"), 0);

    meshes["cube"]->Render();

    // RESTORE defaults
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void Tema1_bonus::Update(float deltaTimeSeconds)
{
    angle2 += 0.5f * deltaTimeSeconds;
    //ClearScreen(glm::vec3(0.12121, 0.168, 0.372));

    if (view_structure == true) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    auto camera = GetSceneCamera();

    glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 50.0f);
    glm::mat4 lightView = glm::lookAt(lightPosition,
        lightPosition + lightDirection * 10.0f,
        glm::vec3(0.0f, 0.0f, 1.0f));
    lightSpaceMatrix = lightProjection * lightView;

    // Render shadow map
    RenderShadowMap();

    // IMPORTANT: Reset state after shadow pass
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
    glCullFace(GL_BACK);

    DrawSkybox();


    // PASUL 1: RENDERARE ÎN FRAMEBUFFER (pentru cubemap cu reflexii)

    // TODO continue 
    if (false && framebuffer_object)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);
        glViewport(0, 0, 1024, 1024);
        //glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader* shader = shaders["Framebuffer"];
        if (shader && shader->GetProgramID()) {
            shader->Use();
            glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);

            // Desenează cubemap-ul de fundal în framebuffer
            {
                glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(180));
                glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
                glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
                glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
                glUniform1i(glGetUniformLocation(shader->program, "texture_cubemap"), 1);
                glUniform1i(glGetUniformLocation(shader->program, "cube_draw"), 1);
                //glUniform1i(glGetUniformLocation(shader->program, "randare_normala"), 1);
                /*if (meshes.find("cube") != meshes.end()) {
                    meshes["cube"]->Render();
                }*/
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

                glm::mat4 cubeView[6] = {
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +X
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -X
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),  // +Y
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f,-1.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f)), // -Y
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +Z
                glm::lookAt(ScreenPos, ScreenPos + glm::vec3(0.0f, 0.0f,-1.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -Z
                };

                /*glm::mat4 cubeView[6] = {
                glm::lookAt(glm::vec3(0,0,0),glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +X
                glm::lookAt(glm::vec3(0,0,0), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -X
                glm::lookAt(glm::vec3(0,0,0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),  // +Y
                glm::lookAt(glm::vec3(0,0,0), glm::vec3(0.0f,-1.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f)), // -Y
                glm::lookAt(glm::vec3(0,0,0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +Z
                glm::lookAt(glm::vec3(0,0,0), glm::vec3(0.0f, 0.0f,-1.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -Z
                };*/

                glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "viewMatrices"), 6, GL_FALSE, glm::value_ptr(cubeView[0]));
                glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, TextureManager::GetTexture("akai_diffuse.png")->GetTextureID());
                glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

                glUniform1i(glGetUniformLocation(shader->program, "cube_draw"), 0);

                meshes["archer"]->Render();
            }

            // Setează view matrices pentru cubemap (6 fețe)
            /*glm::mat4 cubeView[6] = {
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +X
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -X
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),  // +Y
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f)), // -Y
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f)),  // +Z
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f), glm::vec3(0.0f,-1.0f, 0.0f)), // -Z
            };*/

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
            glUniform1i(glGetUniformLocation(shader->program, "cube_draw"), 0);

            // DESENEAZĂ OBIECTELE ÎN FRAMEBUFFER (cu geometry shader)
            // NOTĂ: DrawTable, DrawTV, etc. folosesc deja shader-ul "SurfaceGeneration" cu geometry shader
            // Trebuie să adaptezi aceste funcții să folosească shader-ul "Framebuffer" când desenezi în framebuffer
            // SAU să creezi funcții separate, SAU să pasezi shader-ul ca parametru

            // Pentru moment, le desenăm direct (vor folosi propriul lor shader)
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);

            /*DrawTable(7.5f, 6.5f, -0.5f);
            DrawTable(22.5f, 6.5f, -0.5f);
            DrawVase(3.75f, 6.2f, 0.1f);
            DrawVase(3.75f, 6.2f, 10.8f);
            DrawTV(6.75f, 9.4f, 3.2f);
            DrawScreen(ScreenPos.x, ScreenPos.y, ScreenPos.z);
            DrawLamp(17.5, 6.2f, 5.0f);*/

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
            DrawTable(7.5f, 6.5f, -0.5f);
            //glGetUniformiv(shader->program, loc, &value);
            //std::cout << "DUPĂ DrawTable în framebuffer: render_to_cubemap = " << value << std::endl;
            DrawTable(22.5f, 6.5f, -0.5f);
            DrawVase(3.75f, 6.2f, 0.1f);
            DrawVase(3.75f, 6.2f, 10.8f);
            DrawTV(6.75f, 9.4f, 3.2f);
             //DrawScreen(ScreenPos.x, ScreenPos.y, ScreenPos.z);
            DrawLamp(17.5, 6.2f, 5.0f);
            render_to_cubemap = 0;
            no_of_generated_points = 25;
            no_of_instances = 25;



            // Generează mipmap-uri pentru cubemap-ul creat
            glBindTexture(GL_TEXTURE_CUBE_MAP, color_texture);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

            // Resetează la framebuffer-ul ecranului
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);

        }
    }


    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);

    // Desenează cubemap-ul de fundal pe ecran (dacă ai shader pentru asta)
    // Acest pas este opțional, depinde dacă vrei să vezi cubemap-ul ca skybox
    {
        Shader* shader = shaders["ShaderNormal"]; // sau alt shader pentru cubemap
        // TODO continue
        if (false && shader && shader->GetProgramID()) {
            shader->Use();

            glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(30));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
            glUniform1i(glGetUniformLocation(shader->program, "texture_cubemap"), 0);

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
        // TODO continue
        continue;

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




    Shader* shader = shaders["SurfaceGeneration"];
    shader->Use();

    glUniform1i(glGetUniformLocation(shader->program, "render_to_cubemap"), render_to_cubemap);


    glDisable(GL_CULL_FACE);

    DrawVase(15.5f, 6.f,  5.0f);
    DrawVase(18.95f, 6.f, -14.8f);

    DrawTable(-1.f, 5.f, -20.f);


    DrawTable(2.f, 5.f, -1.f);

    DrawLamp(17.5, 6.f, 5.0f);


    glEnable(GL_CULL_FACE);




    /* TODO uncomment
    DrawTV(6.75f, 9.4f, 3.2f);
    DrawScreen(ScreenPos.x, ScreenPos.y, ScreenPos.z);
    */

    /* TODO continute

    // ============================================
    // PARTICULE ȘI ANIMAȚII
    // ============================================
    fireworkTimer += deltaTimeSeconds;
    float speed2 = 2.0f;
    currentPosition.y += deltaTimeSeconds * speed2;
    glm::vec3 activeColor = fireworkColors[current_color];
    glm::mat4 model2 = glm::translate(glm::mat4(1), generator_position2 + currentPosition);
    model2 = glm::scale(model2, glm::vec3(0.2f));

    Shader* colorShader = shaders["Color"];
    if (colorShader && colorShader->GetProgramID()) {
        colorShader->Use();

        glm::vec3 activeColor2 = fireworkColors[(current_color + 11) % 10];
        glUniform3fv(glGetUniformLocation(colorShader->program, "color"), 1, glm::value_ptr(activeColor2));

        if (fireworkTimer >= 3.0f && meshes.find("box") != meshes.end()) {
            RenderMesh(meshes["box"], colorShader, model2);
        }
    }

    // Desenează particulele
    auto shader15 = shaders["Fireworks"];
    if (shader15 && shader15->GetProgramID())
    {
        shader15->Use();

        TextureManager::GetTexture("particle2.png")->BindToTextureUnit(GL_TEXTURE0);
        glUniform3fv(glGetUniformLocation(shader15->program, "generator_position"), 1, glm::value_ptr(generator_position2));
        glUniform1f(glGetUniformLocation(shader15->program, "deltaTime"), deltaTimeSeconds);
        glUniform1f(glGetUniformLocation(shader15->program, "offset"), offset2);
        glUniform3fv(glGetUniformLocation(shader15->program, "color"), 1, glm::value_ptr(activeColor));

        particleEffect3->Render(GetSceneCamera(), shader15);
    }

    // Reset fireworks
    if (fireworkTimer >= fireworkInterval)
    {
        fireworkTimer = 0.0f;
        current_color++;
        ResetParticlesFireworks2(20, 20, 20);
        currentPosition = initialPosition;
        if (current_color >= 10)
        {
            current_color = 0;
        }
    }*/


    if (false)
    {

        //aici adaug iepurele, care va fi de fapt ecranul
        Shader* shader = shaders["Rabbit"];
        shader->Use();

        //glm::vec3 bunnyPos = glm::vec3(5.5f, 7.5f, 3.75f);
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, bunnyPos);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

        // Folosește view-ul centrat pe iepure pentru cubemap/reflexii
        glm::mat4 view = glm::lookAt(bunnyPos, bunnyPos + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
        surface_type = 1;
        /*glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), surface_type);
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
        glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
        glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), no_of_instances);*/

        // TODO(student): Send to the shaders the number of points that approximate
        // a curve (no_of_generated_points), as well as the characteristics for
        // creating the translation/rotation surfaces (max_translate, max_rotate).
        // NOTE: If you're feeling lost and need a frame of reference while doing
        // this lab, go to `FrameEnd()` and activate `DrawCoordinateSystem()`.

       /* glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
        glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
        glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);*/

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


void Tema1_bonus::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */
void Tema1_bonus::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input

    /* TODO uncomment
    if (window->KeyHold(GLFW_KEY_1))
    {
        tvrotationangle += deltaTime; // rotește spre dreapta
    }
    if (window->KeyHold(GLFW_KEY_2))
    {
        tvrotationangle -= deltaTime; // rotește spre stânga
    }*/
}


void Tema1_bonus::OnKeyPress(int key, int mods)
{
    // TODO(student): Use keys to change the number of instances and the
    // number of generated points. Avoid the camera keys, and avoid the
    // the keys from `OnInputUpdate`.

    // TODO maybe delete
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

    // TODO maybe delete
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


    // TODO
    if (key == GLFW_KEY_5) //vad scena reala
    {
        type = 0;
    }
    if (key == GLFW_KEY_6) //vad scena reala
    {
        type = 1;
    }


}


void Tema1_bonus::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1_bonus::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1_bonus::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1_bonus::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1_bonus::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Tema1_bonus::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
