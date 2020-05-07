// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#include "AssetTypeActions_FootstepDataAsset.h"
#include "FootstepDataAsset.h"

#define LOCTEXT_NAMESPACE "SurfaceFootstepSystem"

FAssetTypeActions_FootstepDataAsset::FAssetTypeActions_FootstepDataAsset(EAssetTypeCategories::Type InAssetCategory)
	: AssetCategory(InAssetCategory)
{
}

FText FAssetTypeActions_FootstepDataAsset::GetName() const
{
	return LOCTEXT("FAssetTypeActions_FootstepDataAssetName", "Footstep Data Asset");
}

uint32 FAssetTypeActions_FootstepDataAsset::GetCategories()
{
	return AssetCategory;
}

FColor FAssetTypeActions_FootstepDataAsset::GetTypeColor() const
{
	return FColor(0, 188, 0, 255);
}

UClass* FAssetTypeActions_FootstepDataAsset::GetSupportedClass() const
{
	return UFootstepDataAsset::StaticClass();
}

#undef LOCTEXT_NAMESPACE
