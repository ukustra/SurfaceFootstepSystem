/***
	Author: Chinyone
	Copyright: Free
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineTypes.h"
#include "SurfaceFootstepSystemSettings.h"
#include "NonAnimNotify_SurfaceFootstep.generated.h"

UCLASS()
class SURFACEFOOTSTEPSYSTEM_API ANonAnimNotify_SurfaceFootstep : public AActor
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

public:	
	/** Has to be one of the names from the Surface Footstep System Settings in the Project Settings. */
	UPROPERTY(Editanywhere)
	FGameplayTag FootstepCategory;

	/** Character that implements the FootstepInterface. Needs to be set*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Setup)
	ACharacter* CharacterReference;

	ANonAnimNotify_SurfaceFootstep();
	virtual void Tick(float DeltaTime) override;
	
	/** Blueprint callable function that produces a Footstep */
	UFUNCTION(BlueprintCallable, Category = "SurfaceFootstepSystem")
	void MakeFootstep();

private:
	UPROPERTY()
	USurfaceFootstepSystemSettings* FootstepSettings;

	UPhysicalMaterial* GetPhysicalMaterial(const FHitResult& HitResult) const;
	FString GetActorName(const AActor* Actor) const;
};
