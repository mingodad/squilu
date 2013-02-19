#ifdef WIN32
#include <windows.h>
// You are explicitly linking to GetDefaultPrinter because linking
// implicitly on Windows 95/98 or NT4 results in a runtime error.
// This block specifies which text version you explicitly link to.
#ifdef UNICODE
  #define GETDEFAULTPRINTER "GetDefaultPrinterW"
#else
  #define GETDEFAULTPRINTER "GetDefaultPrinterA"
#endif

// Size of internal buffer used to hold "printername,drivername,portname"
// string. You may have to increase this for huge strings.
#define MAXBUFFERSIZE 1024

/*----------------------------------------------------------------*/
/* DPGetDefaultPrinter                                            */
/*                                                                */
/* Parameters:                                                    */
/*   pPrinterName: Buffer alloc'd by caller to hold printer name. */
/*   pdwBufferSize: On input, ptr to size of pPrinterName.        */
/*          On output, min required size of pPrinterName.         */
/*                                                                */
/* NOTE: You must include enough space for the NULL terminator!   */
/*                                                                */
/* Returns: TRUE for success, FALSE for failure.                  */
/*----------------------------------------------------------------*/
BOOL DPGetDefaultPrinter(LPTSTR pPrinterName, LPDWORD pdwBufferSize)
{
  BOOL bFlag;
  OSVERSIONINFO osv;
  TCHAR cBuffer[MAXBUFFERSIZE];
  PRINTER_INFO_2 *ppi2 = NULL;
  DWORD dwNeeded = 0;
  DWORD dwReturned = 0;
  HMODULE hWinSpool = NULL;
  PROC fnGetDefaultPrinter = NULL;

  // What version of Windows are you running?
  osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osv);

  // If Windows 95 or 98, use EnumPrinters.
  if (osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
  {
    // The first EnumPrinters() tells you how big our buffer must
    // be to hold ALL of PRINTER_INFO_2. Note that this will
    // typically return FALSE. This only means that the buffer (the 4th
    // parameter) was not filled in. You do not want it filled in here.
    SetLastError(0);
    bFlag = EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, NULL, 0, &dwNeeded, &dwReturned);
    {
      if ((GetLastError() != ERROR_INSUFFICIENT_BUFFER) || (dwNeeded == 0))
        return FALSE;
    }

    // Allocate enough space for PRINTER_INFO_2.
    ppi2 = (PRINTER_INFO_2 *)GlobalAlloc(GPTR, dwNeeded);
    if (!ppi2)
      return FALSE;

    // The second EnumPrinters() will fill in all the current information.
    bFlag = EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, (LPBYTE)ppi2, dwNeeded, &dwNeeded, &dwReturned);
    if (!bFlag)
    {
      GlobalFree(ppi2);
      return FALSE;
    }

    // If specified buffer is too small, set required size and fail.
    if ((DWORD)lstrlen(ppi2->pPrinterName) >= *pdwBufferSize)
    {
      *pdwBufferSize = (DWORD)lstrlen(ppi2->pPrinterName) + 1;
      GlobalFree(ppi2);
      return FALSE;
    }

    // Copy printer name into passed-in buffer.
    lstrcpy(pPrinterName, ppi2->pPrinterName);

    // Set buffer size parameter to minimum required buffer size.
    *pdwBufferSize = (DWORD)lstrlen(ppi2->pPrinterName) + 1;
  }

  // If Windows NT, use the GetDefaultPrinter API for Windows 2000,
  // or GetProfileString for version 4.0 and earlier.
  else if (osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
  {
    if (osv.dwMajorVersion >= 5) // Windows 2000 or later (use explicit call)
    {
      hWinSpool = LoadLibrary("winspool.drv");
      if (!hWinSpool)
        return FALSE;
      fnGetDefaultPrinter = GetProcAddress(hWinSpool, GETDEFAULTPRINTER);
      if (!fnGetDefaultPrinter)
      {
        FreeLibrary(hWinSpool);
        return FALSE;
      }

      bFlag = fnGetDefaultPrinter(pPrinterName, pdwBufferSize);
        FreeLibrary(hWinSpool);
      if (!bFlag)
        return FALSE;
    }

    else // NT4.0 or earlier
    {
      // Retrieve the default string from Win.ini (the registry).
      // String will be in form "printername,drivername,portname".
      if (GetProfileString("windows", "device", ",,,", cBuffer, MAXBUFFERSIZE) <= 0)
        return FALSE;

      // Printer name precedes first "," character.
      strtok(cBuffer, ",");

      // If specified buffer is too small, set required size and fail.
      if ((DWORD)lstrlen(cBuffer) >= *pdwBufferSize)
      {
        *pdwBufferSize = (DWORD)lstrlen(cBuffer) + 1;
        return FALSE;
      }

      // Copy printer name into passed-in buffer.
      lstrcpy(pPrinterName, cBuffer);

      // Set buffer size parameter to minimum required buffer size.
      *pdwBufferSize = (DWORD)lstrlen(cBuffer) + 1;
    }
  }

  // Clean up.
  if (ppi2)
    GlobalFree(ppi2);

  return TRUE;
}
#undef MAXBUFFERSIZE
#undef GETDEFAULTPRINTER


