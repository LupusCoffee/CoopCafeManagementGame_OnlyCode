#pragma once

#include "CoreMinimal.h"

struct FBTExecuteGoapActionMemory
{
	bool bIsWaiting = false;
	float WaitTimeRemaining = 0.0f;
	bool bWaitingForInteraction = false;
	bool bInteractionReceived = false;
	bool bTimedOut = false;
	bool bWaitingForNotify = false;
	bool bMovementCompleted = false;
	bool bMoveRequestActive = false;
	uint8 MovementRetryCount = 0;
};

 