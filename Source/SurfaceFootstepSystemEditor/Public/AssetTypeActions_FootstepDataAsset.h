// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#pragma once

#include "AssetTypeActions_Base.h"

class FAssetTypeActions_FootstepDataAsset : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_FootstepDataAsset(EAssetTypeCategories::Type InAssetCategory);

	//~ Begin IAssetTypeActions interface
	virtual FText GetName() const override;
	virtual uint32 GetCategories() override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	//~ End IAssetTypeActions interface

private:
	EAssetTypeCategories::Type AssetCategory;
};
