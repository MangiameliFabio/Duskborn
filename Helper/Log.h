#pragma once

//Simple logging class to make console prints and prints to screen easier.
class Log
{
public:
	static void Print(const FString& Message);
	static void Print(const int& IntToPrint, const FString& Message = "");
	static void Print(const float& FloatToPrint, const FString& Message = "");
	static void Print(const double& DoubleToPrint, const FString& Message = "");
	static void Print(const FVector& VectorToPrint, const FString& Message = "");
	static void PrintBool(const bool& BoolToPrint, const FString& Message = "");

	static void PrintToScreen(const FString& Message, float Duration = 5.f, FColor Color = FColor::Green);
	static void PrintToScreen(float FloatValue, const FString& Message = "", float Duration = 5.f, FColor Color = FColor::Green);

};
