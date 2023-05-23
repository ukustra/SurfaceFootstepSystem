// Copyright 2019-2023 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Chaos/ChaosEngineInterface.h"
#include "FootstepComponent.generated.h"

class UFootstepDataAsset;
class USurfaceFootstepSystemSettings;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_EightParams(FFootstepDelegate, TEnumAsByte<EPhysicalSurface>, SurfaceType, const FGameplayTag&, Category, const FTransform&, ActorTransform, float, GeneratedVolume, float, GeneratedPitch, float, GeneratedSoundAssetVolume, float, GeneratedSoundAssetPitch, const FVector&, GeneratedParticleRelativeScale);

/**
 * A component from the Surface Footstep System plugin which specifies which footstep should be spawn, depending on the Surface Type.
 */
UCLASS( ClassGroup=(Utility), meta=(BlueprintSpawnableComponent) )
class SURFACEFOOTSTEPSYSTEM_API UFootstepComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

protected:
	/** Which footstep FXes should be spawned, depending on the Surface Type. */
	UPROPERTY(EditDefaultsOnly, Category = "Surface Footstep System")
	TMap<TEnumAsByte<EPhysicalSurface>, TSoftObjectPtr<UFootstepDataAsset>> FootstepFXes;

	/** Length of the trace which searches for the object at which footstep should be spawned. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Surface Footstep System", meta = (ClampMin = 0.f))
	float TraceLength;

	/** Will preload all footstep assets (Data Assets, Sounds, VFXes) asynchronously during registering the component and keep them in memory. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay, Category = "Surface Footstep System")
	bool bPreloadAssetsAsynchronously;
	
	/** Draws a trace which searches for the object at which footstep should be spawned (won't work in Shipping and Testing builds) and prints the debug message both in a log and on the screen. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay, Category = "Surface Footstep System")
	bool bShowDebug;

	//~ Begin UActorComponent Interface
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	//~ End UActorComponent Interface

public:
	/** Called when a new Footstep Actor is generated. */
	UPROPERTY(BlueprintAssignable, Category = "Surface Footstep System")
	FFootstepDelegate OnFootstepGenerated;

	/** What type of a footstep SFX should be spawned. */
	UFUNCTION(BlueprintPure, Category = "Surface Footstep System", meta = (Keywords = "is locally controlled"))
	bool GetPlaySound2D() const;

	/** Sets the new array of Actors ignored during tracing and clears the previous array. */
	UFUNCTION(BlueprintCallable, Category = "Surface Footstep System")
	void SetActorsToIgnoreForTrace(const TArray<AActor*>& NewActorsToIgnore);
	
	/** Adds an Actor to the array of Actors ignored during tracing. */
	UFUNCTION(BlueprintCallable, Category = "Surface Footstep System")
	void AddActorToIgnoreForTrace(AActor* NewActor);

	/** Removes an Actor from the array of Actors ignored during tracing. */
	UFUNCTION(BlueprintCallable, Category = "Surface Footstep System")
	bool RemoveActorToIgnoreForTrace(AActor* ActorToRemove);

	bool CreateFootstepLineTrace(const FVector& Start, const FVector& DirectionNormalVector, FHitResult& OutHit) const;
	UFootstepDataAsset* GetFootstepData(const EPhysicalSurface SurfaceType) const;

	float GetTraceLength() const;
	bool GetShowDebug() const;

private:
	UPROPERTY()
	TObjectPtr<USurfaceFootstepSystemSettings> FootstepSettings;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> ActorsToIgnore;

	bool bPreloading;

	void TryPreloading();
	void CancelPreloading();
};
