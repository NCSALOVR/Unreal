// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "ViveViewGameMode.generated.h"

UCLASS(minimalapi)
class AViveViewGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()


	UFUNCTION(BlueprintCallable, category = vive)
	void updateFromVive();

	virtual void Tick(float DeltaSeconds) OVERRIDE;
};



