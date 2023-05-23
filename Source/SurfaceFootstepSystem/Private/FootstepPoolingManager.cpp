// Copyright 2019-2023 Urszula Kustra. All Rights Reserved.

#include "FootstepPoolingManager.h"
#include "SurfaceFootstepSystemSettings.h"
#include "FootstepActor.h"

UFootstepPoolingManager::UFootstepPoolingManager()
	: Super()
{
}

void UFootstepPoolingManager::RemoveInvalidActors()
{
	TArray<int32> EmptyIndexes;
	
	for (int32 i = 0; i < PooledActors.Num(); ++i)
	{
		const TWeakObjectPtr<AFootstepActor> Actor = PooledActors[i];
		if (!Actor.IsValid())
		{
			EmptyIndexes.Add(i);
		}
	}

	for (const int32 Index : EmptyIndexes)
	{
		PooledActors.RemoveAt(Index);
	}
}

void UFootstepPoolingManager::DestroyFootstepPool(const UObject* WorldContextObject)
{
	if (!GEngine) { return; }

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UFootstepPoolingManager* PoolingManager = World->GetSubsystem<UFootstepPoolingManager>())
		{
			PoolingManager->DestroyPooledActors();
		}
	}
}

bool UFootstepPoolingManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (Super::ShouldCreateSubsystem(Outer))
	{
		if (const UWorld* World = Cast<UWorld>(Outer))
		{
			return !World->IsNetMode(NM_DedicatedServer);
		}
	}

	return false;
}

void UFootstepPoolingManager::Deinitialize()
{
	DestroyFootstepPool(GetWorld());
	
	Super::Deinitialize();
}

bool UFootstepPoolingManager::SafeSpawnPooledActor()
{
	if (const USurfaceFootstepSystemSettings* FootstepSettings = USurfaceFootstepSystemSettings::Get())
	{
		RemoveInvalidActors();
		
		if (PooledActors.Num() < FootstepSettings->GetPoolSize())
		{
			const FActorSpawnParameters SpawnParams = Invoke([]()->FActorSpawnParameters const {
				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				return Params;
			});

			const TWeakObjectPtr<AFootstepActor> FootstepActor = GetWorld()->SpawnActor<AFootstepActor>(AFootstepActor::StaticClass(), FTransform(), SpawnParams);

			PooledActors.Add(FootstepActor);

			return true;
		}
	}

	return false;
}

void UFootstepPoolingManager::DestroyPooledActors()
{
	for (const TWeakObjectPtr<AFootstepActor>& Actor : PooledActors)
	{
		if (Actor.IsValid())
		{
			AFootstepActor* PooledActor = Actor.Get();
			PooledActor->Destroy();
		}
	}

	PooledActors.Reset();
}

AFootstepActor* UFootstepPoolingManager::GetPooledActor(bool bRemoveInvalidActors)
{
	if (bRemoveInvalidActors)
	{
		RemoveInvalidActors();
	}
	
	for (const TWeakObjectPtr<AFootstepActor>& Actor : PooledActors)
	{
		if (Actor.IsValid())
		{
			AFootstepActor* PooledActor = Actor.Get();
			if (!PooledActor->IsPoolingActive())
			{
				return PooledActor;
			}
		}
	}

	if (PooledActors.Num() > 0)
	{
		const TWeakObjectPtr<AFootstepActor> PooledActor = PooledActors[0];
		if (PooledActor.IsValid())
		{
			PooledActors.Add(PooledActor);
			PooledActors.RemoveAt(0);

			return PooledActor.Get();
		}
	}

	return nullptr;
}
