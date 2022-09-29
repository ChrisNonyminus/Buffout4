#include "Patches/Patches.h"

#include "Patches/MemoryManagerPatch.h"
#include "Patches/ScaleformAllocatorPatch.h"
#include "Patches/SmallBlockAllocatorPatch.h"

namespace Patches
{
	void Preload()
	{
		if (*Settings::MemoryManager) {
			MemoryManagerPatch::Install();
		}

	}
}
