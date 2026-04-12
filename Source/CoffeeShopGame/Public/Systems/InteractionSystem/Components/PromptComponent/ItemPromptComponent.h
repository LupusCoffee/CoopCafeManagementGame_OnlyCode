#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ItemPromptComponent.generated.h"

enum class EAction : uint8;
enum class EActionId : uint8;
class UPromptWidgetBox;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UItemPromptComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UItemPromptComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category = "PromptComponent")
	UPromptWidgetBox* GetPromptBox();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PromptComponent")
	float HeightAboveOwnerOffset = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PromptComponent")
	TArray<EAction> CapableActions;
	
	UPROPERTY()
	UPromptWidgetBox* PromptWidgetBox = nullptr;
	
	UPROPERTY()
	APawn* PlayerPawn = nullptr;
	
	UFUNCTION()
	void FacePlayer();
};
