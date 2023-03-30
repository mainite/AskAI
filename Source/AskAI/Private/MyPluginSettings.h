// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyPluginSettings.generated.h"

/**
 * 
 */
UCLASS(config = MyPlugin, defaultconfig)
class UMyPluginSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = General, meta = (DisplayName = "GPT_key"))
		FString GptKey;
	
};
