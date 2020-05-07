// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#include "FoostepPoolingManager.h"
#include "SurfaceFootstepSystemSettings.h"
#include "FootstepActor.h"
#include "Logging/MessageLog.h"

#define LOCTEXT_NAMESPACE "FFootstepPoolingManager"

UFoostepPoolingManager::UFoostepPoolingManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFoostepPoolingManager::DestroyFootstepPool(const UObject* WorldContextObject)
{
	if (!GEngine) { return; }

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (World->GetNetMode() == NM_DedicatedServer) { return; }

		if (const IFootstepInterface* FootstepInterface = Cast<IFootstepInterface>(World->GetWorldSettings()))
		{
			if (UFoostepPoolingManager* PoolingManager = FootstepInterface->GetPoolingManager())
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

void UFoostepPoolingManager::SafeSpawnPooledActor()
{
	if ( !(GetWorld() && GetWorld()->GetNetMode() != NM_DedicatedServer) ) { return; }

	if (const USurfaceFootstepSystemSettings* FootstepSettings = USurfaceFootstepSystemSettings::Get())
	{
		if (PooledActors.Num() < FootstepSettings->GetPoolSize())
		{
			const FActorSpawnParameters SpawnParams = Invoke([]()->FActorSpawnParameters const {
				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				return Params;
			});

			AFootstepActor* FootstepActor = GetWorld()->SpawnActor<AFootstepActor>(AFootstepActor::StaticClass(), FTransform(), SpawnParams);

			PooledActors.Add(FootstepActor);
		}
	}
}

void UFoostepPoolingManager::DestroyPooledActors()
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

AFootstepActor* UFoostepPoolingManager::GetPooledActor()
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
