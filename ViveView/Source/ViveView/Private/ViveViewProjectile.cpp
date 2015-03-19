// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ViveView.h"
#include "ViveViewProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AViveViewProjectile::AViveViewProjectile(const class FPostConstructInitializeProperties& PCIP) 
	: Super(PCIP)
{
	// Use a sphere as a simple collision representation
	CollisionComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
//	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");			// Collision profiles are defined in DefaultEngine.ini
//	CollisionComp->OnComponentHit.AddDynamic(this, &AViveViewProjectile::OnHit);		// set up a notification for when this component overlaps something
	RootComponent = CollisionComp;


	// never die
	//InitialLifeSpan = 0.0f;
}

