#pragma once
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"

class VertexArray {
private:
	unsigned int m_RendererID;
public:
	VertexArray() = default;
	~VertexArray();

	void Init();
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind() const;
	void Unbind() const;
};