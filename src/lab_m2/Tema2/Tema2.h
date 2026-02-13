#pragma once

#include <string>

#include "components/simple_scene.h"
#include "core/gpu/frame_buffer.h"


namespace m2
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void ProcessBlurCPU();
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
        void CreateFramebuffers(int width, int height);
        void RecognizeObjects(int width, int height);
        void OpenDialog();
        void DrawHUD();
        void OnFileSelected(const std::string& fileName);
        void UpdateFilterTexture(const std::vector<unsigned char>& data);
        // Processing effects
        void GrayScale();
        void SaveImage(const std::string& fileName);
        void Blur();
        void ProcessCannyCPU();
        void ComputeBlurMaskCPU();
        std::vector<float> ComputeKernel(int radius);
        void ApplyOilPainting(int radius, int levels); //tasta 6
        void ApplyPencilSketch(); //tasta 7
        void ApplyThermalVision(); //tasta 8
        void ApplyBilateralFilter(float sigmaS, float sigmaR); //tasta 9

        void ComputeCPUBlur();      // Doar calculează pixelii
        void ComputeCPUBlurQuick(); //blurul mai rapid cu metoda SAT
        void UploadCPUToGPU();      // Doar trimite buffer-ul corect la placa video

    private:
        Texture2D* originalImage;
        Texture2D* processedImage;
        //Texture2D* textureVertical;
        //Texture2D* textureHorizontal;

        int outputMode;
        bool gpuProcessing;
        bool saveScreenToImage;
        float radiusSize = 10;
        float  threshold = 0.3;
        // Framebuffers for 2-pass blur
        unsigned int fboHorizontal;
        unsigned int textureHorizontal; // Color attachment for pass 1
        unsigned int fboVertical;
        unsigned int textureVertical;   // Color attachment for pass 2 (Final Blur)

        // Bokeh Parameters
        glm::vec2 focusPos;
        float focusRadius;
        int viewMode;

        bool showDetection;
        glm::vec4 detectedBox;

        bool showCanny;
        Texture2D* cannyTexture;
        void ApplyCannyFilter();


        //pentru partea de cpu
        bool useGPU = true;
        bool dirtyCPU = true;
        bool dirtyTexture = true;
        bool paramsChanged = true;

        // Pentru CPU
        Texture2D* cpuResultTexture = nullptr; // Textura în care scriem rezultatul CPU
        //std::vector<unsigned char> hostPixelData; // Copia imaginii originale în RAM

        // Constante
        const int MAX_BLUR_RADIUS = 20;

        int blurRadius = 20;
        const int ABSOLUTE_MAX_RADIUS = 100;

        //texturi
        std::vector<unsigned char> hostPixelData; // Imaginea originală
        std::vector<unsigned char> pass1Pixels;   // După blur orizontal
        std::vector<unsigned char> pass2Pixels;   // După blur vertical (final)
        std::vector<std::vector<float>> kernels;

        bool showBlurInFinal = true;

        bool show_text = true;
        bool quick_cpu = false;

        bool showBlurMask = false;     // Flag pentru activare/dezactivare
        Texture2D* blurMaskTexture;

        //tastele 6,7,8,9
        Texture2D* extraFilterTexture = nullptr; // Textură generică pentru filtrele 6-9
        int activeFilterMode = 0;




    };
}   // namespace m2
