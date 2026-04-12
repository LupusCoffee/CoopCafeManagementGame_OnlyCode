#pragma once

#include "CoreMinimal.h"
#include "Prompt.generated.h"

enum class EActionId : uint8;

USTRUCT(BlueprintType)
struct FPrompt
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EActionId Input;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Prompt;
};
