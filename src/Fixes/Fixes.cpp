#include "Fixes/Fixes.h"

#include "Fixes/ActorIsHostileToActorFix.h"
#include "Fixes/CellInitFix.h"
#include "Fixes/EncounterZoneResetFix.h"
#include "Fixes/FaderMenuFix.h"
#include "Fixes/SafeExit.h"

namespace Fixes
{

	// any fixes that use addresses not currently correctly in the vr address library are comment out.
	// in other words, all of them currently
	void PreInit()
	{
		Settings::load();

	}

	void PostInit()
	{
	}
	void Preload()
	{
		if (*Settings::UnalignedLoad) {
			UnalignedLoadFix::Install();
		}
	}
}
