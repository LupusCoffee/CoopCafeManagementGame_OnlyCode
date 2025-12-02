#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/Framework/Subsystems/LookTraceSubsystem.h"
#include "Systems/Interaction System/Enums/ActionId.h"
#include "Systems/Interaction System/Structs/InteractionContext.h"
#include "Systems/Interaction System/Subsystems/HighlightRegistrySubsystem.h"
#include "InteractionComponent.generated.h"
enum class EActionId : uint8;

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	Started,
	Ongoing,
	Completed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHighlightEnabled, FLinearColor, HighlightedColor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHighlightDisabled);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

	
public:
	//Contructor
	UInteractionComponent();

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


	//Methods
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void TryInteract(EActionId ActionId, EInteractionType InteractionType);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	FVector GetLookedAtLocation();

	
	//Delegates
	UPROPERTY(BlueprintReadWrite, BlueprintAssignable, Category = "Highlight")
	FOnHighlightEnabled OnHighlightEnabled;

	UPROPERTY(BlueprintReadWrite, BlueprintAssignable, Category = "Highlight")
	FOnHighlightDisabled OnHighlightDisabled;
	
	
protected:
	//Contructors
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	//Variables --> Hidden
	UPROPERTY()
	ULookTraceSubsystem* LookTraceSubsystem = nullptr;

	UPROPERTY()
	UHighlightRegistrySubsystem* HighlightRegistry = nullptr;

	UPROPERTY(VisibleAnywhere)
	FVector LookedAtLocation = FVector::ZeroVector;		//gotta replicate
	
	UPROPERTY(VisibleAnywhere)
	AActor* FocusActor = nullptr;

	UPROPERTY()
	AController* Controller = nullptr;

	UPROPERTY()
	bool IsPlayer = false;

	UPROPERTY()
	FTimerHandle FocusTimer;

	UPROPERTY()
	FInteractionContext InteractionContext;

	UPROPERTY()
	AActor* Server_InteractionActor = nullptr;

	UPROPERTY()
	float CurrentDeltaTime;

	UPROPERTY()
	EActionId ActionIdInUse = EActionId::None;


	//Methods --> Setup
	UFUNCTION()
	void SetupUpdate();

	UFUNCTION()
	void SetupInteractionContext();

	//Methods --> Update
	UFUNCTION()
	void UpdateFocusActorAndLookedAtLocation();

	UFUNCTION()
	void UpdateHighlight(AActor* HitActor);

	//Methods --> Interaction
	UFUNCTION(Server, Reliable)
	void Server_TryInteract(EActionId ActionId, EInteractionType InteractionType, FInteractionContext Context, AActor* inFocusActor, float DeltaTime);

	//optimization for network, so that tick doens't happen all the time and isn't reliable, cause lag
	/*UFUNCTION(Server, Reliable)
	void Server_TryInteractStart(EActionId ActionId, EInteractionType InteractionType, FInteractionContext Context, AActor* inFocusActor);
	UFUNCTION(Server, Unreliable)
	void Server_TryInteractTick(EActionId ActionId, EInteractionType InteractionType, FInteractionContext Context, AActor* inFocusActor);
	UFUNCTION(Server, Reliable)
	void Server_TryInteractCompleted(EActionId ActionId, EInteractionType InteractionType, FInteractionContext Context, AActor* inFocusActor);*/
	
	UFUNCTION()
	bool FocusActorInteract(EActionId ActionId, EInteractionType InteractionType, FInteractionContext Context, AActor* inFocusActor, float DeltaTime);

	UFUNCTION()
	bool HeldItemInteract(EActionId ActionId, EInteractionType InteractionType, FInteractionContext Context, float DeltaTime);
};
