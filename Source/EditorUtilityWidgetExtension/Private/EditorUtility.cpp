// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorUtility.h"

#include "Engine/Blueprint.h"
#include "AssetRegistry/AssetRegistryModule.h"

void UEditorUtility::SetObjectPropertyByName(UBlueprint* BlueprintAsset, FString PropertyName, UObject* Object)
{
	if (!BlueprintAsset) return;

	UObject* DefaultObject = BlueprintAsset->GeneratedClass->GetDefaultObject();

	FProperty* ItemDataProperty = DefaultObject->GetClass()->FindPropertyByName(*PropertyName);

	if (ItemDataProperty)
	{
		if (ItemDataProperty->IsA<FObjectProperty>())
		{
			FObjectProperty* ObjectProperty = (FObjectProperty*)ItemDataProperty;

			ObjectProperty->SetValue_InContainer(DefaultObject, Object);
		}
	}

	BlueprintAsset->Modify();
	BlueprintAsset->PostEditChange();
}

TArray<UObject*> UEditorUtility::GetAllAssetOfClass(UClass* AssetClass, bool SearchSubclass)
{
	TArray<UObject*> OutArray;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByClass(FTopLevelAssetPath(AssetClass->GetPathName()), AssetData, SearchSubclass);
	for (int i = 0; i < AssetData.Num(); i++)
	{
		UObject* Object = Cast<UObject>(AssetData[i].GetAsset());

		OutArray.Add(Object);
	}

	return OutArray;
}
