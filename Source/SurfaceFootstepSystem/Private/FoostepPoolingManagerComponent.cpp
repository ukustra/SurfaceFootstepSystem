// Copyright 2019 Urszula Kustra. All Rights Reserved.

#include "FoostepPoolingManagerComponent.h"
#include "SurfaceFootstepSystemSettings.h"
#include "FootstepActor.h"
#include "Logging/MessageLog.h"

#define LOCTEXT_NAMESPACE "FFootstepPoolingManager"

UFoostepPoolingManagerComponent::UFoostepPoolingManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UFoostepPoolingManagerComponent::DestroyFootstepPool(const UObject* WorldContextObject)
{
	if (!GEngine) { return; }

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (const IFootstepInterface* FootstepInterface = Cast<IFootstepInterface>(World->GetWorldSettings()))
		{
			if (UFoostepPoolingManagerComponent* PoolingManager = FootstepInterface->GetPoolingManagerComponent())
			{
				PoolingManager->DestroyPooledActors();
			}
			else
			{
				FMessageLog("PIE").Error(LOCTEXT("InvalidPoolingManager", "Cannot delete Footstep Actors, because your World Settings class doesn't override the \"GetPoolingManagerComponent\" function."));
			}
		}
		else
		{
			FMessageLog("PIE").Error(LOCTEXT("InvalidWorldSettings", "Cannot delete Footstep Actors, because your Worlds Settings class doesn't implement a Footstep Interface. Change the World Settings class to the FootstepWorldSettings in the Project Settings or implement a Footstep Interface and override the \"GetPoolingManagerComponent\" function in your World Settings C++ class."));
		}
	}
}

void UFoostepPoolingManagerComponent::SafeSpawnPooledActor()
{
	if (!GetWorld()) { return; }

	if (const USurfaceFootstepSystemSettings* FootstepSettings = USurfaceFootstepSystemSettings::Get())
	{
		if (PooledActors.Num() < FootstepSettings->GetPoolSize())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AFootstepActor* FootstepActor = GetWorld()->SpawnActor<AFootstepActor>(AFootstepActor::StaticClass(), FTransform(), SpawnParams);
			
			PooledActors.Add(FootstepActor);
		}
	}
}

void UFoostepPoolingManagerComponent::DestroyPooledActors()
{
	for (AFootstepActor* PooledActor : PooledActors)
	{
		if (PooledActor)
		{
			PooledActor->Destroy();
		}
	}

	PooledActors.Empty();
}

AFootstepActor* UFoostepPoolingManagerComponent::GetPooledActor()
{
	for (AFootstepActor* PooledActor : PooledActors)
	{
		if (PooledActor && !PooledActor->IsPoolingActive())
		{
			return PooledActor;
		}
	}

	if (PooledActors.Num() > 0)
	{
		AFootstepActor* PooledActor = PooledActors[0];
		PooledActors.Add(PooledActor);
		PooledActors.RemoveAt(0);

		return PooledActor;
	}
	
	return nullptr;
}

#undef LOCTEXT_NAMESPACE