// You are explicitly linking to SetDefaultPrinter because implicitly
// linking on Windows 95/98 or NT4 results in a runtime error.
// This block specifies which text version you explicitly link to.
#ifdef UNICODE
  #define SETDEFAULTPRINTER "SetDefaultPrinterW"
#else
  #define SETDEFAULTPRINTER "SetDefaultPrinterA"
#endif

/*-----------------------------------------------------------------*/
/* DPSetDefaultPrinter                                             */
/*                                                                 */
/* Parameters:                                                     */
/*   pPrinterName: Valid name of existing printer to make default. */
/*                                                                 */
/* Returns: TRUE for success, FALSE for failure.                   */
/*-----------------------------------------------------------------*/
BOOL DPSetDefaultPrinter(LPTSTR pPrinterName)

{
  BOOL bFlag;
  OSVERSIONINFO osv;
  DWORD dwNeeded = 0;
  HANDLE hPrinter = NULL;
  PRINTER_INFO_2 *ppi2 = NULL;
  LPTSTR pBuffer = NULL;
  LONG lResult;
  HMODULE hWinSpool = NULL;
  PROC fnSetDefaultPrinter = NULL;

  // What version of Windows are you running?
  osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osv);

  if (!pPrinterName)
    return FALSE;

  // If Windows 95 or 98, use SetPrinter.
  if (osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
  {
    // Open this printer so you can get information about it.
    bFlag = OpenPrinter(pPrinterName, &hPrinter, NULL);
    if (!bFlag || !hPrinter)
      return FALSE;

    // The first GetPrinter() tells you how big our buffer must
    // be to hold ALL of PRINTER_INFO_2. Note that this will
    // typically return FALSE. This only means that the buffer (the 3rd
    // parameter) was not filled in. You do not want it filled in here.
    SetLastError(0);
    bFlag = GetPrinter(hPrinter, 2, 0, 0, &dwNeeded);
    if (!bFlag)
    {
      if ((GetLastError() != ERROR_INSUFFICIENT_BUFFER) || (dwNeeded == 0))
      {
        ClosePrinter(hPrinter);
        return FALSE;
      }
    }

    // Allocate enough space for PRINTER_INFO_2.
    ppi2 = (PRINTER_INFO_2 *)GlobalAlloc(GPTR, dwNeeded);
    if (!ppi2)
    {
      ClosePrinter(hPrinter);
      return FALSE;
    }

    // The second GetPrinter() will fill in all the current information
    // so that all you have to do is modify what you are interested in.
    bFlag = GetPrinter(hPrinter, 2, (LPBYTE)ppi2, dwNeeded, &dwNeeded);
    if (!bFlag)
    {
      ClosePrinter(hPrinter);
      GlobalFree(ppi2);
      return FALSE;
    }

    // Set default printer attribute for this printer.
    ppi2->Attributes |= PRINTER_ATTRIBUTE_DEFAULT;
    bFlag = SetPrinter(hPrinter, 2, (LPBYTE)ppi2, 0);
    if (!bFlag)
    {
      ClosePrinter(hPrinter);
      GlobalFree(ppi2);
      return FALSE;
    }

    // Tell all open programs that this change occurred.
    // Allow each program 1 second to handle this message.
    lResult = SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0L,
      (LPARAM)(LPCTSTR)"windows", SMTO_NORMAL, 1000, NULL);
  }

  // If Windows NT, use the SetDefaultPrinter API for Windows 2000,
  // or WriteProfileString for version 4.0 and earlier.
  else if (osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
  {
    if (osv.dwMajorVersion >= 5) // Windows 2000 or later (use explicit call)
    {
      hWinSpool = LoadLibrary("winspool.drv");
      if (!hWinSpool)
        return FALSE;
      fnSetDefaultPrinter = GetProcAddress(hWinSpool, SETDEFAULTPRINTER);
      if (!fnSetDefaultPrinter)
      {
        FreeLibrary(hWinSpool);
        return FALSE;
      }

      bFlag = fnSetDefaultPrinter(pPrinterName);
      FreeLibrary(hWinSpool);
      if (!bFlag)
        return FALSE;
    }

    else // NT4.0 or earlier
    {
      // Open this printer so you can get information about it.
      bFlag = OpenPrinter(pPrinterName, &hPrinter, NULL);
      if (!bFlag || !hPrinter)
        return FALSE;

      // The first GetPrinter() tells you how big our buffer must
      // be to hold ALL of PRINTER_INFO_2. Note that this will
      // typically return FALSE. This only means that the buffer (the 3rd
      // parameter) was not filled in. You do not want it filled in here.
      SetLastError(0);
      bFlag = GetPrinter(hPrinter, 2, 0, 0, &dwNeeded);
      if (!bFlag)
      {
        if ((GetLastError() != ERROR_INSUFFICIENT_BUFFER) || (dwNeeded == 0))
        {
          ClosePrinter(hPrinter);
          return FALSE;
        }
      }

      // Allocate enough space for PRINTER_INFO_2.
      ppi2 = (PRINTER_INFO_2 *)GlobalAlloc(GPTR, dwNeeded);
      if (!ppi2)
      {
        ClosePrinter(hPrinter);
        return FALSE;
      }

      // The second GetPrinter() fills in all the current<BR/>
      // information.
      bFlag = GetPrinter(hPrinter, 2, (LPBYTE)ppi2, dwNeeded, &dwNeeded);
      if ((!bFlag) || (!ppi2->pDriverName) || (!ppi2->pPortName))
      {
        ClosePrinter(hPrinter);
        GlobalFree(ppi2);
        return FALSE;
      }

      // Allocate buffer big enough for concatenated string.
      // String will be in form "printername,drivername,portname".
      pBuffer = (LPTSTR)GlobalAlloc(GPTR,
        lstrlen(pPrinterName) +
        lstrlen(ppi2->pDriverName) +
        lstrlen(ppi2->pPortName) + 3);
      if (!pBuffer)
      {
        ClosePrinter(hPrinter);
        GlobalFree(ppi2);
        return FALSE;
      }

      // Build string in form "printername,drivername,portname".
      lstrcpy(pBuffer, pPrinterName);  lstrcat(pBuffer, ",");
      lstrcat(pBuffer, ppi2->pDriverName);  lstrcat(pBuffer, ",");
      lstrcat(pBuffer, ppi2->pPortName);

      // Set the default printer in Win.ini and registry.
      bFlag = WriteProfileString("windows", "device", pBuffer);
      if (!bFlag)
      {
        ClosePrinter(hPrinter);
        GlobalFree(ppi2);
        GlobalFree(pBuffer);
        return FALSE;
      }
    }

    // Tell all open programs that this change occurred.
    // Allow each app 1 second to handle this message.
    lResult = SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0L, 0L,
      SMTO_NORMAL, 1000, NULL);
  }

  // Clean up.
  if (hPrinter)
    ClosePrinter(hPrinter);
  if (ppi2)
    GlobalFree(ppi2);
  if (pBuffer)
    GlobalFree(pBuffer);

  return TRUE;
}
#undef SETDEFAULTPRINTER
#endif
