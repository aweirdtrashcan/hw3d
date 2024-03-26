#pragma once

#include "BindableBase.h"
#include <memory>
#include <string>
#include <unordered_map>

class Codex
{
public:
	template<class Type, typename... Params>
	static std::shared_ptr<Bindable> Resolve(Graphics* gfx, Params&&... params) noexcept
	{
		return Get().Resolve_<Type>(gfx, std::forward<Params>(params)...);
	}
private:
	template<class Type, typename... Params>
	std::shared_ptr<Bindable> Resolve_(Graphics* gfx, Params&&... params) noexcept
	{
		std::string key = Type::GenerateUID(std::forward<Params>(params)...);
		auto bind = binds.find(key);
		if (bind == binds.end())
		{
			std::shared_ptr<Type> tbind = std::make_shared<Type>(gfx, std::forward<Params>(params)...);
			tbind->SetUID(key);
			binds[key] = tbind;
			return tbind;
		}
		else
		{
			return bind->second;
		}
	}
	void Store_(std::shared_ptr<Bindable> bind)
	{
		binds[bind->GetUID()] = std::move(bind);
	}
	static Codex& Get()
	{
		static Codex codex;
		return codex;
	}
private:
	std::unordered_map<std::string, std::shared_ptr<Bindable>> binds;
};