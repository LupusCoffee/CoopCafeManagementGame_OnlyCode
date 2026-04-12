// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorUtility.h"

#include "Engine/Blueprint.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "ObjectTools.h"

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

UTexture2D* UEditorUtility::SaveThumbnailAsTexture2D(UObject* obj, FString TextureName, FString GamePath)
{
	int32 pathSeparatorIdx;
	if (TextureName.FindChar('/', pathSeparatorIdx)) {
		// TextureName should not have any path separators in it
		return nullptr;
	}

	FObjectThumbnail* thumb = ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(obj);
	if (!thumb) {
		return nullptr;
	}

	FString PackageName = GamePath;
	if (!PackageName.EndsWith("/")) {
		PackageName += "/";
	}
	PackageName += TextureName;

	UPackage* Package = CreatePackage(*PackageName);
	Package->FullyLoad();

	UTexture2D* NewTexture = NewObject<UTexture2D>(Package, *TextureName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
	NewTexture->AddToRoot();
	FTexturePlatformData* platformData = new FTexturePlatformData();
	platformData->SizeX = thumb->GetImageWidth();
	platformData->SizeY = thumb->GetImageHeight();
	//platformData->NumSlices = 1;
	platformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;
	NewTexture->SetPlatformData(platformData);

	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	platformData->Mips.Add(Mip);
	Mip->SizeX = thumb->GetImageWidth();
	Mip->SizeY = thumb->GetImageHeight();

	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(thumb->GetUncompressedImageData().Num() * 4);
	FMemory::Memcpy(TextureData, thumb->GetUncompressedImageData().GetData(), thumb->GetUncompressedImageData().Num());
	Mip->BulkData.Unlock();

	NewTexture->Source.Init(thumb->GetImageWidth(), thumb->GetImageHeight(), 1, 1, ETextureSourceFormat::TSF_BGRA8, thumb->GetUncompressedImageData().GetData());

	NewTexture->UpdateResource();
	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(NewTexture);

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	SaveArgs.SaveFlags = SAVE_NoError;
	SaveArgs.bForceByteSwapping = true;
	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	bool bSaved = UPackage::SavePackage(Package, NewTexture, *PackageFileName, SaveArgs);

	return NewTexture;
}
