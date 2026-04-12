#include "TimeDateUtility.h"
#include "TimeStructs.h"

int UTimeDateUtility::MonthCodes[12] = { 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 };

bool UTimeDateUtility::EvaluateLeapYear(int Year)
{
	return (Year % 4 == 0) && (!(Year % 100 == 0) || (Year % 400 == 0));
}

int UTimeDateUtility::EvaluateDaysInMonth(int Year, int Month)
{
	bool bIsLeapYear = EvaluateLeapYear(Year);
	if (Month == 2)
	{
		return (bIsLeapYear ? 29 : 28);
	}

	return (((Month > 7) ^ (Month % 2 == 0)) ? 30 : 31);
}

int UTimeDateUtility::EvaluateWeekDay(int Year, int Month, int Day)
{
	bool bIsLeapYear = EvaluateLeapYear(Year);

	int YY = (Year % 100);
	int YC = ((YY / 4) + YY) % 7;
	int MC = MonthCodes[Month - 1];
	int CC = 6 - (2 * (((Year - 1) / 100) % 4));
	int WeekDay = (YC + MC + CC + Day - 1 - (bIsLeapYear ? 1 : 0)) % 7;
	return WeekDay + 1;
}
