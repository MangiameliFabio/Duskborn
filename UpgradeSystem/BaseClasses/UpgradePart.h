#pragma once
#include "Duskborn/Helper/MathHelper.h"
#include "Duskborn/Enums.h"

#include "UpgradePart.generated.h"

UCLASS(Blueprintable)
class DUSKBORN_API UUpgradePart : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Name of the part which is displayed in inventroy */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Name = "PartName";

	UPROPERTY(BlueprintReadOnly)
	float ScrapValue = 0.f;

	/** Describes how chances are to get this part
	 *	Mark_I		= 65%
	 *	Mark_II		= 25%
	 *  Mark_III	= 10%
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERarity Rarity = ERarity::MARK_I;

	/** Energy needed to use this part */
	UPROPERTY(BlueprintReadOnly)
	uint8 EnergyCost = 0;

	/** Describe what the part can do */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Description = "Your description here!";

	/** Add description for lore. Currently not used */ 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString LoreDescription = "Pls nice Lore here";

	/** Image which is used for UpgradePart in UI*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Image = nullptr;

	void Init()
	{
		ScrapValue = 15 * MathHelper::Pow(2, static_cast<int>(Rarity));
		EnergyCost = static_cast<int>(Rarity) + 1;
	};
};
