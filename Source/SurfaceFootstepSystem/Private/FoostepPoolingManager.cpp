// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#include "FoostepPoolingManager.h"
#include "SurfaceFootstepSystemSettings.h"
#include "FootstepActor.h"
#include "Logging/MessageLog.h"

#define LOCTEXT_NAMESPACE "FFootstepPoolingManager"

UFoostepPoolingManager::UFoostepPoolingManager()
	: Super()
{
}

void UFoostepPoolingManager::DestroyFootstepPool(const UObject* WorldContextObject)
{
	if (!GEngine) { return; }

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UFoostepPoolingManager* PoolingManager = World->GetSubsystem<UFoostepPoolingManager>())
		{
			PoolingManager->DestroyPooledActors();
		}
	}
}

bool UFoostepPoolingManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (GEngine)
	{
		if (const UWorld* World = GEngine->GetWorldFromContextObject(Outer, EGetWorldErrorMode::LogAndReturnNull))
		{
			return World->GetNetMode() != NM_DedicatedServer;
		}
	}

	return false;
}

void UFoostepPoolingManager::SafeSpawnPooledActor()
{
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
