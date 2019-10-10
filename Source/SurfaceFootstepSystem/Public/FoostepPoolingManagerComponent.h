// Copyright 2019 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FoostepPoolingManagerComponent.generated.h"

class AFootstepActor;

/**
 * A component from the Surface Footstep System plugin which manages Footstep Actors pooling. Must be added to the World Settings.
 */
UCLASS( NotBlueprintType )
class SURFACEFOOTSTEPSYSTEM_API UFoostepPoolingManagerComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
	/** Destroys all Footstep Actors. This won't work if you use World Settings class which doesn't implement a Footstep Interface and doesn't override the "GetPoolingManagerComponent" function.
	In a multiplayer game, this should be called everywhere the Surface Footstep Anim Notify is executed (it won't be executed on the Dedicated Server). */
	UFUNCTION(BlueprintCallable, Category = "Surface Footstep System", meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
	static void DestroyFootstepPool(const UObject* WorldContextObject);

	void SafeSpawnPooledActor();
	void DestroyPooledActors();
	AFootstepActor* GetPooledActor();

private:
	UPROPERTY()
	TArray<AFootstepActor*> PooledActors;
};
