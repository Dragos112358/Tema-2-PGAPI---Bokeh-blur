#pragma once

#include "components/simple_scene.h"
#include "core/gpu/particle_effect.h"

namespace m2
{
    enum ObjType { SPHERE, CAPSULE };
    struct BezierCurve {
        glm::vec3 p0;
        glm::vec3 p1;
        glm::vec3 p2;
        glm::vec3 p3;
    };
    struct FBOConfig {
        int width;
        int height;
        bool useDepthTexture;       // True = Depth Texture, False = Depth Renderbuffer
        bool isCubemap;             // True = Cubemap, False = 2D Texture
        GLint minFilter = GL_LINEAR;
        GLint magFilter = GL_LINEAR;
        GLint wrapMode = GL_CLAMP_TO_EDGE; // Default modern
        GLenum internalFormat = GL_RGB;    // Format culoare
        GLenum type = GL_UNSIGNED_BYTE;    // Tip date culoare
    };

    // Funcția generică "Workhorse"
    void CreateFramebufferInternal(GLuint& fbo, GLuint& colorTex, GLuint& depthObj, const FBOConfig& config);

    struct TargetObj {
        std::string name;
        ObjType type;
        glm::vec3 pos;
        float radius;
        float height;       // Doar pentru CAPSULE
        glm::vec3 offset;   // Offset centru
        std::string textureName; // Pentru desenare
        bool isAlive = true;     // <--- AICI E CHEIA (Dacă e false, nu îl desenăm)
        bool isVisible = true;
    };
    class Tema1_Oldie : public gfxc::SimpleScene
    {
    public:
        Tema1_Oldie();
        ~Tema1_Oldie();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMeshInstanced(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int instances, const glm::vec3& color = glm::vec3(1));

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void ResetParticlesFireworks2(int xSize, int ySize, int zSize);

        void Tema1_Oldie::DrawTable(Shader* shader, glm::vec3 pos, float rotationAngle);
        void DrawVase(Shader* shader, glm::vec3 pos);
        void DrawTV(Shader* shader, glm::vec3 pos, float angle);
        void DrawLamp(Shader* shader, glm::vec3 pos, float rotationAngle);
        void DrawScreen(Shader* shader, glm::vec3 pos, float angle);
        void DrawRoom(Shader* shader);
        void CreateFramebufferInternal(GLuint& fbo, GLuint& colorTex, GLuint& depthObj, const FBOConfig& cfg);

        void CreateCubemapFramebuffer();
        void UpdateCubemapViewMatrices();

        //void CreateFramebuffer(int width, int height);
        unsigned UploadCubeMapTexture(const std::string& pos_x, const std::string& pos_y, const std::string& pos_z,
            const std::string& neg_x, const std::string& neg_y, const std::string& neg_z);


        void RenderSceneForCubemap(Shader* shader);

        void CreateShadowFramebuffer();
        void RenderShadowMap();

        void RenderSimpleMeshesForShadow(Shader* shader);
        void RenderProceduralMeshesForShadow(Shader* shader);
        void DrawFireworks(float x, float y, float z, float deltaTimeSeconds);
        void CreateTVFramebuffer();
        void RenderFireworksToTexture(float deltaTime); // Funcție nouă dedicată
        void DrawBottle(Shader* shader, glm::vec3 pos, float unghi, const std::string& textureName);
        void UpdateBottleCubemapViewMatrices(glm::vec3 bottlePosition, glm::mat4* viewMatrices);
        void CreateBottleCubemapFramebuffer(GLuint& fbo, GLuint& texture, GLuint& depth);
        void RenderBottleCubemap(GLuint fbo, GLuint texture, GLuint depth,
            glm::mat4* viewMatrices, glm::vec3 bottlePos);
        void DrawBaseballBat(Shader* shader, glm::mat4 incomingMatrix);
        void DrawPringles(Shader* shader, glm::vec3 pos, const std::string& textureName);
        void CheckSceneCollisions(const glm::mat4& batModelMatrix);
        void DrawChair(Shader* shader, glm::vec3 pos, float rotationAngle);
        float Tema1_Oldie::DistSegmentSegmentSq(glm::vec3 p1, glm::vec3 q1, glm::vec3 p2, glm::vec3 q2);

        // Helper pentru coliziune Sferă-Sferă (Sticle, Vaze, Pringles)
        bool CheckSphereCollision(glm::vec3 point, float pointRadius, glm::vec3 sphereCenter, float sphereRadius);

