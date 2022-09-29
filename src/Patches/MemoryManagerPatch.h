#pragma once

namespace Allocator
{
	class ProxyHeap
	{
	public:
		ProxyHeap(const ProxyHeap&) = delete;
		ProxyHeap(ProxyHeap&&) = delete;
		ProxyHeap& operator=(const ProxyHeap&) = delete;
		ProxyHeap& operator=(ProxyHeap&&) = delete;

		[[nodiscard]] static ProxyHeap& get() noexcept
		{
			static ProxyHeap singleton;
			return singleton;
		}

		[[nodiscard]] void* malloc(std::size_t a_size) { return ::malloc(a_size); }

		[[nodiscard]] void* aligned_alloc(std::size_t a_alignment, std::size_t a_size) { return ::_aligned_malloc(a_size, a_alignment); }

		[[nodiscard]] void* realloc(void* a_ptr, std::size_t a_newSize) { return ::realloc(a_ptr, a_newSize); }
		[[nodiscard]] void* aligned_realloc(std::size_t a_alignment, void* a_ptr, std::size_t a_newSize) { return ::_aligned_realloc(a_ptr, a_newSize, a_alignment); }

		void free(void* a_ptr) { ::free(a_ptr); }
		void aligned_free(void* a_ptr) { ::_aligned_free(a_ptr); }

	private:
		ProxyHeap() noexcept = default;
		~ProxyHeap() noexcept = default;
	};

	class ProxyDebugHeap
	{
	public:
		ProxyDebugHeap(const ProxyDebugHeap&) = delete;
		ProxyDebugHeap(ProxyDebugHeap&&) = delete;
		ProxyDebugHeap& operator=(const ProxyDebugHeap&) = delete;
		ProxyDebugHeap& operator=(ProxyDebugHeap&&) = delete;

		[[nodiscard]] static ProxyDebugHeap& get() noexcept
		{
			static ProxyDebugHeap singleton;
			return singleton;
		}

		[[nodiscard]] void* malloc(std::size_t a_size);
		[[nodiscard]] void* aligned_alloc(std::size_t a_alignment, std::size_t a_size);

		[[nodiscard]] void* realloc(void* a_ptr, std::size_t a_newSize);
		[[nodiscard]] void* aligned_realloc(std::size_t a_alignment, void* a_ptr, std::size_t a_newSize);

		void free(void* a_ptr);
		void aligned_free(void* a_ptr);

	private:
		ProxyDebugHeap() noexcept;
		~ProxyDebugHeap() noexcept = default;
	};

	/*class GameHeap
	{
	public:
		GameHeap() = default;
		GameHeap(const GameHeap&) = delete;
		GameHeap(GameHeap&&) = delete;
		~GameHeap() = default;
		GameHeap& operator=(const GameHeap&) = delete;
		GameHeap& operator=(GameHeap&&) = delete;

		[[nodiscard]] void* malloc(std::size_t a_size) { _allocate(_proxy, a_size, 0x0, false); }
		[[nodiscard]] void* aligned_alloc(std::size_t a_alignment, std::size_t a_size) { return _allocate(_proxy, a_size, static_cast<std::uint32_t>(a_alignment), true); }

		[[nodiscard]] void* realloc(void* a_ptr, std::size_t a_newSize) { return _reallocate(_proxy, a_ptr, a_newSize, 0x0, false); }
		[[nodiscard]] void* aligned_realloc(std::size_t a_alignment, void* a_ptr, std::size_t a_newSize) { return _reallocate(_proxy, a_ptr, a_newSize, static_cast<std::uint32_t>(a_alignment), true); }

		void free(void* a_ptr) { _deallocate(_proxy, a_ptr, false); }
		void aligned_free(void* a_ptr) { _deallocate(_proxy, a_ptr, true); }

	private:
		using Allocate_t = void*(RE::MemoryManager&, std::size_t, std::uint32_t, bool);
		using Deallocate_t = void(RE::MemoryManager&, void*, bool);
		using Reallocate_t = void*(RE::MemoryManager&, void*, std::size_t, std::uint32_t, bool);

		RE::MemoryManager& _proxy{ RE::MemoryManager::GetSingleton() };
		Allocate_t* const _allocate{ reinterpret_cast<Allocate_t*>(REL::ID(652767).address()) };
		Deallocate_t* const _deallocate{ reinterpret_cast<Deallocate_t*>(REL::ID(1582181).address()) };
		Reallocate_t* const _reallocate{ reinterpret_cast<Reallocate_t*>(REL::ID(1502917).address()) };
	};*/

