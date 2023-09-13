#include "Log.h"

void Log::Print(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message)
}

void Log::Print(const int& IntToPrint, const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s %d"), *Message, IntToPrint)
}

void Log::Print(const float& FloatToPrint, const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s %f"), *Message, FloatToPrint)
}

void Log::Print(const double& DoubleToPrint, const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s %f"), *Message, DoubleToPrint)
}

void Log::Print(const FVector& VectorToPrint, const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s %s"), *Message, *VectorToPrint.ToString())
}

void Log::PrintBool(const bool& BoolToPrint, const FString& Message)
{
	if (BoolToPrint)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s true"), *Message)
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s false"), *Message)
	}
}

void Log::PrintToScreen(const FString& Message, float Duration, FColor Color)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, Duration, Color, Message);
}

void Log::PrintToScreen(float FloatValue, const FString& Message, float Duration, FColor Color)
{
	FString StringToPrint = Message;
	StringToPrint.Append(FString::SanitizeFloat(FloatValue));

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, Duration, Color, StringToPrint);
}
