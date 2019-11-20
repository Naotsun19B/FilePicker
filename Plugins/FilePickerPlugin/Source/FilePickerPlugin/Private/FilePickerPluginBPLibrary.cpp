// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FilePickerPluginBPLibrary.h"
#include "FilePickerPlugin.h"
#include "Engine.h"
#include "Misc/Paths.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"

UFilePickerPluginBPLibrary::UFilePickerPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UFilePickerPluginBPLibrary::OpenFileDialog(
	EDialogResult& OutputPin,
	TArray<FString>& FilePath,
	const FString& DialogTitle,
	const FString& DefaultPath,
	const FString& DefaultFile,
	const FString& FileType,
	bool IsMultiple
)
{
	//�E�B���h�E�n���h�����擾
	void* windowHandle = UFilePickerPluginBPLibrary::GetWindowHandle();

	if (windowHandle) 
	{
		IDesktopPlatform* desktopPlatform = FDesktopPlatformModule::Get();
		if (desktopPlatform)
		{
			//�_�C�A���O���J��
			bool result = desktopPlatform->OpenFileDialog(
				windowHandle,
				DialogTitle,
				DefaultPath,
				DefaultFile,
				FileType,
				(IsMultiple ? EFileDialogFlags::Type::Multiple : EFileDialogFlags::Type::None),
				FilePath
			);

			if (result)
			{
				//���΃p�X���΃p�X�ɕϊ�
				for (FString& fp : FilePath)
				{
					fp = FPaths::ConvertRelativePathToFull(fp);
					UE_LOG(FilePickerPlugin, Log, TEXT("Acquired file path : %s"), *fp);
				}

				UE_LOG(FilePickerPlugin, Log, TEXT("Open File Dialog : Successful"));
				OutputPin = EDialogResult::Successful;
				return;
			}
		}
	}

	UE_LOG(FilePickerPlugin, Log, TEXT("Open File Dialog : Cancelled"));
	OutputPin = EDialogResult::Cancelled;
}

void UFilePickerPluginBPLibrary::OpenDirectoryDialog(
	EDialogResult& OutputPin,
	FString& FolderPath,
	const FString& DialogTitle,
	const FString& DefaultPath
)
{
	//�E�B���h�E�n���h�����擾
	void* windowHandle = UFilePickerPluginBPLibrary::GetWindowHandle();

	if (windowHandle)
	{
		IDesktopPlatform* desktopPlatform = FDesktopPlatformModule::Get();
		if (desktopPlatform)
		{
			//�_�C�A���O���J��
			bool result = desktopPlatform->OpenDirectoryDialog(
				windowHandle,
				DialogTitle,
				DefaultPath,
				FolderPath
			);

			if (result)
			{
				//���΃p�X���΃p�X�ɕϊ�
				FolderPath = FPaths::ConvertRelativePathToFull(FolderPath);
				UE_LOG(FilePickerPlugin, Log, TEXT("Acquired folder path : %s"), *FolderPath);

				UE_LOG(FilePickerPlugin, Log, TEXT("Open Directory Dialog : Successful"));
				OutputPin = EDialogResult::Successful;
				return;
			}
		}
	}

	UE_LOG(FilePickerPlugin, Log, TEXT("Open Directory Dialog : Cancelled"));
	OutputPin = EDialogResult::Cancelled;
}

void* UFilePickerPluginBPLibrary::GetWindowHandle()
{
	//�G�f�B�^�̏ꍇ
	if (GIsEditor)
	{
		IMainFrameModule& MainFrameModule = IMainFrameModule::Get();
		TSharedPtr<SWindow> MainWindow = MainFrameModule.GetParentWindow();

		if (MainWindow.IsValid() && MainWindow->GetNativeWindow().IsValid())
		{
			return MainWindow->GetNativeWindow()->GetOSWindowHandle();
		}
	}
	//���s���̏ꍇ
	else
	{
		if (GEngine && GEngine->GameViewport)
		{
			return GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		}
	}

	return nullptr;
}