#include "CoffeeShopGame/Public/Systems/ContainerSystem/Subsystems/ResourceConverterSubsystem.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Objects/ContainerObject.h"
#include "CoffeeShopGame/Public/Systems/ContainerSystem/Structs/ResourceAmount.h"



#pragma region Setup
// core
void UResourceConverterSubsystem::InitSubsystem(URecipeArrayDataAsset* InAvailableRecipes,
	URecipeArrayDataAsset* PreDiscoveredRecipes, UConversionData* InGrindConversionData)
{
	InitRecipe(InAvailableRecipes, PreDiscoveredRecipes);
	InitGrind(InGrindConversionData);
}


// recipe
void UResourceConverterSubsystem::InitRecipe(URecipeArrayDataAsset* InAvailableRecipes, URecipeArrayDataAsset* PreDiscoveredRecipes)
{
	if (InAvailableRecipes) AvailableRecipesData = InAvailableRecipes;
	
	//convert AvailableRecipesData to FRecipes and add to BrokenDownRecipes
	for (auto AvailableRecipeData : AvailableRecipesData->Recipes)
	{
		FRecipe NewRecipe = FRecipe(AvailableRecipeData);
		FRecipe BrokenDownRecipe = BreakDownRecipe(NewRecipe);
		if (BrokenDownRecipe.RecipeResourceData.IsEmpty()) continue;
		BrokenDownRecipes.Add(BrokenDownRecipe);
	}

	//convert PreDiscoveredRecipes to FRecipes and add to DiscoveredRecipes
	for (auto PreDiscoveredRecipeData : PreDiscoveredRecipes->Recipes)
	{
		FRecipe NewRecipe = FRecipe(PreDiscoveredRecipeData);
		FRecipe BrokenDownRecipe = BreakDownRecipe(NewRecipe);
		if (BrokenDownRecipe.RecipeResourceData.IsEmpty()) continue;
		TryAddDiscoveredRecipe(BrokenDownRecipe);
	}
}

bool UResourceConverterSubsystem::TryAddDiscoveredRecipe(FRecipe Recipe)
{
	if (!BrokenDownRecipes.Contains(Recipe))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "WARNING: Tried to Discover Non-Available Recipe");
		return false;
	}

	if (DiscoveredRecipes.Contains(Recipe))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "WARNING: Tried to Discover Already Discovered Recipe");
		return false;
	}

	DiscoveredRecipes.Add(Recipe);
	return true;
}

// grind
void UResourceConverterSubsystem::InitGrind(UConversionData* GrindConversionData)
{
	if (!GrindConversionData) return;
	
	for (auto ConversionEntry : GrindConversionData->ConversionEntries)
	{
		AvailableGrindConversions.Add(ConversionEntry);
	}
}
#pragma endregion



#pragma region TryConvert --> Recipe
// main function
bool UResourceConverterSubsystem::TryConvertFromRecipe(TArray<FResourceAmount>& ContentAsAmountsToConvert)
{
	//calculate total volume of content to convert
	float TotalVolume = 0;
	for (auto ResourceAmount : ContentAsAmountsToConvert)
	{
		TotalVolume += ResourceAmount.Amount;
	}
	if (TotalVolume <= 0) return false;


	//for each resource amount --> add to ContentAsPercentages as a percentage
	TArray<FResourcePercentageAsContent> ContentAsPercentages;
	for (auto ResourceAmount : ContentAsAmountsToConvert)
	{
		if (ResourceAmount.Amount <= 0) continue;
		
		ContentAsPercentages.Add(FResourcePercentageAsContent(ResourceAmount.ResourceType,
			ResourceAmount.Amount, TotalVolume));
	}
	if (ContentAsPercentages.IsEmpty()) return false;


	//see if there are any recipes that coincide with the resource percentages
	EResourceType ResultResource = EResourceType::None;
	if (!FindEquivalentRecipeResource(ContentAsPercentages, ResultResource)) return false;


	//convert content
	ContentAsAmountsToConvert.Empty();
	ContentAsAmountsToConvert.Add(FResourceAmount(ResultResource, TotalVolume));
	
	return true;
}


// helpers
bool UResourceConverterSubsystem::FindEquivalentRecipeResource(TArray<FResourcePercentageAsContent> ContentToCompare, EResourceType& ResultResource)
{
	for (auto BrokenDownRecipe : BrokenDownRecipes)
	{
		if (!Coincide(ContentToCompare, BrokenDownRecipe)) continue;

		TryAddDiscoveredRecipe(BrokenDownRecipe);
		ResultResource = BrokenDownRecipe.ResultingResource;
		return true;
	}
	
	return false;
}

