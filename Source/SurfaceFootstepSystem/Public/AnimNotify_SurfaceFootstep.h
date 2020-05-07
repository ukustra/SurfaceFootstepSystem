// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineTypes.h"
#include "AnimNotify_SurfaceFootstep.generated.h"

class USurfaceFootstepSystemSettings;
class UPhysicalMaterial;

UENUM()
enum class EFootstepTraceDirection : uint8
{
	Down,
	Up,
	Forward,
	Backward,
	Right,
	Left
};

/**
 * An anim notify from the Surface Footstep System plugin which describes footstep properties.
 */
UCLASS(NotBlueprintable, NotBlueprintType, meta = (DisplayName = "Surface Footstep"))
class SURFACEFOOTSTEPSYSTEM_API UAnimNotify_SurfaceFootstep : public UAnimNotify
{
	GENERATED_UCLASS_BODY()
	
public:
	/** Has to be one of the names from the Surface Footstep System Settings in the Project Settings. */
	UPROPERTY(EditAnywhere, Category = "AnimNotify", meta = (Categories = "Footstep"))
	FGameplayTag FootstepCategory;

	/** If the socket is not rotated, "Down" direction should be used most of the time, but in some cases (for instance, wall climbing or falling on the floor) you might want to change a trace direction. */
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	EFootstepTraceDirection FootstepTraceDirection;

	/** If false, trace will start at Root socket location. */
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	bool bTraceFromFootSocket;

	/** A socket name from which a trace will be created. If it doesn't exist in the skeletal mesh, Root socket will be used. */
	UPROPERTY(EditAnywhere, Category = "AnimNotify", meta = (EditCondition = bTraceFromFootSocket))
	FName FootSocket;

	//~ Begin UAnimNotify Interface
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;
	//~ End UAnimNotify Interface

private:
	UPROPERTY()
	USurfaceFootstepSystemSettings* FootstepSettings;

	bool TraceFromFootSocket() const;
	UPhysicalMaterial* GetPhysicalMaterial(const FHitResult& HitResult) const;
	FString GetActorName(const AActor* Actor) const;
};
