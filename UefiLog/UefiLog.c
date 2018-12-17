
#include "UefiLog.h"

#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePathToText.h>

#include <Guid/FileInfo.h>

#define MAX_MSG_SIZE  0x200

EFI_FILE_PROTOCOL *gFileHandle = NULL;
STATIC BOOLEAN ShowLevel = TRUE;
STATIC BOOLEAN ShowLine = TRUE;
STATIC BOOLEAN ShowFuncName = TRUE;
STATIC BOOLEAN ShowFileName = FALSE;
STATIC BOOLEAN ShowTime = TRUE;

VOID
SetShowLevel(
	BOOLEAN Is
)
{
	ShowLevel = Is;
}

VOID
SetShowLine(
	BOOLEAN Is
) 
{
	ShowLine = Is;
}

VOID
SetShowFuncName(
	BOOLEAN Is
)
{
	ShowFuncName = Is;
}

VOID
SetShowFileName(
	BOOLEAN Is
)
{
	ShowFileName = Is;
}

VOID
SetShowTime(
	BOOLEAN Is
)
{
	ShowTime = Is;
}

CHAR16 *
GetLevelString(
	LOG_LEVEL Level
)
{
	switch (Level)
	{
	case DEBUG:
		return L"[ DEBUG ]";
	case TRACE:
		return L"[ TRACE ]";
	case INFO:
		return L"[ INFO ]";
	case WRAN:
		return L"[ WARN ]";
	case ERROR:
		return L"[ ERROR ]";
	default:
		return L"[       ]";
		break;
	}
}

CHAR16 *
GetEfiTime(
	VOID
)
{
	EFI_TIME Time;
	EFI_STATUS Status;
	CHAR16 *TimeStr = NULL;
	Status = gRT->GetTime(
		&Time, 
		NULL
	);
	if (EFI_ERROR(Status))
	{
		Print(L"Get Time Failed: %r\n", Status);
		return L"1970-01-01 00:00:00";
	}

	TimeStr = CatSPrint(
		TimeStr,
		L"%d-%02d-%02d %02d:%02d:%02d",
		Time.Year,
		Time.Month,
		Time.Day,
		Time.Hour,
		Time.Minute,
		Time.Second
	);
	return TimeStr;
}

CHAR16 *
GetStrReplace(
	IN CHAR16 *Str,
	IN CHAR16 *OldStr,
	IN CHAR16 *NewStr
)
{
	UINTN LenStr;
	UINTN OldStrLen;
	UINTN Index = 0;
	CHAR16 *BStr = NULL;

	LenStr = StrLen(Str);
	OldStrLen = StrLen(OldStr);

	BStr = AllocatePool(LenStr);
	gBS->SetMem(
		BStr, 
		LenStr,
		0
	);

	for (Index = 0; Index < LenStr; Index++)
	{
		if (!StrnCmp(
			Str + Index, 
			OldStr, 
			OldStrLen
		))
		{
			StrCat(
				BStr, 
				NewStr
			);
			Index += OldStrLen - 1;
		}
		else
		{
			StrnCat(
				BStr, 
				Str + Index, 
				1
			);
		}
	}

	StrCpy(
		Str, 
		BStr
	);

	return Str;
}

EFI_STATUS
GetCurrentPath(
	OUT CHAR16 **Path
)
{
	EFI_STATUS Status;
	EFI_LOADED_IMAGE_PROTOCOL *LoadedImageProtocol;
	EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevicePathToTextProtocol;
	
	Status = gBS->LocateProtocol(
		&gEfiDevicePathToTextProtocolGuid, 
		NULL, 
		(VOID **)&DevicePathToTextProtocol
	);
	if (EFI_ERROR(Status))
	{
		Print(L"Get DevicePathToTextProtocol Error: %r\n", Status);
		return Status;
	}

	Status = gBS->HandleProtocol(
		gImageHandle, 
		&gEfiLoadedImageProtocolGuid, 
		(VOID **)&LoadedImageProtocol
	);
	if (EFI_ERROR(Status))
	{
		Print(L"Get LoadedImageProtocol Error: %r\n", Status);
		return Status;
	}

	*Path = DevicePathToTextProtocol->ConvertDevicePathToText(
		LoadedImageProtocol->FilePath,
		TRUE,
		TRUE
	);

	return EFI_SUCCESS;
}

