#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PlayerPromptComponent.h"
#include "Characters/Player/PlayerCharacter.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ActionPromptSubsystem.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PlayerPromptWidget.h"

UPlayerPromptComponent::UPlayerPromptComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerPromptComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UActionPromptSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UActionPromptSubsystem>();
	if (!Subsystem) return;
	
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return;
	
	APlayerController* Controller = Cast<APlayerController>(Character->GetController());
	if (!Controller) return;
	
	PlayerPromptWidget = Subsystem->CreatePlayerPromptWidget(Controller);
	if (!PlayerPromptWidget) return;
	
	PlayerPromptWidget->AddToViewport();
}

void UPlayerPromptComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

UPlayerPromptWidget* UPlayerPromptComponent::GetPromptWidget()
{
	return PlayerPromptWidget;
}

void UPlayerPromptComponent::SetPromptVisibility(bool Value)
{
	if (Value) PlayerPromptWidget->SetVisibility(ESlateVisibility::Visible);
	else PlayerPromptWidget->SetVisibility(ESlateVisibility::Hidden);
}

