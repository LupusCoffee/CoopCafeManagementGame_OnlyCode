#include "Systems/MachineSystem/MachineParts/ItemSpotWithHighlightMeshMachinePart.h"

bool AItemSpotWithHighlightMeshMachinePart::Init(AMachine* _Owner, FName _ParentSocket)
{
	if (!Super::Init(_Owner, _ParentSocket)) return false;

	//turns mesh comp invisible, but still highlightable --> for switching its mesh based on what we are holding (this stupid?)
	BaseMeshComp->SetRenderInMainPass(false);
	BaseMeshComp->SetRenderInDepthPass(false);
	BaseMeshComp->SetCastShadow(false);
	
	return true;
}

void AItemSpotWithHighlightMeshMachinePart::Local_StartHover_Implementation(FPlayerContext Context)
{
	Super::Local_StartHover_Implementation(Context);
	
	UHolderComponent* HolderComponent = Context.HolderComponent;
	if (!HolderComponent)
	{
		if (ItemAtSpot) return;
		SetHighlightMesh(ItemAtSpot);
		return;
	}

	UHeldItem* HeldItem = HolderComponent->GetHeldItem();
	if (!HeldItem)
	{
		if (!ItemAtSpot) return;
		SetHighlightMesh(ItemAtSpot);
		return;
	}
	
	SetHighlightMesh(HeldItem);
}

void AItemSpotWithHighlightMeshMachinePart::Local_EndHover_Implementation(FPlayerContext Context)
{
	Super::Local_EndHover_Implementation(Context);
	
	BaseMeshComp->SetStaticMesh(nullptr);
}

void AItemSpotWithHighlightMeshMachinePart::SetHighlightMesh(UHeldItem* ItemToHighlight)
{
	if (!ItemToHighlight) return;
	
	UStaticMeshComponent* StaticMeshComponent = ItemToHighlight->GetMeshComp();
	if (!StaticMeshComponent) return;
	
	UStaticMesh* HighlightMesh = StaticMeshComponent->GetStaticMesh();
	if (!HighlightMesh) return;
	
	BaseMeshComp->SetStaticMesh(HighlightMesh);
	BaseMeshComp->SetWorldScale3D(StaticMeshComponent->GetComponentScale());
}
