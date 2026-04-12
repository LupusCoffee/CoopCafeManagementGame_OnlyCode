#pragma once

#include "Widgets/Input/NumericTypeInterface.h"

struct FZeroPaddedIntInterface : public TDefaultNumericTypeInterface<int32>
{
	FZeroPaddedIntInterface(int32 InMinIntegralDigits)
		: MinIntegralDigits(InMinIntegralDigits)
	{
	}

	int32 MinIntegralDigits;

	virtual FString ToString(const int32& Value) const override
	{
		// Force the format to have a minimum amount of digits (adds leading zeros)
		FNumberFormattingOptions Options;
		Options.SetUseGrouping(false);
		Options.SetMinimumIntegralDigits(MinIntegralDigits);
		Options.SetMaximumIntegralDigits(MinIntegralDigits);
		Options.SetMaximumFractionalDigits(0);

		return FastDecimalFormat::NumberToString(Value, ExpressionParser::GetLocalizedNumberFormattingRules(), Options);
	}
};