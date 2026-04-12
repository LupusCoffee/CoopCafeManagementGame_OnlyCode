#pragma once

#include "CoreMinimal.h"
#include "EventSystemStructs.h"
#include "DetailPanelDataObject.generated.h"

UCLASS()
class UDetailPanelDataObject : public UObject
{
	GENERATED_BODY()

	DECLARE_DELEGATE(FOnEventChanged);

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FEventTag Event;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FTimeDate TriggerTime;

	UPROPERTY(EditAnywhere)
	FRepeatList RepeatList;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FDataContainer DataContainer;

#if WITH_EDITOR
	FOnEventChanged OnDataChanged;

private:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
		FName PropertyName = PropertyChangedEvent.GetPropertyName();
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UDetailPanelDataObject, Event))
		{
			OnDataChanged.ExecuteIfBound();
		}
	}
#endif
};