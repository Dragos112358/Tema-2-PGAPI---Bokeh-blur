
/*Bonusuri implementate:
* Functie canny edge detection
* implementare pe gpu
* imbunatatire ca scriu pe ecran datele
* Todo:
* efect bokeh la compute kernels
* Optimizări de tip Circular Separable Convolution (10-20p)
* key up -> creste blurul, key down -> scade blurul
* 5 -> pot sa vad cum arata zona unde blurez
* 
*/

/* Taste folosite
* 0 - tasta pentru imaginea originala
* 1 - tasta pentru pass 1 (blur orizontal)
* 2 - tasta pentru pass 2 (blur vertical)
* 4 - toogle intre cercuri cu poza clasica si cercuri cu poza finala
*  key_up, key_down -> mareste/micsoreaza intensitatea blurului
* +- -> mareste/micsoreaza cercul 
* E -> face switch intre cpu si gpu
* F -> deschide dialogul
* T -> arata/nu arata textul (default il arata)
* D -> pentru canny edge detection
* Q -> pentru a da swap intre optiunea quick si optiunea clasica pe cpu
* 5 -> sa vad cum arata blurul cu alb negru
* 6. Efectul de Pictură în Ulei (Oil Painting)
* 7. Schiță în Creion (Pencil Sketch)
* 8. Viziune heatmap
* 9. Filtrul Bilateral (Edge-Preserving Blur)
* Todo: De adaugat pe numarul 5 sa pot sa vad poza alb cu negru unde dau blurul
*/

#include "lab_m2/Tema2/Tema2.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

#include "pfd/portable-file-dialogs.h"
#include "lab_extra/basic_text/basic_text.h"

using namespace std;
using namespace m2;

// Macro pentru a evita conflictele cu std::min/max pe Windows

#ifdef _WIN32
#undef min
#undef max
#endif

Tema2::Tema2()
{
    focusPos = glm::vec2(0, 0);
    focusRadius = 100.0f;
    viewMode = 0; // Default: Final + Overlay
    showCanny = false; //canny pe tasta d
    cannyTexture = nullptr; //textura canny

    blurMaskTexture = nullptr; //pentru tasta 5, arata masca folosita
    showBlurMask = false; //bool daca vad masca acum

    fboHorizontal = 0; //pentru primul pass (orizontal)
    textureHorizontal = 0;
    fboVertical = 0; //pass 2 (vertical)
    textureVertical = 0;
    extraFilterTexture = nullptr;
}

