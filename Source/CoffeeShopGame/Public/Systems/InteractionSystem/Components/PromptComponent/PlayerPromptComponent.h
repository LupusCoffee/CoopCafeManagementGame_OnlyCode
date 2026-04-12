#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerPromptComponent.generated.h"

enum class EAction : uint8;
class UPlayerPromptWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UPlayerPromptComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerPromptComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category = "PromptComponent")
	UPlayerPromptWidget* GetPromptWidget();
	
	UFUNCTION(BlueprintCallable, Category = "PromptComponent")
	void SetPromptVisibility(bool Value);
	
protected:
	UPROPERTY()
	UPlayerPromptWidget* PlayerPromptWidget = nullptr;
};
