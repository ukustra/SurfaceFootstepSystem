// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#pragma once

#include "SurfaceFootstepSystemSettings.generated.h"

/**
 * Editor settings for the Surface Footstep System plugin.
 */
UCLASS(config = Plugins, defaultconfig)
class SURFACEFOOTSTEPSYSTEM_API USurfaceFootstepSystemSettings : public UObject
{
	GENERATED_UCLASS_BODY()
	
protected:
	/** Put your footstep categories (for example: Footstep.Run, Footstep.Walk, Footstep.Crouch, etc.) here. */
	UPROPERTY(config, EditDefaultsOnly, Category = "General", meta = (Categories = "Footstep"))
	TArray<FGameplayTag> FootstepCategories;

	/** On which objects types footstep should be spawned. If this array is empty, footstep will be spawned on every object type. */
	UPROPERTY(config, EditDefaultsOnly, Category = "Trace")
	TArray<TEnumAsByte<ECollisionChannel>> FootstepObjectTypes;

	/** Used as a default value in Footstep Component. */
	UPROPERTY(config, EditDefaultsOnly, Category = "Trace", meta = (ClampMin = 0.f))
	float DefaultTraceLength;

	/** Whether complex or simple collision should be used in trace. */
	UPROPERTY(config, EditDefaultsOnly, AdvancedDisplay, Category = "Trace")
	bool bTraceComplex;

	/** Maximum amount of spawned Footstep Actors. */
	UPROPERTY(config, EditDefaultsOnly, Category = "Pooling", meta = (ClampMin = 1))
	int32 MaxPoolSize;

	/** The default visibility time of spawned Footstep Actors. */
	UPROPERTY(config, EditDefaultsOnly, Category = "Pooling", meta = (ClampMin = 0.f))
	float DefaultFootstepActorLifeSpan;

	/** Whether footstep SFX should be a 2D sound for a Local Player. If the footstep causer doesn't inherit from a Pawn class, 2D sound won't be spawned. */
	UPROPERTY(config, EditDefaultsOnly, Category = "Sound")
	bool bPlaySound2D_ForLocalPlayer;

	/** Used as a default asset in Footstep Data Assets. */
	UPROPERTY(config, EditDefaultsOnly, Category = "Sound", meta = (AllowedClasses = "SoundAttenuation"))
	FStringAssetReference DefaultAttenuationOverride;

	/** Used as a default asset in Footstep Data Assets. */
	UPROPERTY(config, EditDefaultsOnly, Category = "Sound", meta = (AllowedClasses = "SoundConcurrency"))
	FStringAssetReference DefaultConcurrencyOverride;

public:
	static USurfaceFootstepSystemSettings* Get();

	int32 GetCategoriesNum() const;
	bool ContainsCategory(const FGameplayTag& CategoryTag) const;
	FGameplayTag GetCategoryName(int32 Index) const;

	TArray<TEnumAsByte<ECollisionChannel>> GetFootstepObjectTypes() const;
	float GetDefaultTraceLength() const;
	bool GetTraceComplex() const;

	int32 GetPoolSize() const;
	float GetDefaultPoolingLifeSpan() const;
	
	bool GetPlaySound2D() const;
	FString GetAttenuationAssetPath() const;
	FString GetConcurrencyAssetPath() const;
};
