#include "JavascriptUMG.h"
#include "JavascriptUICommands.h"
#include "Framework/Commands/Commands.h"

PRAGMA_DISABLE_OPTIMIZATION

UJavascriptUICommands::UJavascriptUICommands(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer), bRegistered(false)
{	
}

void UJavascriptUICommands::BeginDestroy()
{
	Super::BeginDestroy();

	Discard();
}

void UJavascriptUICommands::Commit()
{
	Discard();

//	IJavascriptEditorModule::Get().AddExtension(this);
	
	bRegistered = true;
}

void UJavascriptUICommands::Discard()
{
	if (bRegistered)
	{
//		IJavascriptEditorModule::Get().RemoveExtension(this);
	}

	bRegistered = false;
}

void UJavascriptUICommands::Initialize()
{
	for (auto info : Commands)
	{
		FJavascriptUICommandInfo CommandInfo;

		UI_COMMAND_Function(
			BindingContext.Handle.Get(),
			CommandInfo.Handle,
			TEXT(""),
			*info.Id,
			*FString::Printf(TEXT("%s_Tooltip"), *info.Id),
			TCHAR_TO_ANSI(*FString::Printf(TEXT(".%s"), *info.Id)),
			*info.FriendlyName,
			*info.Description,
			EUserInterfaceActionType::Type(info.ActionType.GetValue()),
			info.DefaultChord);

		CommandInfos.Add(CommandInfo);
	}

	BroadcastCommandsChanged();
}

void UJavascriptUICommands::Uninitialize()
{	
	CommandInfos.Empty();

	BroadcastCommandsChanged();
}

void UJavascriptUICommands::BroadcastCommandsChanged()
{
	FBindingContext::CommandsChanged.Broadcast();
}

void UJavascriptUICommands::Refresh()
{
}

void UJavascriptUICommands::Bind(FJavascriptUICommandList CommandList)
{
	Bind(CommandList.Handle.Get());
}

void UJavascriptUICommands::Unbind(FJavascriptUICommandList CommandList)
{
	Unbind(CommandList.Handle.Get());
}

void UJavascriptUICommands::Bind(FUICommandList* CommandList)
{
	for (int32 Index = 0; Index < Commands.Num(); ++Index)
	{
		CommandList->MapAction(
			CommandInfos[Index].Handle,
			FExecuteAction::CreateLambda([this, Index](){
				if (OnExecuteAction.IsBound())
				{
					OnExecuteAction.Execute(Commands[Index].Id);
				}
			}),
			FCanExecuteAction::CreateLambda([this, Index](){
				return !OnCanExecuteAction.IsBound() || OnCanExecuteAction.Execute(Commands[Index].Id);
			}),
			FCanExecuteAction::CreateLambda([this, Index](){
				return !OnIsActionChecked.IsBound() || OnIsActionChecked.Execute(Commands[Index].Id);
			}),
			FCanExecuteAction::CreateLambda([this, Index](){
				return !OnIsActionButtonVisible.IsBound() || OnIsActionButtonVisible.Execute(Commands[Index].Id);
			})
		);
	}
	
}
void UJavascriptUICommands::Unbind(FUICommandList* CommandList)
{
	// unbind is not supported, just rebind again!
}

FJavascriptUICommandInfo UJavascriptUICommands::GetAction(FString Id)
{
	if (Commands.Num() == CommandInfos.Num())
	{
		for (int32 Index = 0; Index < Commands.Num(); ++Index)
		{
			if (Commands[Index].Id == Id)
			{
				return CommandInfos[Index];
			}
		}
	}

	return FJavascriptUICommandInfo();
}

PRAGMA_ENABLE_OPTIMIZATION