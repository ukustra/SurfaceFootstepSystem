// Copyright 2019 Urszula Kustra. All Rights Reserved.

#include "FootstepInterface.h"

UFootstepInterface::UFootstepInterface(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UFoostepPoolingManagerComponent* IFootstepInterface::GetPoolingManagerComponent() const
{
	return nullptr;
}
