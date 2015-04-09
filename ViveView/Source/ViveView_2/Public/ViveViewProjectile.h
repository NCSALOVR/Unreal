// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Actor.h"
#include "ViveViewProjectile.generated.h"

UCLASS(config = Game)
class AViveViewProjectile : public AActor
{
	GENERATED_UCLASS_BODY()

		/** Sphere collision component */
		UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		TSubobjectPtr<class USphereComponent> CollisionComp;

};

