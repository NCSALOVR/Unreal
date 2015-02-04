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



void AViveViewGameMode::updateFromVive()
{
	std::ifstream actorF;
	actorF.open("ActorList.txt");
	if (actorF.is_open())
	{
		std::ofstream configF;
		configF.open("config.txt");
		configF << "Hello!";
		configF.close();
	}
	else
	{
		std::ofstream configF;
		configF.open("config.txt");
		configF << "oh no!";
		configF.close();
	}
	
	
	actorF.close();
	return;
}


void AViveViewGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	updateFromVive();			///going to be changed to only focus on
}