#include "EventAnnouncer.h"
#include "EventSystemStructs.h"
#include "EventSubsystem.h"

void AEventAnnouncer::InvokeEvent(FName Event, FDataContainer DataContainer)
{
    if (!this) return;

    UObject* TargetObject = UEventSubsystem::GetEventAnnouncer();
    if (!TargetObject) return;

    FMulticastInlineDelegateProperty* DelegateProp = FindFProperty<FMulticastInlineDelegateProperty>(TargetObject->GetClass(), Event);

    if (DelegateProp)
    {
        FMulticastScriptDelegate* ScriptDelegate = DelegateProp->GetPropertyValuePtr_InContainer(TargetObject);

        if (ScriptDelegate)
        {
            UFunction* Signature = DelegateProp->SignatureFunction;
            uint8* ParamsBuffer = (uint8*)FMemory_Alloca(Signature->ParmsSize);
            FMemory::Memzero(ParamsBuffer, Signature->ParmsSize);

            for (FProperty* Prop = Signature->PropertyLink; Prop; Prop = Prop->PropertyLinkNext)
            {
                if (Prop->HasAnyPropertyFlags(CPF_Parm))
                {
                    Prop->InitializeValue_InContainer(ParamsBuffer);

                    const double* MappedStringValue = DataContainer.Data.Find(FDataTag(Prop->GetFName()));
                    if (MappedStringValue)
                    {
                           void* DestAddr = Prop->ContainerPtrToValuePtr<void>(ParamsBuffer);
                           Prop->CopySingleValue(DestAddr, MappedStringValue);
                    }
                }
            }
             
            ScriptDelegate->ProcessMulticastDelegate<UObject>(ParamsBuffer);
        }
    }
}
