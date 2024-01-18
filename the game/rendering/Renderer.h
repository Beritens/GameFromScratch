#pragma once


#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "../utility.h"




class Renderer {
public:
    static void Init();
    static void Shutdown();
    static void Clear();
    static void Draw();
    static void DrawQuad(float2 position, float2 size, float rot);
    static void DrawQuad(uint32_t texture, float2 position, float2 size, float rot, float2 ul, float2 br);
    static void BeginBatch(); 
    static void EndBatch();
    static void Flush();
private:
    
};