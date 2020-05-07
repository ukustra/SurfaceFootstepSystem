// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#include "FootstepInterface.h"

UFootstepInterface::UFootstepInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UFoostepPoolingManager* IFootstepInterface::GetPoolingManager() const
{
	return nullptr;
}
