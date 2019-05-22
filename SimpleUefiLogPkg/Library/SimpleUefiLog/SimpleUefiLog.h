

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>

#define LOG_DEBUG(Format, ...) Log(_DEBUG, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_TRACE(Format, ...) Log(_TRACE, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_INFO(Format, ...) Log(_INFO, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_WRAN(Format, ...) Log(_WRAN, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_ERROR(Format, ...) Log(_ERROR, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_HEX(Data, DataSize, Format, ...) LogHex(Data, DataSize, _HEX, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__);

typedef enum _LOG_LEVEL
{
    _DEBUG = 1,
    _TRACE,
    _INFO,
    _WRAN,
    _ERROR,
	_HEX
}LOG_LEVEL;


EFI_STATUS
EFIAPI
LogInit(
    IN CHAR16 *Path
);


VOID
EFIAPI
LogClose(
    VOID
);

VOID
EFIAPI
SetShowLevel(
	IN BOOLEAN Is
);

VOID
EFIAPI
SetShowLine(
	IN BOOLEAN Is
);

VOID
EFIAPI
SetShowFuncName(
	IN BOOLEAN Is
);

VOID
EFIAPI
SetShowFileName(
	IN BOOLEAN Is
);

VOID
EFIAPI
SetShowTime(
	IN BOOLEAN Is
);

VOID
EFIAPI
Log(
	IN LOG_LEVEL level,
	IN CHAR8 *FileName,
	IN CONST CHAR8 *FuncName,
	IN UINTN LineNum,
	IN CONST CHAR16 *Format,
    ...
);

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
);
