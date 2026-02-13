#pragma once

#include "components/simple_scene.h"
#include "core/gpu/particle_effect.h"

namespace m2
{
    class Tema1_bonus : public gfxc::SimpleScene
    {
    public:
        Tema1_bonus();
        ~Tema1_bonus();

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
        void DrawTable(float x, float y, float z);
        void DrawVase(float x, float y, float z);
        void DrawTV(float x, float y, float z);
        void DrawLamp(float x, float y, float z);
        void CreateFramebuffer(int width, int height);
        unsigned UploadCubeMapTexture(const std::string& pos_x, const std::string& pos_y, const std::string& pos_z,
            const std::string& neg_x, const std::string& neg_y, const std::string& neg_z);
        void DrawScreen(float x, float y, float z);
        void DrawSkybox();

        void CreateShadowFramebuffer();
        void RenderShadowMap();
        void RenderSceneForShadow(Shader* shader);

    protected:
        // Info about the generated surfaces
        glm::vec3 control_p0, control_p1, control_p2, control_p3;
        glm::vec2 uvScale;

        unsigned int no_of_generated_points, no_of_instances;
        float max_translate, max_rotate;
        int surface_type = 0;
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
        float tvrotationangle = 0.0f;
        bool view_structure = false;
    private:
        int cubeMapTextureID;
        float angle;
        unsigned int framebuffer_object;
        unsigned int color_texture;
        unsigned int depth_texture;
        unsigned int type = 0;
        float angle2;
        glm::vec3 bunnyPos = glm::vec3(3.75f, 6.2f, 0.1f);
        //3.75f, 6.2f, 0.1f -> celelalte valori pentru screenpos

        glm::vec3 ScreenPos = glm::vec3(6.75f, 9.4f, 3.2f);  //6.75f, 20.0f, 3.2f
        glm::vec3 tvCenter2 = ScreenPos;
        int render_to_cubemap = 0;





        unsigned int shadowFBO;
        unsigned int shadowMapTexture;  // 2D depth texture
        int shadowMapResolution = 2048;

        // TV reflection (cubemap) - you already have these
        unsigned int reflectionFBO;  // or rename to reflectionFBO
        unsigned int reflectionCubemap;       // or rename to reflectionCubemap

        // Fireworks (if separate)
        unsigned int fireworksFBO;
        unsigned int fireworksTexture;


        // Light properties (lamp)
        glm::vec3 lightPosition;
        glm::vec3 lightDirection;
        glm::mat4 lightSpaceMatrix;
        float lightIntensity = 1.0f;
        float spotAngle = 45.0f;
        //ParticleEffect<Particle>* particleEffect2;

    };
}   // namespace m2
