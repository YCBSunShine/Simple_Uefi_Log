

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>

#define LOG_DEBUG(Format, ...) Log(DEBUG, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_TRACE(Format, ...) Log(TRACE, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_INFO(Format, ...) Log(INFO, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_WRAN(Format, ...) Log(WRAN, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)
#define LOG_ERROR(Format, ...) Log(ERROR, __FILE__, __func__, __LINE__, Format, ##__VA_ARGS__)

typedef enum _LOG_LEVEL
{
    DEBUG = 1,
    TRACE,
    INFO,
    WRAN,
    ERROR
}LOG_LEVEL;

// 使用前初始化日志库
// Path  可以传入NULL，则log文件和程序同目录
// Path  非NULL时，需保证上级目录存在
EFI_STATUS
LogInit(
        IN CHAR16 *Path
        );

// 程序结束前调用，用来关闭打开的Log文件
VOID
LogClose(
         VOID
         );

VOID
SetShowLevel(
             BOOLEAN Is                // default 'true'
);

VOID
SetShowLine(
            BOOLEAN Is                // default 'true'
);

VOID
SetShowFuncName(
                BOOLEAN Is                // default 'true'
);

VOID
SetShowFileName(
                BOOLEAN Is                // default 'false'
);

VOID
SetShowTime(
            BOOLEAN Is                // default 'true'
);

VOID
Log(
    LOG_LEVEL level,
    CHAR8 *FileName,
    CHAR8 *FuncName,
    UINTN LineNum,
    CHAR16 *Format,
    ...
    );