	[[nodiscard]] void* ProxyDebugHeap::malloc(std::size_t a_size)
	{
		return ::_malloc_dbg(a_size, _NORMAL_BLOCK, __FILE__, __LINE__);
	}

	[[nodiscard]] void* ProxyDebugHeap::aligned_alloc(std::size_t a_alignment, std::size_t a_size)
	{
		return ::_aligned_malloc_dbg(a_size, a_alignment, __FILE__, __LINE__);
	}

	[[nodiscard]] void* ProxyDebugHeap::realloc(void* a_ptr, std::size_t a_newSize)
	{
		return ::_realloc_dbg(a_ptr, a_newSize, _NORMAL_BLOCK, __FILE__, __LINE__);
	}

	[[nodiscard]] void* ProxyDebugHeap::aligned_realloc(std::size_t a_alignment, void* a_ptr, std::size_t a_newSize)
	{
		return ::_aligned_realloc_dbg(a_ptr, a_newSize, a_alignment, __FILE__, __LINE__);
	}

	void ProxyDebugHeap::free(void* a_ptr)
	{
		::_free_dbg(a_ptr, _NORMAL_BLOCK);
	}

	void ProxyDebugHeap::aligned_free(void* a_ptr)
	{
		::_aligned_free_dbg(a_ptr);
	}

	ProxyDebugHeap::ProxyDebugHeap() noexcept
	{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |
					   //_CRTDBG_DELAY_FREE_MEM_DF |
					   _CRTDBG_CHECK_ALWAYS_DF);
	}
}



namespace Patches
{
	namespace MemoryManagerPatch
	{
		namespace detail
		{
			class MemoryTraces
			{
			public:
				using lock_type = std::mutex;
				using value_type = std::unordered_map<void*, std::pair<bool, boost::stacktrace::stacktrace>>;

				class Accessor
				{
				public:
					Accessor() = delete;
					Accessor(const Accessor&) = delete;
					Accessor(Accessor&&) = delete;

					~Accessor() = default;

					Accessor& operator=(const Accessor&) = delete;
					Accessor& operator=(Accessor&&) = delete;

					[[nodiscard]] value_type& operator*() noexcept { return get(); }
					[[nodiscard]] const value_type& operator*() const noexcept { return get(); }

					[[nodiscard]] value_type* operator->() noexcept { return std::addressof(get()); }
					[[nodiscard]] const value_type* operator->() const noexcept { return std::addressof(get()); }

					[[nodiscard]] value_type& get() noexcept { return _proxy; }
					[[nodiscard]] const value_type& get() const noexcept { return _proxy; }

				protected:
					friend class MemoryTraces;

					Accessor(lock_type& a_lock, value_type& a_proxy) :
						_locker(a_lock),
						_proxy(a_proxy)
					{}

				private:
					std::scoped_lock<lock_type> _locker;
					value_type& _proxy;
				};

				[[nodiscard]] static MemoryTraces& get() noexcept
				{
					static MemoryTraces singleton;
					return singleton;
				}

				[[nodiscard]] Accessor access() { return { _lock, _traces }; }

			private:
				MemoryTraces() noexcept = default;
				MemoryTraces(const MemoryTraces&) = delete;
				MemoryTraces(MemoryTraces&&) = delete;

				~MemoryTraces() noexcept = default;

				MemoryTraces& operator=(const MemoryTraces&) = delete;
				MemoryTraces& operator=(MemoryTraces&&) = delete;

