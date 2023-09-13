#pragma once
#include "Duskborn/Helper/MathHelper.h"
#include "Duskborn/Enums.h"

#include "LevelUpUpgrades.generated.h"

UCLASS(Blueprintable)
class DUSKBORN_API ULevelUpUpgrades : public UDataAsset
{
	GENERATED_BODY()

public:	
	/** Name of the stat which should be upgraded */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString StatName = "UpgradeName";

	/** Enum of the upgraded stat */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EStats StatEnum = EStats::NO_VALUE;

	/** Description which is shown in the upgrade card */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description = FText::FromString("Description here pls");

	/** The amount which should be added to stat */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Amount = 0;

	/** Icon of Stat */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon = nullptr;
};
