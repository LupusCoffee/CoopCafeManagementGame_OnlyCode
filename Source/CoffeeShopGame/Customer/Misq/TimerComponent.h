// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimerFinishedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimerTickSignature, float, RemainingSeconds);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COFFEESHOPGAME_API UTimerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTimerComponent();

	// Start/reset the timer to Duration seconds
	UFUNCTION(BlueprintCallable) void StartTimer(float DurationSeconds);

	// Add/Remove time while running (positive or negative)
	UFUNCTION(BlueprintCallable) void AddTime(float DeltaSeconds);

	// Pause/Resume
	UFUNCTION(BlueprintCallable) void PauseTimer();
	UFUNCTION(BlueprintCallable) void ResumeTimer();

	// Query
	UFUNCTION(BlueprintCallable) float GetRemainingTime() const;
	UFUNCTION(BlueprintCallable) bool IsRunning() const;

	// Events
	UPROPERTY(BlueprintAssignable) FTimerFinishedSignature OnTimerFinished;
	UPROPERTY(BlueprintAssignable) FTimerTickSignature OnTimerTick; // fired by UI update timer

protected:
	virtual void BeginPlay() override;

private:
	// Single authoritative end time (world seconds)
	float EndTimeSeconds = 0.0f;
	bool bIsRunning = false;

	// Timer handle used for the final expiration callback
	FTimerHandle FinishHandle;

	// Optional repeating handle for UI ticks (e.g. 0.1s)
	FTimerHandle UITickHandle;
	float UITickInterval = 0.1f;

	// Helpers
	float NowSeconds() const;
	void ScheduleFinishTimer();
	void OnFinishTimerFired();
	void FireUITick(); // calls OnTimerTick with remaining
};
