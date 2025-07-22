#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define COBJMACROS
#define NONAMELESSUNION

#include "windef.h"
#include "winbase.h"
#include "winerror.h"
#include "wingdi.h"
#include "winuser.h"
#include "winnls.h"
#include "winreg.h"
#include "ole2.h"
#include "ole2ver.h"

#include "wine/unicode.h"
#include "wine/list.h"

#include "windef.h"
#include "winbase.h"
#include "wtypes.h"
#include "winreg.h"
#include "wine/winternl.h"
#include "wine/debug.h"
#include "wine/exception.h"
#include "servprov.h"
#include "combaseapi.h"
#include "hstring.h"
#include "activation.h"
#include "winstring.h"

typedef PVOID* PPVOID;

typedef UNICODE_STRING IUri;

//Hack
typedef PVOID IAsyncOperation;

typedef struct _IUriRuntimeClass IUriRuntimeClass;

typedef struct _IUriRuntimeClassVtbl {
	// IUnknown
	HRESULT (STDMETHODCALLTYPE *QueryInterface) (IUriRuntimeClass *, REFIID, PPVOID);
	ULONG (STDMETHODCALLTYPE *AddRef) (IUriRuntimeClass *);
	ULONG (STDMETHODCALLTYPE *Release) (IUriRuntimeClass *);
	
	// IInspectable
	HRESULT (STDMETHODCALLTYPE *GetIids) (IUriRuntimeClass *, PULONG, IID **);
	HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *GetTrustLevel) (IUriRuntimeClass *, TrustLevel *);

	// IUriRuntimeClass
	HRESULT (STDMETHODCALLTYPE *get_AbsoluteUri) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *get_DisplayUri) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *get_Domain) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *get_Extension) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *get_Fragment) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *get_Host) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *get_Password) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *get_Path) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *get_Query) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *get_QueryParsed) (IUriRuntimeClass *, IUnknown **); // actually IWwwFormUrlDecoderRuntimeClass **
	HRESULT (STDMETHODCALLTYPE *get_RawUri) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *get_SchemeName) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *get_UserName) (IUriRuntimeClass *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *get_Port) (IUriRuntimeClass *, PULONG);
	HRESULT (STDMETHODCALLTYPE *get_Suspicious) (IUriRuntimeClass *, PBOOLEAN);
	HRESULT (STDMETHODCALLTYPE *Equals) (IUriRuntimeClass *, IUriRuntimeClass *, PBOOLEAN);
	HRESULT (STDMETHODCALLTYPE *CombineUri) (IUriRuntimeClass *, HSTRING, IUriRuntimeClass **);
} IUriRuntimeClassVtbl;

typedef struct _IUriRuntimeClass {
	IUriRuntimeClassVtbl	*lpVtbl;
	LONG					RefCount;
	IUri					*Uri;
} IUriRuntimeClass;

typedef struct _IUriRuntimeClassFactory IUriRuntimeClassFactory;

typedef struct _IUriRuntimeClassFactoryVtbl {
	// IUnknown
	HRESULT (STDMETHODCALLTYPE *QueryInterface) (IUriRuntimeClassFactory *, REFIID, PPVOID);
	ULONG (STDMETHODCALLTYPE *AddRef) (IUriRuntimeClassFactory *);
	ULONG (STDMETHODCALLTYPE *Release) (IUriRuntimeClassFactory *);
	
	// IInspectable
	HRESULT (STDMETHODCALLTYPE *GetIids) (IUriRuntimeClassFactory *, PULONG, IID **);
	HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName) (IUriRuntimeClassFactory *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *GetTrustLevel) (IUriRuntimeClassFactory *, TrustLevel *);

	// IUriRuntimeClassFactory
	HRESULT (STDMETHODCALLTYPE *CreateUri) (IUriRuntimeClassFactory *, HSTRING, IUriRuntimeClass **);
	HRESULT (STDMETHODCALLTYPE *CreateWithRelativeUri) (IUriRuntimeClassFactory *, HSTRING, HSTRING, IUriRuntimeClass **);
} IUriRuntimeClassFactoryVtbl;

typedef struct _IUriRuntimeClassFactory {
	IUriRuntimeClassFactoryVtbl *lpVtbl;
} IUriRuntimeClassFactory;

extern IUriRuntimeClassFactory CUriRuntimeClassFactory;

typedef struct _ILauncherStatics ILauncherStatics;

typedef struct _ILauncherStaticsVtbl {
	// IUnknown
	HRESULT (STDMETHODCALLTYPE *QueryInterface) (ILauncherStatics *, REFIID, PVOID*);
	ULONG (STDMETHODCALLTYPE *AddRef) (ILauncherStatics *);
	ULONG (STDMETHODCALLTYPE *Release) (ILauncherStatics *);
	
	// IInspectable
	HRESULT (STDMETHODCALLTYPE *GetIids) (ILauncherStatics *, PULONG, IID **);
	HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName) (ILauncherStatics *, HSTRING *);
	HRESULT (STDMETHODCALLTYPE *GetTrustLevel) (ILauncherStatics *, TrustLevel *);

	// ILauncherStatics
	HRESULT (STDMETHODCALLTYPE *LaunchFileAsync) (ILauncherStatics *, IUnknown *, IAsyncOperation **);
	HRESULT (STDMETHODCALLTYPE *LaunchFileWithOptionsAsync) (ILauncherStatics *, IUnknown *, IUnknown *, IAsyncOperation **);
	HRESULT (STDMETHODCALLTYPE *LaunchUriAsync) (ILauncherStatics *, IUriRuntimeClass *, IAsyncOperation **);
	HRESULT (STDMETHODCALLTYPE *LaunchUriWithOptionsAsync) (ILauncherStatics *, IUriRuntimeClass *, IUnknown *, IAsyncOperation **);
} ILauncherStaticsVtbl;

typedef struct _ILauncherStatics {
	ILauncherStaticsVtbl *lpVtbl;
} ILauncherStatics;

extern ILauncherStatics CLauncherStatics;