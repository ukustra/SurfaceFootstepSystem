// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#include "FootstepComponent.h"
#include "SurfaceFootstepSystemSettings.h"
#include "GameFramework/Controller.h"

#if ENABLE_DRAW_DEBUG
#include "Engine/Private/KismetTraceUtils.h"
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

UFootstepDataAsset* UFootstepComponent::GetFootstepData(EPhysicalSurface SurfaceType) const
{
	return FootstepFXes.Contains(SurfaceType) ? FootstepFXes[SurfaceType] : nullptr;
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