Tema2::~Tema2()
{
    glDeleteFramebuffers(1, &fboHorizontal);
    glDeleteTextures(1, &textureHorizontal);
    glDeleteFramebuffers(1, &fboVertical);
    glDeleteTextures(1, &textureVertical);

    // Stergem textura canny daca exista
    if (cannyTexture) delete cannyTexture;
    if (blurMaskTexture) delete blurMaskTexture;
    if (extraFilterTexture) delete extraFilterTexture;
}
gfxc::TextRenderer* textRenderer;
void Tema2::Init()
{
    // 1. Incarcam imaginea
    originalImage = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES, "cube", "pos_x.png"), nullptr, "image", true, true);
    //pentru partea de text
    glm::vec2 resolution2 = window->GetResolution();
    textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution2.x, resolution2.y);
    textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 18);
    // 2. Mesh Quad
    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }

    // 3. Framebuffers
    glm::ivec2 res = window->GetResolution();
    //CreateFramebuffers(res.x, res.y);
    CreateFramebuffers(originalImage->GetWidth(), originalImage->GetHeight());
    // Initializam focusul in centrul ecranului pentru a se vedea efectul din start
    focusPos = glm::vec2(res.x / 2.0f, res.y / 2.0f);

    std::string shaderPath = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema2", "shaders");

    // 4. Shadere, am folosit un shader pentru blur orizontal, vertical si respectiv randarea rezultatului cu screendisplay
    {
        Shader* shader = new Shader("BlurHorizontal");
        shader->AddShader(PATH_JOIN(shaderPath, "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "FragmentShaderHorizontal.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("BlurVertical");
        shader->AddShader(PATH_JOIN(shaderPath, "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "FragmentShaderVertical.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("ScreenDisplay");
        shader->AddShader(PATH_JOIN(shaderPath, "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "FragmentShaderScreen.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // === NOU: Initializare textură CPU ===
    // =====================================
    //======== Parte CPU ===================
    //======================================
    cpuResultTexture = new Texture2D();
    // Citim pixelii din imaginea incarcata initial in RAM
    int w = originalImage->GetWidth();
    int h = originalImage->GetHeight();
    hostPixelData.resize(w * h * 4);

    glBindTexture(GL_TEXTURE_2D, originalImage->GetTextureID());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, hostPixelData.data());
    glBindTexture(GL_TEXTURE_2D, 0);


    //aici e extra
    hostPixelData.resize(w * h * 4);
    pass1Pixels.resize(w * h * 4);
    pass2Pixels.resize(w * h * 4);

    // Citire pixeli originali
    glBindTexture(GL_TEXTURE_2D, originalImage->GetTextureID());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, hostPixelData.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    dirtyCPU = true; // Forțăm un calcul inițial
    // Inițializare textura CPU
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, hostPixelData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    cpuResultTexture->Init(texID, w, h, 4);
}

//tsata 5
void Tema2::ComputeBlurMaskCPU()
{
    if (!originalImage) return;

    int w = originalImage->GetWidth();
    int h = originalImage->GetHeight();
    glm::ivec2 res = window->GetResolution();

    float scaleX = (float)res.x / (float)w;
    float scaleY = (float)res.y / (float)h;

    std::vector<unsigned char> maskData(w * h * 4);
    float transitionWidth = focusRadius;

#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            // Mapăm pixelul la spațiul ecranului.
            // NOTĂ: Dacă mouse-ul și imaginea sunt inversate, folosește res.y - (y * scaleY)
            glm::vec2 pixelPosOnScreen = glm::vec2(x * scaleX, y * scaleY);

            float dist = glm::distance(pixelPosOnScreen, focusPos);

            // Calculăm t folosind o zonă de tranziție mult mai mare.
            // focusRadius aici reprezintă cât de mare este cercul central "pur negru".
            float t = glm::clamp((dist - focusRadius) / transitionWidth, 0.0f, 1.0f);

            // Formula Smoothstep asigură atenuarea fină la margini.
            float maskVal = t * t * (3.0f - 2.0f * t);

            unsigned char color = (unsigned char)(maskVal * 255.0f);
            int idx = (y * w + x) * 4;

            maskData[idx + 0] = color;
            maskData[idx + 1] = color;
            maskData[idx + 2] = color;
            maskData[idx + 3] = 255;
        }
    }

    // Gestionare textură OpenGL
    if (!blurMaskTexture) blurMaskTexture = new Texture2D();

    GLuint texID = blurMaskTexture->GetTextureID();
    if (texID == 0) glGenTextures(1, &texID);

    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, maskData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    blurMaskTexture->Init(texID, w, h, 4);
}
//functie care transfera de pe cpu pe gpu imaginea
void Tema2::UploadCPUToGPU() 
{
    int w = originalImage->GetWidth();
    int h = originalImage->GetHeight();

    unsigned char* srcData = hostPixelData.data();

    if (viewMode == 1 && pass1Pixels.size() > 0) srcData = pass1Pixels.data();
    else if (viewMode >= 2 && pass2Pixels.size() > 0) srcData = pass2Pixels.data();

    static std::vector<unsigned char> uploadBuffer;
    if (uploadBuffer.size() != w * h * 4) uploadBuffer.resize(w * h * 4);

#pragma omp parallel for
    for (int i = 0; i < w * h; ++i) {
        uploadBuffer[i * 4 + 0] = srcData[i * 4 + 0];
        uploadBuffer[i * 4 + 1] = srcData[i * 4 + 1];
        uploadBuffer[i * 4 + 2] = srcData[i * 4 + 2];
        uploadBuffer[i * 4 + 3] = 255;
    }

    glBindTexture(GL_TEXTURE_2D, cpuResultTexture->GetTextureID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, uploadBuffer.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Tema2::ComputeCPUBlur() //blurul realizat pe cpu
{
    int imgW = originalImage->GetWidth();
    int imgH = originalImage->GetHeight();
    glm::ivec2 winRes = window->GetResolution();
    if (kernels.size() < blurRadius + 1) {
        kernels.resize(blurRadius + 1);
        for (int r = 0; r <= blurRadius; ++r) {
            kernels[r] = ComputeKernel(r);
        }
    }

    float scaleX = (float)winRes.x / (float)imgW;
    float scaleY = (float)winRes.y / (float)imgH;

    if (pass1Pixels.size() != imgW * imgH * 4) pass1Pixels.assign(imgW * imgH * 4, 0);
    if (pass2Pixels.size() != imgW * imgH * 4) pass2Pixels.assign(imgW * imgH * 4, 0);

    static float gammaTable[256];
    static bool tableInit = false;
    if (!tableInit) {
        for (int i = 0; i < 256; i++) gammaTable[i] = pow(i / 255.0f, 2.2f);
        tableInit = true;
    }

    // === PASS 1: Orizontal ===
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < imgH; ++y) {
        for (int x = 0; x < imgW; ++x) {
            glm::vec2 screenPos = glm::vec2(x * scaleX, y * scaleY);
            float dist = glm::distance(screenPos, focusPos);

            // Corespondență shader: t = clamp((dist - focusRadius) / (focusRadius * 2.0), 0.0, 1.0)
            float t = glm::clamp((dist - focusRadius) / (focusRadius * 2.0f), 0.0f, 1.0f);
            float blurAmount = pow(t, 1.001f);

            // Simulare noise din shader: fract(sin(dot...))
            float noise = (float)(fmod(sin(x * 12.9898f + y * 78.233f) * 43758.5453f, 1.0)) - 0.5f;

            float currentRadius = float(blurRadius) * blurAmount + noise;
            int r = int(std::max(0.0f, currentRadius));

            int idx = (y * imgW + x) * 4;

            if (r < 1) {
                for (int c = 0; c < 3; c++) pass1Pixels[idx + c] = hostPixelData[idx + c];
                pass1Pixels[idx + 3] = 0;
            }
            else {
                int safeR = std::min(r, (int)kernels.size() - 1);
                const auto& kernel = kernels[safeR];
                float rSum = 0, gSum = 0, bSum = 0;

                for (int k = -safeR; k <= safeR; k++) {
                    int sx = glm::clamp(x + k, 0, imgW - 1);
                    int sIdx = (y * imgW + sx) * 4;
                    rSum += gammaTable[hostPixelData[sIdx + 0]] * kernel[k + safeR];
                    gSum += gammaTable[hostPixelData[sIdx + 1]] * kernel[k + safeR];
                    bSum += gammaTable[hostPixelData[sIdx + 2]] * kernel[k + safeR];
                }

                pass1Pixels[idx + 0] = (unsigned char)(pow(rSum, 1.0f / 2.2f) * 255.0f);
                pass1Pixels[idx + 1] = (unsigned char)(pow(gSum, 1.0f / 2.2f) * 255.0f);
                pass1Pixels[idx + 2] = (unsigned char)(pow(bSum, 1.0f / 2.2f) * 255.0f);
                pass1Pixels[idx + 3] = (unsigned char)(blurAmount * 255.0f);
            }
        }
    }

    // === PASS 2: Vertical ===
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < imgH; ++y) {
        for (int x = 0; x < imgW; ++x) {
            int idx = (y * imgW + x) * 4;
            float blurAmount = pass1Pixels[idx + 3] / 255.0f;

            // Re-aplicăm jitter-ul și aici pentru consistență
            float noise = (float)(fmod(sin(x * 12.9898f + y * 78.233f) * 43758.5453f, 1.0)) - 0.5f;
            float currentRadius = float(blurRadius) * blurAmount + noise;
            int r = int(std::max(0.0f, currentRadius));

            if (r < 1) {
                for (int c = 0; c < 3; c++) pass2Pixels[idx + c] = pass1Pixels[idx + c];
            }
            else {
                int safeR = std::min(r, (int)kernels.size() - 1);
                const auto& kernel = kernels[safeR];
                float rSum = 0, gSum = 0, bSum = 0;

                for (int k = -safeR; k <= safeR; k++) {
                    int sy = glm::clamp(y + k, 0, imgH - 1);
                    int sIdx = (sy * imgW + x) * 4;
                    rSum += gammaTable[pass1Pixels[sIdx + 0]] * kernel[k + safeR];
                    gSum += gammaTable[pass1Pixels[sIdx + 1]] * kernel[k + safeR];
                    bSum += gammaTable[pass1Pixels[sIdx + 2]] * kernel[k + safeR];
                }
                pass2Pixels[idx + 0] = (unsigned char)(pow(rSum, 1.0f / 2.2f) * 255.0f);
                pass2Pixels[idx + 1] = (unsigned char)(pow(gSum, 1.0f / 2.2f) * 255.0f);
                pass2Pixels[idx + 2] = (unsigned char)(pow(bSum, 1.0f / 2.2f) * 255.0f);
            }
            pass2Pixels[idx + 3] = 255;
        }
    }
}
//aici este partea in care fac suma de 2 gaussiene pentru efect bokeh
std::vector<float> Tema2::ComputeKernel(int radius) {
    std::vector<float> kernel(2 * radius + 1);
    // Două sigme: una pentru centrul blurului, una pentru halou (glow)
    float sigma1 = std::max(radius / 2.0f, 1.0f);
    float sigma2 = sigma1 * 1.5f;
    float sum = 0.0f;

    for (int i = -radius; i <= radius; ++i) {
        float xSq = (float)(i * i);
        // Combinație de două distribuții Gaussiene
        float g1 = exp(-(xSq / (2.0f * sigma1 * sigma1)));
        float g2 = exp(-(xSq / (2.0f * sigma2 * sigma2)));

        // Ponderea: 60% din prima, 40% din a doua (simulează imperfecțiunea lentilei)
        float weight = (0.6f * g1) + (0.4f * g2);

        kernel[i + radius] = weight;
        sum += weight;
    }

    for (float& w : kernel) w /= sum; // Normalizare obligatorie
    return kernel;
}
//parte cpu cu 2 passuri (vertical si orizontal)

//aici e varianta care face blurul gaussian separabil mai rapid pe cpu (dar rezultatul este mai patratos)
void Tema2::ComputeCPUBlurQuick()
{
    int imgW = originalImage->GetWidth();
    int imgH = originalImage->GetHeight();
    glm::ivec2 winRes = window->GetResolution();

    // 1. Management Memorie: Redimensionăm DOAR dacă se schimbă imaginea
    static int lastW = 0, lastH = 0;
    static std::vector<double> satR, satG, satB;

    if (imgW != lastW || imgH != lastH) {
        satR.assign((imgW + 1) * (imgH + 1), 0);
        satG.assign((imgW + 1) * (imgH + 1), 0);
        satB.assign((imgW + 1) * (imgH + 1), 0);
        lastW = imgW; lastH = imgH;
    }

    // 2. Construcție SAT în spațiu Gamma 2.2 (O(N))
    for (int y = 0; y < imgH; ++y) {
        double rowSumR = 0, rowSumG = 0, rowSumB = 0;
        for (int x = 0; x < imgW; ++x) {
            int srcIdx = (y * imgW + x) * 4;
            rowSumR += pow(hostPixelData[srcIdx + 0] / 255.0, 2.2);
            rowSumG += pow(hostPixelData[srcIdx + 1] / 255.0, 2.2);
            rowSumB += pow(hostPixelData[srcIdx + 2] / 255.0, 2.2);

            int satIdx = (y + 1) * (imgW + 1) + (x + 1);
            satR[satIdx] = satR[y * (imgW + 1) + (x + 1)] + rowSumR;
            satG[satIdx] = satG[y * (imgW + 1) + (x + 1)] + rowSumG;
            satB[satIdx] = satB[y * (imgW + 1) + (x + 1)] + rowSumB;
        }
    }

    float scaleX = (float)winRes.x / (float)imgW;
    float scaleY = (float)winRes.y / (float)imgH;

    // 3. Procesare Pass 1 și Pass 2 în O(1)
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < imgH; ++y) {
        for (int x = 0; x < imgW; ++x) {
            glm::vec2 screenPos = glm::vec2(x * scaleX, y * scaleY);
            float dist = glm::distance(screenPos, focusPos);
            float t = glm::clamp((dist - focusRadius) / 80.0f, 0.0f, 1.0f);
            float blurAmount = t * t * (3.0f - 2.0f * t);

            int r = int(float(blurRadius) * blurAmount);
            int idx = (y * imgW + x) * 4;

            if (r < 1) {
                // Zona de focus: Copiem originalul în ambele pass-uri
                for (int c = 0; c < 3; ++c) {
                    pass1Pixels[idx + c] = pass2Pixels[idx + c] = hostPixelData[idx + c];
                }
                pass1Pixels[idx + 3] = 0; // Pentru shader (blurAmount)
            }
            else {
                int x1 = std::max(0, x - r), x2 = std::min(imgW - 1, x + r);
                int y1 = std::max(0, y - r), y2 = std::min(imgH - 1, y + r);

                // --- PASS 1: Orizontal (Query SAT doar pe rândul curent) ---
                auto getRowSum = [&](const std::vector<double>& sat, int py, int px1, int px2) {
                    return sat[(py + 1) * (imgW + 1) + (px2 + 1)]
                        - sat[(py) * (imgW + 1) + (px2 + 1)]
                        - sat[(py + 1) * (imgW + 1) + (px1)]
                        + sat[(py) * (imgW + 1) + (px1)];
                    };
                double rowCount = (x2 - x1 + 1);
                pass1Pixels[idx + 0] = (unsigned char)(pow(getRowSum(satR, y, x1, x2) / rowCount, 1.0 / 2.2) * 255);
                pass1Pixels[idx + 1] = (unsigned char)(pow(getRowSum(satG, y, x1, x2) / rowCount, 1.0 / 2.2) * 255);
                pass1Pixels[idx + 2] = (unsigned char)(pow(getRowSum(satB, y, x1, x2) / rowCount, 1.0 / 2.2) * 255);
                pass1Pixels[idx + 3] = (unsigned char)(blurAmount * 255.0f);

                // --- PASS 2: Full Box Blur (Query SAT 2D) ---
                auto getAreaSum = [&](const std::vector<double>& sat) {
                    return sat[(y2 + 1) * (imgW + 1) + (x2 + 1)]
                        - sat[(y1) * (imgW + 1) + (x2 + 1)]
                        - sat[(y2 + 1) * (imgW + 1) + (x1)]
                        + sat[(y1) * (imgW + 1) + (x1)];
                    };
                double areaCount = (x2 - x1 + 1) * (y2 - y1 + 1);
                pass2Pixels[idx + 0] = (unsigned char)(pow(getAreaSum(satR) / areaCount, 1.0 / 2.2) * 255);
                pass2Pixels[idx + 1] = (unsigned char)(pow(getAreaSum(satG) / areaCount, 1.0 / 2.2) * 255);
                pass2Pixels[idx + 2] = (unsigned char)(pow(getAreaSum(satB) / areaCount, 1.0 / 2.2) * 255);
            }
            pass2Pixels[idx + 3] = 255;
        }
    }
}

//functie care scrie text in coltul stanga sus, iar cu t dau toggle daca vad sau nu textul
void Tema2::DrawHUD()
{
    if (!show_text) return;

    // 1. Pregătire stare OpenGL pentru text (Antialiasing și Blending)
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float xPos = 20.0f;
    float currentY = 30.0f;
    float stepY = 25.0f;
    glm::vec3 colorYellow = glm::vec3(1, 1, 0);
    glm::vec3 colorGreen = glm::vec3(0, 1, 0);
    glm::ivec2 res = window->GetResolution();

    // --- LINIA 1: MODUL DE PROCESARE ---
    string modeStr = useGPU ? "GPU (Shaders)" : (quick_cpu ? "CPU (Quick SAT)" : "CPU (Parallel)");
    textRenderer->RenderText("Processing Mode: " + modeStr, xPos, currentY, 1.0f, colorYellow);
    currentY += stepY;

    // --- LINIA 2: REZOLUȚIA ACTIVĂ ---
    textRenderer->RenderText("Resolution: " + to_string(res.x) + "x" + to_string(res.y), xPos, currentY, 1.0f, colorYellow);
    currentY += stepY;

    // --- LINIA 3: PARAMETRI FOCUS ---
    textRenderer->RenderText("Focus Radius: " + to_string((int)focusRadius), xPos, currentY, 1.0f, colorYellow);
    currentY += stepY;

    // --- LINIA 4: POZIȚIE MOUSE ---
    textRenderer->RenderText("Mouse Position: " + to_string((int)focusPos.x) + ", " + to_string((int)focusPos.y), xPos, currentY, 1.0f, colorYellow);
    currentY += stepY;

    // --- LINIA 5: INTENSITATE BLUR ---
    textRenderer->RenderText("Max Blur Radius: " + to_string(blurRadius), xPos, currentY, 1.0f, colorYellow);
    currentY += stepY;

    // --- LINIA 6: VIEW PASS ACTIV (0-4) ---
    string passName = "";
    switch (viewMode) {
    case 0: passName = "Original Image"; break;
    case 1: passName = "Pass 1 (Horizontal Blur)"; break;
    case 2: passName = "Pass 2 (Vertical Blur)"; break;
    case 4: passName = showBlurInFinal ? "Final Result (Blur + Overlay)" : "Overlay Only"; break;
    case 5: passName = "Blur filter white and black"; break;
    case 6: passName = "Oil painting"; break;
    case 7: passName = "Sketch"; break;
    case 8: passName = "Thermal filter"; break;
    case 9: passName = "Bilateral filter"; break;
    }
    textRenderer->RenderText("View Pass: " + passName, xPos, currentY, 1.0f, colorYellow);
    currentY += stepY * 1.5f; // Spațiu extra înainte de filtre

    // --- LINIA 7: STATUS FILTRE SPECIALE ---
    if (showCanny) {
        textRenderer->RenderText("ACTIVE FILTER: CANNY EDGE DETECTION", xPos, currentY, 1.0f, colorGreen);
    }
    else if (showBlurMask) {
        textRenderer->RenderText("ACTIVE FILTER: BLUR MASK (VISUALIZER)", xPos, currentY, 1.0f, colorGreen);
    }
    else if (activeFilterMode >= 6) {
        string filterName = "";
        if (activeFilterMode == 6) filterName = "OIL PAINTING";
        if (activeFilterMode == 7) filterName = "PENCIL SKETCH";
        if (activeFilterMode == 8) filterName = "THERMAL VISION";
        if (activeFilterMode == 9) filterName = "BILATERAL FILTER";
        textRenderer->RenderText("ACTIVE FILTER: " + filterName, xPos, currentY, 1.0f, colorGreen);
    }

    // 2. Revenire la starea inițială
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
//functie folosita pentru tastele 6-9, scopul este sa imi actualizeze ce am pe ecran cu noua textura calculata
void Tema2::UpdateFilterTexture(const std::vector<unsigned char>& data) {
    int w = originalImage->GetWidth();
    int h = originalImage->GetHeight();
    if (!extraFilterTexture) extraFilterTexture = new Texture2D();

    GLuint texID = extraFilterTexture->GetTextureID();
    if (texID == 0) glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    extraFilterTexture->Init(texID, w, h, 4);
}
//tasta 6
void Tema2::ApplyOilPainting(int radius, int levels) {
    int w = originalImage->GetWidth();
    int h = originalImage->GetHeight();
    std::vector<unsigned char> outData(w * h * 4);
    /*Se calculează cărei categorii de intensitate(curLevel) îi aparține.
     Se incrementează intensityCount[curLevel].
     Se adună valorile de culoare(R, G, B) în rSum[curLevel], gSum[curLevel], etc.*/
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int intensityCount[256] = { 0 };
            float rSum[256] = { 0 }, gSum[256] = { 0 }, bSum[256] = { 0 };

            for (int i = -radius; i <= radius; i++) {
                for (int j = -radius; j <= radius; j++) {
                    int ny = glm::clamp(y + i, 0, h - 1);
                    int nx = glm::clamp(x + j, 0, w - 1);
                    int idx = (ny * w + nx) * 4;

                    unsigned char r = hostPixelData[idx + 0];
                    unsigned char g = hostPixelData[idx + 1];
                    unsigned char b = hostPixelData[idx + 2];

                    int curLevel = (int)((((r + g + b) / 3.0f) * levels) / 255.0f);
                    curLevel = glm::clamp(curLevel, 0, levels - 1);

                    intensityCount[curLevel]++;
                    rSum[curLevel] += r; gSum[curLevel] += g; bSum[curLevel] += b;
                }
            }

            int maxLevel = 0;
            for (int l = 1; l < levels; l++) {
                if (intensityCount[l] > intensityCount[maxLevel]) maxLevel = l;
            }
            /*Pixelul de ieșire nu primește pur și simplu o culoare fixă, ci media culorilor originale care au căzut în acea categorie dominantă*/
            int outIdx = (y * w + x) * 4;
            outData[outIdx + 0] = (unsigned char)(rSum[maxLevel] / intensityCount[maxLevel]);
            outData[outIdx + 1] = (unsigned char)(gSum[maxLevel] / intensityCount[maxLevel]);
            outData[outIdx + 2] = (unsigned char)(bSum[maxLevel] / intensityCount[maxLevel]);
            outData[outIdx + 3] = 255;
        }
    }
    UpdateFilterTexture(outData);
}
//tasta 7
void Tema2::ApplyPencilSketch() {
    int w = originalImage->GetWidth();
    int h = originalImage->GetHeight();
    std::vector<unsigned char> gray(w * h);
    std::vector<unsigned char> blurH(w * h), blurV(w * h);
    std::vector<unsigned char> outData(w * h * 4);

    // 1. Grayscale & Invert (O(N))
#pragma omp parallel for
    for (int i = 0; i < w * h; i++) {
        gray[i] = (unsigned char)(0.299f * hostPixelData[i * 4] + 0.587f * hostPixelData[i * 4 + 1] + 0.114f * hostPixelData[i * 4 + 2]);
        blurH[i] = 255 - gray[i];
    }

    // 2. Separable Box Blur (Mult mai rapid!)
    int r = 4;
    // Pass Orizontal
#pragma omp parallel for
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float sum = 0;
            for (int k = -r; k <= r; k++) sum += blurH[y * w + glm::clamp(x + k, 0, w - 1)];
            blurV[y * w + x] = (unsigned char)(sum / (2 * r + 1));
        }
    }
    // Pass Vertical
