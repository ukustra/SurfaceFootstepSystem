// Copyright 2019-2023 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FootstepInterface.generated.h"

class UFootstepComponent;
class UFootstepPoolingManager;

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
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SurfaceFootstepSystem")
	UFootstepComponent* GetFootstepComponent() const;
};
