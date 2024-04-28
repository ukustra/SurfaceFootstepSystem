// Copyright 2019-2024 Urszula Kustra. All Rights Reserved.

#include "FootstepDataAsset.h"
#include "NiagaraSystem.h"
#include "SurfaceFootstepSystemSettings.h"
#include "Engine/AssetManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundConcurrency.h"
#include "Logging/MessageLog.h"
#include "Particles/ParticleSystem.h"

#define LOCTEXT_NAMESPACE "FFootstepDataAsset"

UFootstepDataAsset::UFootstepDataAsset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MinVolume(1.f)
	, MaxVolume(1.f)
	, MinPitch(1.f)
	, MaxPitch(1.f)
	, MinParticleScale(1.0)
	, MaxParticleScale(1.0)
{
	FootstepSettings = USurfaceFootstepSystemSettings::Get();

	if (FootstepSettings)
	{
		for (int32 i = 0; i < FootstepSettings->GetCategoriesNum(); ++i)
		{
			FootstepData.Add(FootstepSettings->GetCategoryName(i), FFootstepData());
		}

		const ConstructorHelpers::FObjectFinder<USoundAttenuation> DefaultAttenuation(*FootstepSettings->GetAttenuationAssetPath());
		if (DefaultAttenuation.Succeeded())
		{
			AttenuationSettingsOverride = DefaultAttenuation.Object;
		}

		const ConstructorHelpers::FObjectFinder<USoundConcurrency> DefaultConcurrency(*FootstepSettings->GetConcurrencyAssetPath());
		if (DefaultConcurrency.Succeeded())
		{
			ConcurrencySettingsOverride = DefaultConcurrency.Object;
		}

		FootstepLifeSpan = FootstepSettings->GetDefaultPoolingLifeSpan();
	}
}

bool FFootstepData::AreSoundsValid() const
{
	for (const TSoftObjectPtr<USoundBase>& Sound : Sounds)
	{
		if (Sound.IsNull())
		{
			return false;
		}
	}

	return !Sounds.IsEmpty();
}

void UFootstepDataAsset::RequestLoadingAssetsAsynchronously()
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	auto RequestAsyncLoad = [&StreamableManager](const TSoftObjectPtr<UObject>& Asset)
	{
		if (Asset.IsPending())
		{
			StreamableManager.RequestAsyncLoad(Asset.ToSoftObjectPath());
		}
	};
	
	for (const auto& It : FootstepData)
	{
		const FFootstepData& Data = It.Value;

		if (Data.AreSoundsValid())
		{
			for (const TSoftObjectPtr<USoundBase>& Sound : Data.Sounds)
			{
				RequestAsyncLoad(Sound);
			}

			RequestAsyncLoad(AttenuationSettingsOverride);
			RequestAsyncLoad(ConcurrencySettingsOverride);
		}

		for (const TSoftObjectPtr<UParticleSystem>& Particle : Data.Particles)
		{
			RequestAsyncLoad(Particle);
		}

		for (const TSoftObjectPtr<UNiagaraSystem>& Niagara : Data.NiagaraParticles)
		{
			RequestAsyncLoad(Niagara);
		}
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
		const TArray<TSoftObjectPtr<USoundBase>>& Sounds = FootstepData[CategoryTag].Sounds;
		return Sounds.Num() > 0 ? Sounds[FMath::RandHelper(Sounds.Num())].LoadSynchronous() : nullptr;
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
	return AttenuationSettingsOverride.LoadSynchronous();
}

USoundConcurrency* UFootstepDataAsset::GetConcurrencyOverride() const
{
	return ConcurrencySettingsOverride.LoadSynchronous();
}

UFXSystemAsset* UFootstepDataAsset::GetParticle(const FGameplayTag& CategoryTag) const
{
	if (!FootstepSettings) { return nullptr; }

	if (!FootstepSettings->ContainsCategory(CategoryTag))
	{
		PrintEditorError();
	}
	else if (FootstepData.Contains(CategoryTag))
	{
		TArray<TSoftObjectPtr<UFXSystemAsset>> ActualParticles;
		ActualParticles.Append(FootstepData[CategoryTag].Particles);
		ActualParticles.Append(FootstepData[CategoryTag].NiagaraParticles);

		return ActualParticles.Num() > 0 ? ActualParticles[FMath::RandHelper(ActualParticles.Num())].LoadSynchronous() : nullptr;
	}
	else
	{
		PrintEditorWarning();
	}

	return nullptr;
}

FVector UFootstepDataAsset::GetRelScaleParticle() const
{
	const double RandScale = FMath::RandRange(MinParticleScale, MaxParticleScale);

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