#pragma omp parallel for
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            float sum = 0;
            for (int k = -r; k <= r; k++) sum += blurV[glm::clamp(y + k, 0, h - 1) * w + x];
            blurH[y * w + x] = (unsigned char)(sum / (2 * r + 1));
        }
    }

    // 3. Color Dodge Blending
#pragma omp parallel for
    for (int i = 0; i < w * h; i++) {
        int target = blurH[i];
        int val = (target >= 255) ? 255 : glm::min(255, (gray[i] * 255) / (255 - target));
        int outIdx = i * 4;
        outData[outIdx] = outData[outIdx + 1] = outData[outIdx + 2] = (unsigned char)val;
        outData[outIdx + 3] = 255;
    }
    UpdateFilterTexture(outData);
}
//tasta 8
void Tema2::ApplyThermalVision() {
    int w = originalImage->GetWidth();
    int h = originalImage->GetHeight();
    std::vector<unsigned char> outData(w * h * 4);

#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;

            // 1. Calculăm luminozitatea pixelului curent
            float r = hostPixelData[idx + 0];
            float g = hostPixelData[idx + 1];
            float b = hostPixelData[idx + 2];
            float v = (0.2126f * r + 0.7152f * g + 0.0722f * b) / 255.0f;

            // 2. Edge Detection rapid (comparăm cu vecinul din dreapta/jos)
            // Asta creează acel efect de "contur strălucitor"
            float edge = 0;
            if (x < w - 1 && y < h - 1) {
                int nextIdx = (y * w + (x + 1)) * 4;
                float vNext = (hostPixelData[nextIdx] * 0.2f + hostPixelData[nextIdx + 1] * 0.7f) / 255.0f;
                edge = std::abs(v - vNext) * 5.0f; // Amplificăm marginile
            }

            // 3. Mapare culori Predator (Blue -> Magenta -> Red -> Yellow -> White)
            glm::vec3 color;
            if (v < 0.25f)      color = glm::vec3(0, 0, v * 4.0f); // Deep Blue
            else if (v < 0.5f)  color = glm::vec3((v - 0.25f) * 4.0f, 0, 1.0f); // Blue to Magenta
            else if (v < 0.75f) color = glm::vec3(1.0f, 0, 1.0f - (v - 0.5f) * 4.0f); // Magenta to Red
            else                color = glm::vec3(1.0f, (v - 0.75f) * 4.0f, 0); // Red to Yellow

            // 4. Adăugăm Glow-ul de margine (Hot Edges)
            color += glm::vec3(edge, edge * 0.3f, 0);

            // 5. Post-procesare: Contrast și Saturație
            color = glm::clamp(color, 0.0f, 1.0f);

            outData[idx + 0] = (unsigned char)(color.r * 255);
            outData[idx + 1] = (unsigned char)(color.g * 255);
            outData[idx + 2] = (unsigned char)(color.b * 255);
            outData[idx + 3] = 255; //salvez datele de output + aplha 255
        }
    }
    UpdateFilterTexture(outData);
}
//tasta 9

