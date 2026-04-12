#pragma once

UENUM(BlueprintType)
enum class EAction : uint8
{
	None UMETA(DisplayName = "None"),
	
	PickUp UMETA(DisplayName = "Pick Up"),
	PlaceDown UMETA(DisplayName = "Place Down"),
	Throw UMETA(DisplayName = "Throw"),
	
	Pour UMETA(DisplayName = "Pour"),
	
	MachineInteraction_PressButton UMETA(DisplayName = "Machine Interaction, Press Button"),
	MachineInteraction_Turn UMETA(DisplayName = "Machine Interaction, Turn"),
	MachineInteraction_Grab UMETA(DisplayName = "Machine Interaction, Grab")
};