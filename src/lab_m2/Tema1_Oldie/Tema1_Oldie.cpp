


/*  Cum lucreaza tema, ce taste trebuie sa apas

1 si 2 sunt pentru rotatia televizorului
3 sunt pentru switch intre lines si triunghiuri
4 pentru ascunderea obiectelor
5 si 6 sunt intensitatea luminiii
7 este pentru bata
8 va fi pentru pistol
9 va fi pentru schimbarea tematicii lumii

r,g,b pentru a seta culoarea luminii
shift combinat cu r, g, b pentru a scadea culoarea luminii
-de pus z de dat undo la stricat obiecte
w,a,s,d,e,q pentru deplasarea lampei impreuna cu sursa de lumina
-t pentru refacere scena full cu setupul initial
-u, i ,o, j, k, l pentru a face lumina sa mearga intr-o directie sau in alta
-p pentru a schimba forma vezelor, de facut la fel si la picioarele de masa, ceva frumos, elegant
-f pentru a schimba flickeringul (toggle)


Bonusuri implementate:
-sticle si pahare cu reflexie
-a treia masa
-mai multe texturi
-scaune
-pot sa ma uit la sticla/pahar si sa vad artificiile din televizor
-2 scenarii diferite
-pot deplasa sursa de lumina, directia, intensitatea si culoarea
-forma vazelor si a picioarelor scaunelor difera
-am bata/pistol, pe care le pot folosi pentru a distruge obiecte
-t pentru a reseta scena

*/


#include "lab_m2/Tema1_Oldie/Tema1_Oldie.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m2;


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


ParticleEffect<Particle>* particleeffect3 = nullptr;
Tema1_Oldie::Tema1_Oldie()
{
}


Tema1_Oldie::~Tema1_Oldie()
{
}


void Tema1_Oldie::Init()
{
    angle2 = 0;
    auto camera = GetSceneCamera();
    camera->Update();

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
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    ToggleGroundPlane();
    cout << "Face rost de meshuri\n";
    //astea sunt pentru tipul de textura 1, adica cu 9 schimb tematica lumii
    bool vaseLoaded = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "vase.png");
    bool vaseLoaded2 = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "vase2.png");
    bool vaseLoaded3 = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "porcelain_vase.png");
    if (vaseLoaded3)
    {
        // Obtinem ID-ul texturii
        GLuint textureID = TextureManager::GetTexture("porcelain_vase.png")->GetTextureID();

        // O legam (bind) ca saii schimbam setarile
        glBindTexture(GL_TEXTURE_2D, textureID);

        //ii spunem ca pe axa S (orizontala) sa faca oglindire
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

        // Dezlegam textura (unbind)
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    cout << "Incarca si vaza\n";
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "wood.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "pringles.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "pringles2.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "pringles3.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "pringles4.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "gray.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "textured_gray.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "textured_white.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "beer_bottle.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "Wine_bottle.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "white.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "BaseballBat.png");

    //aici sunt texturile pentru tipul 2:
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "wood2.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "cola_can.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "redbull.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "fanta.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "mountain_dew.png");

    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "brick.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "black_tv.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "BaseballBat2.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "glass.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "futuristic_base_lamp.png");

    {
        // Definim o dimensiune mica pentru cruce (în coordonate normalizate de dispozitiv - NDC)
        // Ecranul merge de la -1 la +1. Deci 0.02 este destul de mic.
        float s = 0.02f;

        vector<VertexFormat> vertices =
        {
            // Linia orizontala (-)
            VertexFormat(glm::vec3(-s, 0, 0), glm::vec3(1, 1, 1)), // Stânga
            VertexFormat(glm::vec3(s, 0, 0), glm::vec3(1, 1, 1)), // Dreapta
            // Linia verticala (|)
            VertexFormat(glm::vec3(0, -s, 0), glm::vec3(1, 1, 1)), // Jos
            VertexFormat(glm::vec3(0,  s, 0), glm::vec3(1, 1, 1))  // Sus
        };

        vector<unsigned int> indices =
        {
            0, 1, // Prima linie (orizontala)
            2, 3  // A doua linie (verticala)
        };

        // Cream mesh-ul
        Mesh* crosshairMesh = new Mesh("crosshair");
        crosshairMesh->InitFromData(vertices, indices);
        //ii spunem ca vrem sa deseneze LINII, nu triunghiuri
        crosshairMesh->SetDrawMode(GL_LINES);
        meshes["crosshair"] = crosshairMesh;
    }

    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "grey.jpg");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "particle2.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "fabric.png");


    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "characters", "archer", "Akai_E_Espiritu.fbm"), "akai_diffuse.png");


    // Create a shader program for suprafata generation
    {
        Shader* shader = new Shader("suprafataGeneration");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_Oldie", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_Oldie", "shaders", "GeometryShader.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_Oldie", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("Shadow");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_Oldie", "shaders", "Shadow.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_Oldie", "shaders", "Shadow.GS.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_Oldie", "shaders", "Shadow.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader2 = new Shader("Fireworks");
        shader2->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_Oldie", "shaders", "Particle_fireworks.VS.glsl"), GL_VERTEX_SHADER);
        shader2->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_Oldie", "shaders", "Particle.GS.glsl"), GL_GEOMETRY_SHADER);
        shader2->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1_Oldie", "shaders", "Particle_simple.FS.glsl"), GL_FRAGMENT_SHADER);
        shader2->CreateAndLink();
        shaders[shader2->GetName()] = shader2;
    }
    gameTargets = {
        // Pringles
        {"Pringles 1", CAPSULE, PRINGLES_POS,  0.6f, 4.5f, glm::vec3(0), "pringles.png"},
        {"Pringles 2", CAPSULE, PRINGLES_POS2, 0.6f, 4.5f, glm::vec3(0), "pringles2.png"},
        {"Pringles 3", CAPSULE, PRINGLES_POS3, 0.6f, 4.5f, glm::vec3(0), "pringles3.png"},
        {"Pringles 4", CAPSULE, PRINGLES_POS4, 0.6f, 4.5f, glm::vec3(0), "pringles4.png"},

        // Sticle
        {"Bere",       SPHERE,  Bottle_pos,    0.6f, 0.0f, glm::vec3(0, 1.5f, 0), "beer_bottle.png"},
        {"Vin",        SPHERE,  Bottle_pos2,   0.6f, 0.0f, glm::vec3(0, 1.5f, 0), "Wine_bottle.png"},
        {"Bere 2", SPHERE, Bottle_pos3, 0.6f, 0.0f, glm::vec3(0, 1.5f, 0), "beer_bottle.png"},

        // Vaze
        {"Vaza 1",     SPHERE,  VASE1_POS,     0.9f, 0.0f, glm::vec3(0, 1.5f, 0), "vase.png"},
        {"Vaza 2",     SPHERE,  VASE2_POS,     0.9f, 0.0f, glm::vec3(0, 1.5f, 0), "vase.png"},

        {"Pringles 5", CAPSULE, TABLE3_POS + glm::vec3(7.0f, 1.0f,  -20.0f), 0.6f, 4.5f, glm::vec3(0), "pringles.png"},
        {"Pringles 6", CAPSULE, TABLE3_POS + glm::vec3(17.0f, 1.0f, -1.5f), 0.6f, 4.5f, glm::vec3(0), "pringles2.png"},

        // Vaze noi
        {"Vaza 3",     SPHERE,  TABLE3_POS + glm::vec3(10.0f, 1.0f, -14.0f), 0.9f, 0.0f, glm::vec3(0, 1.5f, 0), "vase.png"},
        {"Vaza 4",     SPHERE,  TABLE3_POS + glm::vec3(10.0f, 1.0f,  -1.0f), 0.9f, 0.0f, glm::vec3(0, 1.5f, 0), "porcelain_vase.png"},

        // Lampa
        {"Lampa",      SPHERE,  LAMP_POS,      1.5f, 0.0f, glm::vec3(0, 4.0f, 0), "grey.jpg"}
    };



    ResetParticlesFireworks2(20, 20, 20);
    // Parameters related to suprafata generation
    puncte_generate = 25;            // number of points on a Bezier curve
    instante = 25;                    // number of instances (number of curves that contain the suprafata)
    max_translate = 12.0f;                   // for the translation suprafata, it's the distance between the first and the last curve
    max_rotate = glm::radians(720.0f);      // for the rotation suprafata, it's the angle between the first and the last curve

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

        meshes["suprafata"] = new Mesh("generated initial suprafata points");
        meshes["suprafata"]->InitFromData(vertices, indices);
        meshes["suprafata"]->SetDrawMode(GL_LINES);
    }
    generator_position2 = glm::vec3(100, 0, 0);
    offset2 = 0.05;


    CreateShadowFramebuffer();

    CreateCubemapFramebuffer();

    CreateTVFramebuffer();

    CreateBottleCubemapFramebuffer(bottleCubemapFBO1, bottleCubemapTexture1, bottleCubemapDepth1);
    CreateBottleCubemapFramebuffer(bottleCubemapFBO2, bottleCubemapTexture2, bottleCubemapDepth2);
    CreateBottleCubemapFramebuffer(bottleCubemapFBO3, bottleCubemapTexture3, bottleCubemapDepth3);

    // Initialize light position (above the lamp)
    lightIntensity = 0.0f;  // Increase intensity
    spotAngle = 45.0f;
    lightTimer = Engine::GetElapsedTime();



    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "characters", "archer", "Akai_E_Espiritu.fbm"), "akai_diffuse.png");


    std::string backgroundPath = PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES, "Tema1_Oldie");
}





void Tema1_Oldie::DrawCrosshair()
{
    // Daca nu avem pistolul, nu desenam tinta
    if (!showPistol) return;

    // Folosim shaderul simplu "Color" (acelasi folosit la artificii)
    Shader* shader = shaders["Color"];
    if (!shader || !shader->GetProgramID()) return;

    glUseProgram(shader->program);

    // =========================================================
    // SETaRI CRITICE PENTRU UI 2D
    // =========================================================

    // 1. Dezactivam Depth Test-ul. 
    // Vrem ca tinta sa fie desenata PESTE orice altceva, chiar daca e un peretein fata.
    glDisable(GL_DEPTH_TEST);

    // 2. Matrici Identitate.
    // Pentru a desena direct pe mijlocul ecranuluiin 2D, 
    // matricile View si Projection trebuie sa fie "neutre" (identitate).
    // Astfel, coordonatele mesh-ului (acea dimensiune mica 's') se aplica direct pe ecran.
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(identityMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "View"), 1, GL_FALSE, glm::value_ptr(identityMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Projection"), 1, GL_FALSE, glm::value_ptr(identityMatrix));

    // 3. Culoarea Alba
    glm::vec3 whiteColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glUniform3fv(glGetUniformLocation(shader->program, "color"), 1, glm::value_ptr(whiteColor));

    // =========================================================
    // DESENARE
    // =========================================================
    // Desenam mesh-ul "crosshair" creatin Init
    meshes["crosshair"]->Render();

    // 4. Reactivam Depth Test-ul (FOARTE IMPORTANT pentru restul scenei)
    glEnable(GL_DEPTH_TEST);
}

void Tema1_Oldie::RenderFireworksToTexture(float deltaTimeSeconds)
{
    glBindFramebuffer(GL_FRAMEBUFFER, tvFBO);
    glViewport(0, 0, 512, 512);

    // Curatam ecranul
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configurare Camera Fixa
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

    // Sincronizare obiect camera (pentru particule)
    static gfxc::Camera* tvCamera = nullptr;
    if (tvCamera == nullptr) {
        tvCamera = new gfxc::Camera();
        tvCamera->SetPerspective(60.0f, 1.0f, 0.1f, 100.0f);
    }

    // Doar actualizam pozitia, nu recream obiectul
    tvCamera->SetPosition(glm::vec3(0, 0, 5));
    tvCamera->SetRotation(glm::vec3(0, 0, 0));
    tvCamera->Update();

    // ---------------------------------------------------------
    // LOGICa AVANSATa DE POZItIONARE
    // ---------------------------------------------------------

    float normalizedTime = 0.0f;
    if (fireworkInterval > 0.0f) {
        normalizedTime = fireworkTimer / fireworkInterval;
    }
    glm::vec3 activeColor = fireworkColors[current_color];
    glm::vec3 activeColor2 = fireworkColors[(current_color + 1) % fireworkColors.size()];
    // Definim limitele: De unde pleaca (-6.0) si unde explodeaza (2.0)
    float startY = -6.0f;
    float endY = 0.0f;

    // Definim durata (procentuala) a exploziei. 
    //in primele 15% din timp, stam pe loc SUS ca sa se vada explozia.
    float explosionPhase = 0.25f;

    glm::vec3 currentPos;
    bool drawRocketMesh = false;

    if (normalizedTime < explosionPhase)
    {
        // FAZA 1: EXPLOZIE (0% -> 15%)
        // tinem emitatorul fixat SUS. Particulele tocmai s-au resetat si explodeaza aici.
        currentPos = glm::vec3(0, endY, 0);

        // Nu desenam cubul (racheta a explodat deja)
        drawRocketMesh = false;
    }
    else
    {
        // FAZA 2: URCARE (15% -> 100%)
        // Recalculam timpul pentru a merge de la 0 la 1in intervalul ramas
        float riseTime = (normalizedTime - explosionPhase) / (1.0f - explosionPhase);

        // Interpolam de la Jos la Sus
        float y = glm::mix(startY, endY, riseTime);
        currentPos = glm::vec3(0, y, 0);

        // Desenam cubul doar când urca
        drawRocketMesh = true;
    }

    // ---------------------------------------------------------
    // 2. DESENARE CUB RACHETa (Doarin faza de urcare)
    // ---------------------------------------------------------
    if (drawRocketMesh)
    {
        Shader* colorShader = shaders["Color"];
        if (colorShader) {
            colorShader->Use();

            glm::mat4 modelBox = glm::translate(glm::mat4(1.0f), currentPos);
            modelBox = glm::scale(modelBox, glm::vec3(0.2f, 0.2f, 0.2f));

            glUniformMatrix4fv(glGetUniformLocation(colorShader->program, "View"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
            glUniformMatrix4fv(glGetUniformLocation(colorShader->program, "Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
            glUniformMatrix4fv(glGetUniformLocation(colorShader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelBox));
            glUniform3fv(glGetUniformLocation(colorShader->program, "color"), 1, glm::value_ptr(activeColor2));

            if (meshes.find("box") != meshes.end()) meshes["box"]->Render();
        }
    }

    // ---------------------------------------------------------
    // 3. DESENARE PARTICULE
    // ---------------------------------------------------------
    auto shader = shaders["Fireworks"];
    if (shader) {
        shader->Use();
        TextureManager::GetTexture("particle2.png")->BindToTextureUnit(GL_TEXTURE0);

        // Emitatorul sta la baza rachetei
        glm::vec3 emitterPos = currentPos + glm::vec3(0, -0.5f, 0);

        // Daca suntemin faza de explozie, centram emitatorul exact pe explozie
        if (!drawRocketMesh) {
            emitterPos = currentPos;
        }
        emitterPos = emitterPos - glm::vec3(0, 8.8, 0);

        glUniform3fv(glGetUniformLocation(shader->program, "generator_position"), 1, glm::value_ptr(emitterPos));
        glUniform1f(glGetUniformLocation(shader->program, "deltaTime"), deltaTimeSeconds);
        glUniform1f(glGetUniformLocation(shader->program, "offset"), offset2);
        glUniform3fv(glGetUniformLocation(shader->program, "color"), 1, glm::value_ptr(activeColor));

        // Trimitem matricile manual
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "View"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        particleeffect3->Render(tvCamera, shader);
    }

    //delete tvCamera;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Tema1_Oldie::CreateTVFramebuffer()
{
    // TV: 2D, Color + Depth Renderbuffer (pentru ca nu citim depth-ul TV-ului in shader)
    FBOConfig cfg;
    cfg.width = 512; cfg.height = 512;
    cfg.useDepthTexture = false; // Folosim Renderbuffer (fix-ul anterior)
    cfg.isCubemap = false;

    // Variabila rbo o definim local daca nu e in clasa, dar helperul foloseste referinta depthObj
    GLuint depthRBO;
    CreateFramebufferInternal(tvFBO, tvTexture, depthRBO, cfg);
}

void Tema1_Oldie::CreateShadowFramebuffer()
{
    // Shadow: 2D, Depth Only (Color = None), Texture Depth (pentru citire)
    FBOConfig cfg;
    cfg.width = shadowMapResolution; cfg.height = shadowMapResolution;
    cfg.useDepthTexture = true;
    cfg.isCubemap = false;
    cfg.internalFormat = GL_NONE; // Nu vrem culoare
    cfg.minFilter = GL_NEAREST; cfg.magFilter = GL_NEAREST; // Pentru umbre clare
    cfg.wrapMode = GL_CLAMP_TO_BORDER; // Esential pentru umbre

    // dummy pentru culoare (nu se va genera)
    GLuint dummyColor;
    CreateFramebufferInternal(shadowFBO, dummyColor, shadowMapTexture, cfg);

    std::cout << "Shadow FBO created: " << cfg.width << "x" << cfg.height << std::endl;
}

void Tema1_Oldie::CreateCubemapFramebuffer()
{
    // Reflection: Cubemap, Color + Depth Texture (daca vrei sa o folosesti)
    FBOConfig cfg;
    cfg.width = cubemapSize; cfg.height = cubemapSize;
    cfg.useDepthTexture = true; // De obicei util la cubemaps
    cfg.isCubemap = true;

    CreateFramebufferInternal(cubemapFBO, cubemapTexture, cubemapDepth, cfg);
}

void Tema1_Oldie::CreateBottleCubemapFramebuffer(GLuint& fbo, GLuint& texture, GLuint& depth)
{
    // Bottle: Identic cu Cubemap principal, dar size mai mic (256)
    FBOConfig cfg;
    cfg.width = 256; cfg.height = 256;
    cfg.useDepthTexture = true;
    cfg.isCubemap = true;

    CreateFramebufferInternal(fbo, texture, depth, cfg);
}

void Tema1_Oldie::CreateFramebufferInternal(GLuint& fbo, GLuint& colorTex, GLuint& depthObj, const FBOConfig& cfg)
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // 1. Color Attachment (Texture)
    // Shadow Map nu are Color Attachment, doar Depth
    if (cfg.internalFormat != GL_NONE)
    {
        glGenTextures(1, &colorTex);
        GLenum target = cfg.isCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
        glBindTexture(target, colorTex);

        if (cfg.isCubemap) {
            for (int i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, cfg.internalFormat, cfg.width, cfg.height, 0, GL_RGB, cfg.type, NULL);
            }
        }
        else {
            glTexImage2D(GL_TEXTURE_2D, 0, cfg.internalFormat, cfg.width, cfg.height, 0, GL_RGB, cfg.type, NULL);
        }

        // Parametrii comuni
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, cfg.minFilter);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, cfg.magFilter);
        glTexParameteri(target, GL_TEXTURE_WRAP_S, cfg.wrapMode);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, cfg.wrapMode);
        if (cfg.isCubemap) glTexParameteri(target, GL_TEXTURE_WRAP_R, cfg.wrapMode);

        // Attach
        if (cfg.isCubemap) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, colorTex, 0);
        }
        else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
        }
    }
    else {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    // 2. Depth Attachment (Texture sau Renderbuffer)
    if (cfg.useDepthTexture)
    {
        glGenTextures(1, &depthObj);
        GLenum target = cfg.isCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
        glBindTexture(target, depthObj);

        if (cfg.isCubemap) {
            for (int i = 0; i < 6; i++)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, cfg.width, cfg.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, cfg.width, cfg.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }

        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, cfg.minFilter);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, cfg.magFilter);
        glTexParameteri(target, GL_TEXTURE_WRAP_S, cfg.wrapMode);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, cfg.wrapMode);
        if (cfg.isCubemap) glTexParameteri(target, GL_TEXTURE_WRAP_R, cfg.wrapMode);

        // Border color pentru Shadow Map (Clamp to Border)
        if (cfg.wrapMode == GL_CLAMP_TO_BORDER) {
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
        }

        if (cfg.isCubemap) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, depthObj, 0);
        }
        else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthObj, 0);
        }
    }
    else
    {
        // Renderbuffer (mai rapid daca nu citim din el in shader)
        glGenRenderbuffers(1, &depthObj);
        glBindRenderbuffer(GL_RENDERBUFFER, depthObj);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cfg.width, cfg.height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthObj);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Tema1_Oldie::UpdateCubemapViewMatrices()
{
    // 1. Calcul Pozitie Centru (Pivot + Offset Ecran)
    // Offset local: Ecranul e putinin fata (Z) si sus (Y)
    glm::vec3 screenOffsetLocal = glm::vec3(0.0f, 5.0f / 2.0f * 1.3f, 5.0f / 2.0f);

    // Aplicam rotatia TV-ului asupra offset-ului
    glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(tvrotationangle), glm::vec3(0, 1, 0));
    glm::vec3 offsetWorld = glm::vec3(rotMatrix * glm::vec4(screenOffsetLocal, 1.0f));

    glm::vec3 center = TV_POS + offsetWorld;

    // 2. Generare Matrici (Folosind un vector de directii pentru concizie)
    struct ViewDir { glm::vec3 target; glm::vec3 up; };
    const ViewDir views[] = {
        { { 1, 0, 0}, {0,-1, 0} }, // +X (Right)
        { {-1, 0, 0}, {0,-1, 0} }, // -X (Left)
        { { 0, 1, 0}, {0, 0, 1} }, // +Y (Top)
        { { 0,-1, 0}, {0, 0,-1} }, // -Y (Bottom) -- Atentie la Up vector aici!
        { { 0, 0, 1}, {0,-1, 0} }, // +Z (Front)
        { { 0, 0,-1}, {0,-1, 0} }  // -Z (Back)
    };

    //in codul original, Up vectorii erau uneori {0,0,1} sau {0,-1,0}. 
    // Am pastrat logica standard OpenGL Cubemap, dar am ajustat la codul tau.
    // Daca imaginile suntintoarse, schimba 'up' vectorii de mai sus.

    for (int i = 0; i < 6; i++) {
        cubemapViewMatrices[i] = glm::lookAt(center, center + views[i].target, views[i].up);
    }
}


