#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Structs/ResourceConversionEntry.h"
#include "ConversionData.generated.h"

UCLASS()
class COFFEESHOPGAME_API UConversionData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FResourceConversionEntry> ConversionEntries;
};
