// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ViveView.h"
#include "ViveViewGameMode.h"
#include "ViveViewHUD.h"
#include "ViveViewCharacter.h"
#include <iostream>
#include <fstream>
#include "CoreMisc.h"
#include <string>


AViveViewGameMode::AViveViewGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/MyCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AViveViewHUD::StaticClass();


	
}



