/***
	Author: Chinyone
	Copyright: Free
*/

#include "NonAnimNotify_SurfaceFootstep.h"
#include "FootstepInterface.h"
#include "FootstepComponent.h"
#include "FootstepActor.h"
#include "FootstepDataAsset.h"
#include "SurfaceFootstepSystemSettings.h"
#include "FoostepPoolingManager.h"
#include "FootstepTypes.h"
#include "Engine.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Logging/MessageLog.h"


ANonAnimNotify_SurfaceFootstep::ANonAnimNotify_SurfaceFootstep()
{
	PrimaryActorTick.bCanEverTick = true;

	FootstepSettings = USurfaceFootstepSystemSettings::Get();

	if (FootstepSettings)
	{
		FootstepCategory = FootstepSettings->GetCategoriesNum() > 0 ? FootstepSettings->GetCategoryName(0) : FGameplayTag::EmptyTag;
	}
}

void ANonAnimNotify_SurfaceFootstep::BeginPlay()
{
	Super::BeginPlay();
}

void ANonAnimNotify_SurfaceFootstep::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANonAnimNotify_SurfaceFootstep::MakeFootstep()
{
	if (!FootstepSettings) { return; }

	if (FootstepSettings->GetCategoriesNum() == 0)
	{
		return;
	}
	else if (!FootstepSettings->ContainsCategory(FootstepCategory))
	{
		return;
	}

	UFoostepPoolingManager* PoolingManager = this->GetWorld()->GetSubsystem<UFoostepPoolingManager>();

	if (!PoolingManager)
	{
		return;
	}

	AActor* MeshOwner = this->CharacterReference;

	UFootstepComponent* FootstepComponent = nullptr;
	if (Cast<IFootstepInterface>(MeshOwner) || MeshOwner->GetClass()->ImplementsInterface(UFootstepInterface::StaticClass()))
	{
		FootstepComponent = IFootstepInterface::Execute_GetFootstepComponent(MeshOwner);
	}

	if (!(FootstepComponent && FootstepComponent->IsActive()))
	{
		return;
	}

	const FVector StartTrace = this->CharacterReference->GetActorLocation();
	const FVector DirectionVector = FVector::DownVector;

	FHitResult TraceHitResult;
	const bool bTracePerformed = FootstepComponent->CreateFootstepLineTrace(StartTrace, DirectionVector, TraceHitResult);

	if (!(bTracePerformed && TraceHitResult.bBlockingHit)) { return; }

	const UPhysicalMaterial* PhysMat = GetPhysicalMaterial(TraceHitResult);

	if (!PhysMat) { return; }
	
	if (const UFootstepDataAsset* FootstepData = FootstepComponent->GetFootstepData(PhysMat->SurfaceType))
	{
		if (GEngine && FootstepComponent->GetShowDebug())
		{
			const FString PhysMatName = PhysMat->GetName();
			const FString DataAssetName = FootstepData->GetName();
			const FString CategoryName = FootstepCategory.ToString();
			const FString OwnerName = GetActorName(MeshOwner);

			const FString DebugMessage = TEXT("PhysMat: ") + PhysMatName + TEXT(", DataAsset: ") 
				+ DataAssetName + CategoryName + TEXT(", Owner: ") 
				+ OwnerName + TEXT(", HitActor: ") + GetActorName(TraceHitResult.GetActor())
				+ TEXT(", HitComp: ") + TraceHitResult.GetComponent()->GetName();

			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, DebugMessage);
			UE_LOG(LogFootstep, Log, TEXT("%s"), *DebugMessage);
		}

		USoundBase* FootstepSound = FootstepData->GetSound(FootstepCategory);
		UObject* FootstepParticle = FootstepData->GetParticle(FootstepCategory);

		
		if (FootstepSound || FootstepParticle)
		{
			PoolingManager->SafeSpawnPooledActor();

			if (AFootstepActor* FootstepActor = PoolingManager->GetPooledActor())
			{
				FootstepActor->SetPoolingActive(false);

				const FQuat ActorQuat = FootstepParticle ? FRotationMatrix::MakeFromZ(TraceHitResult.ImpactNormal).ToQuat() : FQuat(EForceInit::ForceInitToZero);
				const FTransform WorldTransform = FTransform(ActorQuat, TraceHitResult.ImpactPoint, FVector(1.f));
				const FVector RelScaleVFX = FootstepParticle ? FootstepData->GetRelScaleParticle() : FVector(0.f);

				FootstepActor->SetActorTransform(WorldTransform);

				const float Volume = FootstepSound ? FootstepData->GetVolume() : 0.f;
				const float Pitch = FootstepSound ? FootstepData->GetPitch() : 0.f;
				const float SoundAssetVolume = FootstepSound ? FootstepSound->GetVolumeMultiplier() : 0.f;
				const float SoundAssetPitch = FootstepSound ? FootstepSound->GetPitchMultiplier() : 0.f;

				FootstepActor->InitSound(FootstepSound, Volume, Pitch, FootstepComponent->GetPlaySound2D(), FootstepData->GetAttenuationOverride(), FootstepData->GetConcurrencyOverride());
				FootstepActor->InitParticle(FootstepParticle, RelScaleVFX);

				FootstepActor->SetLifeSpan(FootstepData->GetFootstepLifeSpan());
				FootstepActor->SetPoolingActive(true);

				FootstepComponent->OnFootstepGenerated.Broadcast(PhysMat->SurfaceType, FootstepCategory, WorldTransform, Volume, Pitch, SoundAssetVolume, SoundAssetPitch, RelScaleVFX);
			}
		}
	}
}

UPhysicalMaterial* ANonAnimNotify_SurfaceFootstep::GetPhysicalMaterial(const FHitResult& HitResult) const
{
	if (HitResult.PhysMaterial.IsValid())
	{
		return HitResult.PhysMaterial.Get();
	}
	else if (HitResult.Component.IsValid())
	{
		if (const FBodyInstance* BodyInstance = HitResult.GetComponent()->GetBodyInstance())
		{
			return BodyInstance->GetSimplePhysicalMaterial();
		}
	}
 
	return nullptr;
}

FString ANonAnimNotify_SurfaceFootstep::GetActorName(const AActor* Actor) const
{
	if (!Actor) { return FString(); }

#if WITH_EDITOR
	return Actor->GetActorLabel();
#else
	return Actor->GetName();
#endif

}