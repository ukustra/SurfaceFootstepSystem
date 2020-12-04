// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FoostepPoolingManager.generated.h"

class AFootstepActor;

/**
 * A subsystem from the Surface Footstep System plugin which manages Footstep Actors pooling.
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class SURFACEFOOTSTEPSYSTEM_API UFoostepPoolingManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Destroys all Footstep Actors.
	In a multiplayer game, this should be called everywhere the Surface Footstep Anim Notify is executed (it won't be executed on the Dedicated Server). */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Surface Footstep System", meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
	static void DestroyFootstepPool(const UObject* WorldContextObject);

	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End USubsystem Interface

	void SafeSpawnPooledActor();
	void DestroyPooledActors();
	AFootstepActor* GetPooledActor();

	UFoostepPoolingManager();

private:
	UPROPERTY()
	TArray<AFootstepActor*> PooledActors;
};
