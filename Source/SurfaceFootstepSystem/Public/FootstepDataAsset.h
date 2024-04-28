// Copyright 2019-2024 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "FootstepDataAsset.generated.h"

class USoundBase;
class USoundAttenuation;
class USoundConcurrency;
class UFXSystemAsset;
class UParticleSystem;
class UNiagaraSystem;
class USurfaceFootstepSystemSettings;

USTRUCT()
struct FFootstepData
{
	GENERATED_USTRUCT_BODY()

	/** Sound will be played randomly. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep|Sound")
	TArray<TSoftObjectPtr<USoundBase>> Sounds;

	/** A particle will be taken randomly from both Particles and Niagara Particles arrays. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep|Particle")
	TArray<TSoftObjectPtr<UParticleSystem>> Particles;

	/** A particle will be taken randomly from both Particles and Niagara Particles arrays. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep|Particle")
	TArray<TSoftObjectPtr<UNiagaraSystem>> NiagaraParticles;
	
	bool AreSoundsValid() const;
};

/**
 * Data asset which stores footstep audio-visual data from the Surface Footstep System plugin.
 */
UCLASS()
class SURFACEFOOTSTEPSYSTEM_API UFootstepDataAsset : public UDataAsset
{
	GENERATED_UCLASS_BODY()
	
protected:
	/** FX assets. Categories have to be the same as the ones from the Surface Footstep System Settings in the Project Settings. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep", meta = (Categories = "Footstep"))
	TMap<FGameplayTag, FFootstepData> FootstepData;

	/** Minimum volume multiplier. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep|Sound", meta = (ClampMin = 0.f))
	float MinVolume;

	/** Maximum volume multiplier. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep|Sound", meta = (ClampMin = 0.f))
	float MaxVolume;

	/** Minimum pitch multiplier. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep|Sound", meta = (ClampMin = 0.f))
	float MinPitch;

	/** Maximum pitch multiplier. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep|Sound", meta = (ClampMin = 0.f))
	float MaxPitch;

	/** If none, Attenuation Settings from the Sound Base will be applied. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep|Sound")
	TSoftObjectPtr<USoundAttenuation> AttenuationSettingsOverride;

	/** If none, Concurrency Settings from the Sound Base will be applied. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep|Sound")
	TSoftObjectPtr<USoundConcurrency> ConcurrencySettingsOverride;

	/** Minimum scale of the Particle. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep|Particle", meta = (ClampMin = 0.0))
	double MinParticleScale;

	/** Maximum scale of the Particle. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep|Particle", meta = (ClampMin = 0.0))
	double MaxParticleScale;

	/** The visibility time of a spawned Footstep Actor. */
	UPROPERTY(EditDefaultsOnly, Category = "Footstep|Pooling", meta = (ClampMin = 0.f))
	float FootstepLifeSpan;

public:
	void RequestLoadingAssetsAsynchronously();
	
	USoundBase* GetSound(const FGameplayTag& CategoryTag) const;
	float GetVolume() const;
	float GetPitch() const;
	USoundAttenuation* GetAttenuationOverride() const;
	USoundConcurrency* GetConcurrencyOverride() const;

	UFXSystemAsset* GetParticle(const FGameplayTag& CategoryTag) const;
	FVector GetRelScaleParticle() const;

	float GetFootstepLifeSpan() const;

private:
	UPROPERTY()
	TObjectPtr<USurfaceFootstepSystemSettings> FootstepSettings;

	void PrintEditorError() const;
	void PrintEditorWarning() const;
};