				lock_type _lock;
				value_type _traces;
			};
			namespace MemoryManager
			{
				inline void* Allocate(RE::MemoryManager*, std::size_t a_size, std::uint32_t a_alignment, bool a_alignmentRequired)
				{
					if (a_size > 0) {
						auto& heap = Allocator::ProxyHeap::get();
						return a_alignmentRequired ?
								   heap.aligned_alloc(a_alignment, a_size) :
									 heap.malloc(a_size);
					} else {
						return nullptr;
					}
				}

				inline void* DbgAllocate(RE::MemoryManager* a_this, std::size_t a_size, std::uint32_t a_alignment, bool a_alignmentRequired)
				{
					const auto result = Allocate(a_this, a_size, a_alignment, a_alignmentRequired);
					if (result && a_size == sizeof(RE::NiNode)) {
						auto access = MemoryTraces::get().access();
						access->emplace(
							result,
							std::make_pair(a_alignmentRequired, boost::stacktrace::stacktrace{}));
					}
					return result;
				}

				inline void Deallocate(RE::MemoryManager*, void* a_mem, bool a_alignmentRequired)
				{
					auto& heap = Allocator::ProxyHeap::get();
					a_alignmentRequired ?
						heap.aligned_free(a_mem) :
						  heap.free(a_mem);
				}

				void DbgDeallocate(RE::MemoryManager* a_this, void* a_mem, bool a_alignmentRequired)
				{
					Deallocate(a_this, a_mem, a_alignmentRequired); // todo
					/*auto access = MemoryTraces::get().access();
					const auto it = access->find(a_mem);
					if (it != access->end() && it->second.first != a_alignmentRequired) {
						const auto log = spdlog::default_logger();
						log->set_pattern("%v"s);
						log->set_level(spdlog::level::trace);
						log->flush_on(spdlog::level::off);

						const auto modules = Crash::Modules::get_loaded_modules();
						std::array stacktraces{
							std::move(it->second.second),
							boost::stacktrace::stacktrace{}
						};

						for (auto& trace : stacktraces) {
							Crash::Callstack callstack{ std::move(trace) };

							log->critical("");
							callstack.print(
								*log,
								std::span{ modules.begin(), modules.end() });
							log->flush();
						}

						stl::report_and_fail("A bad deallocation has resulted in a crash. Please see Buffout4.log for more details."sv);
					} else {
						Deallocate(a_this, a_mem, a_alignmentRequired);
						access->erase(a_mem);
					}*/
				}

				inline void* Reallocate(RE::MemoryManager*, void* a_oldMem, std::size_t a_newSize, std::uint32_t a_alignment, bool a_alignmentRequired)
				{
					auto& heap = Allocator::ProxyHeap::get();
					return a_alignmentRequired ?
							   heap.aligned_realloc(a_alignment, a_oldMem, a_newSize) :
								 heap.realloc(a_oldMem, a_newSize);
				}

				inline void* DbgReallocate(RE::MemoryManager* a_this, void* a_oldMem, std::size_t a_newSize, std::uint32_t a_alignment, bool a_alignmentRequired)
				{
					auto access = MemoryTraces::get().access();
					const auto result = Reallocate(a_this, a_oldMem, a_newSize, a_alignment, a_alignmentRequired);
					if (const auto it = access->find(a_oldMem); it != access->end()) {
						access->erase(it);
						access->emplace(
							result,
							std::make_pair(a_alignmentRequired, boost::stacktrace::stacktrace{}));
					}
					return result;
				}

