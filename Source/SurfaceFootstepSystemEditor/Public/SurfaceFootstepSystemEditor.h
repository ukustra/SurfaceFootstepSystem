// Copyright 1998-2020 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSurfaceFootstepSystemEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	void RegisterAssetTools();
	void UnregisterAssetTools();

	EAssetTypeCategories::Type FootstepAssetCategoryBit;
	TArray<TSharedPtr<IAssetTypeActions>> RegisteredAssetTypeActions;
};
