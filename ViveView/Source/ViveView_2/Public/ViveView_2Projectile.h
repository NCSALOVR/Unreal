// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Actor.h"
#include "ViveView_2Projectile.generated.h"

UCLASS(config=Game)
class AViveView_2Projectile : public AActor
{
	GENERATED_UCLASS_BODY()

		/** Sphere collision component */
		UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		TSubobjectPtr<class USphereComponent> CollisionComp;
};

