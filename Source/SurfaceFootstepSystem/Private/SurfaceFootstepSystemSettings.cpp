// Copyright 2019-2020 Urszula Kustra. All Rights Reserved.

#include "SurfaceFootstepSystemSettings.h"
#include "GameplayTagContainer.h"

USurfaceFootstepSystemSettings::USurfaceFootstepSystemSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DefaultTraceLength(50.f)
	, MaxPoolSize(20)
	, DefaultFootstepActorLifeSpan(3.f)
	, bPlaySound2D_ForLocalPlayer(true)
{
	FootstepCategories.Add(FGameplayTag::EmptyTag);

	FootstepObjectTypes.Add(ECC_WorldStatic);
	FootstepObjectTypes.Add(ECC_WorldDynamic);

	DefaultAttenuationOverride.SetPath(TEXT("/SurfaceFootstepSystem/FootstepAttenuationExample.FootstepAttenuationExample"));
	DefaultConcurrencyOverride.SetPath(TEXT("/SurfaceFootstepSystem/FootstepConcurrencyExample.FootstepConcurrencyExample"));
}

USurfaceFootstepSystemSettings* USurfaceFootstepSystemSettings::Get()
{
	return Cast<USurfaceFootstepSystemSettings>( USurfaceFootstepSystemSettings::StaticClass()->GetDefaultObject() );
}

int32 USurfaceFootstepSystemSettings::GetCategoriesNum() const
{
	return FootstepCategories.Num();
}

bool USurfaceFootstepSystemSettings::ContainsCategory(const FGameplayTag& CategoryTag) const
{
	return FootstepCategories.Contains(CategoryTag);
}

FGameplayTag USurfaceFootstepSystemSettings::GetCategoryName(int32 Index) const
{
	return FootstepCategories.IsValidIndex(Index) ? FootstepCategories[Index] : FGameplayTag::EmptyTag;
}

TArray<TEnumAsByte<ECollisionChannel>> USurfaceFootstepSystemSettings::GetFootstepObjectTypes() const
{
	return FootstepObjectTypes;
}

float USurfaceFootstepSystemSettings::GetDefaultTraceLength() const
{
	return DefaultTraceLength > 0.f ? DefaultTraceLength : 0.f;
}

bool USurfaceFootstepSystemSettings::GetTraceComplex() const
{
	return bTraceComplex;
}

int32 USurfaceFootstepSystemSettings::GetPoolSize() const
{
	return MaxPoolSize > 1 ? MaxPoolSize : 1;
}

float USurfaceFootstepSystemSettings::GetDefaultPoolingLifeSpan() const
{
	return DefaultFootstepActorLifeSpan > 0.f ? DefaultFootstepActorLifeSpan : 0.f;
}

bool USurfaceFootstepSystemSettings::GetPlaySound2D() const
{
	return bPlaySound2D_ForLocalPlayer;
}

FString USurfaceFootstepSystemSettings::GetAttenuationAssetPath() const
{
	return DefaultAttenuationOverride.GetAssetPathString();
}

FString USurfaceFootstepSystemSettings::GetConcurrencyAssetPath() const
{
	return DefaultConcurrencyOverride.GetAssetPathString();
}
