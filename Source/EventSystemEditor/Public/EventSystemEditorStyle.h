#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FEventSystemEditorStyle
{
public:
	static void Initialize();
	static void Shutdown();
	static FName GetStyleSetName();
private:
	static TSharedPtr<class FSlateStyleSet> StyleInstance;
};