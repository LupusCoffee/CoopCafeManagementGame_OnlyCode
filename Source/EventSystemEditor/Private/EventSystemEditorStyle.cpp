#include "EventSystemEditorStyle.h"
#include "Styling/SlateStyleRegistry.h"

TSharedPtr<FSlateStyleSet> FEventSystemEditorStyle::StyleInstance = nullptr;

void FEventSystemEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = MakeShareable(new FSlateStyleSet(GetStyleSetName()));

		// Set the root directory where your PNGs are located. Adjust "YourPluginName" accordingly.
		FString ImagePath = FPaths::ProjectContentDir() / TEXT("Icons/Editor/EventSystem");
		StyleInstance->SetContentRoot(ImagePath);

		// Register the PNG file (MyCustomIcon.png) as a brush
		// FVector2D is the resolution of the icon.
		StyleInstance->Set("EventSystemEditor.CalendarIcon", new FSlateImageBrush(StyleInstance->RootToContentDir(TEXT("Calendar"), TEXT(".png")), FVector2D(20.0f, 20.0f)));

		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance.Get());
	}
}

void FEventSystemEditorStyle::Shutdown()
{
	if (StyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance.Get());
		StyleInstance.Reset();
	}
}

FName FEventSystemEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("EventSystemEditorStyle"));
	return StyleSetName;
}