// Fill out your copyright notice in the Description page of Project Settings.


#include "PostGPT.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#include "MyPluginSettings.h"

void UPostGPT::Event_OnSuccess(FString ReturnMessage)
{
	if (OnSuccess.IsBound())
	{
		OnSuccess.Broadcast(ReturnMessage);
	}
	RemoveFromRoot();
}

void UPostGPT::Event_OnFailed(FString ReturnMessage)
{
	if (OnFailed.IsBound())
	{
		OnFailed.Broadcast(ReturnMessage);
	}
	RemoveFromRoot();
}

UPostGPT* UPostGPT::AskAI(const FString& question)
{
	UPostGPT* PostTool = NewObject<UPostGPT>();

	//·ÀÖ¹GC
	PostTool->AddToRoot();

	PostTool->StartApply(question);

	return PostTool;

}

FString UPostGPT::GetGPTKey()
{

	UMyPluginSettings* PluginSettings = GetMutableDefault<UMyPluginSettings>();
	FString MyParameterValue = PluginSettings->GptKey;
	return MyParameterValue;
 }

void UPostGPT::StartApply(FString question)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> MessagesArray;



	TSharedPtr<FJsonObject> MessageObject = MakeShareable(new FJsonObject);
	MessageObject->SetStringField("role", "user");
	MessageObject->SetStringField("content", question);
	TSharedPtr<FJsonValueObject> MessageJsonValue = MakeShareable(new FJsonValueObject(MessageObject));
	MessagesArray.Add(MessageJsonValue);
	JsonObject->SetStringField("model", "gpt-3.5-turbo");
	JsonObject->SetArrayField("messages", MessagesArray);
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
	UE_LOG(LogTemp, Display, TEXT("%s"), *JsonString);



	FHttpModule& Http = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http.CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UPostGPT::OnResponseReceived);



	FString URl = "https://api.openai.com/v1/chat/completions";

	Request->SetURL(URl);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", TEXT("application/json"));


	//Request->SetHeader("Authorization", (TEXT("Bearer %s"), *UPostGPT::GetGPTKey()));
	FString key ="Bearer "+ UPostGPT::GetGPTKey();
	

	Request->SetHeader("Authorization", key);
	Request->SetContentAsString(JsonString);

	Request->ProcessRequest();

}

void UPostGPT::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString ReturnMessage = "No Return";
	if (!Response.IsValid())
	{
		ReturnMessage = "{\"success\":\"Error: Unable to process HTTP Request!\"}";
		Event_OnFailed(ReturnMessage);
	}
	else if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		FString TempSting;
		TempSting = Response->GetContentAsString();
		TSharedPtr<FJsonObject> rRoot;
		TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(TempSting);

		if (FJsonSerializer::Deserialize(Reader, rRoot))
		{


			if (rRoot->HasField(TEXT("object")))
			{
				//ReturnMessage = rRoot->GetStringField(TEXT("object"));
				TArray<TSharedPtr<FJsonValue>> choicesArray = rRoot->GetArrayField(TEXT("choices"));
				ReturnMessage = choicesArray[0]->AsObject()->GetObjectField(TEXT("message"))->GetStringField(TEXT("content"));
				//choicesArray[0]->TryGetObject(TEXT("message"), /*out*/ messageObject);

				Event_OnSuccess(ReturnMessage);
			}
			else {
				Event_OnFailed(ReturnMessage);
			}
		}
		else
		{
			Event_OnFailed(ReturnMessage);
		}
		UE_LOG(LogTemp, Display, TEXT("%s"), *TempSting);

	}
	else
	{
		ReturnMessage = FString::Printf(TEXT("{\"success\":\"HTTP Error: %d\"}"), Response->GetResponseCode());
		Event_OnFailed(ReturnMessage);
	}

}

