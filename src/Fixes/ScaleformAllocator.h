#pragma once

namespace Fixes
{
	class ScaleformAllocator :
		public RE::Scaleform::SysAlloc
	{
	public:
		[[nodiscard]] static ScaleformAllocator* GetSingleton()
		{
			static ScaleformAllocator singleton;
			return std::addressof(singleton);
		}

		static void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL::ID(903830), 0xEC };
			auto& trampoline = F4SE::GetTrampoline();
			_init = trampoline.write_call<5>(target.address(), Init);
			logger::info("installed fixes for {}"sv, typeid(ScaleformAllocator).name());
		}

	protected:
		void* Alloc(std::size_t a_size, std::size_t a_align) override
		{
			return _aligned_malloc(a_size, a_align);
		}

		void Free(void* a_ptr, std::size_t, std::size_t) override
		{
			_aligned_free(a_ptr);
		}

		void* Realloc(void* a_oldPtr, std::size_t, std::size_t a_newSize, std::size_t a_align) override
		{
			return _aligned_realloc(a_oldPtr, a_newSize, a_align);
		}

	private:
		static void Init(const RE::Scaleform::MemoryHeap::HeapDesc& a_rootHeapDesc, RE::Scaleform::SysAllocBase*)
		{
			_init(a_rootHeapDesc, GetSingleton());
		}

		ScaleformAllocator() = default;
		ScaleformAllocator(const ScaleformAllocator&) = delete;
		ScaleformAllocator(ScaleformAllocator&&) = delete;

		~ScaleformAllocator() = default;

		ScaleformAllocator& operator=(const ScaleformAllocator&) = delete;
		ScaleformAllocator& operator=(ScaleformAllocator&&) = delete;

		static inline REL::Relocation<decltype(Init)> _init;
	};
}
