// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"

#include "ViveViewCharacter.generated.h"



/*************************************************************************
Struct for containing minimal data of spawned objects in the room.
*************************************************************************/
USTRUCT()
struct FObjectActor
{
	GENERATED_USTRUCT_BODY()



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Vive)
		FString Id;

	//location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Vive)
		FVector Position;

	//size
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Vive)
		double Radius;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Vive)
		AActor* Reference;


};



UCLASS(config = Game)
class AViveViewCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

		/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		TSubobjectPtr<class USkeletalMeshComponent> Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		TSubobjectPtr<class UCameraComponent> FirstPersonCameraComponent;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AViveViewProjectile> ProjectileClass;

	//Offsets for position etc.
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		double XOffset;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		double YOffset;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		double ZOffset;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		double PitchOffset;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		double RollOffset;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		double YawOffset;



	/*************************************************************************
	Arrays for tracking objects in room with player. If made to work, this
	content will be moved to a world settings class. The arrays are separated
	by needed action on the objects
	*************************************************************************/
	TArray<FObjectActor*> ActorsAll;

	TArray<FObjectActor*> ActorsToCreate;

	TArray<FObjectActor*> ActorsToMove;

	TArray<FObjectActor*> ActorsToDestroy;



	/*************************************************************************
	Handles parsing data into array data structures for Blueprints to use
	for creating, moving, and destroying objects.
	*************************************************************************/
	UFUNCTION(BlueprintCallable, Category = Vive)
		void UpdateFromVive();


	/*************************************************************************
	Override to request update from vive every frame
	*************************************************************************/
	virtual void Tick(float DeltaSeconds) override;



	bool etc;
};


