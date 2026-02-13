#include "lab_m2/lab7/lab7.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m2;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab7::Lab7()

{

}





Lab7::~Lab7()

{

}


void Lab7::Init()
{
    auto camera = GetSceneCamera();
    camera->SetPositionAndRotation(glm::vec3(0, 3.5, 4), glm::quat(glm::vec3(-30 * TO_RADIANS, 0, 0)));
    camera->Update();

    // Create a shader program for rendering to texture
    {
        Shader* shader = new Shader("Skinning");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Lab7", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Lab7", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Load a mesh from file into GPU memory
    {
        Mesh* mesh = new Mesh("animation");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "skinning"), "boblampclean.md5mesh");
        meshes[mesh->GetMeshID()] = mesh;
    }
}


void Lab7::FrameStart()
{
}

void Lab7::Update(float deltaTimeSeconds)
{
    ClearScreen();
    auto shader = shaders["Skinning"];
    shader->Use();

    glm::mat4 modelMatrix = glm::mat4(1);

    float runningTime = (float)((double)Engine::GetElapsedTime());
    BoneTransform(meshes["animation"], runningTime);

    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(-90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
    RenderSimpleMesh(meshes["animation"], shader, modelMatrix);
}

void Lab7::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glm::mat4 bones[200] = { glm::mat4(0) };

    for (int i = 0; i < mesh->m_BoneInfo.size(); i++)
    {
        bones[i] = mesh->m_BoneInfo[i].finalTransformation;
    }

    // TODO (student): Send the bone final transformation to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "bones"), mesh->m_BoneInfo.size(), GL_FALSE, glm::value_ptr(bones[0]));
    mesh->Render();
}

void Lab7::BoneTransform(Mesh* mesh, float timeInSeconds)
{
    glm::mat4 Identity = glm::mat4(1.0f);

    // Compute the duration of the animation
    float ticksPerSecond = mesh->anim[0]->mTicksPerSecond != 0 ? mesh->anim[0]->mTicksPerSecond : 25.0f;
    float timeInTicks = timeInSeconds * ticksPerSecond;
    float animationTime = fmod(timeInTicks, mesh->anim[0]->mDuration);

    // Compute the final transformations for each bone at the current time stamp
    // starting from the root node
    // TODO (student): Uncomment this to test the animation
    ReadNodeHierarchy(mesh, animationTime, mesh->rootNode, Identity, mesh->anim);
}

