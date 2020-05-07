// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "FootstepInterface.h"
#include "FootstepWorldSettings.generated.h"

class UFoostepPoolingManager;

/**
 * A World Settings class from the Surface Footstep System plugin. Use it if possible. If not, override the same interface methods in your C++ World Settings class.
 */
UCLASS(NotBlueprintType)
class SURFACEFOOTSTEPSYSTEM_API AFootstepWorldSettings : public AWorldSettings, public IFootstepInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin IFootstepInterface interface
	virtual UFoostepPoolingManager* GetPoolingManager() const override;
	//~ End IFootstepInterface interface

private:
	UPROPERTY()
	UFoostepPoolingManager* PoolingManager;
};
