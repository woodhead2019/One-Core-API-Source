/*
 * Performance Data Helper (pdh.dll)
 *
 * Copyright 2007 Andrey Turkin
 * Copyright 2007 Hans Leidekker
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define WIN32_NO_STATUS

#include <stdarg.h>
#include <math.h>

#define NONAMELESSUNION
#define NONAMELESSSTRUCT
#include <windef.h>
#include <winbase.h>

//#include "winperf.h"

#include <wine/debug.h>
#include <wine/list.h>
#include <wine/unicode.h>
#include <wincrypt.h>
#include <rtlfuncs.h>

WINE_DEFAULT_DEBUG_CHANNEL(pdh);

typedef struct _CRYPT_TIMESTAMP_PARA {
  LPCSTR             pszTSAPolicyId;
  BOOL               fRequestCerts;
  CRYPT_INTEGER_BLOB Nonce;
  DWORD              cExtension;
  PCERT_EXTENSION    rgExtension;
} CRYPT_TIMESTAMP_PARA, *PCRYPT_TIMESTAMP_PARA;

typedef struct _CRYPT_TIMESTAMP_ACCURACY {
  DWORD dwSeconds;
  DWORD dwMillis;
  DWORD dwMicros;
} CRYPT_TIMESTAMP_ACCURACY, *PCRYPT_TIMESTAMP_ACCURACY;

typedef struct _CRYPT_TIMESTAMP_INFO {
  DWORD                      dwVersion;
  LPSTR                      pszTSAPolicyId;
  CRYPT_ALGORITHM_IDENTIFIER HashAlgorithm;
  CRYPT_DER_BLOB             HashedMessage;
  CRYPT_INTEGER_BLOB         SerialNumber;
  FILETIME                   ftTime;
  PCRYPT_TIMESTAMP_ACCURACY  pvAccuracy;
  BOOL                       fOrdering;
  CRYPT_DER_BLOB             Nonce;
  CRYPT_DER_BLOB             Tsa;
  DWORD                      cExtension;
  PCERT_EXTENSION            rgExtension;
} CRYPT_TIMESTAMP_INFO, *PCRYPT_TIMESTAMP_INFO;

typedef struct _CRYPT_TIMESTAMP_CONTEXT {
  DWORD                 cbEncoded;
  BYTE                  *pbEncoded;
  PCRYPT_TIMESTAMP_INFO pTimeStamp;
} CRYPT_TIMESTAMP_CONTEXT, *PCRYPT_TIMESTAMP_CONTEXT;

//Just returning False for stub
BOOL
WINAPI
CryptRetrieveTimeStamp(
  LPCWSTR                    wszUrl,
  DWORD                      dwRetrievalFlags,
  DWORD                      dwTimeout,
  LPCSTR                     pszHashId,
  const CRYPT_TIMESTAMP_PARA *pPara,
  const BYTE                 *pbData,
  DWORD                      cbData,
  PCRYPT_TIMESTAMP_CONTEXT   *ppTsContext,
  PCCERT_CONTEXT             *ppTsSigner,
  HCERTSTORE                 *phStore
)
{
	return FALSE;
}

BOOL
WINAPI
CryptVerifyTimeStampSignature(
  const BYTE               *pbTSContentInfo,
  DWORD                    cbTSContentInfo,
  const BYTE               *pbData,
  DWORD                    cbData,
  HCERTSTORE               hAdditionalStore,
  PCRYPT_TIMESTAMP_CONTEXT *ppTsContext,
  PCCERT_CONTEXT           *ppTsSigner,
  HCERTSTORE               *phStore
)
{
	return FALSE;
}

BOOL
WINAPI 
CryptExportPublicKeyInfoFromBCryptKeyHandle(
  BCRYPT_KEY_HANDLE hBCryptKey,
  DWORD dwCertEncodingType,
  LPSTR pszPublicKeyObjId,
  DWORD dwFlags,
  PVOID pvAuxInfo,
  PCERT_PUBLIC_KEY_INFO pInfo,
  DWORD pcbInfo
)
{
	return FALSE;
}

typedef BOOL (WINAPI *CertGetCertificateChain_t)(
    HCERTCHAINENGINE,
    PCCERT_CONTEXT,
    LPFILETIME,
    HCERTSTORE,
    PCERT_CHAIN_PARA,
    DWORD,
    LPVOID,
    PCCERT_CHAIN_CONTEXT *
);

BOOL 
WINAPI 
CertGetCertificateChain(
	HCERTCHAINENGINE hChainEngine,
	PCCERT_CONTEXT pCertContext, 
	LPFILETIME pTime, 
	HCERTSTORE hAdditionalStore,
	PCERT_CHAIN_PARA pChainPara, 
	DWORD dwFlags, 
	LPVOID pvReserved,
	PCCERT_CHAIN_CONTEXT* ppChainContext
	)
{
    HMODULE hCrypt32;
	BOOL resp;
	//PCCERT_CHAIN_CONTEXT pChainContext;
    char path[MAX_PATH];
    char *filename;
	CertGetCertificateChain_t pCertGetCertificateChain;

    // Obtém o caminho completo do executável atual
    if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0) {
        return 1;
    }

    // Extrai apenas o nome do arquivo (sem o caminho)
    filename = strrchr(path, '\\');
    if (filename != NULL) {
        filename++;  // avança para o nome do executável
    } else {
        filename = path;  // não encontrou barra invertida, usa a string completa
    }

    // Verifica se é explorer.exe (case-insensitive)
    if ((_stricmp(filename, "explorer.exe") == 0) || _stricmp(filename, "taskmgr.exe") == 0) {
		hCrypt32 = LoadLibraryA("cryptcore.dll");
		pCertGetCertificateChain = (CertGetCertificateChain_t)GetProcAddress(hCrypt32, "CertGetCertificateChain");
    } else {
		hCrypt32 = LoadLibraryA("crypt32.dll");
		pCertGetCertificateChain = (CertGetCertificateChain_t)GetProcAddress(hCrypt32, "CertGetCertificateChainNative");
    }			
		
	resp = pCertGetCertificateChain(hChainEngine,
									pCertContext,
									pTime,
									hAdditionalStore,
									pChainPara,
									dwFlags,
									pvReserved,
									ppChainContext);
										
	// pChainContext = *ppChainContext;

    // DbgPrint("CertGetCertificateChain:: ppChainContext size is : %d\n", pChainContext->cbSize);	
    // DbgPrint("CertGetCertificateChain:: ppChainContext TrustStatus->dwErrorStatus is : %d\n", pChainContext->TrustStatus.dwErrorStatus);	
    // DbgPrint("CertGetCertificateChain:: ppChainContext TrustStatus->dwInfoStatus is : %d\n", pChainContext->TrustStatus.dwErrorStatus);	
									
	// DbgPrint("CertGetCertificateChain:: the return is: %d\n", resp);
	// DbgPrint("CertGetCertificateChain:: LastError value: 0x%X\n", GetLastError());									
									
	// // if(GetLastError() == 0x5){
			// // DbgPrint("CertGetCertificateChain:: we get a error, try again\n");
			// hCrypt32 = LoadLibraryA("crypt32.dll");
			// pCertGetCertificateChain = (CertGetCertificateChain_t)GetProcAddress(hCrypt32, "CertGetCertificateChainNative");	
			
			// resp = pCertGetCertificateChain(hChainEngine,
											// pCertContext,
											// pTime,
											// hAdditionalStore,
											// pChainPara,
											// dwFlags,
											// pvReserved,
											// ppChainContext);
											
			// DbgPrint("CertGetCertificateChain:: the return is: %d\n", resp);
			// DbgPrint("CertGetCertificateChain:: LastError value: 0x%X\n", GetLastError());											

			// FreeLibrary(hCrypt32);									
			
			// return resp;
	// }
	
	FreeLibrary(hCrypt32);
	
	return resp;
}
