#pragma once

#include "Drawable.h"
#include "IndexBuffer.h"
#include <random>

template<typename T>
class DrawableBase : public Drawable
{
protected:
	bool IsStaticInitialized() const noexcept
	{
		return !staticBinds.empty();
	}
	void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!_DEBUG)
	{
		if (typeid(*bind) == typeid(IndexBuffer))
		{
			throw ILLEGAL_BINDABLE_STATE("Using Drawable::AddBind to add a Index Buffer. Should use Drawable::AddIndexBuffer instead.");
		}
		staticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!_DEBUG)
	{
		if (pIndexBuffer != nullptr)
		{
			throw ILLEGAL_BINDABLE_STATE("Attempting to add an IndexBuffer more than once.");
		}
		pIndexBuffer = ibuf.get();
		staticIBuf = ibuf.get();
		staticBinds.push_back(std::move(ibuf));
	}
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
	virtual void SetIndexFromStatic() noexcept override
	{
		pIndexBuffer = staticIBuf;
	}
private:
	static inline std::vector<std::unique_ptr<Bindable>> staticBinds;
	static inline IndexBuffer* staticIBuf;
};
