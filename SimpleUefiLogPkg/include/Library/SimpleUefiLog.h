

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>

#define LOG_DEBUG(Format, ...) Log(DEBUG, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_TRACE(Format, ...) Log(TRACE, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_INFO(Format, ...) Log(INFO, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_WRAN(Format, ...) Log(WRAN, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_ERROR(Format, ...) Log(ERROR, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_HEX(Data, DataSize, Format, ...) LogHex(Data, DataSize, HEX, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__);

typedef enum _LOG_LEVEL
{
    DEBUG = 1,
    TRACE,
    INFO,
    WRAN,
    ERROR,
	HEX
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
	IN CHAR8 *FuncName,
	IN UINTN LineNum,
	IN CHAR16 *Format,
    ...
);

VOID
EFIAPI
LogHex(
	IN VOID *Src,
	IN UINT64 size,
	IN LOG_LEVEL level,
	IN CHAR8 *FileName,
	IN CHAR8 *FuncName,
	IN UINTN LineNum,
	IN CHAR16 *Format,
	...
);
