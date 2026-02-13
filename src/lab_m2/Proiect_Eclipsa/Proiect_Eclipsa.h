#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"


namespace m2
{
    class Proiect_Eclipsa : public gfxc::SimpleScene
    {
    public:
        Proiect_Eclipsa();
        ~Proiect_Eclipsa();

        void Init() override;

    private:
        void CreateFramebuffer(int width, int height);

        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void DrawScene(Shader* shader);
        void DrawFramebufferTextures();

        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1);
        void RenderTextureScreen(Shader* shader, unsigned int textureID);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        Mesh* CreateSphereMesh(const std::string& name, float radius, int sectors, int stacks);

        void LoadShader(const std::string& name);


        float angle;

        glm::vec3 light_position;
        glm::vec3 light_direction;

        glm::mat4 light_space_view;
        glm::mat4 light_space_projection;
        float light_space_near_plane;
        float light_space_far_plane;

        bool draw_framebuffer_textures;

        unsigned int framebuffer_object;
        unsigned int color_texture;
        unsigned int depth_texture;
        float sun_rotation_angle = 0;
        float earth_orbit_angle = 0;
        float earth_rotation_angle = 0;
        float moon_orbit_angle = 0;
        float moon_rotation_angle = 0;

        // Orbital parameters
        float earth_orbit_radius;
        float earth_orbit_speed;
        float moon_orbit_radius;
        float moon_orbit_speed;

        // Light properties for sun
        glm::vec3 sun_position;
    private:
    };
}   // namespace m2