        // Helper pentru coliziune Punct-Cutie (Mese, TV)
        bool CheckAABBCollision(glm::vec3 point, glm::vec3 boxCenter, glm::vec3 boxSize);
        // În Tema1_Oldie.h la private:
        float GetDistanceToSegmentSq(glm::vec3 point, glm::vec3 segA, glm::vec3 segB);
        // Funcții noi
        void DrawPistol(Shader* shader, glm::mat4 incomingMatrix);
        void CheckGunShot(); // Verifică ce am lovit cu glonțul
        void DrawCrosshair();
        void ResetScene();
        void RenderProceduralMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix,
            const BezierCurve& curve, int surfaceType,
            int instances, int points, glm::vec2 uvScale, float rotation);
        void DrawSceneObjects(Shader* shader, bool drawTransparentObjects = true, std::string ignoreName = "");

        // Variabilă ca să nu spameze consola la fiecare frame al loviturii



    protected:
        // Info about the generated surfaces
        glm::vec3 control_p0, control_p1, control_p2, control_p3;
        glm::vec2 uvScale;

        unsigned int puncte_generate;
        unsigned int instante;
        float max_translate, max_rotate;
        int suprafata_type = 0;
        glm::vec3 generator_position2;
        GLenum polygonMode2;
        int scene2;
        float offset2;
        float fireworkTimer = 0.0f; // contor de timp
        float fireworkInterval = 4.5f; // spawn la fiecare 3 secunde

        std::vector<glm::vec3> fireworkColors = {
            glm::vec3(1.0f, 0.0f, 0.0f),   // roșu
            glm::vec3(0.0f, 1.0f, 0.0f),   // verde
            glm::vec3(0.0f, 0.0f, 1.0f),   // albastru
            glm::vec3(1.0f, 1.0f, 0.0f),   // galben
            glm::vec3(1.0f, 0.0f, 1.0f),   // magenta
            glm::vec3(0.0f, 1.0f, 1.0f),   // cyan
            glm::vec3(1.0f, 0.5f, 0.0f),   // portocaliu
            glm::vec3(0.5f, 0.0f, 1.0f),   // violet
            glm::vec3(0.8f, 0.8f, 0.8f),   // gri deschis
            glm::vec3(1.0f, 1.0f, 1.0f)    // alb
        };


        glm::vec3 initialPosition = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 currentPosition = glm::vec3(0.f, 0.f, 0.f);
        int current_color = 0;
        //float tvrotationangle = 0.0f;
        bool line_or_triangle = false;
    private:
        int cubeMapTextureID;
        float angle;
        unsigned int framebuffer_object;
        unsigned int color_texture;
        unsigned int depth_texture;
        unsigned int type = 0;
        float angle2;
        glm::vec3 ScreenPos = glm::vec3(6.75f, 9.4f, 3.2f);  //6.75f, 20.0f, 3.2f
        glm::vec3 tvCenter2 = ScreenPos;
        int render_to_cubemap = 0;


        const glm::vec3 Bottle_pos = glm::vec3(2.2f, 6.f, -10.5f);
        const glm::vec3 Bottle_pos2 = glm::vec3(18.95f, 6.f, -10.5f);


        GLuint bottleCubemapFBO3;
        GLuint bottleCubemapTexture3;
        GLuint bottleCubemapDepth3;

        const glm::vec3 VASE1_POS = glm::vec3(3.35f, 6.f, -15.1f);
        const glm::vec3 VASE2_POS = glm::vec3(19.95f, 6.f, -14.6f);

        const glm::vec3 TABLE1_POS = glm::vec3(-1.f, 5.f, -20.f);
        const glm::vec3 TABLE2_POS = glm::vec3(2.f, 5.f, 15.f); //2.f, 5.f, -1,f
        const glm::vec3 TABLE3_POS = glm::vec3(-30.f, 5.f, 15.f);


        glm::vec3 LAMP_POS = glm::vec3(19.8112f, 6.0f, 24.0141f); //22,5.95,32;   17.5, 5.95f, 25.0f
        glm::vec3 LAMP_POS_FINAL = glm::vec3(19.8112f, 6.0f, 24.0141f);

        const glm::vec3 TV_POS = glm::vec3(11.6f, 6.f, -13.3f);


        const glm::vec3 ROOM_CENTER = glm::vec3(10.0f, -2.0f, -5.0f);
        const float ROOM_SIZE = 100.0f;
        const float ROOM_HEIGHT = 30.0f;

