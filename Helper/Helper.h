#pragma once

class Helper
{
public:
	/** Check if the point given is inside of the player field of view */
	static bool CheckIfLocationIsInFOV(const UWorld* World, const FVector& Location);
};