void Lab7::ReadNodeHierarchy(Mesh* mesh, float animationTime, const aiNode* pNode, const glm::mat4& parentTransform, aiAnimation** anim)
{
    std::string NodeName(pNode->mName.data);
    const aiAnimation* pAnimation = anim[0];
    glm::mat4 nodeTransformation(mesh->ConvertMatrix(pNode->mTransformation));
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
    animationTime = animationTime / 1.2f;
    if (pNodeAnim) {
        // Interpolate the scaling and generate the scaling transformation matrix
        aiVector3D Scaling;
        ComputeInterpolatedScaling(Scaling, animationTime, pNodeAnim);
        glm::mat4 ScalingM = glm::transpose(glm::mat4(
            Scaling.x, 0, 0, 0,
            0, Scaling.y, 0, 0,
            0, 0, Scaling.z, 0,
            0, 0, 0, 1));

        // Interpolate the rotation and generate the rotation transformation matrix
        aiQuaternion RotationQ;
        ComputeInterpolatedRotation(RotationQ, animationTime, pNodeAnim);
        glm::quat rotation(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z);
        glm::mat4 RotationM = glm::toMat4(rotation);

        // Interpolate the translation and generate the translation transformation matrix
        aiVector3D Translation;
        ComputeInterpolatedPosition(Translation, animationTime, pNodeAnim);
        glm::mat4 TranslationM = glm::transpose(glm::mat4(
            1, 0, 0, Translation.x,
            0, 1, 0, Translation.y,
            0, 0, 1, Translation.z,
            0, 0, 0, 1));

        // === ANIMAȚII CUSTOM BAZATE PE BONE INDEX ===
        if (mesh->m_BoneMapping.find(NodeName) != mesh->m_BoneMapping.end()) {
            unsigned int BoneIndex = mesh->m_BoneMapping[NodeName];

            // --- ANIMAȚIE PENTRU CAPUL (presupunem că e bone index 5-10) ---
            if (BoneIndex >= 5 && BoneIndex <= 8) {
                float headCycleDuration = 4.0f;
                float headPhase = fmod(animationTime, headCycleDuration);
                float headRotationY = 0.0f;

                if (headPhase < 2.0f) {
                    // Rotație spre dreapta
                    headRotationY = (headPhase / 2.0f) * glm::radians(90.0f);
                }
                else {
                    // Rotație înapoi
                    headRotationY = (1.0f - (headPhase - 2.0f) / 2.0f) * glm::radians(90.0f);
                }

                glm::quat extraHeadRot = glm::angleAxis(headRotationY, glm::vec3(0, 1, 0));
                rotation = extraHeadRot * rotation;
                RotationM = glm::toMat4(rotation);
            }

            if (BoneIndex >= 15 && BoneIndex <= 20) {

                float speed = 0.4f;
                float amplitudeX = 1.0f;
                float amplitudeY = 1.0f;

                float phase = animationTime * speed;

                Translation.x += sin(phase) * amplitudeX;
                Translation.y += sin(phase) * amplitudeY;

                TranslationM = glm::translate(glm::mat4(1.0f), glm::vec3(Translation.x, Translation.y, Translation.z));
            }
            if (BoneIndex >= 21 && BoneIndex <= 25) {

                float speed = 0.4f;
                float amplitudeX = 1.0f;
                float amplitudeY = 1.0f;

                float phase = animationTime * speed + 3.14159f; // inversarea ciclului

                Translation.x += sin(phase) * amplitudeX;
                Translation.y += sin(phase) * amplitudeY;

                TranslationM = glm::translate(glm::mat4(1.0f), glm::vec3(Translation.x, Translation.y, Translation.z));
            }


            // --- ANIMAȚIE PENTRU MÂNA STÂNGĂ (presupunem bone index 21-26) ---
            /*if (BoneIndex >= 21 && BoneIndex <= 26) {
                // Balansare pe axa Z
                float leftArmSwing = sin(animationTime * 2.0f) * glm::radians(30.0f);
                glm::quat leftArmRot = glm::angleAxis(leftArmSwing, glm::vec3(0, 0, 1));
                rotation = leftArmRot * rotation;
                RotationM = glm::toMat4(rotation);
            }

            // --- ANIMAȚIE PENTRU PICIOARE (presupunem bone index 27-35) ---
            if (BoneIndex >= 27 && BoneIndex <= 55) {
                // Pas exagerat - rotație pe X
                float legPhase = (BoneIndex % 2 == 0) ? animationTime : animationTime + 3.14159f;
                float legRotation = sin(legPhase * 1.5f) * glm::radians(45.0f);
                glm::quat legRot = glm::angleAxis(legRotation, glm::vec3(1, 0, 0));
                rotation = legRot * rotation;
                RotationM = glm::toMat4(rotation);
            }

            // --- ANIMAȚIE PENTRU COLOANA VERTEBRALĂ/CORP (presupunem bone index 1-4) ---
            if (BoneIndex >= 1 && BoneIndex <= 4) {
                // Rotație subtilă pe Y pentru twist
                float spineRotY = sin(animationTime * 1.2f) * glm::radians(10.0f);
                glm::quat spineRot = glm::angleAxis(spineRotY, glm::vec3(0, 1, 0));
                rotation = spineRot * rotation;
                RotationM = glm::toMat4(rotation);

                // Balansare verticală
                float bobbing = sin(animationTime * 2.0f) * 0.05f;
                Translation.y += bobbing;
                TranslationM = glm::transpose(glm::mat4(
                    1, 0, 0, Translation.x,
                    0, 1, 0, Translation.y,
                    0, 0, 1, Translation.z,
                    0, 0, 0, 1));
            }*/
        }

        // Combine the above transformations
        nodeTransformation = TranslationM * RotationM * ScalingM;
    }

    // Apply the parent transformation to the current transformation
    glm::mat4 GlobalTransformation = parentTransform * nodeTransformation;

    if (mesh->m_BoneMapping.find(NodeName) != mesh->m_BoneMapping.end()) {
        unsigned int BoneIndex = mesh->m_BoneMapping[NodeName];
        mesh->m_BoneInfo[BoneIndex].finalTransformation = mesh->m_GlobalInverseTransform * GlobalTransformation *
            mesh->m_BoneInfo[BoneIndex].boneOffset;
    }

    // Compute the transformations of the children of the current node
    for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
        ReadNodeHierarchy(mesh, animationTime, pNode->mChildren[i], GlobalTransformation, anim);
    }
}
/*void Lab7::ReadNodeHierarchy(Mesh* mesh, float animationTime, const aiNode* pNode, const glm::mat4& parentTransform, aiAnimation** anim)
{
    std::string NodeName(pNode->mName.data);
    glm::mat4 nodeTransformation(mesh->ConvertMatrix(pNode->mTransformation));

    // Animație procedurală complexă - mișcare în spirală cu rotații multiple
    float t = animationTime;

    // === ROTAȚII COMPLEXE PE MULTIPLE AXE ===
    // Rotație principală continuă pe Y
    float mainRotY = t/200.f * 1.5f;

    // Rotații oscilante pe X și Z pentru efect de "tumbling"
    float oscRotX = sin(t/200.f * 2.3f) * 0.8f + cos(t/200.f * 1.7f) * 0.4f;
    float oscRotZ = cos(t/200.f * 2.1f) * 0.6f + sin(t/200.f * 1.3f) * 0.3f;

    // Rotație suplimentară bazată pe poziția în ierarhie (pentru varietate)
    float hierarchyFactor = (float)(pNode->mNumChildren + 1) * 0.1f;
    float extraRotY = sin(t/200.f * 3.0f + hierarchyFactor) * 0.5f;

    // Compune quaternion-urile pentru rotație fluidă
    glm::quat qY_main = glm::angleAxis(mainRotY, glm::vec3(0, 1, 0));
    glm::quat qX_osc = glm::angleAxis(oscRotX, glm::vec3(1, 0, 0));
    glm::quat qZ_osc = glm::angleAxis(oscRotZ, glm::vec3(0, 0, 1));
    glm::quat qY_extra = glm::angleAxis(extraRotY, glm::vec3(0, 1, 0));

    // Combină toate rotațiile
    glm::quat finalRotation = qY_main * qX_osc * qZ_osc * qY_extra;
    glm::mat4 RotationM = glm::toMat4(finalRotation);

    // === TRANSLAȚIE COMPLEXĂ - TRAIECTORIE ÎN FORMĂ DE SPIRALĂ 3D ===
    // Spirală principală pe planul XZ
    float spiralRadius = 3.0f + sin(t/100.f * 0.8f) * 0.5f; // Rază variabilă
    float spiralSpeed = 1.2f/100.f;
    float spiralHeight = 0.3f/100.f;

    float baseX = spiralRadius * cos(t/200.f * spiralSpeed);
    float baseZ = spiralRadius * sin(t/200.f * spiralSpeed);

    // Componente suplimentare pentru mișcare mai complexă
    float secondaryX = cos(t/200.f * 2.5f) * 0.8f;
    float secondaryZ = sin(t/200.f * 3.2f) * 0.8f;

    // Mișcare verticală - combinație de unde pentru efect de "zbor"
    float wave1 = sin(t/200.f * 1.8f) * 1.2f;
    float wave2 = cos(t/200.f * 2.7f) * 0.6f;
    float wave3 = sin(t/200.f * 4.1f) * 0.3f;
    float baseY = wave1 + wave2 + wave3 + spiralHeight * t;

    // Adaugă un "bounce" periodic
    float bouncePhase = fmod(t/200.f, 2.0f);
    float bounce = 0.0f;
    if (bouncePhase < 1.0f) {
        bounce = abs(sin(bouncePhase * 3.14159f/200.f)) * 0.5f;
    }

    // Poziție finală
    float tx = baseX + secondaryX;
    float ty = baseY + bounce;
    float tz = baseZ + secondaryZ;

    glm::mat4 TranslationM = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz));

    // === TRANSFORMARE FINALĂ ===
    nodeTransformation = TranslationM * RotationM;

    // Aplică transformarea părintelui la transformarea curentă
    glm::mat4 GlobalTransformation = parentTransform * nodeTransformation;

    if (mesh->m_BoneMapping.find(NodeName) != mesh->m_BoneMapping.end()) {
        unsigned int BoneIndex = mesh->m_BoneMapping[NodeName];
        mesh->m_BoneInfo[BoneIndex].finalTransformation =
            mesh->m_GlobalInverseTransform * GlobalTransformation *
            mesh->m_BoneInfo[BoneIndex].boneOffset;
    }

    // Procesează copiii nodului curent
    for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
        ReadNodeHierarchy(mesh, animationTime, pNode->mChildren[i], GlobalTransformation, anim);
    }
}*/

