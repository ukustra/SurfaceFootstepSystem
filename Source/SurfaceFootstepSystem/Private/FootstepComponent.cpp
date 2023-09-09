// Copyright 2019-2023 Urszula Kustra. All Rights Reserved.

#include "FootstepComponent.h"
#include "FootstepDataAsset.h"
#include "SurfaceFootstepSystemSettings.h"
#include "Engine/AssetManager.h"
#include "GameFramework/Controller.h"

#if ENABLE_DRAW_DEBUG
#include "KismetTraceUtils.h"
#endif

UFootstepComponent::UFootstepComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bAutoActivate = true;

	FootstepSettings = USurfaceFootstepSystemSettings::Get();
	if (FootstepSettings)
	{
		TraceLength = FootstepSettings->GetDefaultTraceLength();
	}
}

void UFootstepComponent::OnRegister()
{
	Super::OnRegister();

	TryPreloading();
}

void UFootstepComponent::OnUnregister()
{
	CancelPreloading();
	
	Super::OnUnregister();
}

bool UFootstepComponent::GetPlaySound2D() const
{
	if (!FootstepSettings) { return false; }

	if (FootstepSettings->GetPlaySound2D())
	{
		if (const APawn* PawnOwner = Cast<APawn>(GetOwner()))
		{
			if (const AController* Controller = PawnOwner->GetController())
			{
				return Controller->IsLocalPlayerController();
			}
		}
	}

	return false;
}

void UFootstepComponent::SetActorsToIgnoreForTrace(const TArray<AActor*>& NewActorsToIgnore)
{
	ActorsToIgnore.Empty();

	for (AActor* Actor : NewActorsToIgnore)
	{
		if (Actor)
		{
			ActorsToIgnore.AddUnique(Actor);
		}
	}
}

void UFootstepComponent::AddActorToIgnoreForTrace(AActor* NewActor)
{
	if (NewActor)
	{
		ActorsToIgnore.AddUnique(NewActor);
	}
}

bool UFootstepComponent::RemoveActorToIgnoreForTrace(AActor* ActorToRemove)
{
	if (ActorToRemove)
	{
		for (int32 i = 0; i < ActorsToIgnore.Num(); ++i)
		{
			if (ActorsToIgnore[i] == ActorToRemove)
			{
				ActorsToIgnore.RemoveAt(i);

				return true;
			}
		}
	}

	return false;
}

bool UFootstepComponent::CreateFootstepLineTrace(const FVector& Start, const FVector& DirectionNormalVector, FHitResult& OutHit) const
{
	if (!GetWorld() && !FootstepSettings) { return false; }

	UWorld* World = GetWorld();

	const FVector DirVector = DirectionNormalVector.GetSafeNormal();
	const FVector End = Start + (DirVector * TraceLength);

	const FCollisionQueryParams QueryParams = Invoke([this, World]()->FCollisionQueryParams const {
		FCollisionQueryParams Params;
		Params.bReturnPhysicalMaterial = true;
		Params.bTraceComplex = FootstepSettings->GetTraceComplex();
		Params.AddIgnoredActor(GetOwner());
		Params.AddIgnoredActors(ActorsToIgnore);

#if ENABLE_DRAW_DEBUG
		if (bShowDebug)
		{
			const FName TraceTag = TEXT("Debug");
			Params.TraceTag = TraceTag;

			World->DebugDrawTraceTag = TraceTag;
		}
#endif

		return Params;
	});

	const FCollisionObjectQueryParams ObjectParams = Invoke([this]()->FCollisionObjectQueryParams const {
		FCollisionObjectQueryParams Params;
		for (const ECollisionChannel ObjectType : FootstepSettings->GetFootstepObjectTypes())
		{
			Params.AddObjectTypesToQuery(ObjectType);
		}

		return Params;
	});

	const bool bTraceSuccessful = World->LineTraceSingleByObjectType(OutHit, Start, End, ObjectParams, QueryParams);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		DrawDebugLineTraceSingle(World, Start, End, EDrawDebugTrace::Type::ForDuration, bTraceSuccessful, OutHit, FLinearColor::Red, FLinearColor::Green, 2.f);
	}
#endif

	return bTraceSuccessful;

}

UFootstepDataAsset* UFootstepComponent::GetFootstepData(const EPhysicalSurface SurfaceType) const
{
	return FootstepFXes.Contains(SurfaceType) ? FootstepFXes[SurfaceType].LoadSynchronous() : nullptr;
}

float UFootstepComponent::GetTraceLength() const
{
	return TraceLength;
}

bool UFootstepComponent::GetShowDebug() const
{
#if ENABLE_DRAW_DEBUG
	return bShowDebug;
#endif

	return false;
}

void UFootstepComponent::TryPreloading()
{
	if ( !(bPreloadAssetsAsynchronously && !bPreloading) )
	{
		return;
	}

	const UWorld* World = GetWorld();

	if ( !(World && World->IsGameWorld()) )
	{
		return;
	}

	bPreloading = true;

	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	auto RequestAsyncLoad = [this, &StreamableManager](const EPhysicalSurface SurfaceType, const TSoftObjectPtr<UObject>& Asset)
	{
		if (Asset.IsPending())
		{
			StreamableManager.RequestAsyncLoad(Asset.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this, [this, SurfaceType]()
			{
				if (UFootstepDataAsset* DataAsset = GetFootstepData(SurfaceType))
				{
					DataAsset->RequestLoadingAssetsAsynchronously();
				}
			}));
		}
	};

	for (const auto& It : FootstepFXes)
	{
		const EPhysicalSurface SurfaceType = It.Key;
		const TSoftObjectPtr<UFootstepDataAsset>& DataAsset = It.Value;

		RequestAsyncLoad(SurfaceType, DataAsset);
	}
}

void UFootstepComponent::CancelPreloading()
{
	bPreloading = false;
}