/*Spre deosebire de un Blur Gaussian clasic, care "mânjește" totul uniform, filtrul bilateral este "inteligent": 
el decide cât de mult să blureze un pixel comparându-l cu vecinii săi nu doar prin distanța fizică, ci și prin 
diferența de culoare.*/
void Tema2::ApplyBilateralFilter(float sigmaS, float sigmaR) {
    //filtru bilateral
    int w = originalImage->GetWidth();
    int h = originalImage->GetHeight();
    int r = (int)sigmaS;
    std::vector<unsigned char> temp(w * h * 4);
    std::vector<unsigned char> outData(w * h * 4);
    /*De regula, nu se aplica separare pe filtru bilateral, deoarece nu e chiar identic, insa in acest caz este
    o aproximare foarte buna*/
    // Pass 1: Orizontal
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float rW = 0, gW = 0, bW = 0, weightSum = 0;
            int cIdx = (y * w + x) * 4;
            glm::vec3 cCol(hostPixelData[cIdx], hostPixelData[cIdx + 1], hostPixelData[cIdx + 2]);
            //ma uit la vecini pe raza r
            for (int k = -r; k <= r; k++) {
                int nx = glm::clamp(x + k, 0, w - 1);
                int nIdx = (y * w + nx) * 4;
                glm::vec3 nCol(hostPixelData[nIdx], hostPixelData[nIdx + 1], hostPixelData[nIdx + 2]);

                float wS = exp(-(k * k) / (2 * sigmaS * sigmaS));
                float wR = exp(-glm::length2(cCol - nCol) / (2 * sigmaR * sigmaR));
                float weight = wS * wR;

                rW += nCol.r * weight; gW += nCol.g * weight; bW += nCol.b * weight;
                weightSum += weight;
            }
            int tIdx = (y * w + x) * 4;
            temp[tIdx] = rW / weightSum; temp[tIdx + 1] = gW / weightSum; temp[tIdx + 2] = bW / weightSum;
        }
    }
    // Pass 2: Vertical (folosind datele din temp)
#pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            float rW = 0, gW = 0, bW = 0, weightSum = 0;
            int cIdx = (y * w + x) * 4;
            glm::vec3 cCol(temp[cIdx], temp[cIdx + 1], temp[cIdx + 2]);
            //ma uit la vecini pe raza r, pentru a vedea cat de tare blurez acel pixel
            for (int k = -r; k <= r; k++) {
                int ny = glm::clamp(y + k, 0, h - 1);
                int nIdx = (ny * w + x) * 4;
                glm::vec3 nCol(temp[nIdx], temp[nIdx + 1], temp[nIdx + 2]);

                float wS = exp(-(k * k) / (2 * sigmaS * sigmaS));
                float wR = exp(-glm::length2(cCol - nCol) / (2 * sigmaR * sigmaR));
                float weight = wS * wR;

                rW += nCol.r * weight; gW += nCol.g * weight; bW += nCol.b * weight;
                weightSum += weight;
            }
            int oIdx = (y * w + x) * 4;
            outData[oIdx] = rW / weightSum; outData[oIdx + 1] = gW / weightSum; outData[oIdx + 2] = bW / weightSum; outData[oIdx + 3] = 255;
        }
    }
    UpdateFilterTexture(outData);
}
//filtru canny
void Tema2::ApplyCannyFilter()
{
    int width = originalImage->GetWidth();
    int height = originalImage->GetHeight();

    // 1. Extragem pixelii originali
    std::vector<unsigned char> pixels(width * height * 4);
    glBindTexture(GL_TEXTURE_2D, originalImage->GetTextureID());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    // Buffere intermediare
    std::vector<float> grayscale(width * height);
    std::vector<float> blurred(width * height);
    std::vector<float> magnitude(width * height, 0.0f);
    std::vector<float> angle(width * height, 0.0f);
    std::vector<unsigned char> edgeData(width * height * 4, 0);

    // 2. Conversie Grayscale (Luminanță precisă)
    for (int i = 0; i < width * height; i++) {
        grayscale[i] = 0.299f * pixels[i * 4] + 0.587f * pixels[i * 4 + 1] + 0.114f * pixels[i * 4 + 2];
    }

    // 3. Blur Gaussian simplificat (3x3) pentru a elimina marginile false (zgomotul)
    //in cazuri reale, se folosesc kerneluri mai mari pentru a reduce mai bine zgomotul
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float sum = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    sum += grayscale[(y + i) * width + (x + j)];
                }
            }
            blurred[y * width + x] = sum / 9.0f;
        }
    }

    // 4. Operatori Sobel (Magnitudine și Direcție)
    int Gx[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
    int Gy[3][3] = { {-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };
    //ma uit la toti cei 8 vecini pentru a calcula magnitudini pe x si pe y, iar dimensiunea este sqrt(sume la patrat)
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float sumX = 0, sumY = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    float val = blurred[(y + i) * width + (x + j)];
                    sumX += val * Gx[i + 1][j + 1];
                    sumY += val * Gy[i + 1][j + 1];
                }
            }
            magnitude[y * width + x] = std::sqrt(sumX * sumX + sumY * sumY);
            angle[y * width + x] = std::atan2(sumY, sumX) * 180.0f / 3.14159f; // Convertim in grade
        }
    }

    // 5. Non-Maximum Suppression (Subțierea marginilor la 1 pixel)
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float mag = magnitude[y * width + x];
            float ang = angle[y * width + x];
            if (ang < 0) ang += 180;

            float neighbor1 = 0, neighbor2 = 0;

            // Aproximăm direcția gradientului la 0, 45, 90 sau 135 grade
            if ((ang >= 0 && ang < 22.5) || (ang >= 157.5 && ang <= 180)) {
                neighbor1 = magnitude[y * width + (x + 1)];
                neighbor2 = magnitude[y * width + (x - 1)];
            }
            else if (ang >= 22.5 && ang < 67.5) {
                neighbor1 = magnitude[(y + 1) * width + (x - 1)];
                neighbor2 = magnitude[(y - 1) * width + (x + 1)];
            }
            else if (ang >= 67.5 && ang < 112.5) {
                neighbor1 = magnitude[(y + 1) * width + x];
                neighbor2 = magnitude[(y - 1) * width + x];
            }
            else {
                neighbor1 = magnitude[(y + 1) * width + (x + 1)];
                neighbor2 = magnitude[(y - 1) * width + (x - 1)];
            }

            // Dacă pixelul curent nu este maximul local, îl eliminăm
            unsigned char val = 0;
            if (mag >= neighbor1 && mag >= neighbor2) {
                //prag poate varia, un prag mare reduce numarul de margini detectate
                val = (mag > 70.0f) ? 255 : 0;
            }

            int outIdx = (y * width + x) * 4;
            edgeData[outIdx] = val;
            edgeData[outIdx + 1] = val;
            edgeData[outIdx + 2] = val;
            edgeData[outIdx + 3] = 255;
        }
    }

    // 6. Creare textură OpenGL
    if (cannyTexture) delete cannyTexture;
    cannyTexture = new Texture2D();

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, edgeData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    cannyTexture->Init(texID, width, height, 4);
}
void Tema2::CreateFramebuffers(int width, int height)
{
    // Stergem vechile buffere
    if (fboHorizontal) glDeleteFramebuffers(1, &fboHorizontal);
    if (textureHorizontal) glDeleteTextures(1, &textureHorizontal);
    if (fboVertical) glDeleteFramebuffers(1, &fboVertical);
    if (textureVertical) glDeleteTextures(1, &textureVertical);

    // FBO 1 (Horizontal)
    glGenFramebuffers(1, &fboHorizontal);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHorizontal);
    glGenTextures(1, &textureHorizontal);
    glBindTexture(GL_TEXTURE_2D, textureHorizontal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureHorizontal, 0);

    // FBO 2 (Vertical)
    glGenFramebuffers(1, &fboVertical);
    glBindFramebuffer(GL_FRAMEBUFFER, fboVertical);
    glGenTextures(1, &textureVertical);
    glBindTexture(GL_TEXTURE_2D, textureVertical);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureVertical, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Tema2::Update(float deltaTimeSeconds)
{
    glm::ivec2 res = window->GetResolution();
    int imgW = originalImage->GetWidth();    // Dimensiune Poză Originală
    int imgH = originalImage->GetHeight();
    float imgAspect = (float)imgW / (float)imgH;
    glm::vec2 mouseInImageSpace = glm::vec2(
        (focusPos.x / (float)res.x) * imgW,
        ((res.y - focusPos.y) / (float)res.y) * imgH
    );
    float radiusInImageSpace = (focusRadius / (float)res.x) * imgW;
    // === MOD CANNY (ACTIVAT CU 'D') ===
    if (showCanny && cannyTexture) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, res.x, res.y);
        auto shader = shaders["ScreenDisplay"];
        shader->Use();

        // Trimitem viewMode 0 (Original) pentru a afisa textura bruta
        glUniform1i(shader->GetUniformLocation("viewMode"), 0);

        cannyTexture->BindToTextureUnit(GL_TEXTURE0);
        glUniform1i(shader->GetUniformLocation("texOriginal"), 0);

        RenderMesh(meshes["quad"], shader, glm::mat4(1));
        DrawHUD();
        return; // Iesim din functie, nu mai facem blur, nici nu mai scriu text pe ecran
    }
    //taste 6-9
    if (activeFilterMode >= 6 && extraFilterTexture) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, res.x, res.y);
        auto shader = shaders["ScreenDisplay"];
        shader->Use();
        //trimit la shaderul care deseneaza pe ecran dimensiune ecran, modul de view (6-9) si imaginea de start
        // Trimitem uniformele obligatorii chiar și pentru filtre
        glUniform2f(shader->GetUniformLocation("screenSize"), (float)res.x, (float)res.y);
        glUniform1i(shader->GetUniformLocation("viewMode"), 0); // Afișăm direct textura

        extraFilterTexture->BindToTextureUnit(GL_TEXTURE0);
        glUniform1i(shader->GetUniformLocation("texOriginal"), 0);

        RenderMesh(meshes["quad"], shader, glm::mat4(1));
        DrawHUD();
        return;
    }
    //tasta 5
    if (showBlurMask) {
        // Recalculăm masca în timp real dacă s-a schimbat ceva (mouse/radius)
        if (dirtyCPU) {
            ComputeBlurMaskCPU();
            dirtyCPU = false;
        }
        glViewport(0, 0, res.x, res.y);
        if (blurMaskTexture) {
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto shader = shaders["ScreenDisplay"];
            shader->Use();
            glUniform1i(shader->GetUniformLocation("viewMode"), 0); // Mod afișare directă

            blurMaskTexture->BindToTextureUnit(GL_TEXTURE0);
            glUniform1i(shader->GetUniformLocation("texOriginal"), 0);

            RenderMesh(meshes["quad"], shader, glm::mat4(1));
        }

        // Randare text HUD deasupra măștii
        if (show_text) {
            textRenderer->RenderText("FILTER: BLUR MASK ACTIVE", 20, 30, 1.0f, glm::vec3(1, 1, 1));
        }
        DrawHUD();
        return; // Oprim execuția restului pipeline-ului
    }
    if (!useGPU) {
        bool blurIsVisible = (viewMode == 1 || viewMode == 2 || (viewMode == 4 && showBlurInFinal));
        glViewport(0, 0, res.x, res.y);
        // 1. Recalculăm pixeli DOAR dacă e nevoie de blur ȘI s-a schimbat ceva (dirtyCPU), altfel nu fac calcule inutile
        if (dirtyCPU && blurIsVisible) {
            if (quick_cpu == true) {
                ComputeCPUBlurQuick();
            }
            else
            {
                ComputeCPUBlur();
            }
            dirtyCPU = false;
            dirtyTexture = true;
        }

        // 2. Uploadăm pe GPU doar dacă avem date noi
        if (dirtyTexture && blurIsVisible) {
            UploadCPUToGPU();
            dirtyTexture = false;
        }

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto shader = shaders["ScreenDisplay"];
        shader->Use();

        int effectiveViewMode = (viewMode == 4 && !showBlurInFinal) ? 5 : viewMode;
        glUniform1i(shader->GetUniformLocation("viewMode"), effectiveViewMode);

        // Uniformele de mouse se trimit MEREU (pentru ca cercurile să fie fluide în mod 5)
        glUniform2f(shader->GetUniformLocation("screenSize"), (float)res.x, (float)res.y);
        glUniform2f(shader->GetUniformLocation("focusPos"), focusPos.x, (float)res.y - focusPos.y);
        glUniform1f(shader->GetUniformLocation("focusRadius"), focusRadius);

        // Bindings (rămân la fel ca în soluția anterioară)
        originalImage->BindToTextureUnit(GL_TEXTURE0);
        glUniform1i(shader->GetUniformLocation("texOriginal"), 0);
        cpuResultTexture->BindToTextureUnit(GL_TEXTURE1);
        glUniform1i(shader->GetUniformLocation("texPass1"), 1);
        cpuResultTexture->BindToTextureUnit(GL_TEXTURE2);
        glUniform1i(shader->GetUniformLocation("texPass2"), 2);

        RenderMesh(meshes["quad"], shader, glm::mat4(1));
        //aici randez scris pentru cpu

        if(show_text == true){
            // 1. Pregătire stare OpenGL pentru text clar (Antialiasing)
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // 2. Variabile pentru poziționare automată
            float xPos = 20.0f;
            float currentY = 30.0f;
            float stepY = 25.0f;

            // Culori pentru contrast
            glm::vec3 colorYellow = glm::vec3(1, 1, 0);
            glm::vec3 colorWhite = glm::vec3(1, 1, 1);
            glm::vec3 colorGreen = glm::vec3(0, 1, 0);
            glm::vec3 colorCyan = glm::vec3(0, 1, 1);

            // 3. Randare text linie cu linie (fără suprapuneri)

            // Linie 1: Modul curent (CPU/GPU)
            textRenderer->RenderText("Mode: " + string(useGPU ? "GPU (Fast)" : "CPU (Parallel)"), xPos, currentY, 1.0f, colorYellow);
            currentY += stepY;

            // Linie 2: Raza cercului de focus
            textRenderer->RenderText("Focus Radius: " + to_string((int)focusRadius), xPos, currentY, 1.0f, colorYellow);
            currentY += stepY;

            // Linie 3: Pass-ul vizualizat (0-4)
            textRenderer->RenderText("View Pass: " + to_string(viewMode), xPos, currentY, 1.0f, colorYellow);
            currentY += stepY;

            // Linie 4: Poziția mouse-ului
            textRenderer->RenderText("Mouse Pos: " + to_string((int)focusPos.x) + ", " + to_string((int)focusPos.y), xPos, currentY, 1.0f, colorYellow);
            currentY += stepY;

            // Linie 5: Intensitatea Blur-ului (Raza maximă setată din UP/DOWN)
            textRenderer->RenderText("Blur Intensity (Max Radius): " + to_string(blurRadius), xPos, currentY, 1.0f, colorYellow);
            currentY += stepY;
            // SAU varianta mai sugestivă pentru un HUD (ON/OFF)
            textRenderer->RenderText("Quick CPU (SAT): " + string(quick_cpu ? "ON" : "OFF"), xPos, currentY, 1.0f, colorYellow);
            // Linie 6: Status Canny (apare doar dacă e activat)
            if (showCanny) {
                textRenderer->RenderText("Filter: CANNY ACTIVE", xPos, currentY, 1.0f, colorGreen);
                currentY += stepY;
            }

            // 4. Revenire la starea inițială
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        }

        return;
    }

    // === MOD NORMAL (BLUR PIPELINE) ===

    // PASS 1: Blur Orizontal -> scrie in fboHorizontal
    glBindFramebuffer(GL_FRAMEBUFFER, fboHorizontal);
    //glViewport(0, 0, res.x, res.y);
    glViewport(0, 0, imgW, imgH);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto shaderH = shaders["BlurHorizontal"];
    shaderH->Use();
    glUniform2f(shaderH->GetUniformLocation("screenSize"), (float)res.x, (float)res.y);
    glUniform2f(shaderH->GetUniformLocation("focusPos"), focusPos.x, res.y - focusPos.y);
    glUniform1f(shaderH->GetUniformLocation("focusRadius"), focusRadius);

    glUniform1i(shaderH->GetUniformLocation("u_texture"), 0);
    originalImage->BindToTextureUnit(GL_TEXTURE0);
    glUniform1i(shaderH->GetUniformLocation("blurRadius"), blurRadius);

    RenderMesh(meshes["quad"], shaderH, glm::mat4(1));

    // PASS 2: Blur Vertical -> scrie in fboVertical
    glBindFramebuffer(GL_FRAMEBUFFER, fboVertical);
    glViewport(0, 0, imgW, imgH);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto shaderV = shaders["BlurVertical"];
    shaderV->Use();
    glUniform2f(shaderV->GetUniformLocation("screenSize"), (float)res.x, (float)res.y);

    glUniform1i(shaderV->GetUniformLocation("u_texture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureHorizontal);

    glUniform1i(shaderV->GetUniformLocation("blurRadius"), blurRadius);

    RenderMesh(meshes["quad"], shaderV, glm::mat4(1));
    // PASS 3: Ecran Final -> combina totul
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, res.x, res.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto shaderS = shaders["ScreenDisplay"];
    shaderS->Use();
    
    int effectiveViewMode = (viewMode == 4 && !showBlurInFinal) ? 5 : viewMode;
    glUniform1i(shaderS->GetUniformLocation("viewMode"), effectiveViewMode);
    //glUniform1i(shaderS->GetUniformLocation("viewMode"), viewMode);
    glUniform2f(shaderS->GetUniformLocation("screenSize"), (float)res.x, (float)res.y);
    glUniform2f(shaderS->GetUniformLocation("focusPos"), focusPos.x, res.y - focusPos.y);
    glUniform1f(shaderS->GetUniformLocation("focusRadius"), focusRadius);

    // Bind textures: 0 = Original, 1 = Pass1, 2 = Pass2
    glUniform1i(shaderS->GetUniformLocation("texOriginal"), 0);
    originalImage->BindToTextureUnit(GL_TEXTURE0);

    glUniform1i(shaderS->GetUniformLocation("texPass1"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureHorizontal);

    glUniform1i(shaderS->GetUniformLocation("texPass2"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureVertical);

    RenderMesh(meshes["quad"], shaderS, glm::mat4(1));


    if (show_text == true) {
        // 1. Pregătirea stării OpenGL pentru text
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 2. Definirea variabilelor de poziționare
        float xPos = 20.0f;
        float currentY = 30.0f;
        float stepY = 25.0f;

        // Culori
        glm::vec3 colorYellow = glm::vec3(1, 1, 0);
        glm::vec3 colorWhite = glm::vec3(1, 1, 1);
        glm::vec3 colorGreen = glm::vec3(0, 1, 0);

        // 3. Randarea organizată (linie cu linie)
        textRenderer->RenderText("Mode: " + string(useGPU ? "GPU" : "CPU"), xPos, currentY, 1.0f, colorYellow);
        currentY += stepY;

        textRenderer->RenderText("Focus Radius: " + to_string((int)focusRadius), xPos, currentY, 1.0f, colorYellow);
        currentY += stepY;

        textRenderer->RenderText("View Pass: " + to_string(viewMode), xPos, currentY, 1.0f, colorYellow);
        currentY += stepY;

        textRenderer->RenderText("Mouse Pos: " + to_string((int)focusPos.x) + ", " + to_string((int)focusPos.y), xPos, currentY, 1.0f, colorYellow);
        currentY += stepY;

        textRenderer->RenderText("Blur Radius: " + to_string(blurRadius), xPos, currentY, 1.0f, colorYellow);
        currentY += stepY;

        if (showCanny) {
            textRenderer->RenderText("Filter: CANNY ACTIVE", xPos, currentY, 1.0f, colorYellow);
            currentY += stepY;
        }

        // 4. Revenirea la starea inițială
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }
}

void Tema2::OnKeyPress(int key, int mods)
{
    //F deschide fereastra de dialog pentru a adauga o alta poza
    if (key == GLFW_KEY_F) OpenDialog();
    
    //E face toggle intre cpu si gpu
    if (key == GLFW_KEY_E) {
        useGPU = !useGPU;
        cout << "Mode switched to: " << (useGPU ? "GPU" : "CPU") << endl;
        if (useGPU == false)
        {
            //ComputeCPUBlur();
            dirtyCPU = true;
        }
    }
    //tasta pentru a alege intre cpu rapid si cpu lent in cazul blurului
    if (key == GLFW_KEY_Q)
    {
        quick_cpu = !quick_cpu;
        dirtyCPU = true;
        cout << quick_cpu << "\n";
    }
    //taste pentru oil painting, sketch, thermal view, filtru bilateral
    if (key >= GLFW_KEY_6 && key <= GLFW_KEY_9) {
        activeFilterMode = key - GLFW_KEY_0;
        showCanny = false; showBlurMask = false;
        if (!useGPU)
        {
            dirtyCPU = true;
        }
        if (activeFilterMode == 6) ApplyOilPainting(4, 20);
        if (activeFilterMode == 7) ApplyPencilSketch();
        if (activeFilterMode == 8) ApplyThermalVision();
        if (activeFilterMode == 9) ApplyBilateralFilter(4.0f, 30.0f);
    }
    // Schimba modurile de vizualizare pentru Blur (0=Orig, 1=BlurH, 2=BlurV, 4=Final)
    if (key >= GLFW_KEY_0 && key < GLFW_KEY_4) {
        viewMode = key - GLFW_KEY_0;
        activeFilterMode = 0;
        showBlurMask = false;
        showCanny = false;

        // === FIX ===
        // Semnalizăm că vrem să vedem alt buffer, deci trebuie upload pe GPU
        //cout << "Click" << "\n";
        if (!useGPU) {
            dirtyTexture = true;
        }

    }
    if (key == GLFW_KEY_4) {
        if (viewMode == 4) {
            showBlurMask = false;
            showCanny = false;
            // Dacă suntem deja pe 4, facem toggle între Blur și Original
            showBlurInFinal = !showBlurInFinal;
            activeFilterMode = 0;
        }
        else {
            // Dacă venim din alt mod, activăm modul 4 cu blur-ul activ
            viewMode = 4;
            showBlurInFinal = true;
        }

        if (!useGPU) {
            dirtyTexture = true;
        }

        cout << "Final Mode: " << (showBlurInFinal ? "Blur + Circles" : "Original + Circles") << endl;
    }

    // Toggle Canny Filter
    if (key == GLFW_KEY_D) {
        showCanny = !showCanny;
        if (showCanny) {
            ApplyCannyFilter();
        }
    }

    //maresc kernelul de blur, fac blurul mult mai puternic
    if (key == GLFW_KEY_UP) {
        blurRadius = std::min(ABSOLUTE_MAX_RADIUS, blurRadius + 2);
        dirtyCPU = true; // Forțăm recalcularea kernelului pe CPU
        cout << "Blur Radius: " << blurRadius << endl;
    }


    //scade blurul
    if (key == GLFW_KEY_DOWN) {
        blurRadius = std::max(1, blurRadius - 2);
        dirtyCPU = true;
        cout << "Blur Radius: " << blurRadius << endl;
    }
    //daca vreau sa nu mai vad textul
    if (key == GLFW_KEY_T)
    {
        show_text = !show_text;
    }
    //maresc focus radius, adica zona clara
    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
    {
        focusRadius += 5.0f;
        if (!useGPU || showBlurMask) dirtyCPU = true;
    }
    //scad focus radius
    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) {
        focusRadius = std::max(0.0f, focusRadius - 5.0f);
        if (!useGPU || showBlurMask) dirtyCPU = true;
    }
    //tasta 5 e pentru a vedea alb-negru, unde aplic filtrul gaussian
    if (key == GLFW_KEY_5) {
        showBlurMask = !showBlurMask;
        showCanny = false; // Dezactivăm Canny dacă activăm Masca
        activeFilterMode = 0;
        if (showBlurMask) {
            dirtyCPU = true; // Forțăm prima generare
        }
        cout << "Blur Mask: " << (showBlurMask ? "ON" : "OFF") << endl;
    }
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    //daca mut mouse-ul si sunt pe cpu pe modul 5, trebuie recalculata poza
    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT)) {
        focusPos = glm::vec2(mouseX, mouseY);
        if (!useGPU || showBlurMask) {
            dirtyCPU = true;
        }
    }
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    //si aici fac recalculare
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        focusPos = glm::vec2(mouseX, mouseY);
        // ADAUGĂ: showBlurMask aici
        if (!useGPU || showBlurMask) {
            dirtyCPU = true;
        }
    }
}