void Tema1_Oldie::RenderShadowMap()
{
    // 1. Configurare Framebuffer pentru Umbre
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glViewport(0, 0, shadowMapResolution, shadowMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);

    // 2. Configurare Stare OpenGL (Optimizat pentru Depth Pass)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE); // Critic: Permite scriereain Depth Buffer
    glDisable(GL_BLEND);  // Nu avem nevoie de transparenta la umbre

    // Optional: Front Face Culling ajuta la "Shadow Acne", dar codul tau originalil avea comentat.
    // Daca vrei sa-l activezi, decomenteaza liniile:
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT); 

    // 3. Configurare Shader
    Shader* shadowShader = shaders["Shadow"];
    if (shadowShader)
    {
        shadowShader->Use();

        // Trimitem matricea luminii
        GLint locLightSpace = glGetUniformLocation(shadowShader->program, "LightSpaceMatrix");
        glUniformMatrix4fv(locLightSpace, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // 4. Executare Randare Geometrie
        RenderProceduralMeshesForShadow(shadowShader);
    }

    // 5. Restaurare Stare (Cleanup)
    // Revenim la setarile normale pentru randarea scenei colorate
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Tema1_Oldie::RenderProceduralMeshesForShadow(Shader* shader)
{
    // A. Deseneaza Elementele de Mediu (Decor Static)
    {
        DrawRoom(shader);
        DrawTable(shader, TABLE1_POS, 0.0f);
        DrawTable(shader, TABLE2_POS, 0.0f);
        DrawTable(shader, TABLE3_POS, 90.0f);
        DrawTV(shader, TV_POS, tvrotationangle);

        float yFloorOffset = -7.0f;
        float center = 2.5f;

        DrawChair(shader, TABLE1_POS + glm::vec3(12, yFloorOffset, center), 180.0f);
        DrawChair(shader, TABLE1_POS + glm::vec3(12, yFloorOffset, center + 18), 0.0f);
        DrawChair(shader, TABLE1_POS + glm::vec3(-0.0f, yFloorOffset, 7.75f), -90.0f);
        DrawChair(shader, TABLE1_POS + glm::vec3(23.0f, yFloorOffset, 7.75f), 90.0f);

        DrawChair(shader, TABLE2_POS + glm::vec3(12, yFloorOffset, center), 180.0f);
        DrawChair(shader, TABLE2_POS + glm::vec3(12, yFloorOffset, center + 18), 0.0f);
        DrawChair(shader, TABLE2_POS + glm::vec3(-0.0f, yFloorOffset, 7.75f), -90.0f);
        DrawChair(shader, TABLE2_POS + glm::vec3(23.0f, yFloorOffset, 7.75f), 90.0f);

        DrawChair(shader, TABLE3_POS + glm::vec3(12, yFloorOffset, center - 26.0f), 180.0f);
        DrawChair(shader, TABLE3_POS + glm::vec3(12, yFloorOffset, center - 5.0f), 0.0f);
        DrawChair(shader, TABLE3_POS + glm::vec3(-0.0f, yFloorOffset, -13.0f), -90.0f);
        DrawChair(shader, TABLE3_POS + glm::vec3(18.0f, yFloorOffset, -13.0f), 90.0f);
    }

    // B. Deseneaza Armele (Daca sunt active, pozitia lor e calculatain Update)
    {
        DrawBaseballBat(shader, BASEBALL_MAT);
        DrawPistol(shader, GUN_MAT);
    }

    // C. Deseneaza Entitatile Dinamice (Iterare prin vector)
    for (const auto& entity : gameTargets)
    {
        // 1. Verificari rapide de validitate
        if (!entity.isAlive) continue;

        //in modul Shadow, vrem sa desenam obiectul chiar daca e invizibil pe ecran? 
        // Codul original zicea: if (!obj.isVisible) continue;
        if (!entity.isVisible) continue;

        // 2. Dispatcherin functie de tipul obiectului
        // Verificam numele pentru a decide geometria corecta
        if (entity.name.find("Pringles") != std::string::npos)
        {
            DrawPringles(shader, entity.pos, entity.textureName);
        }
        else if (entity.name.find("Vaza") != std::string::npos)
        {
            DrawVase(shader, entity.pos);
        }
        else if (entity.name == "Lampa")
        {
            DrawLamp(shader, entity.pos, 45.0f);
            //cout << LAMP_POS << "\n";
        }
        else if (entity.name == "Bere" || entity.name == "Vin" || entity.name == "Bere 2")
        {
            // Sticlele au geometrie identica, difera textura
            DrawBottle(shader, entity.pos, 0.0f, entity.textureName);
        }
    }
}

void Tema1_Oldie::DrawSceneObjects(Shader* shader, bool drawTransparentObjects, std::string ignoreName)
{
    // 1. Obiecte Statice (Decor)
    DrawRoom(shader);
    DrawTable(shader, TABLE1_POS, 0.0f);
    DrawTable(shader, TABLE2_POS, 0.0f);
    DrawTable(shader, TABLE3_POS, 90.0f);
    DrawTV(shader, TV_POS, tvrotationangle);
    DrawBaseballBat(shader, BASEBALL_MAT);
    DrawPistol(shader, GUN_MAT);

    float yFloorOffset = -7.0f;
    float center = 2.5f;


    DrawChair(shader, TABLE1_POS + glm::vec3(12, yFloorOffset, center), 180.0f);
    DrawChair(shader, TABLE1_POS + glm::vec3(12, yFloorOffset, center + 18), 0.0f);
    DrawChair(shader, TABLE1_POS + glm::vec3(-0.0f, yFloorOffset, 7.75f), -90.0f);
    DrawChair(shader, TABLE1_POS + glm::vec3(23.0f, yFloorOffset, 7.75f), 90.0f);

    DrawChair(shader, TABLE2_POS + glm::vec3(12, yFloorOffset, center), 180.0f);
    DrawChair(shader, TABLE2_POS + glm::vec3(12, yFloorOffset, center + 18), 0.0f);
    DrawChair(shader, TABLE2_POS + glm::vec3(-0.0f, yFloorOffset, 7.75f), -90.0f);
    DrawChair(shader, TABLE2_POS + glm::vec3(23.0f, yFloorOffset, 7.75f), 90.0f);

    DrawChair(shader, TABLE3_POS + glm::vec3(12, yFloorOffset, center - 26.0f), 180.0f);
    DrawChair(shader, TABLE3_POS + glm::vec3(12, yFloorOffset, center - 5.0f), 0.0f);
    DrawChair(shader, TABLE3_POS + glm::vec3(-0.0f, yFloorOffset, -13.0f), -90.0f);
    DrawChair(shader, TABLE3_POS + glm::vec3(18.0f, yFloorOffset, -13.0f), 90.0f);



    // Ecranul TV (doar daca shaderul suporta texturi - nu la shadow map simplu)
    if (shader->GetName() != "Shadow") {
        DrawScreen(shader, TV_POS, tvrotationangle);
    }

    // 2. Obiecte Dinamice (Game Targets)
    for (const auto& obj : gameTargets)
    {
        if (obj.name == ignoreName) continue;
        // Verificam daca obiectul e viu si vizibil (pentru mecanica de disparitie)
        if (!obj.isAlive || (!obj.isVisible && shader->GetName() != "Shadow")) continue;

        if (obj.name.find("Pringles") != std::string::npos) {
            DrawPringles(shader, obj.pos, obj.textureName);
        }
        else if (obj.name.find("Vaza") != std::string::npos) {
            DrawVase(shader, obj.pos);
        }
        else if (obj.name == "Bere" || obj.name == "Vin" || obj.name == "Bere 2") {
            // Desenam sticlele doar daca flag-ul permite (pentru a evita artefactein propriul cubemap)
            if (drawTransparentObjects) {
                DrawBottle(shader, obj.pos, 0.0f, obj.textureName);
            }
        }
        else if (obj.name == "Lampa") {
            DrawLamp(shader, obj.pos, 45.0f);
        }
    }
}

// -------------------------------------------------------------------------
// REFACTORED: RenderBottleCubemap
// Acum foloseste DrawSceneObjects, reducând codul de la 100+ linii la 30.
// -------------------------------------------------------------------------
void Tema1_Oldie::RenderBottleCubemap(GLuint fbo, GLuint texture, GLuint depth,
    glm::mat4* viewMatrices, glm::vec3 bottlePos)
{
    int size = 256;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, size, size);

    Shader* shader = shaders["suprafataGeneration"];
    shader->Use();

    // Setari globale temporare pentru randarein textura
    render_to_cubemap = 1;
    puncte_generate = 6; // Low poly pentru performanta
    instante = 6;

    // Setam matricea de proiectie o singura data
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Projection"), 1, GL_FALSE, glm::value_ptr(proj));

    // Trimitem toate matricile de view (Geometry shader-ul alege fata)
    GLint loc = glGetUniformLocation(shader->program, "viewMatrices[0]");
    for (int i = 0; i < 6; i++) {
        glUniformMatrix4fv(loc + i, 1, GL_FALSE, glm::value_ptr(viewMatrices[i]));
    }

    // Randam cele 6 feteintr-o singura trecere (sau bucla)
    for (int face = 0; face < 6; face++)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, depth, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniform1i(glGetUniformLocation(shader->program, "cubemap_face"), face);

        // Aici e magia: Apelam functia unica, dar ascundem sticla curenta!
        // Flag-ul 'isVisible' din GameTargets ar fi ideal, dar aici folosim logica de pozitie:
        // Hack temporar: mutam sticla curenta departe sau folosim parametrul drawTransparentObjects

        // Desenam scena. Observatie: Nu desenam sticlain propria reflexie!
        // Vom modifica DrawSceneObjects sa accepte un parametru de ignorare sau vom folosi logica din interior.
        // Pentru simplitate, desenam tot (sticla se va vedea putin ciudat daca nu o ascundem, dar pastram functionalitatea).
        DrawSceneObjects(shader, true, "");
    }

    // Mipmaps & Restore
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    render_to_cubemap = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}




