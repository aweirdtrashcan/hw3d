#pragma once

class Graphics;

#include <vector>
#include <DirectXMath.h>
#include <memory>
#include "ChiliException.h"
#include "Bindable.h"

class Bindable;
class IndexBuffer;

class Drawable
{
public:
	class IllegalBindableStateException : public ChiliException
	{
	public:
		IllegalBindableStateException(int line, const char* file, const char* reason);
		virtual const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
	private:
		std::string reason;
	};
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics* pGfx) const noexcept(!_DEBUG);
	virtual void Update(float dt) noexcept = 0;
	void AddBind(std::unique_ptr<Bindable> bind);
	void AddIndexBuffer(std::unique_ptr<IndexBuffer> bind);
private:
	const class IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};