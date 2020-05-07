// Copyright 1998-2020 Epic Games, Inc. All Rights Reserved.

#include "SurfaceFootstepSystemEditor.h"
#include "AssetTypeActions_FootstepDataAsset.h"

#define LOCTEXT_NAMESPACE "FSurfaceFootstepSystemEditorModule"

void FSurfaceFootstepSystemEditorModule::StartupModule()
{
	RegisterAssetTools();
}

void FSurfaceFootstepSystemEditorModule::ShutdownModule()
{
	UnregisterAssetTools();
}

void FSurfaceFootstepSystemEditorModule::RegisterAssetTools()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	FootstepAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory("SurfaceFootstepSystem", LOCTEXT("SurfaceFootstepSystemAssetCategory", "Surface Footstep System"));
	TSharedRef<IAssetTypeActions> Action = MakeShareable(new FAssetTypeActions_FootstepDataAsset(FootstepAssetCategoryBit));

	AssetTools.RegisterAssetTypeActions(Action);
	RegisteredAssetTypeActions.Add(Action);
}

void FSurfaceFootstepSystemEditorModule::UnregisterAssetTools()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (auto& Action : RegisteredAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
		}
		RegisteredAssetTypeActions.Empty();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSurfaceFootstepSystemEditorModule, SurfaceFootstepSystemEditor)
