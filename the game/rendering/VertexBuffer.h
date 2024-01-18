#pragma once
#include "GLHelper.h"

class VertexBuffer {
public:
	VertexBuffer() = default;
	void Init(const void* data, unsigned int size);
	void Init(unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;

	void Update(GLsizeiptr size, void* data) const;
private:
	unsigned int m_RendererId;
};