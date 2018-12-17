# Simple_Uefi_Log

##### version：1.0.0
UefiLog是在UEFI环境下轻量级日志系统
### 1.
在使用宏定义前调用`LogInit()`进行初始化日志库
在程序退出前调用`LogClose()`关闭日志库
```
// 使用前初始化日志库
// Path  可以传入NULL，则log文件和程序同目录
// Path  非NULL时，需保证上级目录存在
EFI_STATUS
LogInit(
	IN CHAR16 *Path
);
```
### 2.
可以调用`SetShowLevel()`来选择是否显示日志级别
相关设置有 行数、函数名称、文件名、时间、日志级别
默认不显示文件名。

### 3.
才疏学浅，能力有限，望指正！

### 4.
不断更新中





