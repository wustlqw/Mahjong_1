#include <Windows.h>
#include <ImageHlp.h>
#include <Shlwapi.h>

#include <time.h>
#include <tchar.h>

static DWORD CreateFullDump(int code, LPEXCEPTION_POINTERS pExceptionPointers = nullptr){
	TCHAR szFileName[MAX_PATH];
	HANDLE hDumpFile = NULL;
	MINIDUMP_EXCEPTION_INFORMATION medi;

	memset(&medi, 0x00, sizeof(MINIDUMP_EXCEPTION_INFORMATION));
	medi.ThreadId = GetCurrentThreadId();
	medi.ExceptionPointers = pExceptionPointers;
	medi.ClientPointers = FALSE;

	memset(szFileName, 0x00, MAX_PATH);
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	PathRemoveFileSpec(szFileName);

	tm nowTM;
	time_t now;
	time(&now);
	localtime_s(&nowTM, &now);

	TCHAR timeStr[64];
	_stprintf_s(
		timeStr,
		TEXT("\\log\\full_%04d_%02d_%02d_%02d_%02d_%02d.dump"),
		nowTM.tm_year + 1900,
		nowTM.tm_mon + 1,
		nowTM.tm_mday,
		nowTM.tm_hour,
		nowTM.tm_min,
		nowTM.tm_sec
		);

	_tcscat_s(szFileName, timeStr);
	hDumpFile = CreateFile(
		szFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (hDumpFile == INVALID_HANDLE_VALUE){
		return EXCEPTION_EXECUTE_HANDLER;
	}

	BOOL bRetCode = MiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		hDumpFile,
		MiniDumpWithFullMemory,
		&medi,
		NULL,
		NULL
		);

	if (bRetCode != TRUE)
		return (EXCEPTION_EXECUTE_HANDLER);

	return EXCEPTION_EXECUTE_HANDLER;
}