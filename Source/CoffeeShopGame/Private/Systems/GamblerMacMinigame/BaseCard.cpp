#include "Systems/GamblerMacMinigame/BaseCard.h"

void UBaseCard::SetupData(UCardData* CardData)
{
	CardName = CardData->CardName;
	ImmediateDescription = CardData->ImmediateDescription;
	StackDescription = CardData->StackDescription;
	Points = CardData->Points;
}

void UBaseCard::ImmediateEffect()
{
	
}

void UBaseCard::StackEffect()
{
	
}
