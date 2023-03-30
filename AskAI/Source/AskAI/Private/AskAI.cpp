// Copyright HotMonk, Inc. All Rights Reserved.

#include "AskAI.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#include "MyPluginSettings.h"


#define LOCTEXT_NAMESPACE "FAskAIModule"

void FAskAIModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// 获取引擎设置
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");


	if (SettingsModule != nullptr)

	{
		ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");

		if (SettingsContainer.IsValid())
		{
			SettingsContainer->DescribeCategory("MyPlugin",
				LOCTEXT("MyPluginCategory", "My Plugin"),
				LOCTEXT("MyPluginCategoryDescription", "Configuration for the MyPlugin plugin"));

			ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "AskAI",



				LOCTEXT("MyPluginGeneralSettingsName", "AskAI_ChatGPT"),
				LOCTEXT("MyPluginGeneralSettingsDescription", "General settings for the AskAI plugin"),
				GetMutableDefault<UMyPluginSettings>());

			if (SettingsSection.IsValid())
			{
				SettingsSection->OnModified().BindRaw(this, &FAskAIModule::HandleSettingsSaved);
			}
		}
	}

}

void FAskAIModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

  	// 从引擎设置中移除插件设置
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "AskAI");
	}

}

bool FAskAIModule::HandleSettingsSaved()
{
	UMyPluginSettings* PluginSettings = GetMutableDefault<UMyPluginSettings>();
	FString MyParameterValue = PluginSettings->GptKey;


	UE_LOG(LogTemp, Warning, TEXT("SaveOk Key=%s"), *MyParameterValue);


	return false;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAskAIModule, AskAI)