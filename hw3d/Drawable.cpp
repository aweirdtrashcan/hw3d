#include "Drawable.h"

#include "IndexBuffer.h"
#include "Graphics.h"

void Drawable::Draw(Graphics* pGfx) const noexcept(!_DEBUG)
{
    for (const std::shared_ptr<Bindable>& b : binds)
    {
        b->Bind(pGfx);
    }

    pGfx->DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::shared_ptr<Bindable> bind)
{
    if (typeid(*bind) == typeid(IndexBuffer))
    {
        assert(pIndexBuffer == nullptr && "You should only set IndexBuffer for an Instance only once.");
        pIndexBuffer = (IndexBuffer*)bind.get();
    }
    binds.push_back(bind);
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
