// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FoostepPoolingManager.generated.h"

class AFootstepActor;

/**
 * An object from the Surface Footstep System plugin which manages Footstep Actors pooling. Must be added to the World Settings.
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class SURFACEFOOTSTEPSYSTEM_API UFoostepPoolingManager : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Destroys all Footstep Actors. This won't work if you use World Settings class which doesn't implement a Footstep Interface and doesn't override the "GetPoolingManagerComponent" function.
	In a multiplayer game, this should be called everywhere the Surface Footstep Anim Notify is executed (it won't be executed on the Dedicated Server). */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Surface Footstep System", meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
	static void DestroyFootstepPool(const UObject* WorldContextObject);

	void SafeSpawnPooledActor();
	void DestroyPooledActors();
	AFootstepActor* GetPooledActor();

private:
	UPROPERTY()
	TArray<AFootstepActor*> PooledActors;
};
