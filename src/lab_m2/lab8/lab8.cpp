#include "lab_m2/lab8/lab8.h"

#include <vector>
#include <iostream>

#include "pfd/portable-file-dialogs.h"

using namespace std;
using namespace m2;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab8::Lab8()
{
    outputMode = 0;
    gpuProcessing = false;
    saveScreenToImage = false;
    window->SetSize(600, 600);
}


Lab8::~Lab8()
{
}


void Lab8::Init()
{
    // Load default texture fore imagine processing
    originalImage = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES, "cube", "pos_x.png"), nullptr, "image", true, true);
    processedImage = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES, "cube", "pos_x.png"), nullptr, "newImage", true, true);

    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }

    std::string shaderPath = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Lab8", "shaders");

    // Create a shader program for particle system
    {
        Shader* shader = new Shader("ImageProcessing");
        shader->AddShader(PATH_JOIN(shaderPath, "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "FragmentShader.glsl"), GL_FRAGMENT_SHADER);

        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
}


void Lab8::FrameStart()
{
}


void Lab8::Update(float deltaTimeSeconds)
{
    ClearScreen();

    auto shader = shaders["ImageProcessing"];
    shader->Use();

    if (saveScreenToImage)
    {
        window->SetSize(originalImage->GetWidth(), originalImage->GetHeight());
    }

    int flip_loc = shader->GetUniformLocation("flipVertical");
    glUniform1i(flip_loc, saveScreenToImage ? 0 : 1);

    int screenSize_loc = shader->GetUniformLocation("screenSize");
    glm::ivec2 resolution = window->GetResolution();
    glUniform2i(screenSize_loc, resolution.x, resolution.y);

    int outputMode_loc = shader->GetUniformLocation("outputMode");
    glUniform1i(outputMode_loc, outputMode);

    int locTexture = shader->GetUniformLocation("textureImage");
    glUniform1i(locTexture, 0);

    auto textureImage = (gpuProcessing == true) ? originalImage : processedImage;
    textureImage->BindToTextureUnit(GL_TEXTURE0);

    RenderMesh(meshes["quad"], shader, glm::mat4(1));

    if (saveScreenToImage)
    {
        saveScreenToImage = false;

        GLenum format = GL_RGB;
        if (originalImage->GetNrChannels() == 4)
        {
            format = GL_RGBA;
        }

        glReadPixels(0, 0, originalImage->GetWidth(), originalImage->GetHeight(), format, GL_UNSIGNED_BYTE, processedImage->GetImageData());
        processedImage->UploadNewData(processedImage->GetImageData());
        SaveImage("shader_processing_" + std::to_string(outputMode));

        float aspectRatio = static_cast<float>(originalImage->GetWidth()) / originalImage->GetHeight();
        window->SetSize(static_cast<int>(600 * aspectRatio), 600);
    }
}


void Lab8::FrameEnd()
{
    DrawCoordinateSystem();
}


void Lab8::OnFileSelected(const std::string& fileName)
{
    if (fileName.size())
    {
        std::cout << fileName << endl;
        originalImage = TextureManager::LoadTexture(fileName, nullptr, "image", true, true);
        processedImage = TextureManager::LoadTexture(fileName, nullptr, "newImage", true, true);

        float aspectRatio = static_cast<float>(originalImage->GetWidth()) / originalImage->GetHeight();
        window->SetSize(static_cast<int>(600 * aspectRatio), 600);
    }
}


void Lab8::GrayScale()
{
    unsigned int channels = originalImage->GetNrChannels();
    unsigned char* data = originalImage->GetImageData();
    unsigned char* newData = processedImage->GetImageData();

    if (channels < 3)
        return;

    glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());

    for (int i = 0; i < imageSize.y; i++)
    {
        for (int j = 0; j < imageSize.x; j++)
        {
            int offset = channels * (i * imageSize.x + j);

            // Reset save image data
            char value = static_cast<char>(data[offset + 0] * 0.2f + data[offset + 1] * 0.71f + data[offset + 2] * 0.07);
            memset(&newData[offset], value, 3);
        }
    }

    processedImage->UploadNewData(newData);
}


void Lab8::SaveImage(const std::string& fileName)
{
    cout << "Saving image! ";
    processedImage->SaveToFile((fileName + ".png").c_str());
    cout << "[Done]" << endl;
}


void Lab8::OpenDialog()
{
    std::vector<std::string> filters =
    {
        "Image Files", "*.png *.jpg *.jpeg *.bmp",
        "All Files", "*"
    };

    auto selection = pfd::open_file("Select a file", ".", filters).result();
    if (!selection.empty())
    {
        std::cout << "User selected file " << selection[0] << "\n";
        OnFileSelected(selection[0]);
    }
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab8::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input
}

void Lab8::ProcessCannyCPU()
{
    unsigned int channels = originalImage->GetNrChannels();
    int width = originalImage->GetWidth();
    int height = originalImage->GetHeight();

    unsigned char* srcData = originalImage->GetImageData();
    unsigned char* dstData = processedImage->GetImageData();

    if (channels < 3) return;

    // ========== PAS 1: Box Blur (3x3) ==========
    std::vector<unsigned char> blurredRGB(width * height * channels);
    memcpy(blurredRGB.data(), srcData, width * height * channels);

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            int sumR = 0, sumG = 0, sumB = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int offset = channels * ((i + ky) * width + (j + kx));
                    sumR += srcData[offset + 0];
                    sumG += srcData[offset + 1];
                    sumB += srcData[offset + 2];
                }
            }

            int offset = channels * (i * width + j);
            blurredRGB[offset + 0] = sumR / 9;
            blurredRGB[offset + 1] = sumG / 9;
            blurredRGB[offset + 2] = sumB / 9;
        }
    }

    // ========== PAS 2: Grayscale ==========
    std::vector<unsigned char> grayBuffer(width * height);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int offset = channels * (i * width + j);
            unsigned char r = blurredRGB[offset + 0];
            unsigned char g = blurredRGB[offset + 1];
            unsigned char b = blurredRGB[offset + 2];

            grayBuffer[i * width + j] = static_cast<unsigned char>(0.21f * r + 0.71f * g + 0.07f * b);
        }
    }

    // ========== PAS 3: Detector Sobel ==========
    int Gx[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
    int Gy[3][3] = { { 1, 2, 1}, { 0, 0, 0}, {-1, -2, -1} };

    std::vector<float> gradient(width * height);

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            float sumX = 0, sumY = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    unsigned char val = grayBuffer[(i + ky) * width + (j + kx)];
                    sumX += val * Gx[ky + 1][kx + 1];
                    sumY += val * Gy[ky + 1][kx + 1];
                }
            }

            gradient[i * width + j] = sqrt(sumX * sumX + sumY * sumY);
        }
    }

    // ========== PAS 4: Threshold cu Metoda Otsu (Histogramă) ==========

    // Pas 4.1: Construim histograma
    // Discretizăm gradientul în 256 de nivele (0-255)
    int histogram[256] = { 0 };
    float maxGrad = 0;

    // Găsim valoarea maximă pentru normalizare
    for (int i = 0; i < width * height; i++) {
        if (gradient[i] > maxGrad) {
            maxGrad = gradient[i];
        }
    }

    // Construim histograma (normalizăm gradientul la 0-255)
    for (int i = 0; i < width * height; i++) {
        int bin = static_cast<int>((gradient[i] / maxGrad) * 255.0f);
        if (bin >= 0 && bin < 256) {
            histogram[bin]++;
        }
    }

    // Pas 4.2: Calculăm threshold-ul optim cu Otsu
    int totalPixels = width * height;
    float sum = 0;

    // Calculăm suma ponderată totală
    for (int i = 0; i < 256; i++) {
        sum += i * histogram[i];
    }

    float sumBackground = 0;
    int weightBackground = 0;
    float maxVariance = 0;
    int optimalThreshold = 0;

    //vad fiecare threshold, care imparte pct in 2 clase
    for (int t = 0; t < 256; t++) {
        weightBackground += histogram[t];
        if (weightBackground == 0) continue;

        int weightForeground = totalPixels - weightBackground;
        if (weightForeground == 0) break;

        sumBackground += t * histogram[t];

        float meanBackground = sumBackground / weightBackground;
        float meanForeground = (sum - sumBackground) / weightForeground;

        //verific ca punctele separate sa fie cat mai departate ca medie (varianta maxima)
        float variance = weightBackground * weightForeground *
            (meanBackground - meanForeground) *
            (meanBackground - meanForeground);

        //thresholdul care maximizeaza varianta
        if (variance > maxVariance) {
            maxVariance = variance;
            optimalThreshold = t;
        }
    }

    // Convertim threshold-ul înapoi la scala originală a gradientului
    float threshold = (optimalThreshold / 255.0f) * maxGrad/2;

    cout << "Threshold Otsu calculat: " << threshold
        << " (nivel histograma: " << optimalThreshold << "/255)" << endl;

    // ========== PAS 5: Binarizare ==========
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int idx = i * width + j;
            int offset = channels * idx;

            unsigned char edgeValue = (gradient[idx] > threshold) ? 255 : 0;
            memset(&dstData[offset], edgeValue, 3);
        }
    }

    processedImage->UploadNewData(dstData);
}

