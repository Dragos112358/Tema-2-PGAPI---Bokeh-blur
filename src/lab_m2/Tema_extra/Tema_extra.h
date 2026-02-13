#pragma once

#include <string>
#include <vector>
#include <cmath>

#include "components/simple_scene.h"
#include "components/transform.h"
#include "core/gpu/frame_buffer.h"
#include "core/gpu/particle_effect.h"

namespace m2
{
    struct lightinfo2
    {
        glm::vec3 position;
        glm::vec3 color;
        float radius;
    };


    class Tema_extra : public gfxc::SimpleScene
    {
    public:
        Tema_extra();
        ~Tema_extra();

        void Init() override;
        void CreateFramebuffer(int width, int height);
        void GenerateTerrain(float width, float height, int resolutionX, int resolutionZ, float h_max, float r);
        void AddWaterfall(std::vector<VertexFormat>& vertices, float r_cascade, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3);
        glm::vec3 RotatePointAroundCenter(const glm::vec3& point, const glm::vec3& center, float angle);

        unsigned int Tema_extra::UploadCubeMapTexture(const std::string& pos_x, const std::string& pos_y, const std::string& pos_z, const std::string& neg_x, const std::string& neg_y, const std::string& neg_z);
        int cubeMapTextureID;
        void RenderSkybox();

        void ResetParticlesRain(int xSize, int ySize, int zSize);

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void LoadShader(const std::string& fileName, bool hasGeometry = false);


    private:
        FrameBuffer* frameBuffer;
        FrameBuffer* lightBuffer;
        FrameBuffer* reflectionBuffer;
        std::vector<lightinfo2> lights;
        int outputType;

        glm::vec3 P0, P1, P2, P3;
        glm::vec3 generator_position;
        float offset;
        int cubeMapTextureID2;
        float angle;
        unsigned int framebuffer_object;
        unsigned int color_texture;
        unsigned int depth_texture;
        unsigned int type;
    };
}   // namespace m2

