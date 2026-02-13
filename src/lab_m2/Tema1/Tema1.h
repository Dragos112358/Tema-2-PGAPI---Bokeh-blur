#pragma once

#include "components/simple_scene.h"
#include "core/gpu/particle_effect.h"

namespace m2
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

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
        void DrawFireworks(float x, float y, float z, float deltaTimeSeconds);
        void DrawRoom(float x, float y, float z);
        void LoadShader(const std::string& name);
        void DrawScene(Shader* shader);
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture);
        void CreateShadowFramebuffer(int width, int height);

    protected:
        // Info about the generated surfaces
        glm::vec3 control_p0, control_p1, control_p2, control_p3;
        unsigned int no_of_generated_points, no_of_instances;
        float max_translate, max_rotate;
        int surface_type = 0;
        glm::vec3 generator_position2;
        GLenum polygonMode2;
        int scene2;
        float offset2;
        float fireworkTimer = 0.0f; // contor de timp
        float fireworkInterval = 4.5f; // spawn la fiecare 3 secunde

        unsigned int shadowFBO;
        unsigned int shadowMapTexture;

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
        glm::vec3 initialPosition = glm::vec3(-4.5f, 0.f, 0.0f);
        glm::vec3 currentPosition = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 currentPosition2 = glm::vec3(0.f, 0.f, 0.f);
        int current_color = 0;
        float tvrotationangle = 0.0f;
        bool view_structure = false;
    private: 
        int cubeMapTextureID;
        float angle;
        unsigned int framebuffer_object;
        unsigned int framebuffer_object2;
        unsigned int color_texture;
        unsigned int color_texture2;
        unsigned int depth_texture; //asta e pentru oglinda
        unsigned int depth_texture2; //asta e pentru umbre
        unsigned int type = 0;
        float angle2;
        glm::vec3 bunnyPos = glm::vec3(3.75f, 6.2f, 0.1f);

        glm::vec3 light_position;
        glm::vec3 light_direction;

        glm::mat4 light_space_view;
        glm::mat4 light_space_projection;
        float light_space_near_plane;
        float light_space_far_plane;
        //3.75f, 6.2f, 0.1f -> celelalte valori pentru screenpos
        
        glm::vec3 ScreenPos = glm::vec3(6.75f, 9.4f, 3.2f);  //6.75f, 20.0f, 3.2f
        glm::vec3 tvCenter2 = ScreenPos;
        int render_to_cubemap = 0;
        glm::vec3 cubeposition = glm::vec3(0, 0, 0);
        GLuint fireworksFBO;
        GLuint fireworksTexture;
        int width = window->GetResolution().x;
        int height = window->GetResolution().y;
        float x_direction, y_direction, z_direction;
        bool on_screen = false;
        bool draw_framebuffer_textures;

    };
}   // namespace m2
