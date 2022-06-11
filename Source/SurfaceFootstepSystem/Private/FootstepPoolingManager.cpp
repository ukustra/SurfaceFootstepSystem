// Copyright 2019-2022 Urszula Kustra. All Rights Reserved.

#include "FootstepPoolingManager.h"
#include "SurfaceFootstepSystemSettings.h"
#include "FootstepActor.h"
#include "Logging/MessageLog.h"

#define LOCTEXT_NAMESPACE "FFootstepPoolingManager"

UFootstepPoolingManager::UFootstepPoolingManager()
	: Super()
{
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
		if (PooledActors.Num() < FootstepSettings->GetPoolSize())
		{
			const FActorSpawnParameters SpawnParams = Invoke([]()->FActorSpawnParameters const {
				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				return Params;
			});

			AFootstepActor* FootstepActor = GetWorld()->SpawnActor<AFootstepActor>(AFootstepActor::StaticClass(), FTransform(), SpawnParams);

			PooledActors.Add(FootstepActor);

			return true;
		}
	}

	return false;
}

void UFootstepPoolingManager::DestroyPooledActors()
{
	for (TObjectPtr<AFootstepActor>& PooledActor : PooledActors)
	{
		if (PooledActor)
		{
			PooledActor->Destroy();
		}
	}

	PooledActors.Reset();
}

AFootstepActor* UFootstepPoolingManager::GetPooledActor()
{
	for (const TObjectPtr<AFootstepActor>& PooledActor : PooledActors)
	{
		if (PooledActor && !PooledActor->IsPoolingActive())
		{
			return PooledActor.Get();
		}
	}

	if (PooledActors.Num() > 0)
	{
		AFootstepActor* PooledActor = PooledActors[0].Get();
		PooledActors.Add(PooledActor);
		PooledActors.RemoveAt(0);

		return PooledActor;
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
