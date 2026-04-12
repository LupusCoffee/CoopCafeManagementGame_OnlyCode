#pragma once

#include "CoreMinimal.h"

struct FDate;

class EVENTSYSTEM_API UTimeDateUtility
{
public:
	static bool EvaluateLeapYear(int Year);
	static int EvaluateDaysInMonth(int Year, int Month);
	static int EvaluateWeekDay(int Year, int Month, int Day);

private:
	static int MonthCodes[12];
};