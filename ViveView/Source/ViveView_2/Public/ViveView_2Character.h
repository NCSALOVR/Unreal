// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "ViveView_2Character.generated.h"

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
class AViveView_2Character : public ACharacter
{
	GENERATED_UCLASS_BODY()

		/** Pawn mesh: 1st person view (arms; seen only by self) */
		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		TSubobjectPtr<class USkeletalMeshComponent> Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		TSubobjectPtr<class UCameraComponent> FirstPersonCameraComponent;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AViveView_2Projectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;



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



protected:

	/** Handler for a touch input beginning. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface
};

