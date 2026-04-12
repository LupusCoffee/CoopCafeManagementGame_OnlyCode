// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSystemSetting.h"

const UEventSystemSetting* UEventSystemSetting::GetEventSettings()
{
    return GetDefault<UEventSystemSetting>();
}