// Functie pentru actualizarea matricelor de vedere pentru o sticla
void Tema1_Oldie::UpdateBottleCubemapViewMatrices(glm::vec3 bottlePosition, glm::mat4* viewMatrices)
{
    // Centrul sticlei este la pozitia ei +inaltimea la jumatate
    glm::vec3 bottleCenter = bottlePosition + glm::vec3(0.0f, 1.5f, 0.0f);

    // Genereaza cele 6 matrici de vedere pentru cubemap
    viewMatrices[0] = glm::lookAt(bottleCenter, bottleCenter + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0));  // +X
    viewMatrices[1] = glm::lookAt(bottleCenter, bottleCenter + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)); // -X
    viewMatrices[2] = glm::lookAt(bottleCenter, bottleCenter + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));   // +Y
    viewMatrices[3] = glm::lookAt(bottleCenter, bottleCenter + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)); // -Y
    viewMatrices[4] = glm::lookAt(bottleCenter, bottleCenter + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));  // +Z
    viewMatrices[5] = glm::lookAt(bottleCenter, bottleCenter + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0)); // -Z
}



void Tema1_Oldie::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1_Oldie::ResetParticlesFireworks2(int xSize, int ySize, int zSize)
{
    unsigned int nrParticles = 1000;

    // --- OPTIMIZARE 1: Lazy Initialization ---
    // Verificam daca pointerul este null.il alocam DOAR prima data.
    // Asigura-te cain Init() sau constructor ai pus: particleeffect3 = nullptr;
    if (particleeffect3 == nullptr)
    {
        particleeffect3 = new ParticleEffect<Particle>();
        particleeffect3->Generate(nrParticles, true);
    }

    // Obtinem acces la memoria existenta (fara sa realocam)
    auto particleSSBO = particleeffect3->GetParticleBuffer();
    Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

    glm::vec3 generatorPos = glm::vec3(0.0f, 10.0f, 0.0f);

    // --- OPTIMIZARE 2: Calcul eficient (Micro-optimizare) ---
    // Calculam constantelein afara buclei
    // Putem folosi rand() simplu, dar e bine sa evitam calcule redundante.

    for (unsigned int i = 0; i < nrParticles; i++)
    {
        glm::vec4 pos(generatorPos, 1.0f);

        // Generare directie sferica
        // Putem optimiza evitând conversiile float excesive, dar math-ul e rapid pe CPU modern.
        // Ce e important e ca NU mai alocam memorie aici.

        float theta = ((float)(rand() % 360)) * TO_RADIANS;
        float phi = ((float)(rand() % 180)) * TO_RADIANS;

        float speedMag = 1.5f + ((rand() % 100) / 100.0f); // Mica variatie la viteza pentru realism

        float sinPhi = sin(phi);
        float cosPhi = cos(phi);
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        glm::vec4 speed;
        speed.x = speedMag * sinPhi * cosTheta;
        speed.y = speedMag * cosPhi;
        speed.z = speedMag * sinPhi * sinTheta;
        speed.w = 0.0f;

        // Resetam doar valorile, nu obiectul
        data[i].SetInitial(pos, speed, 0.0f, 2.0f);
    }

    // Trimitem noile date la GPU (mult mai rapid decât distrugerea buffer-ului)
    particleSSBO->SetBufferData(data);
}



void Tema1_Oldie::DrawFireworks(float x, float y, float z, float deltaTimeSeconds) {
    fireworkTimer += deltaTimeSeconds;
    initialPosition = glm::vec3(x, y, z);
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

        if (fireworkTimer >= 1.0f && meshes.find("box") != meshes.end()) {
            RenderMesh(meshes["box"], colorShader, model2);
        }
    }

    // Deseneaza particulele
    auto shader15 = shaders["Fireworks"];
    if (shader15 && shader15->GetProgramID())
    {
        shader15->Use();

        TextureManager::GetTexture("particle2.png")->BindToTextureUnit(GL_TEXTURE0);
        glUniform3fv(glGetUniformLocation(shader15->program, "generator_position"), 1, glm::value_ptr(generator_position2));
        glUniform1f(glGetUniformLocation(shader15->program, "deltaTime"), deltaTimeSeconds);
        glUniform1f(glGetUniformLocation(shader15->program, "offset"), offset2);
        glUniform3fv(glGetUniformLocation(shader15->program, "color"), 1, glm::value_ptr(activeColor));

        particleeffect3->Render(GetSceneCamera(), shader15);
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
        generator_position2 = initialPosition;
    }
}

void Tema1_Oldie::RenderMeshInstanced(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int instances, const glm::vec3& color)
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


    glUniform3fv(glGetUniformLocation(shader->program, "light_position"), 1, glm::value_ptr(LIGHT_POSITION));
    glUniform3fv(glGetUniformLocation(shader->program, "light_direction"), 1, glm::value_ptr(LIGHT_DIRECTION));
    glUniform1f(glGetUniformLocation(shader->program, "light_intensity"), lightIntensity);
    glUniform1f(glGetUniformLocation(shader->program, "spot_angle"), spotAngle);


    auto cameraPos = GetSceneCamera()->m_transform->GetWorldPosition();
    glUniform3fv(glGetUniformLocation(shader->program, "camera_position"), 1, glm::value_ptr(cameraPos));
    //light_color = glm::normalize(light_color);
    glUniform3fv(glGetUniformLocation(shader->program, "light_color"), 1, glm::value_ptr(light_color));

    // Bind shadow map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glUniform1i(glGetUniformLocation(shader->program, "shadowMap"), 1);



    glUniform1i(glGetUniformLocation(shader->program, "instante"), instante);
    glUniform1i(glGetUniformLocation(shader->program, "puncte_generate"), puncte_generate);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);
    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), suprafata_type);

    glUniform1f(glGetUniformLocation(shader->program, "time"), glfwGetTime());
    glUniform1i(glGetUniformLocation(shader->program, "myTexture"), 0);

    //aici trimit un global_num_points
    glUniform1i(glGetUniformLocation(shader->program, "num_control_points"), global_num_points);
    if (global_num_points > 0 && !global_control_points.empty()) {

        // Obtinem locatia de start a array-ului uniform "control_points"
        // (Echivalent cu &control_points[0]in shader)
        GLint locArray = glGetUniformLocation(shader->program, "control_points");

        // glUniform3fv(locatie, k, pointer_date) va citi k vec3-uri consecutive din pointer
        glUniform3fv(locArray, global_num_points, glm::value_ptr(global_control_points[0]));
    }


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



void Tema1_Oldie::DrawBottle(Shader* shader, glm::vec3 pos, float unghi, const std::string& textureName)
{
    Mesh* mesh = meshes["suprafata"];
    max_rotate = glm::radians(880.0f);
    // 1. Configurare Texturi
    glActiveTexture(GL_TEXTURE0);
    //TextureManager::GetTexture()->BindToTextureUnit(GL_TEXTURE0);
    TextureManager::GetTexture(textureName.c_str())->BindToTextureUnit(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->program, "fireworks_texture"), 0);

    glActiveTexture(GL_TEXTURE2);
    if (pos == Bottle_pos) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, bottleCubemapTexture1);
    }
    else if (pos == Bottle_pos2) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, bottleCubemapTexture2);
    }
    else if (pos == Bottle_pos3) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, bottleCubemapTexture3);
    }
    else {
        // Fallback la cubemap-ul TV daca e alta pozitie
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    }
    //glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(glGetUniformLocation(shader->program, "texture_cubemap"), 2);

    // 2. Configurare Parametri
    suprafata_type = 7; // BOTTLE
    //max_rotate = glm::radians(360.0f);
    if (!active_texture) {
        control_p0 = glm::vec3(0.45f, 0.0f, 0.0f);
        control_p1 = glm::vec3(0.45f, 1.5f, 0.0f);
        control_p2 = glm::vec3(0.08f, 1.8f, 0.0f);
        control_p3 = glm::vec3(0.1f, 3.0f, 0.0f);
    }
    if (active_texture)
    {
        //pahar de vin
        control_p0 = glm::vec3(0.30f, 0.0f, 0.0f);
        control_p1 = glm::vec3(0.05f, 0.8f, 0.0f);
        control_p2 = glm::vec3(0.80f, 2.0f, 0.0f);
        control_p3 = glm::vec3(0.50f, 3.0f, 0.0f);

        /*control_p0 = glm::vec3(0.30f, 0.0f, 0.0f);
        control_p1 = glm::vec3(0.05f, 0.8f, 0.0f);
        control_p2 = glm::vec3(0.80f, 2.0f, 0.0f);
        control_p3 = glm::vec3(0.50f, 3.0f, 0.0f);*/
    }

    float save_instante = instante;
    float save_puncte_generate = puncte_generate;

    instante = 40.0f;       // Mai multe felii pentru rotunjime fina
    puncte_generate = 20.0f;

    // 3. Transformare
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, pos);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(unghi), glm::vec3(0.f, 1.0f, 0.f));

    // --- FIX-UL ESTE AICI ---
    // Dezactivam Culling-ul pentru a vedea si "spatele" triunghiurilor generate gresit
    // sau pentru a vedea si interiorul sticlei prin gura ei.
    glDisable(GL_CULL_FACE);

    RenderMeshInstanced(mesh, shader, modelMatrix, instante);

    // Reactivam Culling-ul pentru restul scenei (ca sa nu stricam performanta/aspectul la altele)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Asiguram-ne ca e setat pe BACK

    // Restaurare
    instante = save_instante;
    puncte_generate = save_puncte_generate;
    max_rotate = glm::radians(360.0f);
}
void Tema1_Oldie::DrawScreen(Shader* shader, glm::vec3 pos, float angle)
{
    // === TEXTURE BINDING ===
    struct TextureBinding {
        GLenum unit;
        GLuint textureID;
        const char* uniformName;
        GLenum target;
    };

    std::vector<TextureBinding> textures = {
        {GL_TEXTURE0, 0, "myTexture", GL_TEXTURE_2D},
        {GL_TEXTURE3, tvTexture, "fireworks_texture", GL_TEXTURE_2D},
        {GL_TEXTURE4, cubemapTexture, "texture_cubemap", GL_TEXTURE_CUBE_MAP}
    };

    // Bind material texture
    glActiveTexture(GL_TEXTURE0);
    const char* matTex = active_texture ? "black_tv.png" : "textured_gray.png";
    TextureManager::GetTexture(matTex)->BindToTextureUnit(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->program, "myTexture"), 0);

    // Bind special textures
    for (size_t i = 1; i < textures.size(); ++i) {
        const auto& tex = textures[i];
        glActiveTexture(tex.unit);
        glBindTexture(tex.target, tex.textureID);
        glUniform1i(glGetUniformLocation(shader->program, tex.uniformName),
            tex.unit - GL_TEXTURE0);
    }

    // === GEOMETRY CONFIGURATION ===
    struct SurfaceConfig {
        int type;
        float maxRotation;
        glm::vec3 p0, p1, p2, p3;
        float instances;
        float generatedPoints;
    } config = {
        6,                                    // type
        glm::pi<float>(),                    // maxRotation
        glm::vec3(-0.5f, 0.15f, 0.0f),      // p0
        glm::vec3(-0.2f, 0.25f, 0.0f),      // p1
        glm::vec3(0.2f, 0.25f, 0.0f),       // p2
        glm::vec3(0.5f, 0.15f, 0.0f),       // p3
        16.0f,                               // instances
        16.0f                                // generatedPoints
    };

    // Backup current state
    float prevInstances = instante;
    float prevPoints = puncte_generate;

    // Apply configuration
    suprafata_type = config.type;
    max_rotate = config.maxRotation;
    control_p0 = config.p0;
    control_p1 = config.p1;
    control_p2 = config.p2;
    control_p3 = config.p3;
    instante = config.instances;
    puncte_generate = config.generatedPoints;

    // === TRANSFORMATION CALCULATION ===
    const float BOX_SIZE = 5.0f;
    const float NATURAL_WIDTH = 1.0f;
    const float NATURAL_HEIGHT = 0.25f;

    glm::vec3 scale(
        BOX_SIZE / NATURAL_WIDTH,                    // X: 4.0
        (BOX_SIZE / NATURAL_HEIGHT) * 0.5f,         // Y: 8.0
        1.0f                                         // Z: 1.0
    );

    glm::vec3 offset(0.0f, BOX_SIZE * 0.5f, BOX_SIZE * 0.5f);

    // Build transformation matrix
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, pos);
    transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::translate(transform, offset);
    transform = glm::rotate(transform, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, scale);

    // === RENDERING ===
    RenderMeshInstanced(meshes["suprafata"], shader, transform, static_cast<int>(config.instances));

    // === STATE RESTORATION ===
    instante = prevInstances;
    puncte_generate = prevPoints;
}

void Tema1_Oldie::DrawTV(Shader* shader, glm::vec3 pos, float angle)
{
    // 1. Setup
    float s = 5.0f;
    Mesh* mesh = meshes["suprafata"];

    // Backup
    float old_inst = instante;
    float old_pts = puncte_generate;

    // Configurare Textura
    glActiveTexture(GL_TEXTURE0);
    const char* tex = active_texture ? "black_tv.png" : "textured_gray.png";
    TextureManager::GetTexture(tex)->BindToTextureUnit(GL_TEXTURE0);

    // Configurare Geometrie
    control_p0 = glm::vec3(-0.5f, 0.0f, 0.0f);
    control_p1 = glm::vec3(-0.5f, 2.5f, 0.0f);
    control_p2 = glm::vec3(-0.5f, 5.5f, 0.0f);
    control_p3 = glm::vec3(-0.5f, 8.0f, 0.0f);

    suprafata_type = 2; // PLANE
    instante = 2.0f;
    puncte_generate = 2.0f;

    // --- FIX AICI: Dezactivam Culling pentru a vedea toate fetele indiferent de rotatie ---
    glDisable(GL_CULL_FACE);

    // Matricea Pivot
    glm::mat4 pivot = glm::translate(glm::mat4(1.0f), pos);
    pivot = glm::rotate(pivot, glm::radians(angle), glm::vec3(0, 1, 0));

    // Structura Fetei
    struct TVFace {
        glm::vec3 offset;
        float r1; glm::vec3 ax1; // Rotatie 1
        float r2; glm::vec3 ax2; // Rotatie 2 (daca e nevoie, ex: pt dreapta)
    };

    // Definitia celor 6 fete (Matematica ajustata pentru cutieinchisa)
    static const std::vector<TVFace> faces = {
        // 1. SUS (Top)
        { {-s / 2, s, -s / 2}, 0.0f, {0,0,0}, 0.0f, {0,0,0} },

        // 2. JOS (Bottom) - Rotit 180 pe X
        //{ {-s / 2, 0, s / 2}, 180.0f, {1,0,0}, 0.0f, {0,0,0} },

        // 3. SPATE (Back) - Rotit 90 pe X
        { {-s / 2, s, -s / 2}, 90.0f, {1,0,0}, 0.0f, {0,0,0} },

        // 4. FAta (Front) - Rotit -90 pe X
        { {-s / 2, 0, s / 2}, -90.0f, {1,0,0}, 0.0f, {0,0,0} },

        // 5. STÂNGA (Left) - Rotit -90 pe Z (atentie la axa)
        // Nota:in codul tau era 90 pe Z, depinde de orientarea X. 
        // Daca -0.5 e baza, -90 pe Z ridica planul pe YZ.
        { {-s / 2, 0, -s / 2}, 90.0f, {0,0,1}, 0.0f, {0,0,0} },

        // 6. DREAPTA (Right) - Trebuie sa fie opusul stângii
        // Translatat la +s/2 pe X, Rotit -90 pe Z si 180 pe Y (sau X) ca sa priveascain afara
        { {s / 2, 0, s / 2}, 90.0f, {0,0,1}, 180.0f, {1,0,0} }
    };

    // Bucla de randare
    for (const auto& f : faces) {
        glm::mat4 m = glm::translate(pivot, f.offset);

        if (f.r1 != 0.0f) m = glm::rotate(m, glm::radians(f.r1), f.ax1);
        if (f.r2 != 0.0f) m = glm::rotate(m, glm::radians(f.r2), f.ax2);

        m = glm::scale(m, glm::vec3(s, 1.0f, s));

        RenderMeshInstanced(mesh, shader, m, (int)instante);
    }

    // --- Restore ---
    // Reactiveaza culling-ul pentru restul scenei (performanta)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    instante = old_inst;
    puncte_generate = old_pts;
}


