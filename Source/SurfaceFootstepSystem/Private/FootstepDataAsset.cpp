// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#include "FootstepDataAsset.h"
#include "SurfaceFootstepSystemSettings.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundConcurrency.h"
#include "Logging/MessageLog.h"

#define LOCTEXT_NAMESPACE "FFootstepDataAsset"

UFootstepDataAsset::UFootstepDataAsset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MinVolume(1.f)
	, MaxVolume(1.f)
	, MinPitch(1.f)
	, MaxPitch(1.f)
	, MinParticleScale(1.f)
	, MaxParticleScale(1.f)
{
	FootstepSettings = USurfaceFootstepSystemSettings::Get();

	if (FootstepSettings)
	{
		for (int32 i = 0; i < FootstepSettings->GetCategoriesNum(); ++i)
		{
			FootstepData.Add(FootstepSettings->GetCategoryName(i), FFootstepData());
		}

		ConstructorHelpers::FObjectFinder<USoundAttenuation> DefaultAttenuation(*FootstepSettings->GetAttenuationAssetPath());
		if (DefaultAttenuation.Succeeded())
		{
			AttenuationSettingsOverride = DefaultAttenuation.Object;
		}

		ConstructorHelpers::FObjectFinder<USoundConcurrency> DefaultConurrency(*FootstepSettings->GetConcurrencyAssetPath());
		if (DefaultConurrency.Succeeded())
		{
			ConcurrencySettingsOverride = DefaultConurrency.Object;
		}

		FootstepLifeSpan = FootstepSettings->GetDefaultPoolingLifeSpan();
	}
}

USoundBase* UFootstepDataAsset::GetSound(const FGameplayTag& CategoryTag) const
{
	if (!FootstepSettings) { return nullptr; }

	if (!FootstepSettings->ContainsCategory(CategoryTag))
	{
		PrintEditorError();
	}
	else if (FootstepData.Contains(CategoryTag))
	{
		const TArray<USoundBase*> Sounds = FootstepData[CategoryTag].Sounds;
		return Sounds.Num() > 0 ? Sounds[FMath::RandHelper(Sounds.Num())] : nullptr;
	}
	else
	{
		PrintEditorWarning();
	}

	return nullptr;
}

float UFootstepDataAsset::GetVolume() const
{
	return FMath::RandRange(MinVolume, MaxVolume);
}

float UFootstepDataAsset::GetPitch() const
{
	return FMath::RandRange(MinPitch, MaxPitch);
}

USoundAttenuation* UFootstepDataAsset::GetAttenuationOverride() const
{
	return AttenuationSettingsOverride;
}

USoundConcurrency* UFootstepDataAsset::GetConcurrencyOverride() const
{
	return ConcurrencySettingsOverride;
}

UObject* UFootstepDataAsset::GetParticle(const FGameplayTag& CategoryTag) const
{
	if (!FootstepSettings) { return nullptr; }

	if (!FootstepSettings->ContainsCategory(CategoryTag))
	{
		PrintEditorError();
	}
	else if (FootstepData.Contains(CategoryTag))
	{
		TArray<UObject*> ActualParticles;
		ActualParticles.Append(FootstepData[CategoryTag].Particles);
		ActualParticles.Append(FootstepData[CategoryTag].NiagaraParticles);

		return ActualParticles.Num() > 0 ? ActualParticles[FMath::RandHelper(ActualParticles.Num())] : nullptr;
	}
	else
	{
		PrintEditorWarning();
	}

	return nullptr;
}

FVector UFootstepDataAsset::GetRelScaleParticle() const
{
	const float RandScale = FMath::RandRange(MinParticleScale, MaxParticleScale);

	return FVector(RandScale);
}

float UFootstepDataAsset::GetFootstepLifeSpan() const
{
	return FootstepLifeSpan;
}

void UFootstepDataAsset::PrintEditorError() const
{
	FMessageLog("PIE").Error( FText::Format(LOCTEXT("InvalidCategory", "{0} has a Footstep Category which is not set in the Surface Footstep System Settings in the Project Settings."), FText::FromString(GetName())) );
}

void UFootstepDataAsset::PrintEditorWarning() const
{
	FMessageLog("PIE").Warning( FText::Format(LOCTEXT("InvalidCategory", "{0} doesn't have a Footstep Category set in the Surface Footstep System Settings in the Project Settings."), FText::FromString(GetName())) );
}

#undef LOCTEXT_NAMESPACE
