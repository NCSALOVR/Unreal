// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ViveView.h"
#include "ViveViewCharacter.h"
#include "ViveViewProjectile.h"
#include "Animation/AnimInstance.h"
#include "Engine.h"
#include <string>
#include "Json.h"
//#include "json/json.h"

#include <fstream>


//////////////////////////////////////////////////////////////////////////
// AViveViewCharacter

AViveViewCharacter::AViveViewCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	XOffset = 0;
	YOffset = 0;
	ZOffset = 220;

	PitchOffset = 0;
	RollOffset = 0;
	YawOffset = 0; //face east


	// Create a CameraComponent	
	FirstPersonCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh // not anymore!
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetVisibility(false);	//no one should see the default perspective.

	//Mesh1P->SetEnableGravity(false);

	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0, 0, -150);			//"My parent is 150 below me" I think

	//RootComponent = Mesh1P;

	//Set default position direction of model. Rotation is in degrees.
	//this->SetActorLocationAndRotation(FVector(), FRotator()); //constructor call overwritten by unreal engine

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)




	/*************************************************************************
	Init obj containers
	*************************************************************************/
	etc = true;
	debugMode = false;


	ActorsAll.Init(0);
	ActorsToCreate.Init(0);
	ActorsToMove.Init(0);
	ActorsToDestroy.Init(0);
}



void AViveViewCharacter::UpdateFromVive()
{
	//bruteforce to a default location if VIVE doen't update
	SetActorLocation(FVector(XOffset, YOffset, ZOffset)); //accomodate for walls and floors


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
		-adjust avatar
		-for each object under create, parse
		-for each object under move, parse
		-for each object under destroy, parse
		-replace JSON with each category empty
		**************************************************************************/

		TSharedPtr<FJsonObject> JsonParsed;
		TSharedRef<TJsonReader<TCHAR>> JsonReader =
			TJsonReaderFactory<TCHAR>::Create(FString(jsonStr.c_str()));
		if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
		{
			//ADJUST AVATAR
			const TSharedPtr<FJsonObject> avatar = JsonParsed->GetObjectField("avatar");
			if (avatar->GetBoolField("changed"))
			{
				double x = avatar->GetArrayField("position")[0]->AsNumber();
				double y = avatar->GetArrayField("position")[1]->AsNumber();
				double z = avatar->GetArrayField("position")[2]->AsNumber();

				double r = avatar->GetNumberField("rotation");


				SetActorLocation(FVector(x + XOffset, y + YOffset, z + ZOffset)); //accomodate for walls and floors
				//the following is weird, should be yaw, but doesn't give expected results.
				//to acommodate, modifiers are done to roll section
				//GetController()->SetControlRotation(FRotator(PitchOffset, -1 * (RollOffset + r), YawOffset));
			}


			//CREATE
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


				//Plenty of extra work, so debugging is normally disabled
				if (debugMode)
				{
					int j = 0;
					FObjectActor* temp = NULL;
					while (j < ActorsAll.Num() && temp == NULL)
					{
						if (ActorsAll[j]->Id.Equals(obj->AsObject()->GetStringField("name")))
						{
							temp = ActorsAll[j];
						}
						j++;
					}

					if (temp == NULL)
					{
						ActorsToCreate.Add(temp);
					}
					else
					{
						std::ofstream configF;
						configF.open("configCreate.txt");
						std::string errName(TCHAR_TO_UTF8(*(obj->AsObject()->GetStringField("name"))));
						configF << "oh no! 0004 " << errName;
						configF.close();
					}
				}
				else
				{
					ActorsToCreate.Add(temp);
				}
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

				//always debugging because it is one simple check
				if (temp == NULL)
				{
					std::ofstream configF;
					configF.open("configMove.txt");
					std::string errName(TCHAR_TO_UTF8(*(obj->AsObject()->GetStringField("name"))));
					configF << "oh no! 0003 " << errName;
					configF.close();
				}
				else
				{
					temp->Position.Set(x, y, z);
					ActorsToMove.Add(temp);
				}
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

				//always debugging because it is one simple check
				if (temp == NULL)
				{
					std::ofstream configF;
					configF.open("configDestroy.txt");
					std::string errName(TCHAR_TO_UTF8(*(obj->AsObject()->GetStringField("name"))));
					configF << "oh no! 0005 " << errName;
					configF.close();
				}
				else
				{
					ActorsToDestroy.Add(temp);
				}
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
		remove("ActorList.json");
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

				newObj->Reference->SetActorScale3D(FVector(newObj->Radius));

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



