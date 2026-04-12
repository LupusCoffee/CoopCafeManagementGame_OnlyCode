#pragma once

#include "CoreMinimal.h"
#include "HoldableMachinePart.h"
#include "Machine.h"
#include "MachineWithHoldableParts.generated.h"

UCLASS()
class COFFEESHOPGAME_API AMachineWithHoldableParts : public AMachine
{
	GENERATED_BODY()

public:
	AMachineWithHoldableParts();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
	TArray<AHoldableMachinePart*> HoldableMachineParts;
};
