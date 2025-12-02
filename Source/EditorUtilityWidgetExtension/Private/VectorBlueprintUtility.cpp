// Fill out your copyright notice in the Description page of Project Settings.


#include "VectorBlueprintUtility.h"

FIntVector UVectorBlueprintUtility::Add(FIntVector First, FIntVector Second)
{
	return First + Second;
}

FIntVector UVectorBlueprintUtility::Subtract(FIntVector First, FIntVector Second)
{
	return First - Second;
}

bool UVectorBlueprintUtility::Equals(FIntVector First, FIntVector Second)
{
	return First == Second;
}
