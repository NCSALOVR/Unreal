// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ViveView_2.h"
#include "ViveView_2Character.h"
#include "ViveView_2Projectile.h"
#include "Animation/AnimInstance.h"
#include "Engine.h"
#include <string>
#include "Json.h"
//#include "json/json.h"

#include <fstream>


//////////////////////////////////////////////////////////////////////////
// AViveViewCharacter

AViveView_2Character::AViveView_2Character(const class FPostConstructInitializeProperties& PCIP)
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

void AViveView_2Character::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AViveView_2Character::OnFire);
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AViveView_2Character::TouchStarted);

	InputComponent->BindAxis("MoveForward", this, &AViveView_2Character::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AViveView_2Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AViveView_2Character::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AViveView_2Character::LookUpAtRate);
}

void AViveView_2Character::OnFire()
{
	return;
}

void AViveView_2Character::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// only fire for first finger down
	if (FingerIndex == 0)
	{
		OnFire();
	}
}

void AViveView_2Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AViveView_2Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AViveView_2Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AViveView_2Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}



void AViveView_2Character::UpdateFromVive()
{
	std::ifstream actorF;
	actorF.open("ActorList.json");
	if (actorF.is_open())
	{
		std::ofstream configF;
		configF.open("config.txt");
		configF << "Hello!";
		configF.close();


		std::string jsonStr((std::istreambuf_iterator<char>(actorF)), std::istreambuf_iterator<char>());
		/**************************************************************************
		-send file to parser
		-error check
		-for each object under create, parse
		-for each object under move, parse
		-for each object under destroy, parse
		-replace JSON with each category empty
		**************************************************************************/
		//CREATE
		TSharedPtr<FJsonObject> JsonParsed;
		TSharedRef<TJsonReader<TCHAR>> JsonReader =
			TJsonReaderFactory<TCHAR>::Create(FString(jsonStr.c_str()));
		if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
		{
			const TArray<TSharedPtr<FJsonValue>> toCreate = JsonParsed->GetArrayField("create");
			for (int i = 0; i < toCreate.Num(); i++)
			{
				TSharedPtr<FJsonValue> obj = toCreate[i];
				double x = obj->AsObject()->GetArrayField("position")[0]->AsNumber();
				double y = obj->AsObject()->GetArrayField("position")[1]->AsNumber();
				double z = obj->AsObject()->GetArrayField("position")[2]->AsNumber();


				FObjectActor* temp = new FObjectActor();
				temp->Id = obj->AsObject()->GetStringField("name");
				temp->Position.Set(x, y, z);
				temp->Radius = obj->AsObject()->GetNumberField("radius");


				ActorsToCreate.Add(temp);

			}


			//MOVE
			const TArray<TSharedPtr<FJsonValue>> toMove = JsonParsed->GetArrayField("move");
			for (int i = 0; i < toMove.Num(); i++)
			{
				TSharedPtr<FJsonValue> obj = toMove[i];
				double x = obj->AsObject()->GetArrayField("position")[0]->AsNumber();
				double y = obj->AsObject()->GetArrayField("position")[1]->AsNumber();
				double z = obj->AsObject()->GetArrayField("position")[2]->AsNumber();
				FObjectActor* temp = NULL;
				int j = 0;


				while (j < ActorsAll.Num() && temp == NULL)
				{
					if (ActorsAll[j]->Id.Equals(obj->AsObject()->GetStringField("name")))
					{
						temp = ActorsAll[j];
					}
					j++;
				}


				temp->Position.Set(x, y, z);
				ActorsToMove.Add(temp);
			}


			//DESTROY
			const TArray<TSharedPtr<FJsonValue>> toDestroy = JsonParsed->GetArrayField("destroy");
			for (int i = 0; i < toDestroy.Num(); i++)
			{
				TSharedPtr<FJsonValue> obj = toDestroy[i];
				FObjectActor* temp = NULL;
				int j = 0;


				while (j < ActorsAll.Num() && temp == NULL)
				{
					if (ActorsAll[j]->Id.Equals(obj->AsObject()->GetStringField("name")))
					{
						temp = ActorsAll[j];
					}
					j++;
				}


				ActorsToDestroy.Add(temp);
			}
		}
		else
		{
			std::ofstream configF;
			configF.open("config.txt");
			configF << "oh no! 0002";
			configF.close();
		}



		actorF.close();

		std::ofstream actorFReset;
		actorFReset.open("ActorList.json");
		actorFReset << "{\"create\": [],\"move\": [],\"Destroy\": []}";
		actorFReset.close();
	}
	else
	{
		std::ofstream configF;
		configF.open("config.txt");
		configF << "oh no! 0001";
		configF.close();
	}


	return;
}



void AViveView_2Character::Tick(float DeltaSeconds)
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
