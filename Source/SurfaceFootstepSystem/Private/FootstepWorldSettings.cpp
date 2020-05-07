// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#include "FootstepWorldSettings.h"

AFootstepWorldSettings::AFootstepWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (GetNetMode() != NM_DedicatedServer)
	{
		PoolingManager = ObjectInitializer.CreateDefaultSubobject<UFoostepPoolingManager>(this, TEXT("PoolingManager"));
	}
}

UFoostepPoolingManager* AFootstepWorldSettings::GetPoolingManager() const
{
	return PoolingManager;
}