// Adauga 'float rotationAngle' in definitia functiei (si in .h)
void Tema1_Oldie::DrawTable(Shader* shader, glm::vec3 pos, float rotationAngle)
{
    Mesh* mesh = meshes["suprafata"];

    // 1. Texturi
    glActiveTexture(GL_TEXTURE0);
    const char* texName = active_texture ? "wood2.png" : "wood.png";
    TextureManager::GetTexture(texName)->BindToTextureUnit(GL_TEXTURE0);

    // 2. Backup
    float save_inst = instante;
    float save_pts = puncte_generate;

    // 3. Geometrie
    static const float len = 25.0f;
    static const float wid = 20.0f;
    static const float thick = 1.0f;
    static const float legH = 7.0f;
    static const float legW = 2.0f;
    static const float cornerX = 4.0f;
    static const float cornerZ = 3.0f;

    // --- MODIFICARE: Creare matrice Pivot (Transformarea globala a mesei) ---
    // Aceasta matrice muta masa la 'pos' si o roteste cu 'rotationAngle'
    glm::mat4 pivotMatrix = glm::translate(glm::mat4(1.0f), pos);
    pivotMatrix = glm::rotate(pivotMatrix, glm::radians(rotationAngle), glm::vec3(0, 1, 0));


    // Profil standard 
    static const BezierCurve stdProfile = {
        {-0.5f, 0.0f, 0.0f}, {-0.5f, 2.0f, 0.0f},
        {-0.5f, 4.0f, 0.0f}, {-0.5f, 6.0f, 0.0f}
    };
    static const BezierCurve concaveProfile = {
        {0.0f, 0.0f, -2.5f}, // Sus
        {0.0f, 2.0f, -1.0f}, // Interior 1
        {0.0f, 4.0f, -1.0f}, // Interior 2
        {0.0f, 6.0f, -2.5f}  // Jos
    };

    // 4. Randare Blat (Suprafete Plate)
    auto DrawPart = [&](glm::mat4 m, int type, int inst, int pts, glm::vec2 uv, const BezierCurve& p) {
        instante = (float)inst;
        puncte_generate = (float)pts;
        suprafata_type = type;
        uvScale = uv;
        control_p0 = p.p0; control_p1 = p.p1; control_p2 = p.p2; control_p3 = p.p3;
        RenderMeshInstanced(mesh, shader, m, (int)instante);
        };

    // Blat Sus
    // Folosim pivotMatrix * offset local
    glm::mat4 m = pivotMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0, thick, 0));
    m = glm::scale(m, glm::vec3(len, 1.0f, wid));
    DrawPart(m, 2 /*PLANE*/, 2, 2, { 1,1 }, stdProfile);

    // Blat Jos (Rotit 180)
    m = pivotMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(len, 0, 0));
    m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0, 0, 1));
    m = glm::scale(m, glm::vec3(len, 1.0f, wid));
    DrawPart(m, 2 /*PLANE*/, 2, 2, { 1,1 }, stdProfile);

    // 5. Randare Margini (Edges)
    int edgeInst = 2;
    int edgePts = 10;
    glm::vec2 edgeUV = { 1.0f, 5.0f };

    struct Edge { glm::vec3 offset; glm::vec3 scale; float rotY; float rotZ; };
    static const std::vector<Edge> edges = {
        { {0,0,0}, {len, thick, 1.0f}, 0, 0 },
        { {len, 0, 0}, {wid, thick, 1.0f}, -90, 0 },
        { {len, 0, wid}, {len, thick, 1.0f}, -180, 0 },
        { {0, 0, wid}, {wid, thick, 1.0f}, -270, 0 }
    };

    glm::vec3 visScale = glm::vec3(1.0f);
    // (Optional: logica ta pentru shadow map scaling daca o pastrezi)
    // if (std::string(shader->GetName()) != "Shadow") visScale = ...

    for (const auto& e : edges) {
        suprafata_type = 1;
        // Transformare: Pivot -> Offset Local -> Rotatie Locala -> Scalare
        m = pivotMatrix * glm::translate(glm::mat4(1.0f), e.offset);

        if (e.rotY != 0) m = glm::rotate(m, glm::radians(e.rotY), glm::vec3(0, 1, 0));
        m = glm::scale(m, e.scale);
        m = glm::scale(m, visScale);

        DrawPart(m, 1 /*GRID*/, edgeInst, edgePts, edgeUV, concaveProfile);
    }

    // 6. Randare Picioare
    max_rotate = glm::radians(540.0f);
    uvScale = { 1.0f, 1.0f };
    global_control_points.clear();
    global_num_points = 0;

    static const BezierCurve legCurve = {
            {0.3f, 0.0f, 0.0f}, {0.25f, 2.0f, 0.0f},
            {0.6f, 4.0f, 0.0f}, {0.3f, 6.0f, 0.0f}
    };
    if (show_many_points) {
        const std::vector<glm::vec3> pts = {
            {0.0f, 0.0f, 0.0f}, {0.30f, 0.0f, 0.0f}, {0.35f, 0.2f, 0.0f}, {0.20f, 0.5f, 0.0f},
            {0.18f, 1.5f, 0.0f}, {0.20f, 2.5f, 0.0f}, {0.25f, 3.2f, 0.0f},
            {0.45f, 3.8f, 0.0f}, {0.50f, 4.3f, 0.0f}, {0.45f, 4.6f, 0.0f},
            {0.50f, 4.8f, 0.0f}, {0.60f, 5.2f, 0.0f}, {0.40f, 6.0f, 0.0f}
        };
        global_control_points = pts;
        while (global_control_points.size() < 20) global_control_points.push_back({ 0, 6.0f, 0 });
        global_num_points = 20;
        instante = 20.0f;
    }
    else {
        instante = 20.0f;
    }

    static const std::vector<glm::vec3> legPos = {
        {cornerX, -legH, cornerZ},                  // Stanga-Fata
        {len - cornerX, -legH, cornerZ},            // Dreapta-Fata
        {len - cornerX, -legH, wid - cornerZ},      // Dreapta-Spate
        {cornerX, -legH, wid - cornerZ}             // Stanga-Spate
    };
    for (const auto& offset : legPos) {
        // Transformare: Pivot -> Offset Picior -> Scalare
        m = pivotMatrix * glm::translate(glm::mat4(1.0f), offset);
        m = glm::scale(m, glm::vec3(legW, legH, legW));

        if (!show_many_points) DrawPart(m, 0 /*ROTATION*/, (int)instante, 10, { 1,1 }, legCurve);
        else {
            suprafata_type = 0;
            RenderMeshInstanced(mesh, shader, m, (int)instante);
        }
    }

    // 7. Cleanup
    instante = save_inst;
    puncte_generate = save_pts;
    global_num_points = 0;
}
// Nu uita sa actualizezi si in .h: 
// void DrawLamp(Shader* shader, glm::vec3 pos, float rotationAngle);

void Tema1_Oldie::DrawLamp(Shader* shader, glm::vec3 pos, float rotationAngle)
{
    // Scoatem coordonatele individuale doar daca avem nevoie de ele pentru calcule auxiliare, 
    // dar pentru transformari folosim matricea
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;

    glDisable(GL_CULL_FACE);
    Mesh* mesh = meshes["suprafata"];
    float lampHeight = 4.3f;
    float baseWidth = 0.8f;

    // --- MODIFICARE: Matricea Pivot ---
    glm::mat4 pivotMatrix = glm::translate(glm::mat4(1.0f), pos);
    pivotMatrix = glm::rotate(pivotMatrix, glm::radians(rotationAngle), glm::vec3(0, 1, 0));

    // ... (Codul existent pentru shadere si setari calitate ramane la fel) ...
    std::string currentShaderName = shader->GetName();
    if (currentShaderName == "Shadow") {
        max_rotate = glm::radians(0.f);
    }
    else {
        if (show_many_points) {
            max_rotate = glm::radians(427.5f);
        }
        else {
            max_rotate = glm::radians(360.0f);
        }
    }

    float save_instante = instante;
    float save_puncte_generate = puncte_generate;

    if (show_many_points) {
        instante = 20.0f;
        puncte_generate = 10.0f;
    }
    else {
        instante = 15.0f;
        puncte_generate = 8.0f;
    }

    glActiveTexture(GL_TEXTURE0);
    if (!active_texture) {
        TextureManager::GetTexture("grey.jpg")->BindToTextureUnit(GL_TEXTURE0);
    }
    else {
        TextureManager::GetTexture("futuristic_base_lamp.png")->BindToTextureUnit(GL_TEXTURE0);
    }

    suprafata_type = 0; // ROTATION
    global_control_points.clear();

    // ... (Generarea punctelor pentru BAZA ramane neschimbata) ...
    if (show_many_points)
    {
        global_control_points.push_back(glm::vec3(0.00f, 0.0f, 0.0f));
        global_control_points.push_back(glm::vec3(0.35f, 0.0f, 0.0f));
        global_control_points.push_back(glm::vec3(0.7f, 0.0f, 0.0f));
        global_control_points.push_back(glm::vec3(1.05f, 0.0f, 0.0f));
        global_control_points.push_back(glm::vec3(1.40f, 0.0f, 0.0f));
        global_control_points.push_back(glm::vec3(1.45f, 0.2f, 0.0f));
        global_control_points.push_back(glm::vec3(1.20f, 0.5f, 0.0f));
        global_control_points.push_back(glm::vec3(1.60f, 1.0f, 0.0f));
        global_control_points.push_back(glm::vec3(1.80f, 1.8f, 0.0f));
        global_control_points.push_back(glm::vec3(1.50f, 2.5f, 0.0f));
        global_control_points.push_back(glm::vec3(0.80f, 3.0f, 0.0f));
        global_control_points.push_back(glm::vec3(0.50f, 3.5f, 0.0f));
        global_control_points.push_back(glm::vec3(0.40f, 4.0f, 0.0f));
        global_control_points.push_back(glm::vec3(0.45f, 4.8f, 0.0f));

        global_control_points.push_back(glm::vec3(0.45f, 5.0f, 0.0f));
        global_control_points.push_back(glm::vec3(0.45f, 5.2f, 0.0f));
        global_control_points.push_back(glm::vec3(0.30f, 5.2f, 0.0f));
        global_control_points.push_back(glm::vec3(0.15f, 5.2f, 0.0f));
        global_control_points.push_back(glm::vec3(0.0f, 5.2f, 0.0f));

        while (global_control_points.size() < 20) {
            global_control_points.push_back(glm::vec3(0.0f, 5.2f, 0.0f));
        }

        global_num_points = 20;
    }
    else
    {
        control_p0 = glm::vec3(1.2f, 0.0f, 0.0f);
        control_p1 = glm::vec3(1.8f, 1.5f, 0.0f);
        control_p2 = glm::vec3(0.5f, 3.5f, 0.0f);
        control_p3 = glm::vec3(0.6f, 5.0f, 0.0f);
        global_num_points = 0;
    }

    // --- MODIFICARE AICI: Calcul Matrice Baza ---
    // Inainte: translate(x + 0.2f, y, z)
    // Acum:    pivot * translate(0.2f, 0, 0)
    glm::mat4 modelMatrix2 = pivotMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(baseWidth, lampHeight * 1.2f, baseWidth));

    RenderMeshInstanced(mesh, shader, modelMatrix2, instante);


    // =========================================================
    // PARTEA 2: ABAJURUL (LAMPSHADE)
    // =========================================================

    glActiveTexture(GL_TEXTURE0);
    if (!active_texture) {
        TextureManager::GetTexture("white.png")->BindToTextureUnit(GL_TEXTURE0);
    }
    else
    {
        TextureManager::GetTexture("fabric.png")->BindToTextureUnit(GL_TEXTURE0);
    }
    suprafata_type = 5;
    global_control_points.clear();

    // ... (Generarea punctelor pentru ABAJUR ramane neschimbata) ...
    if (show_many_points)
    {
        float bottomRadius = 3.5f;
        float topRadius = 1.2f;
        float height = 6.0f;

        global_control_points.push_back(glm::vec3(bottomRadius, 0.0f, 0.0f));
        global_control_points.push_back(glm::vec3(bottomRadius + 0.1f, 0.1f, 0.0f));

        for (int i = 0; i <= 10; i++) {
            float t = (float)i / 10.0f;
            float h = 0.1f + t * (height - 0.1f);
            float curveFactor = pow(t, 0.8f);
            float r = glm::mix(bottomRadius + 0.1f, topRadius, curveFactor);
            global_control_points.push_back(glm::vec3(r, h, 0.0f));
        }

        global_control_points.push_back(glm::vec3(topRadius, height, 0.0f));
        int currentSize = (int)global_control_points.size();
        int pointsRemaining = 20 - currentSize;

        if (pointsRemaining > 0)
        {
            for (int i = 1; i <= pointsRemaining - 1; i++)
            {
                float t = (float)(i + 5.0f) / (float)pointsRemaining;
                float r = glm::mix(topRadius, 0.0f, t);
                global_control_points.push_back(glm::vec3(r, height, 0.0f));
            }
        }

        global_num_points = 20;
    }
    else
    {
        control_p0 = glm::vec3(3.5, 0.0, 0.0);
        control_p1 = glm::vec3(2.0, 1.5, 0.0);
        control_p2 = glm::vec3(1.5, 5.5, 0.0);
        control_p3 = glm::vec3(0.0, 6.0, 0.0);
        global_num_points = 0;
    }

    // --- MODIFICARE AICI: Calcul Matrice Abajur ---
    // Inainte: translate(x, y + height, z - 1.65f)
    // Acum:    pivot * translate(0, height, -1.65f)
    // Pastram rotatiile locale (100 pe Y si 80 pe Z) pentru forma specifica a lampii
    glm::mat4 modelMatrix3 = pivotMatrix *
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, lampHeight * 1.2f, -1.65f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(100.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), glm::radians(80.0f), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(baseWidth * 4.0f, baseWidth * 3.0f, baseWidth * 4.0f));

    RenderMeshInstanced(mesh, shader, modelMatrix3, instante);

    // --- CLEANUP ---
    glEnable(GL_CULL_FACE);
    global_control_points.clear();
    global_num_points = 0;
    instante = save_instante;
    puncte_generate = save_puncte_generate;
    max_rotate = glm::radians(360.0f);
}

bool Tema1_Oldie::CheckSphereCollision(glm::vec3 point, float pointRadius, glm::vec3 sphereCenter, float sphereRadius)
{
    float distSq = glm::distance2(point, sphereCenter); // Distanta la patrat e mai rapida
    float radiusSum = pointRadius + sphereRadius;
    return distSq < (radiusSum * radiusSum);
}

bool Tema1_Oldie::CheckAABBCollision(glm::vec3 point, glm::vec3 boxCenter, glm::vec3 boxSize)
{
    // boxSize reprezinta "jumatate" din latime/înaltime/adâncime (Half-Extents)
    // Verificam daca punctul estein limitele cutiei pe fiecare axa
    bool collisionX = point.x >= (boxCenter.x - boxSize.x) && point.x <= (boxCenter.x + boxSize.x);
    bool collisionY = point.y >= (boxCenter.y - boxSize.y) && point.y <= (boxCenter.y + boxSize.y);
    bool collisionZ = point.z >= (boxCenter.z - boxSize.z) && point.z <= (boxCenter.z + boxSize.z);

    return collisionX && collisionY && collisionZ;
}

