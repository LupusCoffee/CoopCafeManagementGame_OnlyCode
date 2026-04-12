#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/PromptHelper.h"
#include "CoffeeShopGame/Public/Systems/InteractionSystem/Components/PromptComponent/ItemPromptComponent.h"

UItemPromptComponent* PromptHelper::GetOrCreateItemPromptComponent(AActor* Owner)
{
	UItemPromptComponent* ItemPromptComp = Owner->GetComponentByClass<UItemPromptComponent>();
	if (!ItemPromptComp)
	{
		UActorComponent* Comp = Owner->AddComponentByClass(
			UItemPromptComponent::StaticClass(),
			false,
			FTransform::Identity,
			false);
		
		if (Comp) ItemPromptComp = Cast<UItemPromptComponent>(Comp);

		if (ItemPromptComp) ItemPromptComp->RegisterComponent();
	}
	
	return ItemPromptComp;
}
