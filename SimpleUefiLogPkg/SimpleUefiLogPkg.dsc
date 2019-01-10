[Defines]
PLATFORM_NAME					= SimpleUefiLog
PLATFORM_GUID					= 9DD8D48F-C080-40F0-910E-BAA61AF4B544
PLATFORM_VERSION				= 1.0
DSC_SPECIFICATION				= 0x00010005
OUTPUT_DIRECTORY            	= Build/SimpleUefiLog
SUPPORTED_ARCHITECTURES        	= IA32|IPF|X64|EBC|ARM|AARCH64
BUILD_TARGETS                  	= DEBUG|RELEASE|NOOPT
SKUID_IDENTIFIER               	= DEFAULT

[LibraryClasses]
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf

 [Components]
  SimpleUefiLogPkg/Library/SimpleUefiLog/SimpleUefiLog.inf