void Tema1_Oldie::CheckSceneCollisions(const glm::mat4& baseModelMatrix)
{
    // 1. CALCUL BÂTa (Ramâne la fel)
    float batHeight = 5.0f;
    glm::mat4 collisionMatrix = glm::scale(baseModelMatrix, glm::vec3(1.0f, batHeight, 1.0f));
    glm::vec3 batStart = glm::vec3(collisionMatrix * glm::vec4(0.0f, 0.2f, 0.0f, 1.0f));
    glm::vec3 batEnd = glm::vec3(collisionMatrix * glm::vec4(0.0f, 3.5f, 0.0f, 1.0f));
    float batRadius = 0.35f;

    std::string closestName = "Nimic";
    float minsuprafataDistance = 99999.0f;
    int closestIndex = -1; // Retinem indexul obiectului lovit

    // 2. ITERaM PRIN LISTA GLOBALa (gameTargets)
    for (int i = 0; i < gameTargets.size(); i++)
    {
        // Daca e deja "mort",il ignoram la coliziune
        if (!gameTargets[i].isAlive || !gameTargets[i].isVisible) continue;

        TargetObj& obj = gameTargets[i];
        float centerDistSq = 0.0f;

        if (obj.type == CAPSULE) {
            glm::vec3 pringlesTop = obj.pos + glm::vec3(0, obj.height, 0);
            centerDistSq = DistSegmentSegmentSq(batStart, batEnd, obj.pos, pringlesTop);
        }
        else {
            glm::vec3 sphereCenter = obj.pos + obj.offset;
            centerDistSq = GetDistanceToSegmentSq(sphereCenter, batStart, batEnd);
        }

        float centerDist = sqrt(centerDistSq);
        float suprafataDist = centerDist - (batRadius + obj.radius);

        if (suprafataDist < minsuprafataDistance) {
            minsuprafataDistance = suprafataDist;
            closestName = obj.name;
            closestIndex = i;
        }
    }

    // 3. VERIFICARE LOVITURa
    if (minsuprafataDistance <= 0.0f && closestIndex != -1)
    {
        std::cout << "[LOVIT] >>> " << closestName << " <<< DISPARE!" << std::endl;

        // ELIMINaM OBIECTUL (Îl marcam ca mort)
        gameTargets[closestIndex].isAlive = false;
        destroyedHistory.push_back(closestIndex);
        // Oprim verificarile pentru acest swing ca sa nu loveasca 2 deodata (optional)
        // hasHitObjectThisSwing = true; 
    }
    else {
        std::cout << "Cel mai aproape: " << closestName << " (" << minsuprafataDistance << "m)" << std::endl;
    }
}
// Functie ajutatoare: Calculeaza distanta minima la patrat dintre doua segmente
// Segment 1: p1 -> q1 (Bâta)
// Segment 2: p2 -> q2 (Obiectul/Pringles)
float Tema1_Oldie::DistSegmentSegmentSq(glm::vec3 p1, glm::vec3 q1, glm::vec3 p2, glm::vec3 q2)
{
    glm::vec3 d1 = q1 - p1; // Directia segmentului 1
    glm::vec3 d2 = q2 - p2; // Directia segmentului 2
    glm::vec3 r = p1 - p2;
    float a = glm::dot(d1, d1); // Lungimea patrata a seg 1
    float e = glm::dot(d2, d2); // Lungimea patrata a seg 2
    float f = glm::dot(d2, r);

    // Verificam daca segmentele sunt puncte (lungime 0)
    if (a <= 1e-6f && e <= 1e-6f) return glm::dot(r, r);
    if (a <= 1e-6f) return glm::dot(r, r) - f * f / e; // Primul e punct
    if (e <= 1e-6f) { // Al doilea e punct
        float c = glm::dot(d1, r);
        return glm::dot(r, r) - c * c / a;
    }

    float c = glm::dot(d1, r);
    float b = glm::dot(d1, d2);
    float denom = a * e - b * b; // Determinant

    float s, t; // Parametrii de interpolare pe segmente

    // Daca segmentele nu sunt paralele
    if (denom != 0.0f) {
        s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
    }
    else {
        s = 0.0f; // Paralele, alegem arbitrar startul
    }

    // Calculam t bazat pe s
    float t_nom = b * s + f;
    if (t_nom < 0.0f) {
        t = 0.0f;
        s = glm::clamp(-c / a, 0.0f, 1.0f);
    }
    else if (t_nom > e) {
        t = 1.0f;
        s = glm::clamp((b - c) / a, 0.0f, 1.0f);
    }
    else {
        t = t_nom / e;
    }

    // Punctele cele mai apropiate
    glm::vec3 closestOn1 = p1 + d1 * s;
    glm::vec3 closestOn2 = p2 + d2 * t;

    return glm::distance2(closestOn1, closestOn2);
}

float Tema1_Oldie::GetDistanceToSegmentSq(glm::vec3 point, glm::vec3 segA, glm::vec3 segB)
{
    glm::vec3 segment = segB - segA;
    glm::vec3 pointToA = point - segA;

    // Proiectam punctul pe linia segmentului
    float t = glm::dot(pointToA, segment) / glm::dot(segment, segment);

    // Limitam proiectia la capetele segmentului (Clampingintre 0 si 1)
    // Asta transforma linia infinitain segment finit
    t = glm::clamp(t, 0.0f, 1.0f);

    // Gasim cel mai apropiat punct de pe segment
    glm::vec3 closestPoint = segA + segment * t;

    // Returnam distanta la patrat fata de acel punct
    return glm::distance2(point, closestPoint);
}
void Tema1_Oldie::CheckGunShot()
{
    // 1. DEFINIM RAZA (GLONtUL)
    // Pleaca din pozitia camerei si merge pe directia Forward
    glm::vec3 rayStart = GetSceneCamera()->m_transform->GetWorldPosition();
    glm::vec3 camDir = -glm::normalize(GetSceneCamera()->m_transform->GetLocalOZVector());

    // Glontul merge 50 de metri (sau cât vrei tu raza de actiune)
    glm::vec3 rayEnd = rayStart + camDir * 50.0f;

    // Glontul are si el o grosime mica (ca sa nu ratezi la milimetru)
    float bulletRadius = 0.1f;

    // 2. CaUTaM CEL MAI APROPIAT OBIECT LOVIT
    int hitIndex = -1;
    float closestDistance = 9999.0f; // Distanta de la camera la obiect
    std::string hitName = "";

    for (int i = 0; i < gameTargets.size(); i++)
    {
        // Daca e deja distrus, glontul trece prin el
        if (!gameTargets[i].isAlive || !gameTargets[i].isVisible) continue;

        TargetObj& obj = gameTargets[i];
        float distSq = 0.0f;
        float combinedRadius = bulletRadius + obj.radius;

        // A. Coliziune Ray vs CAPSULE (Pringles)
        if (obj.type == CAPSULE)
        {
            glm::vec3 objTop = obj.pos + glm::vec3(0, obj.height, 0);

            // Folosim functia ta existenta: Distanta dintre Segment(Glont) si Segment(Pringles)
            distSq = DistSegmentSegmentSq(rayStart, rayEnd, obj.pos, objTop);
        }
        // B. Coliziune Ray vs SPHERE (Vaze, Sticle, Lampa)
        else
        {
            glm::vec3 sphereCenter = obj.pos + obj.offset;

            // Folosim functia ta existenta: Distanta de la Punct(Sfera) la Segment(Glont)
            distSq = GetDistanceToSegmentSq(sphereCenter, rayStart, rayEnd);
        }

        // VERIFICaM DACa AM LOVIT
        if (distSq < (combinedRadius * combinedRadius))
        {
            // Am lovit ceva! Dar este cel mai apropiat?
            // Calculam distanta de la camera la obiect ca sa vedem daca ein fata altora
            float distToCamera = glm::distance(rayStart, obj.pos);

            if (distToCamera < closestDistance)
            {
                closestDistance = distToCamera;
                hitIndex = i;
                hitName = obj.name;
            }
        }
    }

    // 3. APLICaM REZULTATUL
    if (hitIndex != -1)
    {
        std::cout << "[BANG] Ai impuscat: " << hitName << "!" << std::endl;
        destroyedHistory.push_back(hitIndex);
        // DISTRUGEM OBIECTUL
        gameTargets[hitIndex].isAlive = false;
    }
    else
    {
        std::cout << "[BANG] Ratat!" << std::endl;
    }
}
//in Tema1_Oldie.cpp

