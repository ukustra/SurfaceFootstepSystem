// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FootstepActor.generated.h"

class UAudioComponent;
class USoundAttenuation;
class USoundConcurrency;
class UParticleSystemComponent;
class UNiagaraComponent;
class UNiagaraSystem;

/**
 * An actor from the Surface Footstep System plugin which visualizes footstep impact.
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class SURFACEFOOTSTEPSYSTEM_API AFootstepActor final : public AActor
{
	GENERATED_UCLASS_BODY()
	
private:
	UPROPERTY()
	UAudioComponent* AudioComponent;

	UPROPERTY()
	UParticleSystemComponent* ParticleComponent;

	UPROPERTY()
	UNiagaraComponent* NiagaraComponent;

	void DeactivatePooling();

public:
	//~ Begin AActor interface
	virtual void SetLifeSpan(float InLifespan) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor interface

	void SetPoolingActive(bool bInActive);
	bool IsPoolingActive() const;

	void InitSound(USoundBase* Sound, float Volume, float Pitch, bool bIs2D, USoundAttenuation* AttenuationOverride = nullptr, USoundConcurrency* ConcurrencyOverride = nullptr);
	void InitParticle(UObject* Particle, const FVector RelativeScale);

private:
	float PoolingLifeSpan;
	FTimerHandle PoolingTimer;
	bool bPoolingActive;
	bool bUseNiagara;
};
