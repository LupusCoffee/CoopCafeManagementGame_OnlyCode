#include "Systems/ActionStack/BaseAction.h"


//virtual
void UBaseAction::OnBegin_Implementation(bool bFirstTime)
{
	Stack = GetTypedOuter<UActionStack>();
}

void UBaseAction::OnTick_Implementation(float DeltaTime)
{
}

void UBaseAction::OnEnd_Implementation()
{
}

bool UBaseAction::IsDone_Implementation() const
{
	return true;
}