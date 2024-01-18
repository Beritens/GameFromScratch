#pragma once

class IndexBuffer {
public:
	IndexBuffer() = default;
	~IndexBuffer();

	void Init(const unsigned int* data, unsigned int count);
	void Bind() const;
	void Unbind() const;

private:
	unsigned int m_RendererId;
};