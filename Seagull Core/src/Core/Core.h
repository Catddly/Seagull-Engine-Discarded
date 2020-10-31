#pragma once

#include <memory>

#ifdef SG_ENABLE_ASSERT
	#define SG_ASSERT(x, ...)      { if (!(x)) { SG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
	#define SG_CORE_ASSERT(x, ...) { if (!(x)) { SG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
	#define SG_ASSERT(x)           { if (!(x)) { SG_ERROR("Assertion Failed!"); __debugbreak(); }}
	#define SG_CORE_ASSERT(x)      { if (!(x)) { SG_CORE_ERROR("Assertion Failed"); __debugbreak(); }}
#else 
	#define SG_ASSERT(x, ...)
	#define SG_CORE_ASSERT(x, ...)
	#define SG_ASSERT(x)      
	#define SG_CORE_ASSERT(x) 
#endif

namespace SG
{
	// TODO : replace to own reference system
	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T>
	using WeakRef = std::weak_ptr<T>;

	template <typename T, typename ...Args>
	Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename ...Args>
	Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename ...Args>
	WeakRef<T> CreateWeakRef(Args&&... args)
	{
		return std::weak_ptr<T>(std::forward<Args>(args)...);
	}
}