void Tema1_Oldie::DrawChair(Shader* shader, glm::vec3 pos, float rotationAngle)
{
    glDisable(GL_CULL_FACE);
    Mesh* mesh = meshes["suprafata"];

    // 1. Texturi
    glActiveTexture(GL_TEXTURE0);
    const char* texName = active_texture ? "wood2.png" : "wood.png";
    TextureManager::GetTexture(texName)->BindToTextureUnit(GL_TEXTURE0);

    // 2. Backup
    float save_inst = instante;
    float save_pts = puncte_generate;

    // 3. Geometrie
    static const float len = 5.0f;
    static const float wid = 5.0f;
    static const float thick = 1.0f;
    static const float legH = 5.0f;
    static const float legW = 1.0f;
    static const float cornerX = 0.5f;
    static const float cornerZ = 0.5f;
    static const float backH = 6.0f;    // Inaltime spatar

    // --- MODIFICARE: Creare matrice Pivot (Transformarea globala a mesei) ---
    // Aceasta matrice muta masa la 'pos' si o roteste cu 'rotationAngle'
    glm::mat4 pivotMatrix = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, legH, 0));
    pivotMatrix = glm::rotate(pivotMatrix, glm::radians(rotationAngle), glm::vec3(0, 1, 0));


    // Profil standard 
    static const BezierCurve stdProfile = {
        {-0.5f, 0.0f, 0.0f}, {-0.5f, 2.0f, 0.0f},
        {-0.5f, 4.0f, 0.0f}, {-0.5f, 6.0f, 0.0f}
    };
    static const BezierCurve concaveProfile = {
        {0.0f, 0.0f, -2.5f}, // Sus
        {0.0f, 2.0f, -1.0f}, // Interior 1
        {0.0f, 4.0f, -1.0f}, // Interior 2
        {0.0f, 6.0f, -2.5f}  // Jos
    };

    // 4. Randare Blat (Suprafete Plate)
    auto DrawPart = [&](glm::mat4 m, int type, int inst, int pts, glm::vec2 uv, const BezierCurve& p) {
        instante = (float)inst;
        puncte_generate = (float)pts;
        suprafata_type = type;
        uvScale = uv;
        control_p0 = p.p0; control_p1 = p.p1; control_p2 = p.p2; control_p3 = p.p3;
        RenderMeshInstanced(mesh, shader, m, (int)instante);
        };

    // Blat Sus
    // Folosim pivotMatrix * offset local
    glm::mat4 m = pivotMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0, thick, 0));
    m = glm::scale(m, glm::vec3(len, 1.0f, wid));
    DrawPart(m, 2 /*PLANE*/, 2, 2, { 1,1 }, stdProfile);

    // Blat Jos (Rotit 180)
    m = pivotMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(len, 0, 0));
    m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0, 0, 1));
    m = glm::scale(m, glm::vec3(len, 1.0f, wid));
    DrawPart(m, 2 /*PLANE*/, 2, 2, { 1,1 }, stdProfile);

    // 5. Randare Margini (Edges)
    int edgeInst = 2;
    int edgePts = 10;
    glm::vec2 edgeUV = { 1.0f, 2.0f };

    struct Edge { glm::vec3 offset; glm::vec3 scale; float rotY; float rotZ; };
    static const std::vector<Edge> edges = {
        { {0,0,0}, {len, thick, 1.0f}, 0, 0 },
        { {len, 0, 0}, {wid, thick, 1.0f}, -90, 0 },
        { {len, 0, wid}, {len, thick, 1.0f}, -180, 0 },
        { {0, 0, wid}, {wid, thick, 1.0f}, -270, 0 }
    };

    glm::vec3 visScale = glm::vec3(1.0f);
    // (Optional: logica ta pentru shadow map scaling daca o pastrezi)
    // if (std::string(shader->GetName()) != "Shadow") visScale = ...

    for (const auto& e : edges) {
        suprafata_type = 1;
        // Transformare: Pivot -> Offset Local -> Rotatie Locala -> Scalare
        m = pivotMatrix * glm::translate(glm::mat4(1.0f), e.offset);

        if (e.rotY != 0) m = glm::rotate(m, glm::radians(e.rotY), glm::vec3(0, 1, 0));
        m = glm::scale(m, e.scale);
        m = glm::scale(m, visScale);

        DrawPart(m, 1 /*GRID*/, edgeInst, edgePts, edgeUV, stdProfile);
    }

    // 6. Randare Picioare
    max_rotate = glm::radians(540.0f);
    uvScale = { 1.0f, 1.0f };
    global_control_points.clear();
    global_num_points = 0;

    static const BezierCurve legCurve = {
            {0.3f, 0.0f, 0.0f}, {0.25f, 2.0f, 0.0f},
            {0.6f, 4.0f, 0.0f}, {0.3f, 6.0f, 0.0f}
    };
    if (show_many_points) {
        static const std::vector<glm::vec3> pts = {
            {0.0f, 0.0f, 0.0f}, {0.30f, 0.0f, 0.0f}, {0.35f, 0.2f, 0.0f}, {0.20f, 0.5f, 0.0f},
            {0.18f, 1.5f, 0.0f}, {0.20f, 2.5f, 0.0f}, {0.25f, 3.2f, 0.0f},
            {0.45f, 3.8f, 0.0f}, {0.50f, 4.3f, 0.0f}, {0.45f, 4.6f, 0.0f},
            {0.50f, 4.8f, 0.0f}, {0.60f, 5.2f, 0.0f}, {0.40f, 6.0f, 0.0f}
        };
        global_control_points = pts;
        while (global_control_points.size() < 20) global_control_points.push_back({ 0, 6.0f, 0 });
        global_num_points = 20;
        instante = 20.0f;
    }
    else {
        instante = 20.0f;
    }

    static const std::vector<glm::vec3> legPos = {
        {cornerX, -legH, cornerZ},                  // Stanga-Fata
        {len - cornerX, -legH, cornerZ},            // Dreapta-Fata
        {len - cornerX, -legH, wid - cornerZ},      // Dreapta-Spate
        {cornerX, -legH, wid - cornerZ}             // Stanga-Spate
    };

    for (const auto& offset : legPos) {
        // Transformare: Pivot -> Offset Picior -> Scalare
        m = pivotMatrix * glm::translate(glm::mat4(1.0f), offset);
        m = glm::scale(m, glm::vec3(legW, legH, legW));

        if (!show_many_points) DrawPart(m, 0 /*ROTATION*/, (int)instante, 10, { 1,1 }, legCurve);
        else {
            suprafata_type = 0;
            RenderMeshInstanced(mesh, shader, m, (int)instante);
        }
    }

    float backThick = 0.8f;
    global_num_points = 0;
    // Mutam spatarul la marginea din spate (wid) si in sus (legH + thick)
    glm::vec3 backBasePos = glm::vec3(0, 1.0f, wid);

    // 3.1 Fata Spatar (cea spre care te sprijini)
    m = pivotMatrix * glm::translate(glm::mat4(1.0f), backBasePos);
    m = glm::translate(m, glm::vec3(0, 0, -backThick)); // Un pic in fata
    m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1, 0, 0)); // Ridicam vertical
    m = glm::scale(m, glm::vec3(len, 1.0f, backH));
    DrawPart(m, 2 /*PLANE*/, 2, 2, { 1,1 }, stdProfile);

    // 3.2 Spate Spatar
    m = pivotMatrix * glm::translate(glm::mat4(1.0f), backBasePos);
    m = glm::translate(m, glm::vec3(0, backH, 0)); // Sus
    m = glm::rotate(m, glm::radians(90.0f), glm::vec3(1, 0, 0)); // Rotim invers
    m = glm::scale(m, glm::vec3(len, 1.0f, backH));
    DrawPart(m, 2 /*PLANE*/, 2, 2, { 1,1 }, stdProfile);
    glm::mat4 backMatrix = pivotMatrix;
    // Mutam la marginea din spate (wid) si putinin sus (legH + thick)
    backMatrix = glm::translate(backMatrix, glm::vec3(0, 1.0f, wid));
    // Rotim 90 grade sa fie vertical (Acum Y-ul local al spatarului arata in sus)
    // NOTA: Daca folosim logica de la sezut unde Z e adancime, rotim pe X.
    backMatrix = glm::rotate(backMatrix, glm::radians(-90.0f), glm::vec3(1, 0, 0));

    // C1. Fete Plane Spatar
    suprafata_type = 2; // PLANE
    instante = 2.0f; puncte_generate = 2.0f;
    control_p0 = stdProfile.p0; control_p1 = stdProfile.p1; control_p2 = stdProfile.p2; control_p3 = stdProfile.p3;
    // Fata 1 (Cea dinspre spate, unde te sprijini)
    {
        glm::mat4 m = backMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0, backThick, 0));
        // Aici "wid" din logica sezutului devine "backH" (inaltimea spatarului)
        m = glm::scale(m, glm::vec3(len, 1.0f, backH));
        //RenderMeshInstanced(mesh, shader, m, (int)instante);
    }
    // Fata 2 (Cea din exteriorul scaunului)
    {
        glm::mat4 m = backMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(len, 0, 0));
        m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0, 0, 1));
        m = glm::scale(m, glm::vec3(len, 1.0f, backH));
        //RenderMeshInstanced(mesh, shader, m, (int)instante);
    }

    // C2. Margini Spatar (Grid)
    suprafata_type = 1; // GRID
    instante = 2.0f; puncte_generate = 10.0f;
    control_p0 = stdProfile.p0; control_p1 = stdProfile.p1; control_p2 = stdProfile.p2; control_p3 = stdProfile.p3;

    static const std::vector<Edge> backEdges = {
        { {0,0,0},         {len, backThick, 1.0f}, 0, 0 },    // Jos (lipit de sezut)
        { {len,0,0},       {backH, backThick, 1.0f}, -90, 0 }, // Dreapta (privit din spate)
        { {len,0,backH},   {len, backThick, 1.0f}, -180, 0 }, // Sus (varful spatarului)
        { {0,0,backH},     {backH, backThick, 1.0f}, -270, 0 } // Stanga
    };

    for (const auto& e : backEdges) {
        glm::mat4 m = backMatrix * glm::translate(glm::mat4(1.0f), e.offset);
        if (e.rotY != 0) m = glm::rotate(m, glm::radians(e.rotY), glm::vec3(0, 1, 0));
        m = glm::scale(m, e.scale);
        RenderMeshInstanced(mesh, shader, m, (int)instante);
    }

    // 7. Cleanup
    glEnable(GL_CULL_FACE);
    instante = save_inst;
    puncte_generate = save_pts;
}
void Tema1_Oldie::DrawPistol(Shader* shader, glm::mat4 incomingMatrix)
{
    if (!showPistol) return;

    Mesh* mesh = meshes["suprafata"];

    // Setam textura
    glActiveTexture(GL_TEXTURE0);
    if (!active_texture) {
        TextureManager::GetTexture("textured_gray.png")->BindToTextureUnit(GL_TEXTURE0);
    }
    else
    {
        TextureManager::GetTexture("black_tv.png")->BindToTextureUnit(GL_TEXTURE0);
    }

    // 1. SALVARE STaRI ORIGINALE
    float save_instante = instante;
    float save_puncte_generate = puncte_generate;
    float save_max_rotate = max_rotate;
    int save_suprafata_type = suprafata_type;
    glm::vec2 save_uvScale = uvScale;
    glm::vec3 save_p0 = control_p0; glm::vec3 save_p1 = control_p1;
    glm::vec3 save_p2 = control_p2; glm::vec3 save_p3 = control_p3;

    // Dezactivam Culling pentru a vedea interiorul daca e cazul, desi la cilindri nu ar trebui sa fie probleme
    glDisable(GL_CULL_FACE);

    // =========================================================
    // CONFIGURARE COMUNa PENTRU CILINDRI (Suprafete de Rotatie)
    // =========================================================
    suprafata_type = 0;              // ROTATION (Cilindru)
    max_rotate = glm::radians(427.5f); // Cerc complet
    instante = 10;          // Detaliu rotund fin
    puncte_generate = 5;    // Minim necesar pentru linie dreapta
    uvScale = glm::vec2(1.0f, 1.0f);

    // Folosim o linie dreapta ca profil -> Rotatia ei creeaza un cilindru
    // Raza cilindrului este data de coordonata X a punctelor (0.15f aici)
    float radius = 0.15f;
    control_p0 = glm::vec3(radius, 0.0f, 0.0f);
    control_p1 = glm::vec3(radius, 1.0f, 0.0f);
    control_p2 = glm::vec3(radius, 1.0f, 0.0f);
    control_p3 = glm::vec3(radius, 1.0f, 0.0f); //inaltime locala 1.0 pe Y

    // Trimitem configuratia la shader O SINGURa DATa
    glUniform3f(glGetUniformLocation(shader->program, "control_p0"), control_p0.x, control_p0.y, control_p0.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
    glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);

    glUniform1i(glGetUniformLocation(shader->program, "SURFACE_TYPE"), suprafata_type);
    glUniform1i(glGetUniformLocation(shader->program, "instante"), (int)instante);
    glUniform1i(glGetUniformLocation(shader->program, "puncte_generate"), (int)puncte_generate);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);
    glUniform2f(glGetUniformLocation(shader->program, "uvScale"), uvScale.x, uvScale.y);


    // =========================================================
    // PIESA 1: tEAVA (BARREL) - Cilindru Orizontal
    // =========================================================
    {
        glm::mat4 model = incomingMatrix;

        // Pozitionare: Sus
        model = glm::translate(model, glm::vec3(0, 1.3f, -2.5f));

        // Rotatie: 90 grade pe X ca sa stea culcat (orizontal)
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));

        // Scalare: Lungime (Y local devine Z global) si Grosime
        // Y=2.5inseamna lungime 2.5 unitati
        model = glm::scale(model, glm::vec3(1.0f, 2.5f, 1.0f));

        RenderMeshInstanced(mesh, shader, model, (int)instante);
    }


    // =========================================================
    // PIESA 2: MÂNERUL (GRIP) - Cilindru Verticalinclinat
    // =========================================================
    {
        glm::mat4 model = incomingMatrix;

        // Pozitionare: Jos siin spate
        model = glm::translate(model, glm::vec3(0.05, 0.0f, 0.2f));

        // Rotatie:inclinat la 15 grade pe spate
        model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(1, 0, 0));

        // Scalare: Mai gros (1.2x) si lungime medie (1.5x)
        // Turtit putin pe X (0.8) ca sa fie oval, ergonomic
        model = glm::scale(model, glm::vec3(0.8f, 1.5f, 1.2f));

        RenderMeshInstanced(mesh, shader, model, (int)instante);
    }


    // 3. RESTAURARE STaRI
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    instante = save_instante;
    puncte_generate = save_puncte_generate;
    max_rotate = save_max_rotate;
    suprafata_type = save_suprafata_type;
    uvScale = save_uvScale;
    control_p0 = save_p0; control_p1 = save_p1;
    control_p2 = save_p2; control_p3 = save_p3;
}

void Tema1_Oldie::DrawBaseballBat(Shader* shader, glm::mat4 incomingMatrix)
{
    Mesh* mesh = meshes["suprafata"];
    if (!showBaseBallBat) return;

    // 1. TEXTURA (Ramâne la fel)
    glActiveTexture(GL_TEXTURE0);
    if (!active_texture) {
        TextureManager::GetTexture("BaseballBat.png")->BindToTextureUnit(GL_TEXTURE0);
    }
    else
    {
        TextureManager::GetTexture("BaseballBat2.png")->BindToTextureUnit(GL_TEXTURE0);
    }

    // 2. SALVARE STaRI (Ramâne la fel)
    float save_instante = instante;
    float save_puncte_generate = puncte_generate;
    float save_max_rotate = max_rotate;

    if (render_to_cubemap == 1) {
        instante = 15.0f;
        puncte_generate = 8.0f;
    }
    else {
        instante = 40.0f;
        puncte_generate = 20.0f;
    }

    // 3. DEFINIRE FORMa (Ramâne la fel)
    suprafata_type = 0;
    uvScale = glm::vec2(1.0f, 1.0f);
    control_p0 = glm::vec3(0.045f, 0.0f, 0.0f);
    control_p1 = glm::vec3(0.05f, 1.5f, 0.0f);
    control_p2 = glm::vec3(0.14f, 2.5f, 0.0f);
    control_p3 = glm::vec3(0.14f, 3.5f, 0.0f);

    std::string currentShaderName = shader->GetName();
    if (currentShaderName == "Shadow") {
        max_rotate = glm::radians(360.0f);
    }
    else {
        max_rotate = glm::radians(880.0f);
    }

    // 4. TRANSFORMARI (AICI E MODIFICAREA CRITICa)
    float batHeight = 5.0f;

    // Nu mai cream matricea de la zero (Identity).
    // O folosim pe cea primita din Update, care are rotatia corecta a camerei si swing-ul.
    glm::mat4 finalModelMatrix = incomingMatrix;

    // Aplicam doar scalarea specifica geometriei bâtei (alungirea)
    // IMPORTANT: Scalarea se face pe axa Y locala a bâtei
    finalModelMatrix = glm::scale(finalModelMatrix, glm::vec3(1.0f, batHeight, 1.0f));

    // 5. RANDARE
    glDisable(GL_CULL_FACE);

    // Trimitem matricea finala combinata
    RenderMeshInstanced(mesh, shader, finalModelMatrix, instante);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // 6. RESTAURARE (Ramâne la fel)
    max_rotate = glm::radians(360.0f);
    uvScale = glm::vec2(1.0f);
    instante = save_instante;
    puncte_generate = save_puncte_generate;
}


void Tema1_Oldie::DrawPringles(Shader* shader, glm::vec3 pos, const std::string& textureName)
{
    float x, y, z;
    x = pos.x;
    y = pos.y;
    z = pos.z;

    glActiveTexture(GL_TEXTURE0);
    TextureManager::GetTexture(textureName.c_str())->BindToTextureUnit(GL_TEXTURE0);

    // SAVE original control points
    glm::vec3 original_p0 = control_p0;
    glm::vec3 original_p1 = control_p1;
    glm::vec3 original_p2 = control_p2;
    glm::vec3 original_p3 = control_p3;
    float original_max_rotate = max_rotate;

    float save_instante = instante;
    float save_puncte_generate = puncte_generate;


    if (render_to_cubemap == 1) {
        instante = 17.0f;
        puncte_generate = 8.0f;
    }
    else {
        instante = 25.0f;
        puncte_generate = 15.0f;
    }



    suprafata_type = 0;
    uvScale = glm::vec2(1.0f, 1.0f);

    control_p0 = glm::vec3(0.7f, 0.0f, 0.0f);
    control_p1 = glm::vec3(0.7f, 1.0f, 0.0f);
    control_p2 = glm::vec3(0.7f, 4.5f, 0.0f);
    control_p3 = glm::vec3(0.7f, 6.0f, 0.0f);


    std::string currentShaderName = shader->GetName();
    //cout << max_rotate << "\n";
    if (currentShaderName == "Shadow") {
        max_rotate = glm::radians(1080.0f);
    }
    else {
        max_rotate = glm::radians(540.0f);
    }
    if (draw_pringles_on_screen == true)
    {
        max_rotate = glm::radians(360.0f);
    }

    float pringlesHeight = 4.5f;
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));

    // 2. Verificam daca este "pringles2" si aplicam rotatia
    if (textureName == "pringles.png" || textureName.find("cola_can") != std::string::npos)
    {
        modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0, 1, 0));
    }
    // GRUPUL 2: Pringles 2 si Redbull (180 grade)
    // Folosim 'else if' ca sa nu se cumuleze cu rotatia de sus
    else if (textureName.find("pringles2") != std::string::npos ||
        textureName.find("redbull") != std::string::npos)
    {
        modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));
    }
    // GRUPUL 3: Restul (180 grade sau alt unghi)
    else if (textureName.find("pringles3") != std::string::npos ||
        textureName.find("fanta") != std::string::npos ||
        textureName.find("pringles4") != std::string::npos ||
        textureName.find("mountain_dew") != std::string::npos)
    {
        // Aici poti pune 90 sau 180, depinde cum vrei
        modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    }

    // 3. Aplicam Scalarea la final
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, pringlesHeight, 1.0f));

    Mesh* mesh = meshes["suprafata"];
    glDisable(GL_CULL_FACE);
    RenderMeshInstanced(mesh, shader, modelMatrix, instante);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Asiguram-ne ca e setat pe BACK
    max_rotate = glm::radians(360.0f);
    uvScale = glm::vec2(1.0f);

    instante = save_instante;
    puncte_generate = save_puncte_generate;
}

void Tema1_Oldie::DrawVase(Shader* shader, glm::vec3 pos)
{
    Mesh* mesh = meshes["suprafata"];

    // 1. Configurare Textura (Ternary operator pentru concizie)
    glActiveTexture(GL_TEXTURE0);
    const char* texName = active_texture ? "vase2.png" : "porcelain_vase.png";
    TextureManager::GetTexture(texName)->BindToTextureUnit(GL_TEXTURE0);

    // 2. Salvare Stare Curenta (Backup)
    float save_instante = instante;
    float save_puncte = puncte_generate;

    // 3. Configurare Parametri Randare (Logic driven)
    if (render_to_cubemap == 1) {
        instante = 17.0f;
        puncte_generate = 8.0f;
    }
    else {
        instante = 25.0f;
        puncte_generate = show_many_points ? 20.0f : 15.0f;
    }

    suprafata_type = 0; // ROTATION
    uvScale = glm::vec2(3.0f, 3.0f);

    // Resetam vectorul global
    global_control_points.clear();

    // 4. Configurare Geometrie (High Poly vs Low Poly)
    if (show_many_points)
    {
        //cout << "Trece\n";
        // Folosim o lista de initializare pentru a evita 20 de apeluri push_back individuale
        // Aceasta este "forma" vazei definita compact
        static const std::vector<glm::vec3> vaseProfile = {
            {0.0f, 0.0f, 0.0f}, {0.3f, 0.0f, 0.0f}, {0.35f, 0.1f, 0.0f}, {0.4f, 0.3f, 0.0f},
            {0.5f, 0.6f, 0.0f}, {0.65f, 1.0f, 0.0f}, {0.8f, 1.5f, 0.0f}, {0.9f, 2.0f, 0.0f},
            {0.92f, 2.5f, 0.0f}, {0.85f, 3.0f, 0.0f}, {0.7f, 3.5f, 0.0f}, {0.5f, 4.0f, 0.0f},
            {0.35f, 4.3f, 0.0f}, {0.25f, 4.5f, 0.0f}, {0.22f, 4.8f, 0.0f}, {0.25f, 5.1f, 0.0f},
            {0.35f, 5.4f, 0.0f}, {0.45f, 5.5f, 0.0f}, {0.5f, 5.55f, 0.0f}, {0.4f, 5.6f, 0.0f}
        };

        // Copiem datelein vectorul global al clasei
        global_control_points = vaseProfile;
        global_num_points = 20;
    }
    else
    {
        // Configurare Low-Poly (Setam punctele Bezier clasice)
        control_p0 = glm::vec3(0.4f, 0.0f, 0.0f);
        control_p1 = glm::vec3(0.85f, 1.2f, 0.0f);
        control_p2 = glm::vec3(1.8f, 4.5f, 0.0f);
        control_p3 = glm::vec3(0.8f, 6.0f, 0.0f);
        global_num_points = 0;
    }

    // 5. Configurare Rotatie (Pastrând logica specifica din tema ta)
    std::string sName = shader->GetName();
    if (draw_vase_on_screen) {
        max_rotate = glm::radians(360.0f);
    }
    else if (sName == "Shadow") {
        max_rotate = glm::radians(1080.0f); // Hack-ul pentru umbre dense
    }
    else {
        max_rotate = glm::radians(540.0f);  // Default din tema
    }
    //cout << sName <<" " << max_rotate << "\n";

    // 6. Matrice Model & Randare
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), pos);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 4.5f, 1.0f));

    // Culling off pentru a vedea interiorul
    glDisable(GL_CULL_FACE);

    // Apelam functia existenta din framework-ul tau
    RenderMeshInstanced(mesh, shader, modelMatrix, (int)instante);

    // 7. Restaurare Stare (Cleanup)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    global_num_points = 0;
    uvScale = glm::vec2(1.0f, 1.0f);
    instante = save_instante;
    puncte_generate = save_puncte;
    max_rotate = glm::radians(360.0f); // Resetare la default safe
}


