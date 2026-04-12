#pragma once

#include "CoreMinimal.h"
#include "Prompt.h"
#include "Engine/DataAsset.h"
#include "ActionPromptsData.generated.h"

enum class EAction : uint8;
enum class EActionId : uint8;

USTRUCT(BlueprintType)
struct FActionPromptEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAction Action;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPrompt Prompt;
};

UCLASS()
class COFFEESHOPGAME_API UActionPromptsData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	const TArray<FActionPromptEntry> GetEntries(){ return Entries; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FActionPromptEntry> Entries;
};