				inline void ReplaceAllocRoutines()
				{
					using tuple_t = std::tuple<std::uint64_t, std::size_t, void*>;
					const std::array todo{
						tuple_t{ 652767, 0x24B, *Settings::MemoryManagerDebug ? &DbgAllocate : &Allocate },
						tuple_t{ 1582181, 0x115, *Settings::MemoryManagerDebug ? &DbgDeallocate : &Deallocate },
						tuple_t{ 1502917, 0xA2, *Settings::MemoryManagerDebug ? &DbgReallocate : &Reallocate },
					};

					for (const auto& [id, size, func] : todo) {
						REL::Relocation<std::uintptr_t> target{ REL::ID(id) };
						REL::safe_fill(target.address(), REL::INT3, size);
						stl::asm_jump(target.address(), size, reinterpret_cast<std::uintptr_t>(func));
					}
				}

				inline void StubInit()
				{
					REL::Relocation<std::uintptr_t> target{ REL::ID(597736) };
					REL::safe_fill(target.address(), REL::INT3, 0x9C);
					REL::safe_write(target.address(), REL::RET);

					REL::Relocation<std::uint32_t*> initFence{ REL::ID(1570354) };
					*initFence = 2;
				}

				inline void Install()
				{
					StubInit();
					ReplaceAllocRoutines();

					RE::MemoryManager::GetSingleton().RegisterMemoryManager();
					RE::BSThreadEvent::InitSDM();
				}
			}

			namespace ScrapHeap
			{
				inline void* Allocate(RE::ScrapHeap*, std::size_t a_size, std::size_t a_alignment)
				{
					//logger::trace("Function called: ScrapHeap::Allocate({:16X}, {}, {})", reinterpret_cast<uintptr_t>(a_this), a_size, a_alignment);
					auto& heap = Allocator::ProxyHeap::get();
					return a_size > 0 ?
							   heap.aligned_alloc(a_alignment, a_size) :
								 nullptr;
				}

				inline RE::ScrapHeap* Ctor(RE::ScrapHeap* a_this)
				{
					std::memset(a_this, 0, sizeof(RE::ScrapHeap));
					F4SE::stl::emplace_vtable(a_this);
					return a_this;
				}

				inline void Deallocate(RE::ScrapHeap*, void* a_mem)
				{
					//logger::trace("Function called: ScrapHeap::Deallocate({:16X}, {:16X})", reinterpret_cast<uintptr_t>(a_this), reinterpret_cast<uintptr_t>(a_mem));
					auto& heap = Allocator::ProxyHeap::get();
					heap.aligned_free(a_mem);
				}

				inline void WriteHooks()
				{
					using tuple_t = std::tuple<std::uint64_t, std::size_t, void*>;
					const std::array todo{
						tuple_t{ 1085394, 0x5F6, &Allocate },
						tuple_t{ 923307, 0x144, &Deallocate },
						tuple_t{ 48809, 0x12B, &Ctor },
					};

					for (const auto& [id, size, func] : todo) {
						REL::Relocation<std::uintptr_t> target{ REL::ID(id) };
						REL::safe_fill(target.address(), REL::INT3, size);
						stl::asm_jump(target.address(), size, reinterpret_cast<std::uintptr_t>(func));
					}
				}

				inline void WriteStubs()
				{
					using tuple_t = std::tuple<std::uint64_t, std::size_t>;
					const std::array todo{
						tuple_t{ 550677, 0xC3 },  // Clean
						tuple_t{ 111657, 0x8 },	  // ClearKeepPages
						tuple_t{ 975239, 0xF6 },  // InsertFreeBlock
						tuple_t{ 84225, 0x183 },  // RemoveFreeBlock
						tuple_t{ 1255203, 0x4 },  // SetKeepPages
						tuple_t{ 912706, 0x32 },  // dtor
					};

					for (const auto& [id, size] : todo) {
						REL::Relocation<std::uintptr_t> target{ REL::ID(id) };
						REL::safe_fill(target.address(), REL::INT3, size);
						REL::safe_write(target.address(), REL::RET);
					}
				}

				inline void Install()
				{
					WriteStubs();
					WriteHooks();
				}
			}
		}

		void Install()
		{
			//detail::MemoryManager::Install(); // currently crashes the game
			detail::ScrapHeap::Install();
			logger::info("installed memory manager patch"sv);
		}
	};
}