void Tema1_Oldie::DrawRoom(Shader* shader)
{
    // 1. Configurare Resurse & Stare (Setup)
    Mesh* mesh = meshes["suprafata"];

    // Backup la starea curenta
    const float saved_instante = instante;
    const float saved_puncte = puncte_generate;

    // Setari specifice camerei (2x2 grid simplu)
    instante = 2.0f;
    puncte_generate = 2.0f;
    suprafata_type = 2; // PLANE

    // Configurare Puncte de Control (Profil plan)
    control_p0 = glm::vec3(-0.5f, 0.0f, 0.0f);
    control_p1 = glm::vec3(-0.5f, 2.0f, 0.0f);
    control_p2 = glm::vec3(-0.5f, 4.0f, 0.0f);
    control_p3 = glm::vec3(-0.5f, 6.0f, 0.0f);

    // Configurare Textura (One-liner)
    glActiveTexture(GL_TEXTURE0);
    TextureManager::GetTexture(active_texture ? "brick.png" : "grey.jpg")->BindToTextureUnit(GL_TEXTURE0);

    // 2. Definitia Datelor Geometrice
    // Structura pentru a stoca transformarile fiecarei fete a camerei
    struct RoomFace {
        glm::vec3 offset;                   // Pozitia relativa la ROOM_CENTER
        glm::vec3 scale;                    // Scalarea (Latime, Grosime, Lungime/Înaltime)
        std::vector<std::pair<float, glm::vec3>> rotations; // Lista de rotatii (unghi, axa)
    };

    // Vectorul cu toate cele 6 fete (Podea, Tavan, 4 Pereti)
    // Nota: Datele sunt extrase exact din codul tau original
    const std::vector<RoomFace> faces = {
        // Floor
        {
            glm::vec3(-ROOM_SIZE / 2.0f, 0, -ROOM_SIZE / 2.0f),
            glm::vec3(ROOM_SIZE, 1.0f, ROOM_SIZE),
            {} // Nicio rotatie
        },
        // Ceiling
        {
            glm::vec3(-ROOM_SIZE / 2.0f, ROOM_HEIGHT, ROOM_SIZE / 2.0f),
            glm::vec3(ROOM_SIZE, 1.0f, ROOM_SIZE),
            { {180.0f, glm::vec3(1, 0, 0)} }
        },
        // Wall 1 (Back)
        {
            glm::vec3(-ROOM_SIZE / 2.0f, ROOM_HEIGHT, -ROOM_SIZE / 2.0f),
            glm::vec3(ROOM_SIZE, 1.0f, ROOM_HEIGHT),
            { {90.0f, glm::vec3(1, 0, 0)} }
        },
        // Wall 2 (Front)
        {
            glm::vec3(-ROOM_SIZE / 2.0f, 0.0f, ROOM_SIZE / 2.0f),
            glm::vec3(ROOM_SIZE, 1.0f, ROOM_HEIGHT),
            { {-90.0f, glm::vec3(1, 0, 0)} }
        },
        // Wall 3 (Right)
        {
            glm::vec3(ROOM_SIZE / 2.0f, 0.0f, ROOM_SIZE / 2.0f),
            glm::vec3(ROOM_SIZE, 1.0f, ROOM_HEIGHT),
            { {-90.0f, glm::vec3(1, 0, 0)}, {-270.0f, glm::vec3(0, 0, 1)} }
        },
        // Wall 4 (Left)
        {
            glm::vec3(-ROOM_SIZE / 2.0f, 0.0f, -ROOM_SIZE / 2.0f),
            glm::vec3(ROOM_SIZE, 1.0f, ROOM_HEIGHT),
            { {-90.0f, glm::vec3(1, 0, 0)}, {-90.0f, glm::vec3(0, 0, 1)} }
        }
    };

    // 3. Bucla de Randare (Render Loop)
    // Folosim un lambda pentru a nu repeta codul de transformare
    auto RenderFace = [&](const RoomFace& face) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), ROOM_CENTER + face.offset);

        // Aplicam toate rotatiile necesarein ordine
        for (const auto& rot : face.rotations) {
            model = glm::rotate(model, glm::radians(rot.first), rot.second);
        }

        model = glm::scale(model, face.scale);
        RenderMeshInstanced(mesh, shader, model, (int)instante);
        };

    // Randam tot
    for (const auto& face : faces) {
        RenderFace(face);
    }

    // 4. Restaurare Stare (Cleanup)
    instante = saved_instante;
    puncte_generate = saved_puncte;
}






void Tema1_Oldie::Update(float deltaTimeSeconds)
{
    //cout << deltaTimeSeconds << "\n";
    //cout << LIGHT_DIRECTION << "\n";
    //cout << LAMP_POS << "\n";
    for (auto& obj : gameTargets)
    {
        if (!obj.isAlive || !obj.isVisible) continue; // <--- AICI ESTE SECRETUL DISPARItIEI
        if (obj.name == "Lampa")
        {
            //ii actualizam pozitia din lista cu cea modificata de taste (LAMP_POS)
            obj.pos = LAMP_POS;
        }

        if (obj.name.find("Bere") != std::string::npos) {
            if (active_texture) {
                obj.textureName = "glass.png";
            }
            else
            {
                obj.textureName = "beer_bottle.png";
            }
        }
        if (obj.name.find("Bere 2") != std::string::npos) {
            if (active_texture) {
                obj.textureName = "glass.png";
            }
            else {
                obj.textureName = "beer_bottle.png"; // Sau alta textura
            }
        }

        if (obj.name.find("Vin") != std::string::npos) {
            if (active_texture) {
                obj.textureName = "glass.png";
            }
            else
            {
                obj.textureName = "Wine_bottle.png";
            }
        }


        if (obj.name.find("Pringles 1") != std::string::npos) {
            if (active_texture) {
                obj.textureName = "cola_can.png";
            }
            else
            {
                obj.textureName = "pringles.png";
            }
        }


        if (obj.name.find("Pringles 2") != std::string::npos) {
            if (active_texture) {
                obj.textureName = "redbull.png";
            }
            else
            {
                obj.textureName = "pringles2.png";
            }
        }
        if (obj.name.find("Pringles 3") != std::string::npos) {
            if (active_texture) {
                obj.textureName = "fanta.png";
            }
            else
            {
                obj.textureName = "pringles3.png";
            }
        }

        if (obj.name.find("Pringles 4") != std::string::npos) {
            if (active_texture) {
                obj.textureName = "mountain_dew.png";
            }
            else
            {
                obj.textureName = "pringles4.png";
            }
        }
        if (obj.name.find("Pringles 5") != std::string::npos) {
            if (active_texture) {
                obj.textureName = "cola_can.png"; // Sau ce textura vrei tu
            }
            else {
                obj.textureName = "pringles.png";
            }
        }

        if (obj.name.find("Pringles 6") != std::string::npos) {
            if (active_texture) {
                obj.textureName = "mountain_dew.png"; // Sau alta
            }
            else {
                obj.textureName = "pringles2.png";
            }
        }
    }


    //aici fac partea cu bata si pistolul
    glm::vec3 camPos = GetSceneCamera()->m_transform->GetWorldPosition();
    glm::vec3 camForward = -glm::normalize(GetSceneCamera()->m_transform->GetLocalOZVector());
    glm::vec3 camRight = glm::normalize(GetSceneCamera()->m_transform->GetLocalOXVector());
    glm::vec3 camUp = glm::normalize(GetSceneCamera()->m_transform->GetLocalOYVector());

    glm::vec3 batPos = camPos
        + camForward * 2.5f
        + camRight * 1.2f
        - camUp * 1.0f;

    // =========================================================
    // 2. CALCUL LOGICa SWING (Doar unghiul)
    // =========================================================
    float currentSwingAngle = 0.0f;

    if (isSwinging)
    {
        swingTimer += deltaTimeSeconds;
        float progress = swingTimer / swingDuration;
        if (progress >= 1.0f) {
            isSwinging = false;
            swingTimer = 0.0f;
        }
        else {
            // Folosim SINUS pentru miscare.
            // Vrem -120 grade (convertitin radiani)
            float swingPower = sin(progress * 3.14159f);
            currentSwingAngle = glm::radians(-120.0f * swingPower);
        }
    }

    // =========================================================
    // 3. CONSTRUCtIA MATRICEI (MODEL MATRIX)
    // =========================================================

    // A. Pornim de la matricea identitate
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // B. TRANSLATIA: Mutam bâta la pozitia de lânga camera
    modelMatrix = glm::translate(modelMatrix, batPos);
    float camYaw = atan2(camForward.z, camForward.x);
    modelMatrix = glm::rotate(modelMatrix, -camYaw - glm::radians(90.0f), glm::vec3(0, 1, 0));

    // 2. Pitch (Sus-Jos): Ne aliniem cu privireain sus/jos.
    float camPitch = asin(camForward.y);
    modelMatrix = glm::rotate(modelMatrix, -camPitch, glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, currentSwingAngle, glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(1, 0, 0)); // Tilt fata
    modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(0, 0, 1)); // Tilt lateral
    BASEBALL_MAT = modelMatrix;

    float progress = swingTimer / swingDuration;
    if (progress > 0.01f && progress < 0.99f) {
        CheckSceneCollisions(modelMatrix);
    }
    float recoilBackZ = 0.0f; // Cât se ducein spate
    float recoilUpX = 0.0f; // Cât se roteste vârfulin sus

    if (isShooting)
    {
        recoilTimer += deltaTimeSeconds;
        float progress = recoilTimer / recoilDuration; // recoilDuration e scurt (ex: 0.2s)

        if (progress >= 1.0f) {
            isShooting = false;
            recoilTimer = 0.0f;
        }
        else {
            // Folosim tot SINUS pentru o miscare brusca du-te-vino
            float kick = sin(progress * 3.14159f);

            // 1. RECULiN SPATE (Translatie pe Z)
            //in OpenGL, +Z este "în spate" (spre camera)in spatiul local.
            recoilBackZ = 0.5f * kick; // Se duce 0.5 unitati spre tine

            // 2. RECULiN SUS (Rotatie pe X)
            // Bâta avea -120 (jos). Pistolul are +45 (sus).
            // 120 e prea mult pentru pistol (îti intrain ochi), am pus 45, dar poti pune 120.
            recoilUpX = glm::radians(45.0f * kick);
        }
    }

    // A. Constructia Matricei Pistol (Identica la baza cu cea a bâtei)
    glm::mat4 modelMatrix2 = glm::mat4(1.0f);

    glm::vec3 handPos = camPos
        + camForward * 2.5f
        + camRight * 1.2f
        - camUp * 1.0f;
    // B. Translatia la pozitia mâinii
    modelMatrix2 = glm::translate(modelMatrix2, handPos);

    // C. Rotatia Camerei (Aceeasi ca la bâta, ca sa urmareasca privirea)
    modelMatrix2 = glm::rotate(modelMatrix2, -camYaw - glm::radians(90.0f), glm::vec3(0, 1, 0));
    modelMatrix2 = glm::rotate(modelMatrix2, camPitch, glm::vec3(1, 0, 0));
    modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 0, recoilBackZ));
    modelMatrix2 = glm::rotate(modelMatrix2, recoilUpX, glm::vec3(1, 0, 0));
    GUN_MAT = modelMatrix2;


    // change the intendity of the light
    double time = Engine::GetElapsedTime();
    //in OnKeyPress
    //cout << "MIN: " << LIGHT_MIN_INTENSITY << " MAX: " << LIGHT_MAX_INTENSITY << endl;

    //in Update (chiarinainte de a trimite la shader)
    // cout << "Final Intensity: " << lightIntensity << endl;
    if (flickeringEnabled)
    {
        double currentTime = Engine::GetElapsedTime();

        // 1. Verificam daca e timpul sa pornim un nou fulger
        if (!isLightningStriking && currentTime > nextLightningTime)
        {
            isLightningStriking = true;
            // Un fulger dureaza putin (între 0.1s si 0.5s)
            float duration = 0.1f + (rand() % 40) / 100.0f;
            lightningEndTime = currentTime + duration;
        }

        // 2. Comportamentulin timpul fulgerului vs. timpul de liniste
        if (isLightningStriking)
        {
            if (currentTime < lightningEndTime)
            {
                // SUNTEMiN TIMPUL FULGERULUI
                // Facem stroboscop: Uneori eintuneric bezna, alteori e super luminos
                int strobe = rand() % 100;

                if (strobe < 50) {
                    // 30% sanse sa fieintuneric scurt (între flash-uri)
                    lightIntensity = 0.1f;
                }
                else {
                    // 70% sanse sa fie FLASH PUTERNIC
                    // Dam o valoare mult mai mare decât MAX-ul normal (ex: 5.0f - 15.0f)
                    // Asta va face scena sa para "arsa" de lumina pentru o milisecunda
                    lightIntensity = 5.0f + (rand() % 500) / 100.0f;
                }
            }
            else
            {
                // S-A TERMINAT FULGERUL
                isLightningStriking = false;

                // Programam urmatorul fulger peste 1.0 - 4.0 secunde
                nextLightningTime = currentTime + 1.0f + (rand() % 300) / 100.0f;

                // Revenim la lumina normala (sau stinsa, cum preferi)
                // Aici o las la un nivel ambiental mic, ca si cum ar fi noapte
                lightIntensity = LIGHT_MAX_INTENSITY / 2.0f;
            }
        }
        else
        {
            // LINIsTE (între fulgere)
            // Poti lasa o lumina ambientala mica sau sa o faci sa varieze foarte putin
            lightIntensity = LIGHT_MAX_INTENSITY / 2.0f;
        }
    }
    else
    {
        // Daca e oprit efectul din tasta F, lumina e stabila
        lightIntensity = LIGHT_MAX_INTENSITY;
    }
    //lightIntensity = 0.0f;

    //ClearScreen(glm::vec3(0.12121, 0.168, 0.372));

    if (line_or_triangle == true) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    RenderFireworksToTexture(deltaTimeSeconds);
    float shadowFOV = 120.0f;  // Wide FOV for shadow map
    glm::mat4 lightProjection = glm::perspective(glm::radians(shadowFOV), 1.0f, 0.5f, 100.0f);

    // Choose an up vector that's NOT parallel to light direction
    glm::vec3 lightDir = glm::normalize(LIGHT_DIRECTION);
    glm::vec3 lightUp = glm::vec3(0, 1, 0);

    glm::mat4 lightView = glm::lookAt(
        LIGHT_POSITION,
        LIGHT_POSITION + LIGHT_DIRECTION,
        lightUp
    );

    lightSpaceMatrix = lightProjection * lightView;

    // Debug print (remove later)


    // Render shadow map
    RenderShadowMap();

    // IMPORTANT: Reset state after shadow pass
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
    //glCullFace(GL_BACK);

    glDisable(GL_CULL_FACE);


    auto camera = GetSceneCamera();


    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
    Shader* shader = shaders["suprafataGeneration"];

    cubemapUpdateCounter++;
    // Index static pentru a tine minte al cui rând este sa fie actualizat
    // 0 = TV, 1 = Sticla 1, 2 = Sticla 2, 3 = Sticla 3
    static int currentCubemapIndex = 0;
    if (cubemapUpdateCounter >= cubemapUpdateFrequency || cubemapNeedsUpdate)
    {
        cubemapUpdateCounter = 0;
        cubemapNeedsUpdate = false;

        Shader* shader = shaders["suprafataGeneration"];
        shader->Use();

        // 1. Configurare Comuna (Proiectie si flag)
        render_to_cubemap = 1;
        glm::mat4 cubemapProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "Projection"), 1, GL_FALSE, glm::value_ptr(cubemapProjection));

        // 2. Pregatirea Variabilelor (Pointeri catre datele specifice obiectului curent)
        GLuint targetFBO = 0;
        GLuint targetTexture = 0;
        GLuint targetDepth = 0;
        glm::mat4* targetViewMatrices = nullptr;
        std::string excludeName = "";
        int currentResolution = 256;

        // 3. Switch pentru a selecta DOAR obiectul curent
        switch (currentCubemapIndex)
        {
        case 0: // --- TV (Reflexia principala) ---
            UpdateCubemapViewMatrices();
            targetViewMatrices = cubemapViewMatrices;
            targetFBO = cubemapFBO;
            targetTexture = cubemapTexture;
            targetDepth = cubemapDepth;
            excludeName = ""; // Nu excludem nimic la TV
            currentResolution = cubemapSize; // De obicei 512 sau 1024
            break;

        case 1: // --- Sticla 1 (Bere) ---
            UpdateBottleCubemapViewMatrices(Bottle_pos + glm::vec3(0, 0.2f, 0), bottleCubemapViewMatrices1);
            targetViewMatrices = bottleCubemapViewMatrices1;
            targetFBO = bottleCubemapFBO1;
            targetTexture = bottleCubemapTexture1;
            targetDepth = bottleCubemapDepth1;
            excludeName = "Bere"; // Nu desenam sticlain propria reflexie
            currentResolution = 256;
            break;

        case 2: // --- Sticla 2 (Vin) ---
            UpdateBottleCubemapViewMatrices(Bottle_pos2 + glm::vec3(0, 0.2f, 0), bottleCubemapViewMatrices2);
            targetViewMatrices = bottleCubemapViewMatrices2;
            targetFBO = bottleCubemapFBO2;
            targetTexture = bottleCubemapTexture2;
            targetDepth = bottleCubemapDepth2;
            excludeName = "Vin";
            currentResolution = 256;
            break;

        case 3: // --- Sticla 3 (Bere 2) ---
            UpdateBottleCubemapViewMatrices(Bottle_pos3 + glm::vec3(0, 0.2f, 0), bottleCubemapViewMatrices3);
            targetViewMatrices = bottleCubemapViewMatrices3;
            targetFBO = bottleCubemapFBO3;
            targetTexture = bottleCubemapTexture3;
            targetDepth = bottleCubemapDepth3;
            excludeName = "Bere 2";
            currentResolution = 256;
            break;
        }

        // 4. Executarea Randarii (Generica pentru obiectul selectat)
        if (targetFBO != 0)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, targetFBO);
            glViewport(0, 0, currentResolution, currentResolution);

            // Trimitem matricile de view la shader
            GLint baseLoc = glGetUniformLocation(shader->program, "viewMatrices[0]");
            for (int i = 0; i < 6; i++) {
                glUniformMatrix4fv(baseLoc + i, 1, GL_FALSE, glm::value_ptr(targetViewMatrices[i]));
            }

            // Backup setari calitate
            int save_instante = instante;
            int save_pts = puncte_generate;

            // Optimizare: Calitate mai mica pentru reflexii (mai ales la sticle)
            if (currentResolution <= 256) {
                puncte_generate = 6;
                instante = 6;
            }
            else {
                puncte_generate = 8;
                instante = 8;
            }

            // Randam cele 6 fete
            for (int face = 0; face < 6; face++)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, targetTexture, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, targetDepth, 0);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glUniform1i(glGetUniformLocation(shader->program, "cubemap_face"), face);

                // Setup specific pentru DrawSceneObjects
                draw_vase_on_screen = true;
                draw_pringles_on_screen = true;

                // AICI DESENaM SCENA
                DrawSceneObjects(shader, true, excludeName);

                draw_vase_on_screen = false;
                draw_pringles_on_screen = false;
            }

            // Generare mipmaps pentru textura proaspat actualizata
            glBindTexture(GL_TEXTURE_CUBE_MAP, targetTexture);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

            // Restaurare calitate
            instante = save_instante;
            puncte_generate = save_pts;
        }

        // 5. Resetare si pregatire pentru urmatorul ciclu
        render_to_cubemap = 0;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Trecem la urmatorul obiect pentru data viitoare când intrain if (peste 7 frame-uri)
        currentCubemapIndex = (currentCubemapIndex + 1) % 4;
    }




    // ============================================
    // PASS 2: RENDER TO SCREEN (normal rendering)
    // ============================================

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, resolution.x, resolution.y);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    shader->Use();

    // Set normal view/projection matrices
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "View"),
        1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Projection"),
        1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));


    // Render everything
    glDepthMask(GL_TRUE);    // Asigura-te ca poti scriein Z-buffer
    glEnable(GL_DEPTH_TEST); // Activeaza testul de adâncime
    glDepthFunc(GL_LESS);

    // --- AICI E FIX-UL PENTRU INTERIOR ---
    // Dezactivam Culling-ul ca sa vedem si interiorul obiectelor (peretii interiori)
    glDisable(GL_CULL_FACE);
    DrawSceneObjects(shader, true);
    // Render screen with cubemap bound for reflections
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(glGetUniformLocation(shader->program, "texture_cubemap"), 2);
    DrawScreen(shader, TV_POS, tvrotationangle);
    glEnable(GL_CULL_FACE);



    // ============================================
    // PARTICULE sI ANIMAtII
    // ============================================
    //fireworkTimer += deltaTimeSeconds;
    glm::vec3 generator_position3;
    float radius = 2.0f;
    generator_position3.x = generator_position2.x + radius * cos(-tvrotationangle);
    generator_position3.y = generator_position2.y;
    generator_position3.z = generator_position2.z + radius * sin(-tvrotationangle);
    glm::vec3 finalSourcePos = generator_position3 + currentPosition;
    DrawFireworks(100.f, -26.f, -12.1f, deltaTimeSeconds);
    DrawCrosshair();
}


