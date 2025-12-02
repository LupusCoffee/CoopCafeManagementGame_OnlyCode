// Fill out your copyright notice in the Description page of Project Settings.


#include "IdleGoal.h"

bool UIdleGoal::IsValid(FWorldState CurrentState) const
{
	if (DesiredState.Thirstiness <= CurrentState.Thirstiness)
		return true;

	return false;
}

UIdleGoal::UIdleGoal()
{
	GoalName = "IdleGoal";

	Priority = 0.1f;
	
	DesiredState.Thirstiness = 0.75f;
}