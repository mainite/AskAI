// Copyright HotMonk, Inc. All Rights Reserved.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Http.h"
#include "HttpModule.h"
#include "PostGPT.generated.h"

/**
 * 
 */
UCLASS()
class ASKAI_API UPostGPT : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPostResult, FString, ReturnMessage);


	UPROPERTY(BlueprintAssignable)
		FPostResult OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FPostResult OnFailed;

	void Event_OnSuccess(FString ReturnMessage);


	void Event_OnFailed(FString ReturnMessage);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "PostTool")
	static UPostGPT* AskAI(const FString& question);

	UFUNCTION(BlueprintCallable)
	static	FString GetGPTKey();



	void StartApply(FString question);


	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
};
