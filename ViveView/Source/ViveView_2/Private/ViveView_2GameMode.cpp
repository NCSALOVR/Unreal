// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ViveView_2.h"
#include "ViveView_2GameMode.h"
#include "ViveView_2HUD.h"
#include "ViveView_2Character.h"

AViveView_2GameMode::AViveView_2GameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/MyCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AViveView_2HUD::StaticClass();
}
