// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

/** GameInGame — independent Slate & UMG minigame overlay framework (runtime module). */
class FGameInGameModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