bool UResourceConverterSubsystem::Coincide(TArray<FResourcePercentageAsContent> ContentToCompare,
                                           FRecipe RecipeToCompare)
{
	//note: don't have to break down the recipe, as it's already broken down since init
	TArray<FResourcePercentageAsContent> BrokenDownContent = BreakDownContent(ContentToCompare);
	
	for (auto ResourcePercentageData : RecipeToCompare.RecipeResourceData)
	{
		int ResourceIndex = BrokenDownContent.IndexOfByPredicate(
		[ResourcePercentageData](const FResourcePercentageAsContent& ResourcePercentageContent)
		{
			return ResourcePercentageContent.ResourceType == ResourcePercentageData.ResourceType;
		});
		if (ResourceIndex == -1) return false;
		
		float CurrentPercentage = BrokenDownContent[ResourceIndex].CurrentPercentage;
		float MinPercentage = ResourcePercentageData.Percentage - ResourcePercentageData.MarginOfError;
		float MaxPercentage = ResourcePercentageData.Percentage + ResourcePercentageData.MarginOfError;
		
		if (CurrentPercentage <= 0) return false;
		if (CurrentPercentage <= MinPercentage) return false;
		if (CurrentPercentage >= MaxPercentage) return false;
	}
	
	return true;
}

FRecipe UResourceConverterSubsystem::BreakDownRecipe(FRecipe InRecipe)
{
	FRecipe ReturnRecipe = FRecipe();
	TArray<FResourcePercentageAsData> ReturnBaseIngredients;

	
	for (auto ResourceData : InRecipe.RecipeResourceData)
	{
		//if a resource's enum is a base ingredient, add to ReturnRecipe.RecipeResourceData
		if (BaseIngredients.Contains(ResourceData.ResourceType))
		{
			ReturnBaseIngredients.Add(ResourceData);
			continue;
		}
		
		//find resource data as a recipe result and break down --> then break
		bool ResourceDataAsRecipeResultFound = false;
		for (auto RecipeData : AvailableRecipesData->Recipes)
		{
			if (ResourceDataAsRecipeResultFound) break;
			
			if (ResourceData.ResourceType == RecipeData->ResultingResource)
			{
				FRecipe NewRecipe = FRecipe(RecipeData);
				FRecipe BrokenDownRecipe = BreakDownRecipe(NewRecipe);

				for (auto BaseResourceData : BrokenDownRecipe.RecipeResourceData)
				{
					FResourcePercentageAsData BrokenDownPercentagesData;
					BrokenDownPercentagesData.ResourceType = BaseResourceData.ResourceType;
					BrokenDownPercentagesData.Percentage = BaseResourceData.Percentage * ResourceData.Percentage;
					BrokenDownPercentagesData.MarginOfError = BaseResourceData.MarginOfError * ResourceData.Percentage; //* ResourceData.Percentage?
					
					ReturnBaseIngredients.Add(BrokenDownPercentagesData);
				}
				
				ResourceDataAsRecipeResultFound = true;
			}
		}

		//if resource data wasn't found as a recipe result, add as base ingredients and to ReturnRecipe.RecipeResourceData
		if (!ResourceDataAsRecipeResultFound)
		{
			BaseIngredients.Add(ResourceData.ResourceType);
			ReturnBaseIngredients.Add(ResourceData);
		}
	}


	//remove any resource that is 0 or less
	for (int i = 0; i < ReturnBaseIngredients.Num(); ++i)
	{
		if (ReturnBaseIngredients[i].Percentage <= 0)
		{
			ReturnBaseIngredients.RemoveAt(i);
			i--;
		}
	}
	

	//add together any resources that are of the same type in ReturnBaseIngredients
	for (int i = 0; i < ReturnBaseIngredients.Num(); ++i)
	{
		for (int j = 0; j < ReturnBaseIngredients.Num(); ++j)
		{
			if (ReturnBaseIngredients[i].ResourceType == ReturnBaseIngredients[j].ResourceType && i != j)
			{
				ReturnBaseIngredients[i].Percentage += ReturnBaseIngredients[j].Percentage;
				ReturnBaseIngredients.RemoveAt(j);
				if (i > j) i--;
				j--;
			}
		}
	}


	//return
	ReturnRecipe.RecipeResourceData = ReturnBaseIngredients;
	ReturnRecipe.ResultingResource = InRecipe.ResultingResource;
	return ReturnRecipe;
}

