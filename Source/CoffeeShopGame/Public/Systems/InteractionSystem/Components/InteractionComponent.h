#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/Framework/Subsystems/LookTraceSubsystem.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Enums/ActionId.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Structs/PlayerContext.h"
#include "InteractionComponent.generated.h"
enum class EActionId : uint8;


UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	Started,
	Completed
};


USTRUCT()
struct FActiveInteraction
{
	GENERATED_BODY()

	UPROPERTY()
	EActionId ActionId = EActionId::None;

	UPROPERTY()
	AActor* Actor = nullptr;
	
	UPROPERTY()
	TArray<UActorComponent*> Components;

	void Set(EActionId InActionId, AActor* InActor, TArray<UActorComponent*> InComponents)
	{
		ActionId = InActionId;
		Actor = InActor;
		Components = InComponents;
	}

	bool IsActive() const
	{
		return ActionId != EActionId::None && (Actor != nullptr || !Components.IsEmpty());
	}

	void Clear()
	{
		ActionId = EActionId::None;
		Actor = nullptr;
		Components.Empty();
	}
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

	
public:
	//Contructor
	UInteractionComponent();
	
	
	//Methods --> Interact
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void TryInteract(EActionId ActionId, EInteractionType InteractionType);
	
	//Methods --> Getters
	UFUNCTION(BlueprintCallable, Category="Interaction")
	FVector GetLookedAtLocation();
	
	
protected:
	//Setup & Tick
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void IntervalTick();

	
	//Variables --> Editable: Trace Draw
	UPROPERTY(EditAnywhere, Category="Interaction")
	float TraceLength = 500.0f;

	UPROPERTY(EditAnywhere, Category="Interaction")
	float TraceRadius = 12.0f;

	UPROPERTY(EditAnywhere, Category="Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, Category="Interaction")
	bool bDrawTraceDebug = false;
	
	UPROPERTY(EditAnywhere, Category="Interaction", meta = (ToolTip = "How many times we update the focus actor per second."))
	float UpdateFocusActorFrequency = 30.0f;

	
	//Variables --> Hidden, Interaction
	UPROPERTY(BlueprintReadOnly)
	FPlayerContext PlayerContext;

	UPROPERTY()
	FActiveInteraction ActiveInteraction;
	
	
	//Variables --> Hidden, Hover
	UPROPERTY()
	AActor* HoveringActor = nullptr;
	
	UPROPERTY()
	TArray<UActorComponent*> HoveredComponents;
	
	
	//Variables --> Hidden, Other
	UPROPERTY()
	APawn* CachedPawn = nullptr;
	
	UPROPERTY()
	FTimerHandle ControllerSetupTimerHandle;
	
	UPROPERTY()
	ULookTraceSubsystem* LookTraceSubsystem = nullptr;


	//Methods --> Setup
	UFUNCTION()
	void SetupIntervalTick();

	//Methods --> Context
	UFUNCTION()
	void SetupPersistentContext();
	
	UFUNCTION()
	void HandleControllerForPersistentContext();

	UFUNCTION()
	FPlayerContext RefreshDynamicContext();

	//Methods --> Hover
	UFUNCTION()
	void UpdateHover();

	//Methods --> Interaction
	// start
	UFUNCTION()
	void TryStartInteraction(EActionId ActionId);
	
	UFUNCTION()
	void Local_TryStartInteraction(EActionId ActionId);
	
	UFUNCTION(Server, Reliable)
	void Server_TryStartInteraction(EActionId ActionId);

	UFUNCTION()
	void Server_ResolveInteraction(EActionId ActionId, FPlayerContext Context, AActor*& OutActor, TArray<UActorComponent*>& OutComponents);
	
	UFUNCTION()
	bool Server_TryResolveInteractionGroup(AActor* ActorToResolve, EActionId ActionId, FPlayerContext Context, AActor*& OutActor, TArray<UActorComponent*>& OutComponents);
	
	// tick
	UFUNCTION()
	void TryTickInteraction(float DeltaTime);
	
	// end
	UFUNCTION()
	void TryEndInteraction();
	
	UFUNCTION()
	void Local_TryEndInteraction();
	
	UFUNCTION(Server, Reliable)
	void Server_TryEndInteraction();
	
	//Methods --> Helpers
	UFUNCTION()
	AActor* GetHeldInteractable(UHolderComponent* HolderComponent);
};