void Tema1_Oldie::FrameEnd()
{
}

void Tema1_Oldie::OnInputUpdate(float deltaTime, int mods)
{
    float tv_speed = 32.0f;
    if (window->KeyHold(GLFW_KEY_1))
    {
        tvrotationangle += deltaTime * tv_speed;
    }
    if (window->KeyHold(GLFW_KEY_2))
    {
        tvrotationangle -= deltaTime * tv_speed;
    }


    float lampSpeed = 5.0f; // Viteza de miscare
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        // Miscare pe planul orizontal (X si Z)
        if (window->KeyHold(GLFW_KEY_W)) {
            LAMP_POS.z -= lampSpeed * deltaTime; //inainte
            LIGHT_POSITION.z -= lampSpeed * deltaTime;

        }
        if (window->KeyHold(GLFW_KEY_S)) {
            LAMP_POS.z += lampSpeed * deltaTime; //inapoi
            LIGHT_POSITION.z += lampSpeed * deltaTime;
        }
        if (window->KeyHold(GLFW_KEY_A)) {
            LAMP_POS.x -= lampSpeed * deltaTime; // Stânga
            LIGHT_POSITION.x -= lampSpeed * deltaTime;
        }
        if (window->KeyHold(GLFW_KEY_D)) {
            LAMP_POS.x += lampSpeed * deltaTime; // Dreapta
            LIGHT_POSITION.x += lampSpeed * deltaTime;
        }
        if (window->KeyHold(GLFW_KEY_E)) {
            LAMP_POS.y += lampSpeed * deltaTime; // Sus
            LIGHT_POSITION.y += lampSpeed * deltaTime;
        }
        if (window->KeyHold(GLFW_KEY_Q)) {
            LAMP_POS.y -= lampSpeed * deltaTime; // Jos
            LIGHT_POSITION.y -= lampSpeed * deltaTime;
        }
        float rotSpeed = 2.0f * deltaTime;
        if (window->KeyHold(GLFW_KEY_U)) {
            LIGHT_DIRECTION = glm::vec3(glm::rotate(glm::mat4(1.0f), rotSpeed, glm::vec3(0, 1, 0)) * glm::vec4(LIGHT_DIRECTION, 0.0f));
        }
        if (window->KeyHold(GLFW_KEY_O)) {
            LIGHT_DIRECTION = glm::vec3(glm::rotate(glm::mat4(1.0f), -rotSpeed, glm::vec3(0, 1, 0)) * glm::vec4(LIGHT_DIRECTION, 0.0f));
        }
        if (window->KeyHold(GLFW_KEY_I)) {
            LIGHT_DIRECTION = glm::vec3(glm::rotate(glm::mat4(1.0f), rotSpeed, glm::vec3(1, 0, 0)) * glm::vec4(LIGHT_DIRECTION, 0.0f));
        }
        if (window->KeyHold(GLFW_KEY_K)) {
            LIGHT_DIRECTION = glm::vec3(glm::rotate(glm::mat4(1.0f), -rotSpeed, glm::vec3(1, 0, 0)) * glm::vec4(LIGHT_DIRECTION, 0.0f));
        }
        if (window->KeyHold(GLFW_KEY_J)) {
            LIGHT_DIRECTION = glm::vec3(glm::rotate(glm::mat4(1.0f), rotSpeed, glm::vec3(0, 0, 1)) * glm::vec4(LIGHT_DIRECTION, 0.0f));
        }
        if (window->KeyHold(GLFW_KEY_L)) {
            LIGHT_DIRECTION = glm::vec3(glm::rotate(glm::mat4(1.0f), -rotSpeed, glm::vec3(0, 0, 1)) * glm::vec4(LIGHT_DIRECTION, 0.0f));
        }
        LIGHT_DIRECTION = glm::normalize(LIGHT_DIRECTION);
    }
}


void Tema1_Oldie::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_F)
    {
        flickeringEnabled = !flickeringEnabled;
        if (flickeringEnabled) {
            cout << "[FLICKERING] ON" << endl;
        }
        else {
            cout << "[FLICKERING] OFF" << endl;
        }

    }
    if (key == GLFW_KEY_P)
    {
        show_many_points = !show_many_points;
    }
    if (key == GLFW_KEY_Z)
    {
        if (!destroyedHistory.empty())
        {
            // Luam ultimul index adaugat
            int lastDestroyedIndex = destroyedHistory.back();

            //il scoatem din lista
            destroyedHistory.pop_back();

            // Reînviem obiectul
            if (lastDestroyedIndex >= 0 && lastDestroyedIndex < gameTargets.size()) {
                gameTargets[lastDestroyedIndex].isAlive = true;
                std::cout << "[UNDO] Obiectul " << gameTargets[lastDestroyedIndex].name << " a reaparut." << std::endl;
            }
        }
        else {
            std::cout << "[UNDO] Nimic de refacut." << std::endl;
        }
    }

    // --- RESET SCENE (Tasta T) ---
    if (key == GLFW_KEY_T)
    {
        ResetScene();
    }

    if (key == GLFW_KEY_N)
    {
        cout << "Click stanga\n"; // Decomenteaza pentru debug
        if (!isSwinging && showBaseBallBat == true) {
            isSwinging = true;
            swingTimer = 0.0f;
        }
        if (!isShooting && showPistol == true)
        {
            isShooting = true;
            recoilTimer = 0.0f;
            CheckGunShot();
        }
    }

    if (key == GLFW_KEY_3) //vad structura de pornire
    {
        line_or_triangle = !line_or_triangle;
    }
    if (key == GLFW_KEY_4) //vad scena reala
    {
        view_objects = !view_objects;
        for (auto& obj : gameTargets) {
            obj.isVisible = !obj.isVisible;
        }

    }

    if (key == GLFW_KEY_6) //creste lumina de la veioza
    {
        float step = 0.2f;
        lightIntensity += step;
        LIGHT_MIN_INTENSITY += step / 2.0f;
        LIGHT_MAX_INTENSITY += step * 2.0f;
    }
    if (key == GLFW_KEY_5)
    {
        float step = 0.2f;
        if (LIGHT_MIN_INTENSITY - step >= 0.0f) //scade lumina de la veioza
        {
            lightIntensity -= step;
            LIGHT_MIN_INTENSITY -= step / 2.0f;
            LIGHT_MAX_INTENSITY -= step * 2.0f;
        }
    }
    if (key == GLFW_KEY_7)
    {
        showPistol = false;
        showBaseBallBat = !showBaseBallBat;
    }
    if (key == GLFW_KEY_8) {
        showBaseBallBat = false;
        showPistol = !showPistol;
        isShooting = false; // Reset
    }
    if (key == GLFW_KEY_9)
    {
        active_texture = !active_texture;
    }
    float step = 0.1f; // Cu cât modificam valoarea la fiecare apasare

    // --- ROSU (Red) ---
    if (key == GLFW_KEY_R)
    {
        // Daca tinem apasat SHIFT, scadem. Daca nu, crestem.
        if (mods & GLFW_MOD_SHIFT) {
            light_color.r -= step;
        }
        else {
            light_color.r += step;
        }
    }

    // --- VERDE (Green) ---
    if (key == GLFW_KEY_G)
    {
        if (mods & GLFW_MOD_SHIFT) {
            light_color.g -= step;
        }
        else {
            light_color.g += step;
        }
    }

    // --- ALBASTRU (Blue) ---
    if (key == GLFW_KEY_B)
    {
        if (mods & GLFW_MOD_SHIFT) {
            light_color.b -= step;
        }
        else {
            light_color.b += step;
        }
    }
    light_color = glm::clamp(light_color, glm::vec3(0.0f), glm::vec3(1.0f));
}

void Tema1_Oldie::ResetScene()
{
    active_texture = false;
    showBaseBallBat = false;
    showPistol = false;
    LIGHT_DIRECTION = LIGHT_DIRECTION_FINAL;
    // 1. Reînvie toate tintele
    for (auto& obj : gameTargets) {
        obj.isAlive = true;
        obj.isVisible = true;
        view_objects = true;
        // Optional: Poti reseta si textura daca o schimbai (ex: la Cola)
        // obj.textureName = "pringles.png"; (sau textura originala)
        if (obj.name == "Lampa")
        {
            cout << "O ia pe aici\n";
            //ii actualizam pozitia din lista cu cea modificata de taste (LAMP_POS)
            obj.pos = LAMP_POS_FINAL;
            LAMP_POS = LAMP_POS_FINAL;
        }
    }
    LIGHT_POSITION = LIGHT_POSITION_FINAL;

    // 2. Goleste istoricul de undo
    destroyedHistory.clear();

    // 3. Resetam si particulele/artificiile daca e cazul
    // ResetParticlesFireworks2(...);

    std::cout << "[SCENA RESETATA] Toate obiectele sunt la locul lor." << std::endl;
}

void Tema1_Oldie::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1_Oldie::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1_Oldie::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // CORECT: Verificam daca butonul apasat ESTE egal cu cel stâng
}


void Tema1_Oldie::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1_Oldie::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Tema1_Oldie::OnWindowResize(int width, int height)
{
    // Treat window resize event
}