void Lab8::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_F || key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE)
    {
        OpenDialog();
    }

    if (key == GLFW_KEY_E)
    {
        gpuProcessing = !gpuProcessing;
        if (gpuProcessing == false)
        {
            outputMode = 0;
        }
        cout << "Processing on GPU: " << (gpuProcessing ? "true" : "false") << endl;
    }

    if (key - GLFW_KEY_0 >= 0 && key <= GLFW_KEY_4) // Am extins intervalul la KEY_4
    {
        outputMode = key - GLFW_KEY_0;

        if (gpuProcessing == false)
        {
            if (outputMode == 0) {
                // Reload original image to reset
                unsigned char* data = originalImage->GetImageData();
                processedImage->UploadNewData(data);
            }
            else if (outputMode == 1) {
                GrayScale();
            }
            else if (outputMode == 4) {
                // Aici apelăm funcția nouă
                cout << "Applying CPU Canny (Blur->Gray->Sobel->Threshold)..." << endl;
                ProcessCannyCPU();
                cout << "Done." << endl;
            }
        }
    }

    if (key == GLFW_KEY_S && mods & GLFW_MOD_CONTROL)
    {
        if (!gpuProcessing)
        {
            SaveImage("processCPU_" + std::to_string(outputMode));
        }
        else {
            saveScreenToImage = true;
        }
    }
}


void Lab8::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab8::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab8::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab8::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab8::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab8::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
