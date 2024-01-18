#include "Renderer.h"
#include <iostream>
#include <array>
#include "GLHelper.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>
#include "Texture.h"
#include <glm/gtx/compatibility.hpp>


static const size_t MaxQuadCount = 1000;
static const size_t MaxVertexCount = MaxQuadCount * 4;
static const size_t MaxIndexCount = MaxQuadCount * 6;
//todo find the actual texture count
static const size_t MaxTextures = 32;

struct v3 {
    float x;
    float y;
    float z;
};
struct v2 {
    float x;
    float y;
};

struct Vertex {
    v2 Position;
    v2 TexCoords;
    float TexIndex;
};
// Vertices for a simple quad
//Vertex vertices[] = {
//    // Position             // Texture Coordinates   // Texture Index
//    { { -0.5f,  0.5f}, { 0.0f, 0.0f }, 0.0f }, // Top-left
//    { {  0.5f,  0.5f}, { 0.0f, 0.0f }, 0.0f }, // Top-right
//    { {  0.5f, -0.5f}, { 0.0f, 0.0f }, 0.0f }, // Bottom-right
//    { { -0.5f, -0.5f}, { 0.0f, 0.0f }, 0.0f }  // Bottom-left
//};

unsigned int* createQuadIndices(unsigned int quadCount) {
    unsigned int numIndices = quadCount*6; // 6 indices per quad
    unsigned int* indices = (unsigned int*)malloc(numIndices * sizeof(unsigned int));

    if (indices == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    for (unsigned int i = 0; i < quadCount; ++i) {
        // The offset for each quad's vertices
        unsigned int offset = i * 4;

        // First triangle of the quad
        indices[i * 6 + 0] = offset + 0;
        indices[i * 6 + 1] = offset + 1;
        indices[i * 6 + 2] = offset + 2;

        // Second triangle of the quad
        indices[i * 6 + 3] = offset + 2;
        indices[i * 6 + 4] = offset + 3;
        indices[i * 6 + 5] = offset + 0;
    }

    return indices;
}

struct RendererData {
    uint32_t IndexCount = 0;
    Vertex* QuadBuffer = nullptr;
    Vertex* QuadBufferPtr = nullptr;

    std::array<GLuint, MaxTextures> TextureSlots;
    uint32_t TextureSlotIndex = 1;

    VertexBuffer vb;
    Shader shader;
    VertexArray va;
    IndexBuffer ib;
    RendererData()
    {
    }
};

static RendererData s_Data;



void Renderer::Init()
{
    

    s_Data.va.Init();
    


    //s_Data.vb.Init( MaxVertexCount * sizeof(Vertex));
    //s_Data.vb.Init(vertices, MaxVertexCount * sizeof(Vertex));
    s_Data.vb.Init(MaxVertexCount * sizeof(Vertex));
    VertexBufferLayout layout;
    layout.Push<GL_FLOAT>(2);
    layout.Push<GL_FLOAT>(2);
    layout.Push<GL_FLOAT>(1);
    s_Data.va.AddBuffer(s_Data.vb, layout);


    unsigned int* indices = createQuadIndices(MaxQuadCount);

    s_Data.ib.Init(indices, MaxIndexCount * sizeof(unsigned int));

    //gonna do this in a camera class maybe
    float ratio = static_cast<float>(800) / static_cast<float>(600);
    float camSize = 20.f;
    float camX = camSize * ratio;


    glm::mat4 proj = glm::ortho(-camX, camX, -camSize, camSize, -1.0f, 1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    glm::mat4 mvp = proj * view;
    

    //s_Data.vb = &vb;
    s_Data.shader.Init("./res/shaders/Basic.shader");
    s_Data.shader.Bind();

    GLuint texID = Texture::createWhiteTexture();
    Texture::Bind(texID,0);
    s_Data.TextureSlots[0] = texID;
    s_Data.shader.SetUniformMat4f("u_MVP", mvp);
    //int samplers[2] = { 0,1 };
    int samplers[32] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
    s_Data.shader.SetUniform1iv("u_Textures", samplers, 32);

    for (int i = 1; i < 32; i++ ) {
        s_Data.TextureSlots[i] = 0;
    }


    s_Data.QuadBuffer = new Vertex[MaxVertexCount];
}

void Renderer::Shutdown()
{
    delete[] s_Data.QuadBuffer;
}

void Renderer::Clear()
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw()
{
    s_Data.shader.Bind();
    s_Data.va.Bind();
    s_Data.ib.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, s_Data.IndexCount, GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawQuad(float2 position, float2 size, float rot)
{
    if (s_Data.IndexCount >= MaxIndexCount) {
        EndBatch();
        Flush();
        BeginBatch();

    }

    float textureIndex = 0.0f;

    float cosRot = cos(rot);
    float sinRot = sin(rot);

    
    // Apply rotation to each vertex
    float x = ( - 0.5f * size.x) * cosRot - (-0.5f * size.y) * sinRot;
    float y = ( - 0.5f * size.x) * sinRot + (-0.5f * size.y) * cosRot;

    // Apply translation
    x += position.x;
    y += position.y;

    // Fill the QuadBuffer
    s_Data.QuadBufferPtr->Position = { x, y };
    s_Data.QuadBufferPtr->TexCoords = { 0 , 0 };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    x = (0.5f * size.x) * cosRot - (-0.5f * size.y) * sinRot;
    y = (0.5f * size.x) * sinRot + (-0.5f * size.y) * cosRot;

    // Apply translation
    x += position.x;
    y += position.y;

    // Fill the QuadBuffer
    s_Data.QuadBufferPtr->Position = { x, y };
    s_Data.QuadBufferPtr->TexCoords = { 1 , 0 };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;


    x = (0.5f * size.x) * cosRot - (0.5f * size.y) * sinRot;
    y = (0.5f * size.x) * sinRot + (0.5f * size.y) * cosRot;

    // Apply translation
    x += position.x;
    y += position.y;

    // Fill the QuadBuffer
    s_Data.QuadBufferPtr->Position = { x, y };
    s_Data.QuadBufferPtr->TexCoords = { 1 , 1 };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;


    x = (-0.5f * size.x) * cosRot - (0.5f * size.y) * sinRot;
    y = (-0.5f * size.x) * sinRot + (0.5f * size.y) * cosRot;

    // Apply translation
    x += position.x;
    y += position.y;

    // Fill the QuadBuffer
    s_Data.QuadBufferPtr->Position = { x, y };
    s_Data.QuadBufferPtr->TexCoords = { 0 , 1 };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;
    s_Data.IndexCount += 6;

}
//todo add new Method for sprite atlas with different texture coords

void Renderer::DrawQuad(uint32_t texture, float2 position, float2 size, float rot, float2 ul, float2 br)
{
    if (s_Data.IndexCount >= MaxIndexCount || s_Data.TextureSlotIndex > 31) {
        EndBatch();
        Flush();
        BeginBatch();

    }



    float textureIndex = 0.0f;

    for (int i = 1; i < s_Data.TextureSlotIndex; i++) {
        if (s_Data.TextureSlots[i] == texture) {
            textureIndex = (float)i;
            break;
        }
    }
    if (textureIndex == 0.0f) {
        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
        s_Data.TextureSlotIndex++;
    }

    float cosRot = cos(rot);
    float sinRot = sin(rot);


    // Apply rotation to each vertex
    float x = (-0.5f * size.x) * cosRot - (-0.5f * size.y) * sinRot;
    float y = (-0.5f * size.x) * sinRot + (-0.5f * size.y) * cosRot;

    // Apply translation
    x += position.x;
    y += position.y;

    // Fill the QuadBuffer
    s_Data.QuadBufferPtr->Position = { x, y };
    s_Data.QuadBufferPtr->TexCoords = { ul.x , br.y };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;

    x = (0.5f * size.x) * cosRot - (-0.5f * size.y) * sinRot;
    y = (0.5f * size.x) * sinRot + (-0.5f * size.y) * cosRot;

    // Apply translation
    x += position.x;
    y += position.y;

    // Fill the QuadBuffer
    s_Data.QuadBufferPtr->Position = { x, y };
    s_Data.QuadBufferPtr->TexCoords = { br.x , br.y };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;


    x = (0.5f * size.x) * cosRot - (0.5f * size.y) * sinRot;
    y = (0.5f * size.x) * sinRot + (0.5f * size.y) * cosRot;

    // Apply translation
    x += position.x;
    y += position.y;

    // Fill the QuadBuffer
    s_Data.QuadBufferPtr->Position = { x, y };
    s_Data.QuadBufferPtr->TexCoords = { br.x , ul.y };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;


    x = (-0.5f * size.x) * cosRot - (0.5f * size.y) * sinRot;
    y = (-0.5f * size.x) * sinRot + (0.5f * size.y) * cosRot;

    // Apply translation
    x += position.x;
    y += position.y;

    // Fill the QuadBuffer
    s_Data.QuadBufferPtr->Position = { x, y };
    s_Data.QuadBufferPtr->TexCoords = { ul.x , ul.y };
    s_Data.QuadBufferPtr->TexIndex = textureIndex;
    s_Data.QuadBufferPtr++;
    s_Data.IndexCount += 6;

}

void Renderer::BeginBatch()
{
    s_Data.QuadBufferPtr = s_Data.QuadBuffer;
}

void Renderer::EndBatch()
{
    GLsizeiptr size = (uint8_t*)s_Data.QuadBufferPtr - (uint8_t*)s_Data.QuadBuffer;
    s_Data.vb.Update(size,s_Data.QuadBuffer);
}

void Renderer::Flush()
{
    for (GLuint i = 0; i < s_Data.TextureSlotIndex; i++) {
        //Texture::Bind(s_Data.TextureSlots[i], i);
        GLCall(glBindTextureUnit(i, s_Data.TextureSlots[i]));
    }

    Draw();

    s_Data.IndexCount = 0;
    s_Data.TextureSlotIndex = 1;

}
