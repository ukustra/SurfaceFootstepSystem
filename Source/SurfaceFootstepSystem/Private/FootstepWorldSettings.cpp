// Copyright 2019 Urszula Kustra. All Rights Reserved.

#include "FootstepWorldSettings.h"

AFootstepWorldSettings::AFootstepWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PoolingManagerComponent = ObjectInitializer.CreateDefaultSubobject<UFoostepPoolingManagerComponent>(this, TEXT("PoolingManagerComponent"));
}

UFoostepPoolingManagerComponent* AFootstepWorldSettings::GetPoolingManagerComponent() const
{
	return PoolingManagerComponent;
}
