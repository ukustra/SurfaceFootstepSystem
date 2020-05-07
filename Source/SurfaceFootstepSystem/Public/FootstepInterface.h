// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FootstepInterface.generated.h"

class UFootstepComponent;
class UFoostepPoolingManager;

UINTERFACE(BlueprintType)
class UFootstepInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * An interface from the Surface Footstep System plugin.
 */
class SURFACEFOOTSTEPSYSTEM_API IFootstepInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SurfaceFootstepSystem")
	UFootstepComponent* GetFootstepComponent() const;

	virtual UFoostepPoolingManager* GetPoolingManager() const;
};
