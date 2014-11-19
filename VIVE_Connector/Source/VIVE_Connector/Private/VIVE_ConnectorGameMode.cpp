// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "VIVE_Connector.h"
#include "VIVE_ConnectorGameMode.h"
#include "VIVE_ConnectorHUD.h"
#include "VIVE_ConnectorCharacter.h"

AVIVE_ConnectorGameMode::AVIVE_ConnectorGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/MyCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AVIVE_ConnectorHUD::StaticClass();
}
