
#include "precomp.h"

SECURITY_STATUS WINAPI schan_AcceptSecurityContext(
 PCredHandle phCredential, PCtxtHandle phContext, PSecBufferDesc pInput,
 ULONG fContextReq, ULONG TargetDataRep, PCtxtHandle phNewContext,
 PSecBufferDesc pOutput, ULONG *pfContextAttr, PTimeStamp ptsExpiry)
{
    return SEC_E_UNSUPPORTED_FUNCTION;
}

SECURITY_STATUS WINAPI schan_ApplyControlToken(PCtxtHandle phContext,
 PSecBufferDesc pInput)
{
    return SEC_E_UNSUPPORTED_FUNCTION;
}

SECURITY_STATUS WINAPI schan_CompleteAuthToken(PCtxtHandle phContext,
 PSecBufferDesc pToken)
{
    return SEC_E_UNSUPPORTED_FUNCTION;
}

SECURITY_STATUS WINAPI schan_ImpersonateSecurityContext(PCtxtHandle phContext)
{
    return SEC_E_UNSUPPORTED_FUNCTION;
}


#pragma comment(linker, "/EXPORT:CloseSslPerformanceData=schannelbase.CloseSslPerformanceData")
#pragma comment(linker, "/EXPORT:CollectSslPerformanceData=schannelbase.CollectSslPerformanceData")
#pragma comment(linker, "/EXPORT:OpenSslPerformanceData=schannelbase.OpenSslPerformanceData")
#pragma comment(linker, "/EXPORT:SslCrackCertificate=schannelbase.SslCrackCertificate")
#pragma comment(linker, "/EXPORT:SslFreeCertificate=schannelbase.SslFreeCertificate")
#pragma comment(linker, "/EXPORT:SslGenerateKeyPair=schannelbase.SslGenerateKeyPair")
#pragma comment(linker, "/EXPORT:SslGenerateRandomBits=schannelbase.SslGenerateRandomBits")
#pragma comment(linker, "/EXPORT:SslGetMaximumKeySize=schannelbase.SslGetMaximumKeySize")
#pragma comment(linker, "/EXPORT:SslLoadCertificate=schannelbase.SslLoadCertificate")
#pragma comment(linker, "/EXPORT:SpLsaModeInitialize=schannelbase.SpLsaModeInitialize")
#pragma comment(linker, "/EXPORT:SpUserModeInitialize=schannelbase.SpUserModeInitialize")
#pragma comment(linker, "/EXPORT:SslEmptyCacheA=schannelbase.SslEmptyCacheA")
#pragma comment(linker, "/EXPORT:SslEmptyCacheW=schannelbase.SslEmptyCacheW")