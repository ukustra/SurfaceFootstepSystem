// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#include "AnimNotify_SurfaceFootstep.h"
#include "FootstepInterface.h"
#include "FootstepComponent.h"
#include "FootstepActor.h"
#include "FootstepDataAsset.h"
#include "SurfaceFootstepSystemSettings.h"
#include "FoostepPoolingManager.h"
#include "FootstepTypes.h"
#include "Engine.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/SkeletalMeshComponent.h"
#include "Logging/MessageLog.h"

#define LOCTEXT_NAMESPACE "FAnimNotify_SurfaceFootstep"

UAnimNotify_SurfaceFootstep::UAnimNotify_SurfaceFootstep(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(0, 188, 0, 255);
#endif

	FootstepSettings = USurfaceFootstepSystemSettings::Get();

	if (FootstepSettings)
	{
		FootstepCategory = FootstepSettings->GetCategoriesNum() > 0 ? FootstepSettings->GetCategoryName(0) : FGameplayTag::EmptyTag;
	}
}

void UAnimNotify_SurfaceFootstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	// Check the most important conditions
	if ( !(FootstepSettings && MeshComp && MeshComp->GetWorld() && MeshComp->GetNetMode() != NM_DedicatedServer && MeshComp->GetOwner() && MeshComp->GetOwner()->GetClass()->ImplementsInterface(UFootstepInterface::StaticClass())) ) { return; }

	if (FootstepSettings->GetCategoriesNum() == 0)
	{
		FMessageLog("PIE").Error(LOCTEXT("InvalidCategory", "There is no Footstep Category. Add any Footstep Category in the Surface Footstep System Settings in the Project Settings."));
		return;
	}
	else if (!FootstepSettings->ContainsCategory(FootstepCategory))
	{
		FMessageLog("PIE").Error( FText::Format(LOCTEXT("InvalidCategory", "\"{0}\" category is invalid. Add this Footstep Category in the Surface Footstep System Settings in the Project Settings or use a proper Footstep Category in the Surface Footstep Anim Notify."), FText::FromName(FootstepCategory.GetTagName())) );
		return;
	}

	// Ensure the World Settings class implements Footstep Interface
	UFoostepPoolingManager* PoolingManager = nullptr;
	if (const IFootstepInterface* FootstepInterface = Cast<IFootstepInterface>(MeshComp->GetWorld()->GetWorldSettings()))
	{
		PoolingManager = FootstepInterface->GetPoolingManager();
	}

	if (!PoolingManager)
	{
		FMessageLog("PIE").Error(LOCTEXT("InvalidWorldSettings", "Your Worlds Settings class doesn't implement a Footstep Interface. Change the World Settings class to the FootstepWorldSettings in the Project Settings or implement a Footstep Interface and override the \"GetPoolingManagerComponent\" function in your World Settings C++ class."));
		return;
	}

	UFootstepComponent* FootstepComponent = IFootstepInterface::Execute_GetFootstepComponent(MeshComp->GetOwner());

	if (!FootstepComponent) { return; }

	// Prepare tracing
	const bool bUseFootSocketLocation = TraceFromFootSocket() && MeshComp->DoesSocketExist(FootSocket);
	const FVector StartTrace = bUseFootSocketLocation ? MeshComp->GetSocketLocation(FootSocket) : MeshComp->GetComponentLocation();

	const FVector DirectionVector = Invoke([this, bUseFootSocketLocation, MeshComp]()->FVector const {
		const FVector DefaultDirVector = FVector::DownVector;

		if (bUseFootSocketLocation)
		{
			const FRotator SocketRotation = MeshComp->GetSocketRotation(FootSocket);

			switch (FootstepTraceDirection)
			{
			case EFootstepTraceDirection::Down:
				return FRotationMatrix(SocketRotation).GetScaledAxis(EAxis::Z) * -1.f;
			case EFootstepTraceDirection::Up:
				return FRotationMatrix(SocketRotation).GetScaledAxis(EAxis::Z);
			case EFootstepTraceDirection::Forward:
				return SocketRotation.Vector();
			case EFootstepTraceDirection::Backward:
				return SocketRotation.Vector() * -1.f;
			case EFootstepTraceDirection::Right:
				return FRotationMatrix(SocketRotation).GetScaledAxis(EAxis::Y);
			case EFootstepTraceDirection::Left:
				return FRotationMatrix(SocketRotation).GetScaledAxis(EAxis::Y) * -1.f;
			default:
				return DefaultDirVector;
			}
		}
		else
		{
			switch (FootstepTraceDirection)
			{
			case EFootstepTraceDirection::Down:
				return MeshComp->GetUpVector() * -1.f;
			case EFootstepTraceDirection::Up:
				return MeshComp->GetUpVector();
			case EFootstepTraceDirection::Forward:
				return MeshComp->GetForwardVector();
			case EFootstepTraceDirection::Backward:
				return MeshComp->GetForwardVector() * -1.f;
			case EFootstepTraceDirection::Right:
				return MeshComp->GetRightVector();
			case EFootstepTraceDirection::Left:
				return MeshComp->GetRightVector() * -1.f;
			default:
				return DefaultDirVector;
			}
		}
	});

	FHitResult TraceHitResult;
	const bool bTracePerformed = FootstepComponent->CreateFootstepLineTrace(StartTrace, DirectionVector, TraceHitResult);

	if (!(bTracePerformed && TraceHitResult.bBlockingHit)) { return; }

	// Get the Physical Material from the TraceHitResult
	const UPhysicalMaterial* PhysMat = GetPhysicalMaterial(TraceHitResult);

	if (!PhysMat) { return; }

	// Get data from a Data Asset
	if (const UFootstepDataAsset* FootstepData = FootstepComponent->GetFootstepData(PhysMat->SurfaceType))
	{
		if (GEngine && FootstepComponent->GetShowDebug())
		{
			const FString PhysMatName = PhysMat->GetName();
			const FString DataAssetName = FootstepData->GetName();
			const FString AnimationName = Animation->GetName();
			const FString CategoryName = FootstepCategory.ToString();
			const FString SocketName = TraceFromFootSocket() ? FootSocket.ToString() : TEXT("ROOT");
			const FString OwnerName = GetActorName(MeshComp->GetOwner());

			const FString DebugMessage = TEXT("PhysMat: ") + PhysMatName + TEXT(", DataAsset: ") + DataAssetName + TEXT(", Anim: ") + AnimationName + TEXT(", Category: ") + CategoryName + TEXT(", Socket: ") + SocketName + TEXT(", Owner: ") + OwnerName + TEXT(", HitActor: ") + GetActorName(TraceHitResult.GetActor()) + TEXT(", HitComp: ") + TraceHitResult.GetComponent()->GetName();

			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, DebugMessage);
			UE_LOG(LogFootstep, Log, TEXT("%s"), *DebugMessage);
		}

		USoundBase* FootstepSound = FootstepData->GetSound(FootstepCategory);
		UObject* FootstepParticle = FootstepData->GetParticle(FootstepCategory);
		
		// Finally, activate a footstep actor
		if (FootstepSound || FootstepParticle)
		{
			PoolingManager->SafeSpawnPooledActor();

			if (AFootstepActor* FootstepActor = PoolingManager->GetPooledActor())
			{
				FootstepActor->SetPoolingActive(false);

				const FQuat ActorQuat = FootstepParticle ? FRotationMatrix::MakeFromZ(TraceHitResult.ImpactNormal).ToQuat() : FQuat(EForceInit::ForceInitToZero);
				const FTransform WorldTransform = FTransform(ActorQuat, TraceHitResult.ImpactPoint, FVector(1.f));
				const FVector RelScaleVFX = FootstepData->GetRelScaleParticle();

				FootstepActor->SetActorTransform(WorldTransform);

				FootstepActor->InitSound(FootstepSound, FootstepData->GetVolume(), FootstepData->GetPitch(), FootstepComponent->GetPlaySound2D(), FootstepData->GetAttenuationOverride(), FootstepData->GetConcurrencyOverride());
				FootstepActor->InitParticle(FootstepParticle, RelScaleVFX);

				FootstepActor->SetLifeSpan(FootstepData->GetFootstepLifeSpan());
				FootstepActor->SetPoolingActive(true);
			}
		}
	}
}

FString UAnimNotify_SurfaceFootstep::GetNotifyName_Implementation() const
{
	return TraceFromFootSocket() ? Super::GetNotifyName_Implementation() + TEXT("_") + FootSocket.ToString() : Super::GetNotifyName_Implementation();
}

bool UAnimNotify_SurfaceFootstep::TraceFromFootSocket() const
{
	return bTraceFromFootSocket && FootSocket != NAME_None;
}

UPhysicalMaterial* UAnimNotify_SurfaceFootstep::GetPhysicalMaterial(const FHitResult& HitResult) const
{
	if (HitResult.PhysMaterial.IsValid())
	{
		return HitResult.PhysMaterial.Get();
	}
	else if (HitResult.GetComponent())
	{
		if (const FBodyInstance* BodyInstance = HitResult.GetComponent()->GetBodyInstance())
		{
			return BodyInstance->GetSimplePhysicalMaterial();
		}
	}

	return nullptr;
}

FString UAnimNotify_SurfaceFootstep::GetActorName(const AActor* Actor) const
{
	if (!Actor) { return FString(); }

#if WITH_EDITOR
	return Actor->GetActorLabel();
#else
	return Actor->GetName();
#endif

}

#undef LOCTEXT_NAMESPACE
