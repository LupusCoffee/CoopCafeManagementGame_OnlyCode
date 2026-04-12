#include "EventDelegateUtility.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "K2Node_FunctionEntry.h"
#include "EventSystemStructs.h"

void UEventDelegateUtility::CreateEventDispatcher(UBlueprint* TargetBlueprint, FString DesiredName, TArray<FDataTag> Params)
{
    FName EventName = FBlueprintEditorUtils::FindUniqueKismetName(TargetBlueprint, DesiredName);

    const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
    check(nullptr != K2Schema);
    UBlueprint* const Blueprint = TargetBlueprint;
    check(nullptr != Blueprint);

    Blueprint->Modify();

    FEdGraphPinType DelegateType;
    DelegateType.PinCategory = UEdGraphSchema_K2::PC_MCDelegate;
    const bool bVarCreatedSuccess = FBlueprintEditorUtils::AddMemberVariable(Blueprint, EventName, DelegateType);
    if (!bVarCreatedSuccess)
    {
        return;
    }

    UEdGraph* const NewGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, EventName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
    if (!NewGraph)
    {
        FBlueprintEditorUtils::RemoveMemberVariable(Blueprint, EventName);
        return;
    }

    NewGraph->bEditable = false;

    K2Schema->CreateDefaultNodesForGraph(*NewGraph);
    K2Schema->CreateFunctionGraphTerminators(*NewGraph, (UClass*)nullptr);
    K2Schema->AddExtraFunctionFlags(NewGraph, (FUNC_BlueprintCallable | FUNC_BlueprintEvent | FUNC_Public));
    K2Schema->MarkFunctionEntryAsEditable(NewGraph, true);

    TArray<UK2Node_FunctionEntry*> EntryNodes;
    NewGraph->GetNodesOfClass(EntryNodes);

    if (EntryNodes.Num() > 0)
    {
        UK2Node_FunctionEntry* EntryNode = EntryNodes[0];
        EntryNode->Modify();

        FEdGraphPinType FloatParamType;
        FloatParamType.PinCategory = UEdGraphSchema_K2::PC_Real;
        FloatParamType.PinSubCategory = UEdGraphSchema_K2::PC_Double; // Use PC_Double for UE5+

		for (const FDataTag& ParamName : Params)
            EntryNode->CreateUserDefinedPin(ParamName.GetTagName(), FloatParamType, EGPD_Output);
    }

    Blueprint->DelegateSignatureGraphs.Add(NewGraph);
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
    FKismetEditorUtilities::CompileBlueprint(Blueprint);
}

void UEventDelegateUtility::DeleteEventDispatcher(UBlueprint* TargetBlueprint, FName EventName)
{
    if (!TargetBlueprint) return;

    bool bModified = false;

    UEdGraph* SignatureGraph = FBlueprintEditorUtils::GetDelegateSignatureGraphByName(TargetBlueprint, EventName);
    if (SignatureGraph)
    {
        TargetBlueprint->DelegateSignatureGraphs.Remove(SignatureGraph);

        FBlueprintEditorUtils::RemoveGraph(TargetBlueprint, SignatureGraph, EGraphRemoveFlags::Default);
        bModified = true;
    }

    int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(TargetBlueprint, EventName);
    if (VarIndex != INDEX_NONE)
    {
        FBlueprintEditorUtils::RemoveMemberVariable(TargetBlueprint, EventName);
        bModified = true;
    }

    if (bModified)
    {
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(TargetBlueprint);
        FKismetEditorUtilities::CompileBlueprint(TargetBlueprint);
    }
}

void UEventDelegateUtility::UpdateDispatcherParameters(UBlueprint* TargetBlueprint, FString EventName, TArray<FDataTag> Params, UDetailPanelDataObject* obj)
{
    UBlueprint* const Blueprint = TargetBlueprint;
    check(nullptr != Blueprint);

    Blueprint->Modify();

    UEdGraph* DispatcherGraph = nullptr;
    for (UEdGraph* Graph : Blueprint->DelegateSignatureGraphs)
    {
        if (Graph->GetFName() == FName(*EventName))
        {
            DispatcherGraph = Graph;
            break;
        }
    }

    if (!DispatcherGraph)
    {
        return;
    }

    TArray<UK2Node_FunctionEntry*> EntryNodes;
    DispatcherGraph->GetNodesOfClass(EntryNodes);
    if (EntryNodes.Num() > 0)
    {
        UK2Node_FunctionEntry* EntryNode = EntryNodes[0];
        EntryNode->Modify();
        TSet<FName> ExistingPinNames;
        for (UEdGraphPin* Pin : EntryNode->Pins)
        {
            if (Pin->Direction == EGPD_Output && Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
            {
                ExistingPinNames.Add(Pin->PinName);
            }
        }
		TSet<FName> DesiredPinNames;
        for(auto& DataTag : Params)
        {
            DesiredPinNames.Add(DataTag.GetTagName());
		}

        for(const FName& ExistingPinName : ExistingPinNames)
        {
            if (!DesiredPinNames.Contains(ExistingPinName))
            {
                EntryNode->RemoveUserDefinedPinByName(ExistingPinName);
            }
		}

        FEdGraphPinType FloatParamType;
        FloatParamType.PinCategory = UEdGraphSchema_K2::PC_Real;
        FloatParamType.PinSubCategory = UEdGraphSchema_K2::PC_Double; // Use PC_Double for UE5+
        for (const FDataTag& ParamName : Params)
        {
            if (!ExistingPinNames.Contains(ParamName.GetTagName()))
            {
                EntryNode->CreateUserDefinedPin(ParamName.GetTagName(), FloatParamType, EGPD_Output);
            }
        }
    }

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
    FKismetEditorUtilities::CompileBlueprint(Blueprint);
}