TArray<FResourcePercentageAsContent> UResourceConverterSubsystem::BreakDownContent(TArray<FResourcePercentageAsContent> Content)
{
	TArray<FResourcePercentageAsContent> ReturnContent;
	
	//break down Content into base ingredients with correct percentages (added to ReturnContent)
	for (auto ResourcePercentage : Content)
	{
		//if base ingredient, simply add to ReturnContent
		if (BaseIngredients.Contains(ResourcePercentage.ResourceType))
		{
			ReturnContent.Add(ResourcePercentage);
			continue;
		}

		//if not base ingredient, find base ingredients of ResourcePercentage via
		//BrokenDownrecipes, adjust percentages, and add to ReturnContent
		for (auto BrokenDownRecipe : BrokenDownRecipes)
		{
			if (BrokenDownRecipe.ResultingResource == ResourcePercentage.ResourceType)
			{
				for (auto ResourceData : BrokenDownRecipe.RecipeResourceData)
				{
					FResourcePercentageAsContent BrokenDownPercentageData;
					BrokenDownPercentageData.ResourceType = ResourceData.ResourceType;
					BrokenDownPercentageData.CurrentPercentage = ResourceData.Percentage * ResourcePercentage.CurrentPercentage;
					ReturnContent.Add(BrokenDownPercentageData);
				}
				break;
			}
		}
	}

	
	//add together any resources that are of the same type
	for (int i = 0; i < ReturnContent.Num(); ++i)
	{
		for (int j = 0; j < ReturnContent.Num(); ++j)
		{
			if (ReturnContent[i].ResourceType == ReturnContent[j].ResourceType && i != j)
			{
				ReturnContent[i].CurrentPercentage += ReturnContent[j].CurrentPercentage;
				ReturnContent.RemoveAt(j);
				if (i > j) i--;
				j--;
			}
		}
	}

	
	return ReturnContent;
}
#pragma endregion



#pragma region TryConvert --> Grind
// main function
bool UResourceConverterSubsystem::TryMoveContentAndGrind(float AmountToMove, UContainerObject* ContainerA, UContainerObject* ContainerB)
{
	if (ContainerB->IsFull()) return false;

	
	UContainerObject* TempContainer = NewObject<UContainerObject>(this);
	TempContainer->SetupData(true, TArray<EResourceType>(), ContainerA->GetCurrentTotalVolume(), TArray<FResourceAmount>());

	
	bool MoveStatus = false;
	
	MoveStatus = TryMoveContent(AmountToMove, ContainerA, TempContainer);
	if (!MoveStatus) return false;
	
	TryGrindContent(TempContainer);
	
	MoveStatus = TryMoveContent(AmountToMove, TempContainer, ContainerB);
	if (!MoveStatus) return false;

	
	return true;
}

void UResourceConverterSubsystem::TryGrindContent(UContainerObject* Container)
{
	int InitialNum = Container->GetCurrentVolumeMap().Num();
	
	for (int i = 0; i < InitialNum; ++i)
	{
		EResourceType PreConversionResource = Container->GetCurrentVolumeMap()[i].ResourceType;
		EResourceType PostConversionResource = TryGetGrindConversionOutputResource(PreConversionResource);
		if (PostConversionResource == EResourceType::None) continue;

		float AmountRemoved = Container->ZeroSpecificVolumeFromIndex(i);
		float ExcessAmount = 0.0f;
		Container->TryAddVolume(PostConversionResource, AmountRemoved, ExcessAmount, false);
	}
}


// helpers
EResourceType UResourceConverterSubsystem::TryGetGrindConversionOutputResource(EResourceType InputResourceType)
{
	for (auto GrindConversion : AvailableGrindConversions)
	{
		if (GrindConversion.InputResourceType == InputResourceType) return GrindConversion.OutputResourceType;
	}
	
	return EResourceType::None;
}
#pragma endregion



#pragma region Miscellaneous Functions
//will return true only if EVERYTHING could be moved
bool UResourceConverterSubsystem::TryMoveContent(float AmountToMove, UContainerObject* ContainerA, UContainerObject* ContainerB)
{
	if (ContainerB->IsFull()) return false;
	
	
	if (ContainerA->GetCurrentTotalVolume() < AmountToMove)
		AmountToMove = ContainerA->GetCurrentTotalVolume();
	
	float CurrentValueToMove = AmountToMove;

	for (int i = 0; i < ContainerA->GetCurrentVolumeMap().Num(); ++i)
	{
		FResourceAmount ResourceAmount = ContainerA->GetCurrentVolumeMap()[i];
		EResourceType ResourceType = ResourceAmount.ResourceType;

		float ClampedValueToMove = FMath::Clamp(CurrentValueToMove, 0.0f, ResourceAmount.Amount);
		float RemovedExcessA = 0;
		ContainerA->RemoveSpecificVolume(ResourceType, ClampedValueToMove, RemovedExcessA);
		float ExcessAmountB = 0.0f;
		ContainerB->TryAddVolume(ResourceType, ClampedValueToMove, ExcessAmountB, false);
		
		CurrentValueToMove -= ClampedValueToMove;
		
		if(ExcessAmountB > 0)
		{
			float ExcessAmountA = 0.0f;
			ContainerA->TryAddVolume(ResourceType, ExcessAmountB, ExcessAmountA, false);
			return false;
		}
	}


	for (auto& ResourceAmount : ContainerA->GetCurrentVolumeMap())
	{
		if (CurrentValueToMove <= 0.0f) break;
		
		float ClampedValueToRemove = FMath::Clamp(CurrentValueToMove, 0.0f, ResourceAmount.Amount);
		ResourceAmount.Amount -= ClampedValueToRemove;
		CurrentValueToMove -= ClampedValueToRemove;
	}
	
	return true;
}
#pragma endregion
