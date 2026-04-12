#pragma once

class UActorComponent;
class UItemPromptComponent;

namespace PromptHelper
{
	UItemPromptComponent* GetOrCreateItemPromptComponent(AActor* Owner);
}
