// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/EnumProperty.h"

template<typename TEnum>
TEnum GetRandomEnumValue()
{
	static_assert(TIsEnumClass<TEnum>::Value, "TEnum must be an enum class");

	UEnum* EnumPtr = StaticEnum<TEnum>();
	if (!EnumPtr)
	{
		return static_cast<TEnum>(0);
	}
	
	int32 NumEnums = EnumPtr->NumEnums();
	
	int32 RandomIndex = FMath::RandRange(0, NumEnums - 2);

	return static_cast<TEnum>(EnumPtr->GetValueByIndex(RandomIndex));
}