// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#include "FootstepActor.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"

AFootstepActor::AFootstepActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, UseComponentTag(TEXT("UseComponent"))
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootComponent"));

	AudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->SetupAttachment(RootComponent);

	ParticleComponent = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("ParticleComponent"));
	ParticleComponent->bAutoActivate = false;
	ParticleComponent->SetupAttachment(RootComponent);

	NiagaraComponent = ObjectInitializer.CreateDefaultSubobject<UNiagaraComponent>(this, TEXT("NiagaraComponent"));
	NiagaraComponent->bAutoActivate = false;
	NiagaraComponent->SetupAttachment(RootComponent);
}

void AFootstepActor::SetLifeSpan(float InLifespan)
{
	PoolingLifeSpan = InLifespan;

	if (PoolingLifeSpan > 0.f)
	{
		GetWorldTimerManager().SetTimer(PoolingTimer, FTimerDelegate::CreateWeakLambda(this, [this]() {
			SetPoolingActive(false);
		}), PoolingLifeSpan, false);
	}
	else
	{
		SetPoolingActive(false);
	}
}

void AFootstepActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(PoolingTimer);
}

void AFootstepActor::SetPoolingActive(bool bInActive)
{
	check(AudioComponent && ParticleComponent && NiagaraComponent);

	const bool bUseAudio = AudioComponent->ComponentHasTag(UseComponentTag);
	const bool bUseCascade = ParticleComponent->ComponentHasTag(UseComponentTag);
	const bool bUseNiagara = NiagaraComponent->ComponentHasTag(UseComponentTag);

	if (bInActive && (bUseAudio || bUseCascade || bUseNiagara))
	{
		bPoolingActive = true;

		if (bUseAudio)
		{
			AudioComponent->Play();
		}

		if (bUseCascade)
		{
			ParticleComponent->Activate(true);
		}
		else if (bUseNiagara)
		{
			NiagaraComponent->Activate(true);
		}
	}
	else if (bPoolingActive)
	{
		bPoolingActive = false;
		AudioComponent->Stop();
		ParticleComponent->Deactivate();
		NiagaraComponent->Deactivate();

		AudioComponent->ComponentTags.Empty();
		ParticleComponent->ComponentTags.Empty();
		NiagaraComponent->ComponentTags.Empty();
	}
}

bool AFootstepActor::IsPoolingActive() const
{
	return bPoolingActive;
}

void AFootstepActor::InitSound(USoundBase* Sound, float Volume, float Pitch, bool bIs2D, USoundAttenuation* AttenuationOverride, USoundConcurrency* ConcurrencyOverride)
{
	if (!Sound) { return; }

	check(AudioComponent);

	AudioComponent->SetSound(Sound);
	AudioComponent->SetVolumeMultiplier(Volume);
	AudioComponent->SetPitchMultiplier(Pitch);

	if (bIs2D)
	{
		if (AudioComponent->bAllowSpatialization)
		{
			AudioComponent->bAllowSpatialization = false;
			AudioComponent->bIsUISound = true;
		}
	}
	else
	{
		if (!AudioComponent->bAllowSpatialization)
		{
			AudioComponent->bAllowSpatialization = true;
			AudioComponent->bIsUISound = false;
		}
		AudioComponent->AttenuationSettings = AttenuationOverride;
	}

	AudioComponent->ConcurrencySet.Empty();
	if (ConcurrencyOverride)
	{
		AudioComponent->ConcurrencySet.Add(ConcurrencyOverride);
	}

	AudioComponent->ComponentTags.AddUnique(UseComponentTag);
}

void AFootstepActor::InitParticle(UObject* Particle, const FVector RelativeScale)
{
	if (!Particle) { return; }

	check(ParticleComponent && NiagaraComponent);

	if (UParticleSystem* ParticleSystem = Cast<UParticleSystem>(Particle))
	{
		ParticleComponent->SetTemplate(ParticleSystem);
		ParticleComponent->SetRelativeScale3D(RelativeScale);
		ParticleComponent->ComponentTags.AddUnique(UseComponentTag);
	}
	else if (UNiagaraSystem* NiagaraSystem = Cast<UNiagaraSystem>(Particle))
	{
		NiagaraComponent->SetAsset(NiagaraSystem);
		NiagaraComponent->SetRelativeScale3D(RelativeScale);
		NiagaraComponent->ComponentTags.AddUnique(UseComponentTag);
	}
}