void Tema2::OnWindowResize(int width, int height)
{
    //CreateFramebuffers(width, height);
    //pe gpu se face automat, pe cpu trebuie facuta recalcularea pentru a functiona corect
    dirtyCPU = true;
}

void Tema2::OnFileSelected(const std::string& fileName)
{
    if (!fileName.empty()) {
        originalImage = TextureManager::LoadTexture(fileName, nullptr, "image", true, true);

        int w = originalImage->GetWidth();
        int h = originalImage->GetHeight();

        // 1. RECREARE FRAMEBUFFERS GPU (Esențial pentru rezolvarea bug-ului)
        // Fără asta, GPU randează la rezoluția pozei anterioare
        CreateFramebuffers(w, h);

        // 2. ACTUALIZARE DATE CPU
        hostPixelData.resize(w * h * 4);
        glBindTexture(GL_TEXTURE_2D, originalImage->GetTextureID());
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, hostPixelData.data());
        glBindTexture(GL_TEXTURE_2D, 0);

        // 3. RE-INIȚIALIZARE TEXTURĂ REZULTAT CPU
        // Trebuie să ștergem ID-ul vechi și să generăm unul nou pentru noile dimensiuni
        if (cpuResultTexture) {
            GLuint oldTexID = cpuResultTexture->GetTextureID();
            glDeleteTextures(1, &oldTexID);

            GLuint newTexID;
            glGenTextures(1, &newTexID);
            glBindTexture(GL_TEXTURE_2D, newTexID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, hostPixelData.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            cpuResultTexture->Init(newTexID, w, h, 4);
        }

        if (activeFilterMode >= 6) {
            if (activeFilterMode == 6) ApplyOilPainting(4, 20);
            else if (activeFilterMode == 7) ApplyPencilSketch();
            else if (activeFilterMode == 8) ApplyThermalVision();
            else if (activeFilterMode == 9) ApplyBilateralFilter(4.0f, 30.0f);
        }

        // 4. RESETARE FILTRE ȘI MASCI
        if (showCanny) ApplyCannyFilter();
        if (blurMaskTexture) ComputeBlurMaskCPU();
        dirtyCPU = true;

        // 5. RESETARE INTERFAȚĂ
        float ar = static_cast<float>(w) / h;
        window->SetSize(static_cast<int>(600 * ar), 600);
        glm::ivec2 res = window->GetResolution();
        focusPos = glm::vec2(res.x / 2.0f, res.y / 2.0f);
    }
}

//aici pot incarca poze care au extensiile de mai jos
void Tema2::OpenDialog()
{
    std::vector<std::string> filters = { "Image Files", "*.png *.jpg *.jpeg *.bmp", "All Files", "*" };
    auto selection = pfd::open_file("Select a file", ".", filters).result();
    if (!selection.empty()) OnFileSelected(selection[0]);
}

void Tema2::FrameStart() {}
void Tema2::FrameEnd() { DrawCoordinateSystem(); }
void Tema2::OnInputUpdate(float deltaTime, int mods) {}
void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {}
void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}
void Tema2::OnKeyRelease(int key, int mods) {}