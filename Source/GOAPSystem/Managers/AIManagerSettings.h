// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Structs/ManagerHelperStructs.h"
#include "AIManagerSettings.generated.h"

/**
 * Developer Settings for AI Manager
 * Configure in: Edit -> Project Settings -> Plugins -> AI Manager
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="AI Manager"))
class GOAPSYSTEM_API UAIManagerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UAIManagerSettings()
	{
		CategoryName = TEXT("Plugins");
		SectionName = TEXT("AI Manager");
		
		bAutoPreSpawn = true;
		bGlobalDebugDraw = false;
		PoolHiddenLocation = FVector(0, 0, -10000);
		DefaultPlanningInterval = 2.0f;
		MaxPlanningNodes = 200;
		bReplanOnFailure = true;
		bEnableTelemetry = false;
		TelemetryUpdateInterval = 1.0f;
	}

	// ===== POOLING SETTINGS =====
	
	/** Automatically pre-spawn pooled AI on BeginPlay */
	UPROPERTY(Config, EditAnywhere, Category = "Pooling", meta=(DisplayName="Auto Pre-Spawn"))
	bool bAutoPreSpawn;

	/** Location where pooled AI are hidden when inactive */
	UPROPERTY(Config, EditAnywhere, Category = "Pooling", meta=(DisplayName="Pool Hidden Location"))
	FVector PoolHiddenLocation;

	/** Pool configurations for different AI types */
	UPROPERTY(Config, EditAnywhere, Category = "Pooling", meta=(TitleProperty="PoolName", DisplayName="Default Pools"))
	TArray<FPoolConfig> DefaultPools;

	// ===== GOAP SETTINGS =====
	
	/** Default planning interval for AI (seconds) */
	UPROPERTY(Config, EditAnywhere, Category = "GOAP", meta=(ClampMin="0.1", ClampMax="10.0", DisplayName="Default Planning Interval"))
	float DefaultPlanningInterval;

	/** Maximum planning nodes for pathfinding */
	UPROPERTY(Config, EditAnywhere, Category = "GOAP", meta=(ClampMin="50", ClampMax="2000", DisplayName="Max Planning Nodes"))
	int32 MaxPlanningNodes;

	/** Should AI replan when actions fail? */
	UPROPERTY(Config, EditAnywhere, Category = "GOAP", meta=(DisplayName="Replan On Failure"))
	bool bReplanOnFailure;

	// ===== DEBUG SETTINGS =====
	
	/** Global debug visualization toggle for all AI */
	UPROPERTY(Config, EditAnywhere, Category = "Debug", meta=(DisplayName="Global Debug Draw"))
	bool bGlobalDebugDraw;

	/** Enable AI telemetry tracking (performance impact) */
	UPROPERTY(Config, EditAnywhere, Category = "Debug", meta=(DisplayName="Enable Telemetry"))
	bool bEnableTelemetry;

	/** Update interval for AI telemetry (seconds) */
	UPROPERTY(Config, EditAnywhere, Category = "Debug", meta=(EditCondition="bEnableTelemetry", ClampMin="0.1", DisplayName="Telemetry Update Interval"))
	float TelemetryUpdateInterval;

	// ===== PERFORMANCE SETTINGS =====
	
	/** Maximum number of AI that can be active simultaneously (0 = unlimited) */
	UPROPERTY(Config, EditAnywhere, Category = "Performance", meta=(ClampMin="0", ClampMax="500", DisplayName="Max Active AI"))
	int32 MaxActiveAI = 0;

	/** Enable AI LOD system to disable distant AI */
	UPROPERTY(Config, EditAnywhere, Category = "Performance", meta=(DisplayName="Enable AI LOD"))
	bool bEnableAILOD = false;

	/** Distance at which AI behavior is simplified (cm) */
	UPROPERTY(Config, EditAnywhere, Category = "Performance", meta=(EditCondition="bEnableAILOD", ClampMin="1000", DisplayName="LOD Distance"))
	float LODDistance = 5000.0f;

	// ===== LIFECYCLE SETTINGS =====
	
	/** Automatically register AI characters on spawn */
	UPROPERTY(Config, EditAnywhere, Category = "Lifecycle", meta=(DisplayName="Auto Register AI"))
	bool bAutoRegisterAI = true;

	/** Clean up invalid entries interval (seconds) */
	UPROPERTY(Config, EditAnywhere, Category = "Lifecycle", meta=(ClampMin="1.0", ClampMax="60.0", DisplayName="Cleanup Interval"))
	float CleanupInterval = 10.0f;

	// Accessor for convenience
	static const UAIManagerSettings* Get()
	{
		return GetDefault<UAIManagerSettings>();
	}
};

