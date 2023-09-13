#include "Helper.h"

bool Helper::CheckIfLocationIsInFOV(const UWorld* World, const FVector& Location)
{
	// Get the player controller (assuming a single player)
	const APlayerController* PlayerController = World->GetFirstPlayerController();

	// Get the player camera manager

	if (const APlayerCameraManager* CameraManager = PlayerController ? PlayerController->PlayerCameraManager : nullptr)
	{
		// Get the camera location and forward vector
		const FVector CameraLocation = CameraManager->GetCameraLocation();
		const FVector CameraForwardVector = CameraManager->GetCameraRotation().Vector();

		// Get the vector from the camera to the actor
		const FVector ActorToCameraVector = Location - CameraLocation;

		// Calculate yaw and pitch angles between the actor and the camera
		float YawAngle = FMath::Acos(FVector::DotProduct(ActorToCameraVector.GetSafeNormal(), CameraForwardVector));
		float PitchAngle = FMath::Acos(FVector::DotProduct(ActorToCameraVector.GetSafeNormal(),
		                                                   CameraManager->GetCameraRotation().Vector().Rotation().
		                                                                  Vector()));

		// Convert the angles from radians to degrees
		YawAngle = FMath::RadiansToDegrees(YawAngle);
		PitchAngle = FMath::RadiansToDegrees(PitchAngle);

		const float FOV = CameraManager->GetFOVAngle() / 2;

		if (YawAngle < FOV && PitchAngle < FOV)
		{
			return true;
		}
		return false;
	}
	return false;
}
