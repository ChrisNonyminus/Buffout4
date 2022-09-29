#pragma once

namespace Fixes
{
	void PreInit();
	void PostInit();
	void Preload();
}

namespace Fixes::UnalignedLoadFix
{
	namespace detail
	{
		inline void ApplySkinningToGeometry()
		{
			REL::Relocation<std::uintptr_t> target{ REL::ID(44611), 0x179 + 2};
			REL::safe_write(target.address(), std::uint32_t{ 0x10 });
		}

		inline void CreateCommandBuffer()
		{
			//constexpr std::array offsets{
			//	0x320,
			//	0x339,
			//	0x341 + 0x1,  // rex prefix
			//	0x353,
			//};

			//REL::Relocation<std::uintptr_t> base{ REL::ID(768994) }; // the corresponding offset for this id has not been found in fo4vr yet so I'll just stub CreateCommandBuffer for now
			//for (const auto off : offsets) {
			//	REL::safe_write(base.address() + off + 0x1, std::uint8_t{ 0x11 });	// movaps -> movups
			//}

		}
	}

	inline void Install()
	{
		detail::ApplySkinningToGeometry();
		detail::CreateCommandBuffer();
		logger::debug("installed UnalignedLoad fix"sv);
	}
}