const aiNodeAnim* Lab7::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
    for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return NULL;
}

unsigned int Lab7::FindRotation(float animationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    // TODO (student): Inside the aiNodeAnim variable we have the mRotationKeys array which stores
    // the rotation transformation of the vertex at different time stamps. The data is sorted. Find
    // the last rotation transformation which is at a lower time stamp so that we can compute the
    // interpolation of the rotation transformation of the bone. Return the index.
    for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (animationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

void Lab7::ComputeInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumRotationKeys == 1) {
        out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    // Find the time stamps at which we should interpolate
    unsigned int rotationIndex = FindRotation(animationTime, pNodeAnim);
    unsigned int nextRotationIndex = (rotationIndex + 1);
    assert(nextRotationIndex < pNodeAnim->mNumRotationKeys);

    // TODO (student): Compute the duration between the two selected time stamps
    // HINT! An entry from the mRotationKeys array has an mTime field
    float deltaTime = (float)(pNodeAnim->mRotationKeys[nextRotationIndex].mTime - pNodeAnim->mRotationKeys[rotationIndex].mTime);

    // Compute the factor of interpolation for a frame
    float factor = (animationTime - (float)pNodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);

    // TODO (student): Compute the final rotation factor by interpolating the values from the two selected key frames
    // HINT! Use the Interpolate method from the aiQuaternion class! Don't forget to normalize the result!
    // Save the result in the out variable
    const aiQuaternion startRotationQ = pNodeAnim->mRotationKeys[rotationIndex].mValue;
    const aiQuaternion endRotationQ = pNodeAnim->mRotationKeys[nextRotationIndex].mValue;
    aiQuaternion::Interpolate(out, startRotationQ, endRotationQ, factor);
    out = out.Normalize();
}

unsigned int Lab7::FindScaling(float animationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    // TODO (student): Inside the aiNodeAnim variable we have the mScalingKeys array which stores
    // the scaling transformation of the vertex at different time stamps. The data is sorted. Find
    // the last scale transformation which is at a lower time stamp so that we can compute the
    // interpolation of the scaling transformation of the bone. Return the index.
    for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (animationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

void Lab7::ComputeInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    // Find the time stamps at which we should interpolate
    unsigned int scalingIndex = FindScaling(animationTime, pNodeAnim);
    unsigned int nextScalingIndex = (scalingIndex + 1);
    assert(nextScalingIndex < pNodeAnim->mNumScalingKeys);

    // TODO (student): Compute the duration between the two selected time stamps
    // HINT! An entry from the mScalingKeys array has an mTime field
    float deltaTime = (float)(pNodeAnim->mScalingKeys[nextScalingIndex].mTime - pNodeAnim->mScalingKeys[scalingIndex].mTime);

    // Compute the factor of interpolation for a frame
    float factor = (animationTime - (float)pNodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);

    // TODO (student): Compute the final scale factor by interpolating the values from the two selected key frames
    // Save the result in the out variable
    const aiVector3D startScaling = pNodeAnim->mScalingKeys[scalingIndex].mValue;
    const aiVector3D endScaling = pNodeAnim->mScalingKeys[nextScalingIndex].mValue;
    //aiQuaternion::Interpolate(out, startScaling, endScaling, factor);
    out = startScaling + factor * (endScaling - startScaling);
}

unsigned int Lab7::FindPosition(float animationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumPositionKeys > 0);

    // TODO (student): Inside the aiNodeAnim variable we have the mPositionKeys array which stores
    // the translation transformation of the vertex at different time stamps. The data is sorted. Find
    // the last translation transformation which is at a lower time stamp so that we can compute the
    // interpolation of the translation transformation of the bone. Return the index.
    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (animationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

void Lab7::ComputeInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    // Find the time stamps at which we should interpolate
    unsigned int positionIndex = FindPosition(animationTime, pNodeAnim);
    unsigned int nextPositionIndex = (positionIndex + 1);
    assert(nextPositionIndex < pNodeAnim->mNumPositionKeys);

    // TODO (student): Compute the duration between the two selected time stamps
    // HINT! An entry from the mPositionKeys array has an mTime field
    float deltaTime = (float)(pNodeAnim->mPositionKeys[nextPositionIndex].mTime - pNodeAnim->mPositionKeys[positionIndex].mTime);

    // Compute the factor of interpolation for a frame
    float factor = (animationTime - (float)pNodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);

    // TODO (student): Compute the final translation factor by interpolating the values from the two selected key frames
    // Save the result in the out variable
    const aiVector3D startPosition = pNodeAnim->mPositionKeys[positionIndex].mValue;
    const aiVector3D endPosition = pNodeAnim->mPositionKeys[nextPositionIndex].mValue;
    //aiQuaternion::Interpolate(out, startPosition, endPosition, factor);
    out = startPosition + factor * (endPosition - startPosition);
}

void Lab7::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab7::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input with window->KeyHold()
}


void Lab7::OnKeyPress(int key, int mods)
{
    // Add key press event

}


void Lab7::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab7::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab7::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab7::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab7::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab7::OnWindowResize(int width, int height)
{
    // Treat window resize event
}