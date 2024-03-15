#include "Drawable.h"

#include "IndexBuffer.h"
#include "Graphics.h"

void Drawable::Draw(Graphics* pGfx) const noexcept(!_DEBUG)
{
    for (const std::unique_ptr<Bindable>& b : binds) 
    {
        b->Bind(pGfx);
    }

    for (const std::unique_ptr<Bindable>& b : GetStaticBinds())
    {
        b->Bind(pGfx);
    }
    
    pGfx->DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind)
{
    if (typeid(*bind) == typeid(IndexBuffer))
    {
        throw ILLEGAL_BINDABLE_STATE("Using Drawable::AddBind to add a Index Buffer. Should use Drawable::AddIndexBuffer instead.");
    }
    binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> bind)
{
    if (pIndexBuffer != nullptr)
    {
        throw ILLEGAL_BINDABLE_STATE("Attempting to add an IndexBuffer more than once.");
    }
    pIndexBuffer = bind.get();
    binds.push_back(std::move(bind));
}

Drawable::IllegalBindableStateException::IllegalBindableStateException(int line, const char* file, const char* reason)
    :
    ChiliException(line, file),
    reason(reason)
{}

const char* Drawable::IllegalBindableStateException::what() const noexcept
{
    return reason.c_str();
}

const char* Drawable::IllegalBindableStateException::GetType() const noexcept
{
    return "Chili Exception IllegalBindableType";
}
