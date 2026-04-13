#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ItemPromptComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ActionPromptSubsystem.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PromptWidgetBox.h"

enum class EAction : uint8;

UItemPromptComponent::UItemPromptComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	//default values
	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(150, 150);
}

void UItemPromptComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
	//Set Invisible, Set Using Absolute Scale
	SetVisibility(true);
	SetUsingAbsoluteScale(true);
	
	
	//Attach Myself At The Center of RootComponent/PhysicsObject
	AActor* Owner = GetOwner();

	UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(Owner->GetRootComponent());
	if (!RootPrim) RootPrim = Owner->GetComponentByClass<UMeshComponent>();

	if (RootPrim)
	{
		AttachToComponent(RootPrim, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		FVector Origin = RootPrim->Bounds.Origin;
		SetWorldLocation(Origin + FVector(0, 0, HeightAboveOwnerOffset));
	}

	
	//Set Widget
	UActionPromptSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UActionPromptSubsystem>();
	if (!Subsystem) return;
	
	PromptWidgetBox = Subsystem->CreatePromptWidgetBox();
	if (!PromptWidgetBox) return;
	
	SetWidget(PromptWidgetBox);
}

void UItemPromptComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FacePlayer();
}

void UItemPromptComponent::FacePlayer()
{
	if (!PlayerPawn)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController) PlayerPawn = PlayerController->GetPawn();
	}
	
	if (!PlayerPawn) return;
	
	FVector CompLocation = GetComponentLocation();
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(CompLocation, PlayerLocation);
	SetWorldRotation(LookRotation);
}

UPromptWidgetBox* UItemPromptComponent::GetPromptBox()
{
	return PromptWidgetBox;
}
