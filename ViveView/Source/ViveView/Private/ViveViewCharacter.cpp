// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ViveView.h"
#include "ViveViewCharacter.h"
#include "ViveViewProjectile.h"
#include "Animation/AnimInstance.h"

#include <fstream>


//////////////////////////////////////////////////////////////////////////
// AViveViewCharacter

AViveViewCharacter::AViveViewCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Set size for collision capsule
	CapsuleComponent->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = CapsuleComponent;
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)



	/*************************************************************************
	Init obj containers
	*************************************************************************/
	etc = true;
	ActorsAll.Init(0);
	ActorsToCreate.Init(0);
	ActorsToMove.Init(0);
	ActorsToDestroy.Init(0);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AViveViewCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AViveViewCharacter::OnFire);
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AViveViewCharacter::TouchStarted);

	InputComponent->BindAxis("MoveForward", this, &AViveViewCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AViveViewCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AViveViewCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AViveViewCharacter::LookUpAtRate);
}

void AViveViewCharacter::OnFire()
{
	return;
}

void AViveViewCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// only fire for first finger down
	if (FingerIndex == 0)
	{
		OnFire();
	}
}

void AViveViewCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AViveViewCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AViveViewCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AViveViewCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}



void AViveViewCharacter::UpdateFromVive()
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


	/*TODO:
	use this for storing in ToMove or ToDestroy

	FObjectActor* temp = NULL;
	int i = 0;
	while (i < ActorsAll.Num() && temp == NULL)
	{
	if (ActorsAll[i]->Id.Equals(objToKill->Id))
	{
	temp = ActorsAll[i];
	}
	i++;
	}
	*/



	if (etc)
	{
		//DUMMY DATA
		FString testObj1 = FString(TEXT("TestObj1"));
		FObjectActor* temp = new FObjectActor();
		temp->Id = testObj1;
		temp->Position.Set(300, 300, 300);
		temp->Radius = 1;


		//simulate creation
		ActorsToCreate.Add(temp);
		/*
		//simulate move
		temp->Position.Set(200, 200, 200);
		ActorsToMove.Add(temp);

		//simulate deletion
		ActorsToDestroy.Add(temp);
		*/


		etc = false;
	}



	actorF.close();
	return;
}



void AViveViewCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateFromVive();
	UWorld* const world = GetWorld();



	//handle create requests
	for (FObjectActor* newObj : ActorsToCreate)
	{
		if (ProjectileClass != NULL)
		{
			if (world != NULL)
			{
				newObj->Reference = world->SpawnActor
					(
					ProjectileClass,
					&(newObj->Position)
					);

				newObj->Reference->SetActorScale3D(FVector(10));

				ActorsAll.Add(newObj);
			}
		}
	}
	ActorsToCreate.Empty();


	//handle move requests
	for (FObjectActor* objToMove : ActorsToMove)
	{
		objToMove->Reference->SetActorLocation(objToMove->Position, false);
	}
	ActorsToMove.Empty();


	//handle destroy requests
	for (FObjectActor* objToKill : ActorsToDestroy)
	{
		world->DestroyActor(objToKill->Reference);

		ActorsAll.Remove(objToKill);
		delete objToKill;
	}
	ActorsToDestroy.Empty();



	return;
}



