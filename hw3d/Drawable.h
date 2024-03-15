#pragma once

class Graphics;

#include <vector>
#include <DirectXMath.h>
#include <memory>
#include "ChiliException.h"
#include "Bindable.h"

#define ILLEGAL_BINDABLE_STATE(reason) IllegalBindableStateException(__LINE__, __FILE__, (reason))

class Bindable;
class IndexBuffer;

class Drawable
{
	template<typename T>
	friend class DrawableBase;
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
protected:
	void AddBind(std::unique_ptr<Bindable> bind);
	void AddIndexBuffer(std::unique_ptr<IndexBuffer> bind);
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
	virtual void SetIndexFromStatic() noexcept = 0;
private:
	const class IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};