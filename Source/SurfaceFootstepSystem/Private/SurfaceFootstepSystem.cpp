// Copyright 1998-2020 Epic Games, Inc. All Rights Reserved.

#include "SurfaceFootstepSystem.h"
#include "SurfaceFootstepSystemSettings.h"
#include "FootstepTypes.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"

DEFINE_LOG_CATEGORY(LogFootstep);

#define LOCTEXT_NAMESPACE "FSurfaceFootstepSystemModule"

void FSurfaceFootstepSystemModule::StartupModule()
{
	RegisterSettings();
}

void FSurfaceFootstepSystemModule::ShutdownModule()
{
	if (UObjectInitialized())
	{
		UnregisterSettings();
	}
}

bool FSurfaceFootstepSystemModule::HandleSettingsSaved()
{
	if (USurfaceFootstepSystemSettings* Settings = GetMutableDefault<USurfaceFootstepSystemSettings>())
	{
		Settings->SaveConfig();
		return true;
	}
	return false;
}

void FSurfaceFootstepSystemModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "Surface Footstep System",
			LOCTEXT("RuntimeGeneralSettingsName", "Surface Footstep System"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "Configure the Surface Footstep System plugin"),
			GetMutableDefault<USurfaceFootstepSystemSettings>()
		);

		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FSurfaceFootstepSystemModule::HandleSettingsSaved);
		}
	}
}

void FSurfaceFootstepSystemModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Surface Footstep System");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSurfaceFootstepSystemModule, SurfaceFootstepSystem)