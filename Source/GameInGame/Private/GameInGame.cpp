// Copyright 2026 Silvan Teufel All Rights Reserved.

#include "GameInGame.h"
#include "GameInGameLog.h"

DEFINE_LOG_CATEGORY(LogGameInGame);

#define LOCTEXT_NAMESPACE "FGameInGameModule"

void FGameInGameModule::StartupModule()
{
	UE_LOG(LogGameInGame, Log, TEXT("GameInGame started."));
}

void FGameInGameModule::ShutdownModule()
{
	UE_LOG(LogGameInGame, Log, TEXT("GameInGame shut down."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameInGameModule, GameInGame)