        // Light properties (lamp)
        glm::vec3 LIGHT_POSITION = glm::vec3(19.8112f, 11.45f, 24.0141f);
        const glm::vec3 LIGHT_POSITION_FINAL = glm::vec3(19.8112f, 11.45f, 24.0141f);
        glm::vec3 LIGHT_DIRECTION = glm::normalize(glm::vec3(-0.472557f, - 0.483281f, - 0.736973f));
        const glm::vec3 LIGHT_DIRECTION_FINAL = glm::normalize(glm::vec3(-0.472557f, -0.483281f, -0.736973f));
        glm::mat4 lightSpaceMatrix;
        float lightIntensity;
        float lightChangeSign;
        float LIGHT_MAX_INTENSITY = 2.0f;
        float LIGHT_MIN_INTENSITY = 0.0f; //0.3f
        double lightTimer, lightFlickerInterval = 0.9f;
        float spotAngle;

        float tvrotationangle = 0.0f;
        const glm::vec3 PRINGLES_POS = glm::vec3(17.95f, 6.f, -18.8f);
        //const glm::vec3 PRINGLES_POS2 = glm::vec3(-22.95f, 6.f, 10.8f);
        const glm::vec3 PRINGLES_POS2 = glm::vec3(0.95f, 6.f, -4.8f);
        const glm::vec3 PRINGLES_POS3 = glm::vec3(22.5, 6.0f, 20.0f);
        const glm::vec3 PRINGLES_POS4 = glm::vec3(5.75, 6.0f, 20.0f);

        unsigned int shadowFBO;
        unsigned int shadowMapTexture;  // 2D depth texture
        int shadowMapResolution = 8192; //2048

        // TV reflection (cubemap) - you already have these
        unsigned int reflectionFBO;  // or rename to reflectionFBO
        unsigned int reflectionCubemap;       // or rename to reflectionCubemap

        glm::vec3 Bottle_pos3 = TABLE3_POS + glm::vec3(10.0f, 1.0f, -7.5f);
        unsigned int cubemapFBO;
        unsigned int cubemapTexture;
        GLuint cubemapTextureBottle;
        unsigned int cubemapDepth;
        int cubemapSize = 512;

        glm::mat4 cubemapViewMatrices[6];

        int cubemapUpdateCounter = 0;
        int cubemapUpdateFrequency = 3;
        bool cubemapNeedsUpdate = true;


        // Fireworks (if separate)
        unsigned int fireworksFBO;
        unsigned int fireworksTexture;
        glm::vec3 currentPosition2 = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 cubeposition = glm::vec3(0, 0, 0);

        GLuint tvFBO;       // Framebuffer Object pentru TV
        GLuint tvTexture;   // Textura în care desenăm artificiile


        //aici sunt pentru sticle
        GLuint bottleCubemapTexture1;
        GLuint bottleCubemapDepth1;
        GLuint bottleCubemapFBO1;

        GLuint bottleCubemapTexture2;
        GLuint bottleCubemapDepth2;
        GLuint bottleCubemapFBO2;

        glm::mat4 bottleCubemapViewMatrices1[6];
        glm::mat4 bottleCubemapViewMatrices2[6];
        glm::mat4 bottleCubemapViewMatrices3[6];

        //parte de baseballbat
        bool showBaseBallBat = false;
        glm::vec3 BASEBALL_POS = glm::vec3(20.95f, 6.f, -10.5f);
        glm::vec3 BASEBALL_ROT = glm::vec3(0, 0, 0);
        glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::mat4 BASEBALL_MAT = glm::mat4(1);

        bool isSwinging = false;      // Ești în mijlocul atacului?
        float swingTimer = 0.0f;      // Cronometrul animației
        float swingDuration = 0.5f;
        bool hasHitObjectThisSwing = false;

        //pentru obiecte
        std::vector<TargetObj> gameTargets;

        //pentru pistol
        bool showPistol = false;       // Comutator între Bâtă (7) și Pistol (8)
        float recoilTimer = 0.0f;      // Cronometru pentru recul
        float recoilDuration = 0.2f;   // Cât durează reculul rapid
        bool isShooting = false;       // Stare tragere
        glm::mat4 GUN_MAT = glm::mat4(1);
        bool active_texture = false;

        std::vector<int> destroyedHistory;

        std::vector<glm::vec3> global_control_points; // Vectorul de puncte
        int global_num_points;
        bool show_many_points = false;

        bool draw_vase_on_screen = false;
        bool draw_pringles_on_screen = false;
        bool view_objects = true;
        bool flickeringEnabled = true;

        //parte de fulgere
        float lightningEndTime = 0.0f;    // Când se termină rafla curentă de fulger
        float nextLightningTime = 0.0f;   // Când începe următorul fulger
        bool isLightningStriking = false; // Dacă suntem în mijlocul unui fulger acum

    };
}   // namespace m2