EFI_STATUS
OpenLogFile(
	OUT EFI_FILE_PROTOCOL **FileHandle,
	IN CHAR16 *Path
)
{
	EFI_STATUS Status;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystemProtocol;
	EFI_FILE_PROTOCOL *Root = 0;

	Status = gBS->LocateProtocol(
		&gEfiSimpleFileSystemProtocolGuid,
		NULL,
		(VOID **)&SimpleFileSystemProtocol
	);

	if (EFI_ERROR(Status))
	{
		Print(L"Get SimpleFileSystemProtocol Error: %r\n", Status);
		return Status;
	}

	Status = SimpleFileSystemProtocol->OpenVolume(
		SimpleFileSystemProtocol,
		&Root
	);

	if (EFI_ERROR(Status))
	{
		Print(L"OpenVolume Failed : %r\n", Status);
		return Status;
	}

	Status = Root->Open(
		Root,
		FileHandle,
		Path,
		EFI_FILE_MODE_CREATE |
		EFI_FILE_MODE_WRITE |
		EFI_FILE_MODE_READ,
		0
	);

	if (EFI_ERROR(Status))
	{
		Print(L"Open File Failed : %r\n", Status);
		return Status;
	}

	return EFI_SUCCESS;
}


EFI_STATUS
LogInit(
	IN CHAR16 *Path
)
{
	EFI_STATUS Status;
	if (Path == NULL)
	{
		Status = GetCurrentPath(&Path);
		Path = GetStrReplace(Path, L".efi", L".log");
		if (EFI_ERROR(Status))
		{
			Print(L"GetCurrentPathError: %r\n", Status);
			return Status;
		}
	}

	Status = OpenLogFile(
		&gFileHandle, 
		Path
	);

	return Status;
}

EFI_STATUS
WriteData(
	IN VOID *Buffer,
	IN UINT64 BufferSize
)
{
	if (gFileHandle == NULL)
	{
		Print(L"Unknow File!\n");
		return EFI_UNSUPPORTED;
	}

	if (Buffer == NULL)
	{
		Print(L"Unknow Buffer!\n");
		return EFI_UNSUPPORTED;
	}

	if (BufferSize == 0)
	{
		Print(L"Unknow BufferSize!\n");
		return EFI_UNSUPPORTED;
	}

	EFI_STATUS Status;
	UINTN InfoBufferSize;
	EFI_FILE_INFO *FileInfo = NULL;

	Status = gFileHandle->GetInfo(
		gFileHandle,
		&gEfiFileInfoGuid, 
		&InfoBufferSize, 
		(VOID *)FileInfo
	);

	if (Status == EFI_BUFFER_TOO_SMALL)
	{
		FileInfo = AllocatePool(InfoBufferSize);
		Status = gFileHandle->GetInfo(
			gFileHandle,
			&gEfiFileInfoGuid,
			&InfoBufferSize,
			(VOID *)FileInfo
		);
	}

	if (EFI_ERROR(Status))
	{
		Print(L"Get File Info Failed: %r\n", Status);
		return Status;
	}

	Status = gFileHandle->SetPosition(
		gFileHandle,
		FileInfo->FileSize
	);

	if (EFI_ERROR(Status))
	{
		Print(L"Set Position Failed: %r\n", Status);
		return Status;
	}

	Status = gFileHandle->Write(
		gFileHandle,
		&BufferSize,
		Buffer
	);
	if (EFI_ERROR(Status))
	{
		Print(L"Write File Failed: %r\n", Status);
		return Status;
	}

	Status = gFileHandle->Flush(gFileHandle);

	if (EFI_ERROR(Status))
	{
		Print(L"Flush Data Failed: %r\n", Status);
		return Status;
	}
	return Status;
}

VOID
LogClose(
	VOID
)
{
	gFileHandle->Flush(gFileHandle);
	gFileHandle->Close(gFileHandle);
}

VOID
Log(
	LOG_LEVEL level,
	CHAR8 *FileName,
	CHAR8 *FuncName,
	UINTN LineNum,
	CHAR16 *Format,
	...
)
{
	VA_LIST Marker;
	CHAR16 Buffer[MAX_MSG_SIZE];
	CHAR16 *Header = NULL;
	CHAR16 *Data = NULL;

	VA_START(
		Marker, 
		Format
	);
	UnicodeVSPrint(
		Buffer, 
		MAX_MSG_SIZE, 
		Format, 
		Marker
	);
	VA_END(Marker);

	if (ShowLevel)
	{
		Header = CatSPrint(
			Header, 
			L"%s ", 
			GetLevelString(level)
		);
	}

	if (ShowTime)
	{
		Header = CatSPrint(
			Header,
			L"%s ",
			GetEfiTime()
		);
	}

	if (ShowFileName)
	{
		Header = CatSPrint(
			Header,
			L"%a ",
			FileName
		);
	}

	if (ShowFuncName)
	{
		Header = CatSPrint(
			Header,
			L"%a ",
			FuncName
		);
	}

	if (ShowLine)
	{
		Header = CatSPrint(
			Header,
			L"%d ",
			LineNum
		);
	}

	if (Header == NULL)
	{
		Header = L"";
	}

	Data = CatSPrint(
		Data, 
		L"%s%s",
		Header,
		Buffer
	);

	Print(L"%s", Data);
	WriteData((VOID *)Data, StrLen(Data) * 2);
}

