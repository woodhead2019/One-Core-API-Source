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
//#include "olestd.h"

#include "wine/list.h"

#include "windef.h"
#include "winbase.h"
#include "wtypes.h"
//#include "dcom_p.h"
#include <dcom.h>
#include "winreg.h"
#include "wine/winternl.h"
#include "wine/debug.h"
#include "wine/exception.h"
#include "servprov.h"

#define APTTYPEQUALIFIER_APPLICATION_STA 6
#define APTTYPEQUALIFIER_RESERVED_1 7

#define IRPCSS_PROTSEQ {'n','c','a','l','r','p','c',0}
#define IRPCSS_ENDPOINT {'i','r','p','c','s','s',0}

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

#define RPCSS_CALL_START \
    HRESULT hr; \
    for (;;) { \
        __TRY {

#define RPCSS_CALL_END \
        } __EXCEPT(rpc_filter) { \
            hr = HRESULT_FROM_WIN32(GetExceptionCode()); \
        } \
        __ENDTRY \
        if (hr == HRESULT_FROM_WIN32(RPC_S_SERVER_UNAVAILABLE)) { \
            if (start_rpcss()) \
                continue; \
        } \
        break; \
    } \
    return hr;

/* private flag indicating that the caller does not want to notify the stub
 * when the proxy disconnects or is destroyed */
#define SORFP_NOLIFETIMEMGMT SORF_OXRES2


/* Since Visual Studio 2012, volatile accesses do not always imply acquire and
 * release semantics.  We explicitly use ISO volatile semantics, manually
 * placing barriers as appropriate.
 */
#define __WINE_LOAD32_NO_FENCE(src) (*(src))

#define COWAIT_DISPATCH_CALLS 0x8
#define COWAIT_DISPATCH_WINDOW_MESSAGES 0x10