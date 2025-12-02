// TimerComponent.cpp
#include "TimerComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UTimerComponent::UTimerComponent()
{
    PrimaryComponentTick.bCanEverTick = false; // we don't need Tick by default
}

void UTimerComponent::BeginPlay()
{
    Super::BeginPlay();
}

float UTimerComponent::NowSeconds() const
{
    if (!GetWorld()) return 0.0f;
    return GetWorld()->GetTimeSeconds(); // use GetWorld()->GetRealTimeSeconds() if you want unaffected by pausing
}

void UTimerComponent::StartTimer(float DurationSeconds)
{
    if (!GetWorld()) return;

    EndTimeSeconds = NowSeconds() + FMath::Max(0.0f, DurationSeconds);
    bIsRunning = true;

    // Schedule finish timer
    ScheduleFinishTimer();

    // Start UI tick if someone listens (optional)
    if (!UITickHandle.IsValid())
    {
        GetWorld()->GetTimerManager().SetTimer(UITickHandle, this, &UTimerComponent::FireUITick, UITickInterval, true);
    }
}

void UTimerComponent::AddTime(float DeltaSeconds)
{
    if (!bIsRunning) return;

    EndTimeSeconds += DeltaSeconds;

    // If end time moved past now (negative remaining), fire immediately
    float Rem = GetRemainingTime();
    if (Rem <= 0.0f)
    {
        // Cancel and call finish
        GetWorld()->GetTimerManager().ClearTimer(FinishHandle);
        OnFinishTimerFired();
        return;
    }

    // Reset finish timer with updated remaining time
    ScheduleFinishTimer();
}

void UTimerComponent::PauseTimer()
{
    if (!bIsRunning || !GetWorld()) return;
    // Clear finish timer but keep EndTime relative? Simpler: compute remaining and store it, mark paused.
    float remaining = GetRemainingTime();
    GetWorld()->GetTimerManager().ClearTimer(FinishHandle);
    GetWorld()->GetTimerManager().ClearTimer(UITickHandle);
    bIsRunning = false;
    EndTimeSeconds = NowSeconds() + remaining; // keep EndTime consistent (but bIsRunning false)
}

void UTimerComponent::ResumeTimer()
{
    if (bIsRunning || !GetWorld()) return;
    bIsRunning = true;
    // EndTimeSeconds already stores (Now + remaining) from PauseTimer — but if you used a different method, set appropriately
    ScheduleFinishTimer();
    if (!UITickHandle.IsValid())
    {
        GetWorld()->GetTimerManager().SetTimer(UITickHandle, this, &UTimerComponent::FireUITick, UITickInterval, true);
    }
}

float UTimerComponent::GetRemainingTime() const
{
    if (!bIsRunning) return 0.0f;
    return FMath::Max(0.0f, EndTimeSeconds - NowSeconds());
}

bool UTimerComponent::IsRunning() const
{
    return bIsRunning;
}

void UTimerComponent::ScheduleFinishTimer()
{
    if (!GetWorld()) return;

    float remaining = FMath::Max(0.0f, EndTimeSeconds - NowSeconds());
    // Clear existing and set new
    GetWorld()->GetTimerManager().ClearTimer(FinishHandle);
    if (remaining > SMALL_NUMBER)
    {
        GetWorld()->GetTimerManager().SetTimer(FinishHandle, this, &UTimerComponent::OnFinishTimerFired, remaining, false);
    }
    else
    {
        // Immediate fire if <= 0
        OnFinishTimerFired();
    }
}

void UTimerComponent::OnFinishTimerFired()
{
    bIsRunning = false;
    GetWorld()->GetTimerManager().ClearTimer(FinishHandle);
    GetWorld()->GetTimerManager().ClearTimer(UITickHandle);
    OnTimerFinished.Broadcast();
}

void UTimerComponent::FireUITick()
{
    if (!bIsRunning) return;
    float rem = GetRemainingTime();
    OnTimerTick.Broadcast(rem);
    if (rem <= 0.0f)
    {
        // ensure finish fires
        OnFinishTimerFired();
    }
}
