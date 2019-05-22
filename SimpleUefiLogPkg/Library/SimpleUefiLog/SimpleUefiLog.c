
#include "SimpleUefiLog.h"

#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Guid/FileInfo.h>

#define MAX_MSG_SIZE  0x200

EFI_FILE_PROTOCOL *gFileHandle = NULL;
EFI_FILE_PROTOCOL *Root = 0;

STATIC BOOLEAN ShowLevel = TRUE;
STATIC BOOLEAN ShowLine = TRUE;
STATIC BOOLEAN ShowFuncName = TRUE;
STATIC BOOLEAN ShowFileName = FALSE;
STATIC BOOLEAN ShowTime = TRUE;

VOID
EFIAPI
SetShowLevel(
	IN BOOLEAN Is
)
{
	ShowLevel = Is;
}

VOID
EFIAPI
SetShowLine(
	IN BOOLEAN Is
) 
{
	ShowLine = Is;
}

VOID
EFIAPI
SetShowFuncName(
	IN BOOLEAN Is
)
{
	ShowFuncName = Is;
}

VOID
EFIAPI
SetShowFileName(
	IN BOOLEAN Is
)
{
	ShowFileName = Is;
}

VOID
EFIAPI
SetShowTime(
	IN BOOLEAN Is
)
{
	ShowTime = Is;
}

CHAR16 *
GetLevelString(
	IN LOG_LEVEL Level
)
{
	switch (Level)
	{
	case _DEBUG:
		return L"[ DEBUG ]";
	case _TRACE:
		return L"[ TRACE ]";
	case _INFO:
		return L"[ INFO ]";
	case _WRAN:
		return L"[ WARN ]";
	case _ERROR:
		return L"[ ERROR ]";
	case _HEX:
		return L"[ HEX ]";
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

EFI_STATUS
OpenLogFile(
	OUT EFI_FILE_PROTOCOL **FileHandle,
	IN CHAR16 *Path
)
{
	EFI_STATUS Status;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystemProtocol;

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
EFIAPI
LogInit(
	IN CHAR16 *Path
)
{
	EFI_STATUS Status;
	if (Path == NULL)
	{
        return EFI_UNSUPPORTED;
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
	UINTN InfoBufferSize = 0;
	EFI_FILE_INFO *FileInfo = NULL;
    BOOLEAN FileInfoBufferTooSmall = FALSE;

	Status = gFileHandle->GetInfo(
		gFileHandle,
		&gEfiFileInfoGuid, 
		&InfoBufferSize, 
		(VOID *)FileInfo
	);

	if (Status == EFI_BUFFER_TOO_SMALL)
	{
		FileInfo = AllocatePool(InfoBufferSize);
        FileInfoBufferTooSmall = TRUE;
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
        if (FileInfoBufferTooSmall) {
            FreePool(FileInfo);
        }
		return Status;
	}

	Status = gFileHandle->SetPosition(
		gFileHandle,
		FileInfo->FileSize
	);

	if (EFI_ERROR(Status))
	{
		Print(L"Set Position Failed: %r\n", Status);
        goto _FreeFileInfoBuffer;
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
        goto _FreeFileInfoBuffer;
		return Status;
	}

	Status = gFileHandle->Flush(gFileHandle);

	if (EFI_ERROR(Status))
	{
		Print(L"Flush Data Failed: %r\n", Status);
        goto _FreeFileInfoBuffer;
        return Status;
	}
    
    goto _FreeFileInfoBuffer;
    
_FreeFileInfoBuffer:
    if(FileInfoBufferTooSmall) FreePool(FileInfo);
    
	return Status;
    

}

VOID
EFIAPI
LogClose(
	VOID
)
{
	gFileHandle->Flush(gFileHandle);
	gFileHandle->Close(gFileHandle);
    Root->Close(Root);
}

CHAR16 *
SetInfo(
	IN LOG_LEVEL level,
	IN CHAR8 *FileName,
	IN CHAR8 *FuncName,
	IN UINTN LineNum
)
{
	CHAR16 *Header = NULL;
    CHAR16 *str_1 = NULL;
    CHAR16 *str_2 = NULL;
    CHAR16 *str_3 = NULL;
    CHAR16 *str_4 = NULL;
    CHAR16 *str_5 = NULL;
	if (ShowLevel)
	{
		str_1 = CatSPrint(
			NULL,
			L"%s ",
			GetLevelString(level)
		);
	}

	if (ShowTime)
	{
		str_2 = CatSPrint(
			NULL,
			L"%s ",
			GetEfiTime()
		);
	}

	if (ShowFileName)
	{
		str_3 = CatSPrint(
			NULL,
			L"%a ",
			FileName
		);
	}

	if (ShowFuncName)
	{
		str_4 = CatSPrint(
			NULL,
			L"%a ",
			FuncName
		);
	}

	if (ShowLine)
	{
		str_5 = CatSPrint(
			NULL,
			L"%d ",
			LineNum
		);
	}

    Header = CatSPrint(NULL, L"%s %s %s %s %s", str_1, str_2, str_3, str_4, str_5);
    
    FreePool(str_1);
    FreePool(str_2);
    FreePool(str_3);
    FreePool(str_4);
    FreePool(str_5);
	return Header;
}

VOID
EFIAPI
Log(
	IN LOG_LEVEL level,
	IN CHAR8 *FileName,
	IN CONST CHAR8 *FuncName,
	IN UINTN LineNum,
	IN CONST CHAR16 *Format,
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

	Header = SetInfo(level, FileName, FuncName, LineNum);

	Data = CatSPrint(
		Data, 
		L"%s%s",
		Header,
		Buffer
	);

	Print(L"%s", Data);
	WriteData((VOID *)Data, StrLen(Data) * 2);
    FreePool(Data);
    FreePool(Header);
}

VOID
EFIAPI
LogHex(
	IN VOID *Src,
	IN UINT64 size,
	IN LOG_LEVEL level,
	IN CHAR8 *FileName,
	IN CONST CHAR8 *FuncName,
	IN UINTN LineNum,
	IN CONST CHAR16 *Format,
	...
)
{
	VA_LIST Marker;
	CHAR16 Buffer[MAX_MSG_SIZE];
	CHAR16 *Header = NULL;
	CHAR16 *Data = NULL;
	UINT64 Index = 0;
	CHAR16 *HexStr = NULL;

	Print(L"size:%d\n", size);

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

	Header = SetInfo(level, FileName, FuncName, LineNum);

	for (Index = 0; Index < size; Index++)
	{
		if (Index % 16 == 0)
		{
			HexStr = CatSPrint(HexStr, L"%8X:  ", Index);
		}

		HexStr = CatSPrint(HexStr, L"%2X  ", ((CHAR8 *)Src)[Index]);

		if (Index % 16 == 15)
		{
			HexStr = CatSPrint(HexStr, L"\n");
		}
	}

	Data = CatSPrint(
		Data,
		L"%s%s\n%s\n",
		Header,
		Buffer,
		HexStr
	);

	Print(L"%s\n", Data);
	WriteData((VOID *)Data, StrLen(Data) * 2);
}

