#pragma once

#include "CoreMinimal.h"
#include "Core/Data/Interfaces/Fillable.h"
#include "Systems/Items/Components/ContainerComponent.h"
#include "Systems/MachineSystem/MachinePart.h"
#include "ContainerMachinePart.generated.h"

UCLASS()
class COFFEESHOPGAME_API AContainerMachinePart : public AMachinePart, public IFillable
{
	GENERATED_BODY()

public:
	//Public Overrides / Constructor
	AContainerMachinePart();
	
	//Other Overrides
	virtual bool Init(AMachine* _Owner, FName _ParentSocket) override;
	

protected:
	//Variables --> Visible, Components
	UPROPERTY(BlueprintReadOnly)
	UContainerComponent* ContainerComponent = nullptr;
	

	//Interface
	virtual void Hover_Implementation(FInteractionContext Context) override;
	virtual void Unhover_Implementation(FInteractionContext Context) override;
	virtual void Recieve_Implementation(EResourceType ResourceTypeToAdd, float FilledVolume) override;
};
