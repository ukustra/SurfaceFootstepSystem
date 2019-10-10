// Copyright 2019 Urszula Kustra. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_SurfaceFootstep.generated.h"

class USurfaceFootstepSystemSettings;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimNotify", meta = (ExposeOnSpawn = true, Categories = "Footstep"))
	FGameplayTag FootstepCategory;

	/** If the socket is not rotated, "Down" direction should be used most of the time, but in some cases (for instance, wall climbing or falling on the floor) you might want to change a trace direction. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	EFootstepTraceDirection FootstepTraceDirection;

	/** If false, trace will start at Root socket location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	bool bTraceFromFootSocket;

	/** A socket name from which a trace will be created. If it doesn't exist in the skeletal mesh, Root socket will be used. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimNotify", meta = (ExposeOnSpawn = true, EditCondition = bTraceFromFootSocket))
	FName FootSocket;

	//~ Begin UAnimNotify Interface
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;
	//~ End UAnimNotify Interface

private:
	UPROPERTY()
	USurfaceFootstepSystemSettings* FootstepSettings;

	bool TraceFromFootSocket() const;
	FString GetActorName(const AActor* Actor) const;
};
