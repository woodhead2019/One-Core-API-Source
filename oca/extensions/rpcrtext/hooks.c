/*
 * Copyright 2009 Henri Verbeet for CodeWeavers
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
 *
 */

#include <wine/config.h>

#pragma warning( disable : 4005 )

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "windef.h"
#include "winbase.h"
#include "winerror.h"

#include "objbase.h"
#include "rpc.h"
#include "rpcproxy.h"

#include "wine/exception.h"
#include "wine/asm.h"
#include "wine/debug.h"

//#include "cpsf.h"
#include "ndr_misc.h"
#include "ndr_stubless.h"

WINE_DEFAULT_DEBUG_CHANNEL(rpc);

#define NDR_TABLE_MASK 127
#define NDR_TABLE_SIZE 128

#define ARG_FROM_OFFSET(args, offset) ((args) + (offset))

static unsigned char *WINAPI NdrBaseTypeMarshall(PMIDL_STUB_MESSAGE, unsigned char *, PFORMAT_STRING);
static unsigned char *WINAPI NdrBaseTypeUnmarshall(PMIDL_STUB_MESSAGE, unsigned char **, PFORMAT_STRING, unsigned char);
static void WINAPI NdrBaseTypeBufferSize(PMIDL_STUB_MESSAGE, unsigned char *, PFORMAT_STRING);
static void WINAPI NdrBaseTypeFree(PMIDL_STUB_MESSAGE, unsigned char *, PFORMAT_STRING);
static ULONG WINAPI NdrBaseTypeMemorySize(PMIDL_STUB_MESSAGE, PFORMAT_STRING);

static unsigned char *WINAPI NdrContextHandleMarshall(PMIDL_STUB_MESSAGE, unsigned char *, PFORMAT_STRING);
static void WINAPI NdrContextHandleBufferSize(PMIDL_STUB_MESSAGE, unsigned char *, PFORMAT_STRING);
static unsigned char *WINAPI NdrContextHandleUnmarshall(PMIDL_STUB_MESSAGE, unsigned char **, PFORMAT_STRING, unsigned char);

static unsigned char *WINAPI NdrRangeMarshall(PMIDL_STUB_MESSAGE,unsigned char *, PFORMAT_STRING);
static void WINAPI NdrRangeBufferSize(PMIDL_STUB_MESSAGE, unsigned char *, PFORMAT_STRING);
static ULONG WINAPI NdrRangeMemorySize(PMIDL_STUB_MESSAGE, PFORMAT_STRING);
static void WINAPI NdrRangeFree(PMIDL_STUB_MESSAGE, unsigned char *, PFORMAT_STRING);

static ULONG WINAPI NdrByteCountPointerMemorySize(PMIDL_STUB_MESSAGE, PFORMAT_STRING);

struct ndr_client_call_ctx
{
    MIDL_STUB_MESSAGE *stub_msg;
    INTERPRETER_OPT_FLAGS Oif_flags;
    INTERPRETER_OPT_FLAGS2 ext_flags;
    const NDR_PROC_HEADER *proc_header;
    void *This;
    PFORMAT_STRING handle_format;
    handle_t hbinding;
};

/* Complex types */

#include "pshpack1.h"
typedef struct
{
    unsigned char type;
    unsigned char flags_type; /* flags in upper nibble, type in lower nibble */
    ULONG low_value;
    ULONG high_value;
} NDR_RANGE;
#include "poppack.h"

ULONG DbgPrint(
  PCSTR Format,
  ...   
);

/* copies data to the buffer, checking that there is enough space to do so */
static inline void safe_copy_to_buffer(MIDL_STUB_MESSAGE *pStubMsg, const void *p, ULONG size)
{
    if ((pStubMsg->Buffer + size < pStubMsg->Buffer) || /* integer overflow of pStubMsg->Buffer */
        (pStubMsg->Buffer + size > (unsigned char *)pStubMsg->RpcMsg->Buffer + pStubMsg->BufferLength))
    {
        ERR("buffer overflow - Buffer = %p, BufferEnd = %p, size = %u\n",
            pStubMsg->Buffer, (unsigned char *)pStubMsg->RpcMsg->Buffer + pStubMsg->BufferLength,
            size);
        RpcRaiseException(RPC_X_BAD_STUB_DATA);
    }
    memcpy(pStubMsg->Buffer, p, size);
    pStubMsg->Buffer += size;
}

static inline void align_pointer_clear( unsigned char **ptr, unsigned int align )
{
    ULONG_PTR mask = align - 1;
    memset( *ptr, 0, (align - (ULONG_PTR)*ptr) & mask );
    *ptr = (unsigned char *)(((ULONG_PTR)*ptr + mask) & ~mask);
}

static inline void align_pointer( unsigned char **ptr, unsigned int align )
{
    ULONG_PTR mask = align - 1;
    *ptr = (unsigned char *)(((ULONG_PTR)*ptr + mask) & ~mask);
}

static inline void safe_buffer_increment(MIDL_STUB_MESSAGE *pStubMsg, ULONG size)
{
    if ((pStubMsg->Buffer + size < pStubMsg->Buffer) || /* integer overflow of pStubMsg->Buffer */
        (pStubMsg->Buffer + size > (unsigned char *)pStubMsg->RpcMsg->Buffer + pStubMsg->BufferLength))
        RpcRaiseException(RPC_X_BAD_STUB_DATA);
    pStubMsg->Buffer += size;
}

/* copies data from the buffer, checking that there is enough data in the buffer
 * to do so */
static inline void safe_copy_from_buffer(MIDL_STUB_MESSAGE *pStubMsg, void *p, ULONG size)
{
    if ((pStubMsg->Buffer + size < pStubMsg->Buffer) || /* integer overflow of pStubMsg->Buffer */
        (pStubMsg->Buffer + size > pStubMsg->BufferEnd))
    {
        ERR("buffer overflow - Buffer = %p, BufferEnd = %p, size = %u\n",
            pStubMsg->Buffer, pStubMsg->BufferEnd, size);
        RpcRaiseException(RPC_X_BAD_STUB_DATA);
    }
    if (p == pStubMsg->Buffer)
        ERR("pointer is the same as the buffer\n");
    memcpy(p, pStubMsg->Buffer, size);
    pStubMsg->Buffer += size;
}

static inline void safe_buffer_length_increment(MIDL_STUB_MESSAGE *pStubMsg, ULONG size)
{
    if (pStubMsg->BufferLength + size < pStubMsg->BufferLength) /* integer overflow of pStubMsg->BufferSize */
    {
        ERR("buffer length overflow - BufferLength = %u, size = %u\n",
            pStubMsg->BufferLength, size);
        RpcRaiseException(RPC_X_BAD_STUB_DATA);
    }
    pStubMsg->BufferLength += size;
}


static BOOL is_by_value( PFORMAT_STRING format )
{
    switch (*format)
    {
    case FC_USER_MARSHAL:
    case FC_STRUCT:
    case FC_PSTRUCT:
    case FC_CSTRUCT:
    case FC_CPSTRUCT:
    case FC_CVSTRUCT:
    case FC_BOGUS_STRUCT:
        return TRUE;
    default:
        return FALSE;
    }
}

static inline BOOL IsConformanceOrVariancePresent(PFORMAT_STRING pFormat)
{
    return (*(const ULONG *)pFormat != -1);
}

static inline void align_length( ULONG *len, unsigned int align )
{
    *len = (*len + align - 1) & ~(align - 1);
}

static inline PFORMAT_STRING SkipConformance(const PMIDL_STUB_MESSAGE pStubMsg, const PFORMAT_STRING pFormat)
{
    return pFormat + 4 + pStubMsg->CorrDespIncrement;
}

static inline void call_buffer_sizer(PMIDL_STUB_MESSAGE pStubMsg, unsigned char *pMemory,
                                     const NDR_PARAM_OIF *param)
{
    PFORMAT_STRING pFormat;
    NDR_BUFFERSIZE m;

    if (param->attr.IsBasetype)
    {
        pFormat = &param->u.type_format_char;
        if (param->attr.IsSimpleRef) pMemory = *(unsigned char **)pMemory;
    }
    else
    {
        pFormat = &pStubMsg->StubDesc->pFormatTypes[param->u.type_offset];
        if (!param->attr.IsByValue) pMemory = *(unsigned char **)pMemory;
    }

    m = NdrBufferSizer[pFormat[0] & NDR_TABLE_MASK];
    if (m) m(pStubMsg, pMemory, pFormat);
    else
    {
        FIXME("format type 0x%x not implemented\n", pFormat[0]);
        RpcRaiseException(RPC_X_BAD_STUB_DATA);
    }
}

static inline BOOL is_oicf_stubdesc(const PMIDL_STUB_DESC pStubDesc)
{
    return pStubDesc->Version >= 0x20000;
}

static size_t get_handle_desc_size(const NDR_PROC_HEADER *proc_header, PFORMAT_STRING format)
{
    if (!proc_header->handle_type)
    {
        if (*format == FC_BIND_PRIMITIVE)
            return sizeof(NDR_EHD_PRIMITIVE);
        else if (*format == FC_BIND_GENERIC)
            return sizeof(NDR_EHD_GENERIC);
        else if (*format == FC_BIND_CONTEXT)
            return sizeof(NDR_EHD_CONTEXT);
    }
    return 0;
}

const NDR_MARSHALL NdrMarshaller[NDR_TABLE_SIZE] = {
  0,
  NdrBaseTypeMarshall, NdrBaseTypeMarshall, NdrBaseTypeMarshall,
  NdrBaseTypeMarshall, NdrBaseTypeMarshall, NdrBaseTypeMarshall, NdrBaseTypeMarshall,
  NdrBaseTypeMarshall, NdrBaseTypeMarshall, NdrBaseTypeMarshall, NdrBaseTypeMarshall,
  NdrBaseTypeMarshall, NdrBaseTypeMarshall, NdrBaseTypeMarshall, NdrBaseTypeMarshall,
  /* 0x10 */
  NdrBaseTypeMarshall,
  /* 0x11 */
  NdrPointerMarshall, NdrPointerMarshall,
  NdrPointerMarshall, NdrPointerMarshall,
  /* 0x15 */
  NdrSimpleStructMarshall, NdrSimpleStructMarshall,
  NdrConformantStructMarshall, NdrConformantStructMarshall,
  NdrConformantVaryingStructMarshall,
  NdrComplexStructMarshall,
  /* 0x1b */
  NdrConformantArrayMarshall, 
  NdrConformantVaryingArrayMarshall,
  NdrFixedArrayMarshall, NdrFixedArrayMarshall,
  NdrVaryingArrayMarshall, NdrVaryingArrayMarshall,
  NdrComplexArrayMarshall,
  /* 0x22 */
  NdrConformantStringMarshall, 0, 0,
  NdrConformantStringMarshall,
  NdrNonConformantStringMarshall, 0, 0, 0,
  /* 0x2a */
  NdrEncapsulatedUnionMarshall,
  NdrNonEncapsulatedUnionMarshall,
  NdrByteCountPointerMarshall,
  NdrXmitOrRepAsMarshall, NdrXmitOrRepAsMarshall,
  /* 0x2f */
  NdrInterfacePointerMarshall,
  /* 0x30 */
  NdrContextHandleMarshall,
  /* 0xb1 */
  0, 0, 0,
  NdrUserMarshalMarshall,
  0, 0,
  /* 0xb7 */
  NdrRangeMarshall,
  NdrBaseTypeMarshall,
  NdrBaseTypeMarshall
};
const NDR_UNMARSHALL NdrUnmarshaller[NDR_TABLE_SIZE] = {
  0,
  NdrBaseTypeUnmarshall, NdrBaseTypeUnmarshall, NdrBaseTypeUnmarshall,
  NdrBaseTypeUnmarshall, NdrBaseTypeUnmarshall, NdrBaseTypeUnmarshall, NdrBaseTypeUnmarshall,
  NdrBaseTypeUnmarshall, NdrBaseTypeUnmarshall, NdrBaseTypeUnmarshall, NdrBaseTypeUnmarshall,
  NdrBaseTypeUnmarshall, NdrBaseTypeUnmarshall, NdrBaseTypeUnmarshall, NdrBaseTypeUnmarshall,
  /* 0x10 */
  NdrBaseTypeUnmarshall,
  /* 0x11 */
  NdrPointerUnmarshall, NdrPointerUnmarshall,
  NdrPointerUnmarshall, NdrPointerUnmarshall,
  /* 0x15 */
  NdrSimpleStructUnmarshall, NdrSimpleStructUnmarshall,
  NdrConformantStructUnmarshall, NdrConformantStructUnmarshall,
  NdrConformantVaryingStructUnmarshall,
  NdrComplexStructUnmarshall,
  /* 0x1b */
  NdrConformantArrayUnmarshall, 
  NdrConformantVaryingArrayUnmarshall,
  NdrFixedArrayUnmarshall, NdrFixedArrayUnmarshall,
  NdrVaryingArrayUnmarshall, NdrVaryingArrayUnmarshall,
  NdrComplexArrayUnmarshall,
  /* 0x22 */
  NdrConformantStringUnmarshall, 0, 0,
  NdrConformantStringUnmarshall,
  NdrNonConformantStringUnmarshall, 0, 0, 0,
  /* 0x2a */
  NdrEncapsulatedUnionUnmarshall,
  NdrNonEncapsulatedUnionUnmarshall,
  NdrByteCountPointerUnmarshall,
  NdrXmitOrRepAsUnmarshall, NdrXmitOrRepAsUnmarshall,
  /* 0x2f */
  NdrInterfacePointerUnmarshall,
  /* 0x30 */
  NdrContextHandleUnmarshall,
  /* 0xb1 */
  0, 0, 0,
  NdrUserMarshalUnmarshall,
  0, 0,
  /* 0xb7 */
  NdrRangeUnmarshall,
  NdrBaseTypeUnmarshall,
  NdrBaseTypeUnmarshall
};
const NDR_BUFFERSIZE NdrBufferSizer[NDR_TABLE_SIZE] = {
  0,
  NdrBaseTypeBufferSize, NdrBaseTypeBufferSize, NdrBaseTypeBufferSize,
  NdrBaseTypeBufferSize, NdrBaseTypeBufferSize, NdrBaseTypeBufferSize, NdrBaseTypeBufferSize,
  NdrBaseTypeBufferSize, NdrBaseTypeBufferSize, NdrBaseTypeBufferSize, NdrBaseTypeBufferSize,
  NdrBaseTypeBufferSize, NdrBaseTypeBufferSize, NdrBaseTypeBufferSize, NdrBaseTypeBufferSize,
  /* 0x10 */
  NdrBaseTypeBufferSize,
  /* 0x11 */
  NdrPointerBufferSize, NdrPointerBufferSize,
  NdrPointerBufferSize, NdrPointerBufferSize,
  /* 0x15 */
  NdrSimpleStructBufferSize, NdrSimpleStructBufferSize,
  NdrConformantStructBufferSize, NdrConformantStructBufferSize,
  NdrConformantVaryingStructBufferSize,
  NdrComplexStructBufferSize,
  /* 0x1b */
  NdrConformantArrayBufferSize, 
  NdrConformantVaryingArrayBufferSize,
  NdrFixedArrayBufferSize, NdrFixedArrayBufferSize,
  NdrVaryingArrayBufferSize, NdrVaryingArrayBufferSize,
  NdrComplexArrayBufferSize,
  /* 0x22 */
  NdrConformantStringBufferSize, 0, 0,
  NdrConformantStringBufferSize,
  NdrNonConformantStringBufferSize, 0, 0, 0,
  /* 0x2a */
  NdrEncapsulatedUnionBufferSize,
  NdrNonEncapsulatedUnionBufferSize,
  NdrByteCountPointerBufferSize,
  NdrXmitOrRepAsBufferSize, NdrXmitOrRepAsBufferSize,
  /* 0x2f */
  NdrInterfacePointerBufferSize,
  /* 0x30 */
  NdrContextHandleBufferSize,
  /* 0xb1 */
  0, 0, 0,
  NdrUserMarshalBufferSize,
  0, 0,
  /* 0xb7 */
  NdrRangeBufferSize,
  NdrBaseTypeBufferSize,
  NdrBaseTypeBufferSize
};

static const char *debugstr_INTERPRETER_OPT_FLAGS(INTERPRETER_OPT_FLAGS Oi2Flags)
{
    char buffer[160];

    buffer[0] = 0;
    if (Oi2Flags.ServerMustSize) strcat(buffer, " ServerMustSize");
    if (Oi2Flags.ClientMustSize) strcat(buffer, " ClientMustSize");
    if (Oi2Flags.HasReturn) strcat(buffer, " HasReturn");
    if (Oi2Flags.HasPipes) strcat(buffer, " HasPipes");
    if (Oi2Flags.Unused) strcat(buffer, " Unused");
    if (Oi2Flags.HasAsyncUuid) strcat(buffer, " HasAsyncUuid");
    if (Oi2Flags.HasExtensions) strcat(buffer, " HasExtensions");
    if (Oi2Flags.HasAsyncHandle) strcat(buffer, " HasAsyncHandle");
    return buffer[0] ? wine_dbg_sprintf( "%s", buffer + 1 ) : "";
}

static const char *debugstr_PROC_PF(PARAM_ATTRIBUTES param_attributes)
{
    char buffer[160];

    buffer[0] = 0;
    if (param_attributes.MustSize) strcat(buffer, " MustSize");
    if (param_attributes.MustFree) strcat(buffer, " MustFree");
    if (param_attributes.IsPipe) strcat(buffer, " IsPipe");
    if (param_attributes.IsIn) strcat(buffer, " IsIn");
    if (param_attributes.IsOut) strcat(buffer, " IsOut");
    if (param_attributes.IsReturn) strcat(buffer, " IsReturn");
    if (param_attributes.IsBasetype) strcat(buffer, " IsBasetype");
    if (param_attributes.IsByValue) strcat(buffer, " IsByValue");
    if (param_attributes.IsSimpleRef) strcat(buffer, " IsSimpleRef");
    if (param_attributes.IsDontCallFreeInst) strcat(buffer, " IsDontCallFreeInst");
    if (param_attributes.SaveForAsyncFinish) strcat(buffer, " SaveForAsyncFinish");
    if (param_attributes.ServerAllocSize)
        sprintf( buffer + strlen(buffer), " ServerAllocSize = %d", param_attributes.ServerAllocSize * 8);
    return buffer[0] ? wine_dbg_sprintf( "%s", buffer + 1 ) : "";
}

static inline BOOL param_needs_alloc( PARAM_ATTRIBUTES attr )
{
    return attr.IsOut && !attr.IsIn && !attr.IsBasetype && !attr.IsByValue;
}

static inline BOOL param_is_out_basetype( PARAM_ATTRIBUTES attr )
{
    return attr.IsOut && !attr.IsIn && attr.IsBasetype && attr.IsSimpleRef;
}

static size_t basetype_arg_size( unsigned char fc )
{
    switch (fc)
    {
    case FC_BYTE:
    case FC_CHAR:
    case FC_SMALL:
    case FC_USMALL:
        return sizeof(char);
    case FC_WCHAR:
    case FC_SHORT:
    case FC_USHORT:
        return sizeof(short);
    case FC_LONG:
    case FC_ULONG:
    case FC_ENUM16:
    case FC_ENUM32:
    case FC_ERROR_STATUS_T:
        return sizeof(int);
    case FC_FLOAT:
        return sizeof(float);
    case FC_HYPER:
        return sizeof(LONGLONG);
    case FC_DOUBLE:
        return sizeof(double);
    case FC_INT3264:
    case FC_UINT3264:
        return sizeof(INT_PTR);
    default:
        FIXME("Unhandled basetype %#x.\n", fc);
        return 0;
    }
}

static DWORD calc_arg_size(MIDL_STUB_MESSAGE *pStubMsg, PFORMAT_STRING pFormat)
{
    DWORD size;
    switch(*pFormat)
    {
    case FC_RP:
        if (pFormat[1] & FC_SIMPLE_POINTER)
        {
            size = 0;
            break;
        }
        size = calc_arg_size(pStubMsg, &pFormat[2] + *(const SHORT*)&pFormat[2]);
        break;
    case FC_STRUCT:
    case FC_PSTRUCT:
        size = *(const WORD*)(pFormat + 2);
        break;
    case FC_BOGUS_STRUCT:
        size = *(const WORD*)(pFormat + 2);
        if(*(const WORD*)(pFormat + 4))
            FIXME("Unhandled conformant description\n");
        break;
    case FC_CARRAY:
    case FC_CVARRAY:
        size = *(const WORD*)(pFormat + 2);
        ComputeConformance(pStubMsg, NULL, pFormat + 4, 0);
        size *= pStubMsg->MaxCount;
        break;
    case FC_SMFARRAY:
    case FC_SMVARRAY:
        size = *(const WORD*)(pFormat + 2);
        break;
    case FC_LGFARRAY:
    case FC_LGVARRAY:
        size = *(const DWORD*)(pFormat + 2);
        break;
    case FC_BOGUS_ARRAY:
        pFormat = ComputeConformance(pStubMsg, NULL, pFormat + 4, *(const WORD*)&pFormat[2]);
        TRACE("conformance = %ld\n", pStubMsg->MaxCount);
        pFormat = ComputeVariance(pStubMsg, NULL, pFormat, pStubMsg->MaxCount);
        size = ComplexStructSize(pStubMsg, pFormat);
        size *= pStubMsg->MaxCount;
        break;
    case FC_USER_MARSHAL:
        size = *(const WORD*)(pFormat + 4);
        break;
    case FC_CSTRING:
        size = *(const WORD*)(pFormat + 2);
        break;
    case FC_WSTRING:
        size = *(const WORD*)(pFormat + 2) * sizeof(WCHAR);
        break;
    case FC_C_CSTRING:
    case FC_C_WSTRING:
        if (*pFormat == FC_C_CSTRING)
            size = sizeof(CHAR);
        else
            size = sizeof(WCHAR);
        if (pFormat[1] == FC_STRING_SIZED)
            ComputeConformance(pStubMsg, NULL, pFormat + 2, 0);
        else
            pStubMsg->MaxCount = 0;
        size *= pStubMsg->MaxCount;
        break;
    default:
        FIXME("Unhandled type %02x\n", *pFormat);
        /* fallthrough */
    case FC_UP:
    case FC_OP:
    case FC_FP:
    case FC_IP:
        size = sizeof(void *);
        break;
    }
    return size;
}

static inline unsigned char *call_marshaller(PMIDL_STUB_MESSAGE pStubMsg, unsigned char *pMemory,
                                             const NDR_PARAM_OIF *param)
{
    PFORMAT_STRING pFormat;
    NDR_MARSHALL m;

    if (param->attr.IsBasetype)
    {
        pFormat = &param->u.type_format_char;
        if (param->attr.IsSimpleRef) pMemory = *(unsigned char **)pMemory;
    }
    else
    {
        pFormat = &pStubMsg->StubDesc->pFormatTypes[param->u.type_offset];
        if (!param->attr.IsByValue) pMemory = *(unsigned char **)pMemory;
    }

    m = NdrMarshaller[pFormat[0] & NDR_TABLE_MASK];
    if (m) return m(pStubMsg, pMemory, pFormat);
    else
    {
        FIXME("format type 0x%x not implemented\n", pFormat[0]);
        RpcRaiseException(RPC_X_BAD_STUB_DATA);
        return NULL;
    }
}

static inline unsigned char *call_unmarshaller(PMIDL_STUB_MESSAGE pStubMsg, unsigned char **ppMemory,
                                               const NDR_PARAM_OIF *param, unsigned char fMustAlloc)
{
    PFORMAT_STRING pFormat;
    NDR_UNMARSHALL m;

    if (param->attr.IsBasetype)
    {
        pFormat = &param->u.type_format_char;
        if (param->attr.IsSimpleRef) ppMemory = (unsigned char **)*ppMemory;
    }
    else
    {
        pFormat = &pStubMsg->StubDesc->pFormatTypes[param->u.type_offset];
        if (!param->attr.IsByValue) ppMemory = (unsigned char **)*ppMemory;
    }

    m = NdrUnmarshaller[pFormat[0] & NDR_TABLE_MASK];
    if (m) return m(pStubMsg, ppMemory, pFormat, fMustAlloc);
    else
    {
        FIXME("format type 0x%x not implemented\n", pFormat[0]);
        RpcRaiseException(RPC_X_BAD_STUB_DATA);
        return NULL;
    }
}

void client_do_args( PMIDL_STUB_MESSAGE pStubMsg, PFORMAT_STRING pFormat, enum stubless_phase phase,
                     void **fpu_args, unsigned short number_of_params, unsigned char *pRetVal )
{
    const NDR_PARAM_OIF *params = (const NDR_PARAM_OIF *)pFormat;
    unsigned int i;

    for (i = 0; i < number_of_params; i++)
    {
        unsigned char *pArg = pStubMsg->StackTop + params[i].stack_offset;
        PFORMAT_STRING pTypeFormat = (PFORMAT_STRING)&pStubMsg->StubDesc->pFormatTypes[params[i].u.type_offset];

#ifdef __x86_64__  /* floats are passed as doubles through varargs functions */
        float f;

        if (params[i].attr.IsBasetype &&
            params[i].u.type_format_char == FC_FLOAT &&
            !params[i].attr.IsSimpleRef &&
            !fpu_args)
        {
            f = *(double *)pArg;
            pArg = (unsigned char *)&f;
        }
#endif

        TRACE("param[%d]: %p type %02x %s\n", i, pArg,
              params[i].attr.IsBasetype ? params[i].u.type_format_char : *pTypeFormat,
              debugstr_PROC_PF( params[i].attr ));

        switch (phase)
        {
        case STUBLESS_INITOUT:
            if (*(unsigned char **)pArg)
            {
                if (param_needs_alloc(params[i].attr))
                    memset( *(unsigned char **)pArg, 0, calc_arg_size( pStubMsg, pTypeFormat ));
                else if (param_is_out_basetype(params[i].attr))
                    memset( *(unsigned char **)pArg, 0, basetype_arg_size( params[i].u.type_format_char ));
            }
            break;
        case STUBLESS_CALCSIZE:
            if (params[i].attr.IsSimpleRef && !*(unsigned char **)pArg)
                RpcRaiseException(RPC_X_NULL_REF_POINTER);
            if (params[i].attr.IsIn) call_buffer_sizer(pStubMsg, pArg, &params[i]);
            break;
        case STUBLESS_MARSHAL:
            if (params[i].attr.IsIn) call_marshaller(pStubMsg, pArg, &params[i]);
            break;
        case STUBLESS_UNMARSHAL:
            if (params[i].attr.IsOut)
            {
                if (params[i].attr.IsReturn && pRetVal) pArg = pRetVal;
                call_unmarshaller(pStubMsg, &pArg, &params[i], 0);
            }
            break;
        case STUBLESS_FREE:
            if (!params[i].attr.IsBasetype && params[i].attr.IsOut && !params[i].attr.IsByValue)
                NdrClearOutParameters( pStubMsg, pTypeFormat, *(unsigned char **)pArg );
            break;
        default:
            RpcRaiseException(RPC_S_INTERNAL_ERROR);
        }
    }
}

static handle_t client_get_handle(const MIDL_STUB_MESSAGE *pStubMsg,
        const NDR_PROC_HEADER *pProcHeader, const PFORMAT_STRING pFormat)
{
    /* binding */
    switch (pProcHeader->handle_type)
    {
    /* explicit binding: parse additional section */
    case 0:
        switch (*pFormat) /* handle_type */
        {
        case FC_BIND_PRIMITIVE: /* explicit primitive */
            {
                const NDR_EHD_PRIMITIVE *pDesc = (const NDR_EHD_PRIMITIVE *)pFormat;

                TRACE("Explicit primitive handle @ %d\n", pDesc->offset);

                if (pDesc->flag) /* pointer to binding */
                    return **(handle_t **)ARG_FROM_OFFSET(pStubMsg->StackTop, pDesc->offset);
                else
                    return *(handle_t *)ARG_FROM_OFFSET(pStubMsg->StackTop, pDesc->offset);
            }
        case FC_BIND_GENERIC: /* explicit generic */
            {
                const NDR_EHD_GENERIC *pDesc = (const NDR_EHD_GENERIC *)pFormat;
                void *pObject = NULL;
                void *pArg;
                const GENERIC_BINDING_ROUTINE_PAIR *pGenPair;

                TRACE("Explicit generic binding handle #%d\n", pDesc->binding_routine_pair_index);

                if (pDesc->flag_and_size & HANDLE_PARAM_IS_VIA_PTR)
                    pArg = *(void **)ARG_FROM_OFFSET(pStubMsg->StackTop, pDesc->offset);
                else
                    pArg = ARG_FROM_OFFSET(pStubMsg->StackTop, pDesc->offset);
                memcpy(&pObject, pArg, pDesc->flag_and_size & 0xf);
                pGenPair = &pStubMsg->StubDesc->aGenericBindingRoutinePairs[pDesc->binding_routine_pair_index];
                return pGenPair->pfnBind(pObject);
            }
        case FC_BIND_CONTEXT: /* explicit context */
            {
                const NDR_EHD_CONTEXT *pDesc = (const NDR_EHD_CONTEXT *)pFormat;
                NDR_CCONTEXT context_handle;
                TRACE("Explicit bind context\n");
                if (pDesc->flags & HANDLE_PARAM_IS_VIA_PTR)
                {
                    TRACE("\tHANDLE_PARAM_IS_VIA_PTR\n");
                    context_handle = **(NDR_CCONTEXT **)ARG_FROM_OFFSET(pStubMsg->StackTop, pDesc->offset);
                }
                else
                    context_handle = *(NDR_CCONTEXT *)ARG_FROM_OFFSET(pStubMsg->StackTop, pDesc->offset);

                if (context_handle) return NDRCContextBinding(context_handle);
                else if (pDesc->flags & NDR_CONTEXT_HANDLE_CANNOT_BE_NULL)
                {
                    ERR("null context handle isn't allowed\n");
                    RpcRaiseException(RPC_X_SS_IN_NULL_CONTEXT);
                    return NULL;
                }
                /* FIXME: should we store this structure in stubMsg.pContext? */
            }
        default:
            ERR("bad explicit binding handle type (0x%02x)\n", pProcHeader->handle_type);
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
        }
        break;
    case FC_BIND_GENERIC: /* implicit generic */
        FIXME("FC_BIND_GENERIC\n");
        RpcRaiseException(RPC_X_BAD_STUB_DATA); /* FIXME: remove when implemented */
        break;
    case FC_BIND_PRIMITIVE: /* implicit primitive */
        TRACE("Implicit primitive handle\n");
        return *pStubMsg->StubDesc->IMPLICIT_HANDLE_INFO.pPrimitiveHandle;
    case FC_CALLBACK_HANDLE: /* implicit callback */
        TRACE("FC_CALLBACK_HANDLE\n");
        /* server calls callback procedures only in response to remote call, and most recent
           binding handle is used. Calling back to a client can potentially result in another
           callback with different current handle. */
        return I_RpcGetCurrentCallHandle();
    case FC_AUTO_HANDLE: /* implicit auto handle */
        /* strictly speaking, it isn't necessary to set hBinding here
         * since it isn't actually used (hence the automatic in its name),
         * but then why does MIDL generate a valid entry in the
         * MIDL_STUB_DESC for it? */
        TRACE("Implicit auto handle\n");
        return *pStubMsg->StubDesc->IMPLICIT_HANDLE_INFO.pAutoHandle;
    default:
        ERR("bad implicit binding handle type (0x%02x)\n", pProcHeader->handle_type);
        RpcRaiseException(RPC_X_BAD_STUB_DATA);
    }
    return NULL;
}

static void client_free_handle(
    PMIDL_STUB_MESSAGE pStubMsg, const NDR_PROC_HEADER *pProcHeader,
    PFORMAT_STRING pFormat, handle_t hBinding)
{
    /* binding */
    switch (pProcHeader->handle_type)
    {
    /* explicit binding: parse additional section */
    case 0:
        switch (*pFormat) /* handle_type */
        {
        case FC_BIND_GENERIC: /* explicit generic */
            {
                const NDR_EHD_GENERIC *pDesc = (const NDR_EHD_GENERIC *)pFormat;
                void *pObject = NULL;
                void *pArg;
                const GENERIC_BINDING_ROUTINE_PAIR *pGenPair;

                TRACE("Explicit generic binding handle #%d\n", pDesc->binding_routine_pair_index);

                if (pDesc->flag_and_size & HANDLE_PARAM_IS_VIA_PTR)
                    pArg = *(void **)ARG_FROM_OFFSET(pStubMsg->StackTop, pDesc->offset);
                else
                    pArg = ARG_FROM_OFFSET(pStubMsg->StackTop, pDesc->offset);
                memcpy(&pObject, pArg, pDesc->flag_and_size & 0xf);
                pGenPair = &pStubMsg->StubDesc->aGenericBindingRoutinePairs[pDesc->binding_routine_pair_index];
#ifdef __REACTOS__
                if (hBinding) pGenPair->pfnUnbind(pObject, hBinding);
#else
                pGenPair->pfnUnbind(pObject, hBinding);
#endif
                break;
            }
        case FC_BIND_CONTEXT: /* explicit context */
        case FC_BIND_PRIMITIVE: /* explicit primitive */
            break;
        default:
            ERR("bad explicit binding handle type (0x%02x)\n", pProcHeader->handle_type);
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
        }
        break;
    case FC_BIND_GENERIC: /* implicit generic */
        FIXME("FC_BIND_GENERIC\n");
        RpcRaiseException(RPC_X_BAD_STUB_DATA); /* FIXME: remove when implemented */
        break;
    case FC_CALLBACK_HANDLE: /* implicit callback */
    case FC_BIND_PRIMITIVE: /* implicit primitive */
    case FC_AUTO_HANDLE: /* implicit auto handle */
        break;
    default:
        ERR("bad implicit binding handle type (0x%02x)\n", pProcHeader->handle_type);
        RpcRaiseException(RPC_X_BAD_STUB_DATA);
    }
}

static void CALLBACK ndr_client_call_finally(BOOL normal, void *arg)
{
    struct ndr_client_call_ctx *ctx = arg;

    if (ctx->ext_flags.HasNewCorrDesc)
    {
        /* free extra correlation package */
        NdrCorrelationFree(ctx->stub_msg);
    }

    if (ctx->Oif_flags.HasPipes)
    {
        /* NdrPipesDone(...) */
    }

    /* free the full pointer translation tables */
    if (ctx->proc_header->Oi_flags & Oi_FULL_PTR_USED)
        NdrFullPointerXlatFree(ctx->stub_msg->FullPtrXlatTables);

    /* free marshalling buffer */
    if (ctx->proc_header->Oi_flags & Oi_OBJECT_PROC)
        NdrProxyFreeBuffer(ctx->This, ctx->stub_msg);
    else
    {
        NdrFreeBuffer(ctx->stub_msg);
        client_free_handle(ctx->stub_msg, ctx->proc_header, ctx->handle_format, ctx->hbinding);
    }
}

/* Helper for ndr_client_call, to factor out the part that may or may not be
 * guarded by a try/except block. */
static LONG_PTR do_ndr_client_call( const MIDL_STUB_DESC *stub_desc, const PFORMAT_STRING format,
        const PFORMAT_STRING handle_format, void **stack_top, void **fpu_stack, MIDL_STUB_MESSAGE *stub_msg,
        unsigned short procedure_number, unsigned short stack_size, unsigned int number_of_params,
        INTERPRETER_OPT_FLAGS Oif_flags, INTERPRETER_OPT_FLAGS2 ext_flags, const NDR_PROC_HEADER *proc_header )
{
    struct ndr_client_call_ctx finally_ctx;
    RPC_MESSAGE rpc_msg;
    handle_t hbinding = NULL;
    /* the value to return to the client from the remote procedure */
    LONG_PTR retval = 0;
    /* the pointer to the object when in OLE mode */
    void *This = NULL;
    /* correlation cache */
    ULONG_PTR NdrCorrCache[256];

    /* create the full pointer translation tables, if requested */
    if (proc_header->Oi_flags & Oi_FULL_PTR_USED)
        stub_msg->FullPtrXlatTables = NdrFullPointerXlatInit(0,XLAT_CLIENT);

    if (proc_header->Oi_flags & Oi_OBJECT_PROC)
    {
        /* object is always the first argument */
        This = stack_top[0];
        NdrProxyInitialize(This, &rpc_msg, stub_msg, stub_desc, procedure_number);
    }

    finally_ctx.stub_msg = stub_msg;
    finally_ctx.Oif_flags = Oif_flags;
    finally_ctx.ext_flags = ext_flags;
    finally_ctx.proc_header = proc_header;
    finally_ctx.This = This;
    finally_ctx.handle_format = handle_format;
    finally_ctx.hbinding = hbinding;

    __TRY
    {
        if (!(proc_header->Oi_flags & Oi_OBJECT_PROC))
            NdrClientInitializeNew(&rpc_msg, stub_msg, stub_desc, procedure_number);

        stub_msg->StackTop = (unsigned char *)stack_top;

        /* we only need a handle if this isn't an object method */
        if (!(proc_header->Oi_flags & Oi_OBJECT_PROC))
        {
            hbinding = client_get_handle(stub_msg, proc_header, handle_format);
            if (!hbinding) return 0;
        }

        stub_msg->BufferLength = 0;

        /* store the RPC flags away */
        if (proc_header->Oi_flags & Oi_HAS_RPCFLAGS)
            rpc_msg.RpcFlags = ((const NDR_PROC_HEADER_RPC *)proc_header)->rpc_flags;

        /* use alternate memory allocation routines */
        if (proc_header->Oi_flags & Oi_RPCSS_ALLOC_USED)
            NdrRpcSmSetClientToOsf(stub_msg);

        if (Oif_flags.HasPipes)
        {
            FIXME("pipes not supported yet\n");
            RpcRaiseException(RPC_X_WRONG_STUB_VERSION); /* FIXME: remove when implemented */
            /* init pipes package */
            /* NdrPipesInitialize(...) */
        }
        if (ext_flags.HasNewCorrDesc)
        {
            /* initialize extra correlation package */
            NdrCorrelationInitialize(stub_msg, NdrCorrCache, sizeof(NdrCorrCache), 0);
            if (ext_flags.Unused & 0x2) /* has range on conformance */
                stub_msg->CorrDespIncrement = 12;
        }

        /* order of phases:
         * 1. INITOUT - zero [out] parameters (proxies only)
         * 2. CALCSIZE - calculate the buffer size
         * 3. GETBUFFER - allocate the buffer
         * 4. MARSHAL - marshal [in] params into the buffer
         * 5. SENDRECEIVE - send/receive buffer
         * 6. UNMARSHAL - unmarshal [out] params from buffer
         * 7. FREE - clear [out] parameters (for proxies, and only on error)
         */

        /* 1. INITOUT */
        if (proc_header->Oi_flags & Oi_OBJECT_PROC)
        {
            TRACE( "INITOUT\n" );
            client_do_args(stub_msg, format, STUBLESS_INITOUT, fpu_stack,
                           number_of_params, (unsigned char *)&retval);
        }

        /* 2. CALCSIZE */
        TRACE( "CALCSIZE\n" );
        client_do_args(stub_msg, format, STUBLESS_CALCSIZE, fpu_stack,
                       number_of_params, (unsigned char *)&retval);

        /* 3. GETBUFFER */
        TRACE( "GETBUFFER\n" );
        if (proc_header->Oi_flags & Oi_OBJECT_PROC)
            NdrProxyGetBuffer(This, stub_msg);
        else if (Oif_flags.HasPipes)
            FIXME("pipes not supported yet\n");
        else if (proc_header->handle_type == FC_AUTO_HANDLE)
// #if 0
            NdrNsGetBuffer(stub_msg, stub_msg->BufferLength, hbinding);
// #else
            // FIXME("using auto handle - call NdrNsGetBuffer when it gets implemented\n");
// #endif
        else
            NdrGetBuffer(stub_msg, stub_msg->BufferLength, hbinding);

        /* 4. MARSHAL */
        TRACE( "MARSHAL\n" );
        client_do_args(stub_msg, format, STUBLESS_MARSHAL, fpu_stack,
                       number_of_params, (unsigned char *)&retval);

        /* 5. SENDRECEIVE */
        TRACE( "SENDRECEIVE\n" );
        if (proc_header->Oi_flags & Oi_OBJECT_PROC)
            NdrProxySendReceive(This, stub_msg);
        else if (Oif_flags.HasPipes)
            /* NdrPipesSendReceive(...) */
            FIXME("pipes not supported yet\n");
        else if (proc_header->handle_type == FC_AUTO_HANDLE)
// #if 0
            NdrNsSendReceive(stub_msg, stub_msg->Buffer, stub_desc->IMPLICIT_HANDLE_INFO.pAutoHandle);
// #else
           // FIXME("using auto handle - call NdrNsSendReceive when it gets implemented\n");
// #endif
        else
            NdrSendReceive(stub_msg, stub_msg->Buffer);

        /* convert strings, floating point values and endianness into our
         * preferred format */
        if ((rpc_msg.DataRepresentation & 0x0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION)
            NdrConvert(stub_msg, format);

        /* 6. UNMARSHAL */
        TRACE( "UNMARSHAL\n" );
        client_do_args(stub_msg, format, STUBLESS_UNMARSHAL, fpu_stack,
                       number_of_params, (unsigned char *)&retval);
    }
    __FINALLY_CTX(ndr_client_call_finally, &finally_ctx)

    return retval;
}

LONG_PTR CDECL DECLSPEC_HIDDEN ndr_client_call( PMIDL_STUB_DESC pStubDesc, PFORMAT_STRING pFormat,
                                                void **stack_top, void **fpu_stack )
{
    /* pointer to start of stack where arguments start */
    MIDL_STUB_MESSAGE stubMsg;
    /* procedure number */
    unsigned short procedure_number;
    /* size of stack */
    unsigned short stack_size;
    /* number of parameters. optional for client to give it to us */
    unsigned int number_of_params;
    /* cache of Oif_flags from v2 procedure header */
    INTERPRETER_OPT_FLAGS Oif_flags = { 0 };
    /* cache of extension flags from NDR_PROC_HEADER_EXTS */
    INTERPRETER_OPT_FLAGS2 ext_flags = { 0 };
    /* header for procedure string */
    const NDR_PROC_HEADER * pProcHeader = (const NDR_PROC_HEADER *)&pFormat[0];
    /* the value to return to the client from the remote procedure */
    LONG_PTR RetVal = 0;
    PFORMAT_STRING pHandleFormat;
    NDR_PARAM_OIF old_args[256];

    TRACE("pStubDesc %p, pFormat %p, ...\n", pStubDesc, pFormat);

    TRACE("NDR Version: 0x%x\n", pStubDesc->Version);

    if (pProcHeader->Oi_flags & Oi_HAS_RPCFLAGS)
    {
        const NDR_PROC_HEADER_RPC *header_rpc = (const NDR_PROC_HEADER_RPC *)&pFormat[0];
        stack_size = header_rpc->stack_size;
        procedure_number = header_rpc->proc_num;
        pFormat += sizeof(NDR_PROC_HEADER_RPC);
    }
    else
    {
        stack_size = pProcHeader->stack_size;
        procedure_number = pProcHeader->proc_num;
        pFormat += sizeof(NDR_PROC_HEADER);
    }
    TRACE("stack size: 0x%x\n", stack_size);
    TRACE("proc num: %d\n", procedure_number);
    TRACE("Oi_flags = 0x%02x\n", pProcHeader->Oi_flags);
    TRACE("MIDL stub version = 0x%x\n", pStubDesc->MIDLVersion);

    pHandleFormat = pFormat;

    /* we only need a handle if this isn't an object method */
    if (!(pProcHeader->Oi_flags & Oi_OBJECT_PROC))
        pFormat += get_handle_desc_size(pProcHeader, pFormat);

    if (is_oicf_stubdesc(pStubDesc))  /* -Oicf format */
    {
        const NDR_PROC_PARTIAL_OIF_HEADER *pOIFHeader =
            (const NDR_PROC_PARTIAL_OIF_HEADER *)pFormat;

        Oif_flags = pOIFHeader->Oi2Flags;
        number_of_params = pOIFHeader->number_of_params;

        pFormat += sizeof(NDR_PROC_PARTIAL_OIF_HEADER);

        TRACE("Oif_flags = %s\n", debugstr_INTERPRETER_OPT_FLAGS(Oif_flags) );

        if (Oif_flags.HasExtensions)
        {
            const NDR_PROC_HEADER_EXTS *pExtensions = (const NDR_PROC_HEADER_EXTS *)pFormat;
            ext_flags = pExtensions->Flags2;
            pFormat += pExtensions->Size;
#ifdef __x86_64__
            if (pExtensions->Size > sizeof(*pExtensions) && fpu_stack)
            {
                int i;
                unsigned short fpu_mask = *(unsigned short *)(pExtensions + 1);
                for (i = 0; i < 4; i++, fpu_mask >>= 2)
                    switch (fpu_mask & 3)
                    {
                    case 1: *(float *)&stack_top[i] = *(float *)&fpu_stack[i]; break;
                    case 2: *(double *)&stack_top[i] = *(double *)&fpu_stack[i]; break;
                    }
            }
#endif
        }
    }
    else
    {
        pFormat = convert_old_args( &stubMsg, pFormat, stack_size,
                                    pProcHeader->Oi_flags & Oi_OBJECT_PROC,
                                    old_args, sizeof(old_args), &number_of_params );
    }

    if (pProcHeader->Oi_flags & Oi_OBJECT_PROC)
    {
        __TRY
        {
            RetVal = do_ndr_client_call(pStubDesc, pFormat, pHandleFormat,
                    stack_top, fpu_stack, &stubMsg, procedure_number, stack_size,
                    number_of_params, Oif_flags, ext_flags, pProcHeader);
        }
        __EXCEPT_ALL
        {
            /* 7. FREE */
            TRACE( "FREE\n" );
            client_do_args(&stubMsg, pFormat, STUBLESS_FREE, fpu_stack,
                           number_of_params, (unsigned char *)&RetVal);
            RetVal = NdrProxyErrorHandler(GetExceptionCode());
        }
        __ENDTRY
    }
    else if (pProcHeader->Oi_flags & Oi_HAS_COMM_OR_FAULT)
    {
        __TRY
        {
            RetVal = do_ndr_client_call(pStubDesc, pFormat, pHandleFormat,
                    stack_top, fpu_stack, &stubMsg, procedure_number, stack_size,
                    number_of_params, Oif_flags, ext_flags, pProcHeader);
        }
        __EXCEPT_ALL
        {
            const COMM_FAULT_OFFSETS *comm_fault_offsets = &pStubDesc->CommFaultOffsets[procedure_number];
            ULONG *comm_status;
            ULONG *fault_status;

            TRACE("comm_fault_offsets = {0x%hx, 0x%hx}\n", comm_fault_offsets->CommOffset, comm_fault_offsets->FaultOffset);

            if (comm_fault_offsets->CommOffset == -1)
                comm_status = (ULONG *)&RetVal;
            else if (comm_fault_offsets->CommOffset >= 0)
                comm_status = *(ULONG **)ARG_FROM_OFFSET(stubMsg.StackTop, comm_fault_offsets->CommOffset);
            else
                comm_status = NULL;

            if (comm_fault_offsets->FaultOffset == -1)
                fault_status = (ULONG *)&RetVal;
            else if (comm_fault_offsets->FaultOffset >= 0)
                fault_status = *(ULONG **)ARG_FROM_OFFSET(stubMsg.StackTop, comm_fault_offsets->FaultOffset);
            else
                fault_status = NULL;

            NdrMapCommAndFaultStatus(&stubMsg, comm_status, fault_status,
                                     GetExceptionCode());
        }
        __ENDTRY
    }
    else
    {
        RetVal = do_ndr_client_call(pStubDesc, pFormat, pHandleFormat,
                stack_top, fpu_stack, &stubMsg, procedure_number, stack_size,
                number_of_params, Oif_flags, ext_flags, pProcHeader);
    }

    TRACE("RetVal = 0x%lx\n", RetVal);
    return RetVal;
}

static ULONG EmbeddedComplexSize(MIDL_STUB_MESSAGE *pStubMsg,
                                 PFORMAT_STRING pFormat)
{
  switch (*pFormat) {
  case FC_STRUCT:
  case FC_PSTRUCT:
  case FC_CSTRUCT:
  case FC_BOGUS_STRUCT:
  case FC_SMFARRAY:
  case FC_SMVARRAY:
  case FC_CSTRING:
    return *(const WORD*)&pFormat[2];
  case FC_LGFARRAY:
  case FC_LGVARRAY:
    return *(const ULONG*)&pFormat[2];
  case FC_USER_MARSHAL:
    return *(const WORD*)&pFormat[4];
  case FC_RANGE: {
    switch (((const NDR_RANGE *)pFormat)->flags_type & 0xf) {
    case FC_BYTE:
    case FC_CHAR:
    case FC_SMALL:
    case FC_USMALL:
        return sizeof(UCHAR);
    case FC_WCHAR:
    case FC_SHORT:
    case FC_USHORT:
        return sizeof(USHORT);
    case FC_LONG:
    case FC_ULONG:
    case FC_ENUM32:
        return sizeof(ULONG);
    case FC_FLOAT:
        return sizeof(float);
    case FC_DOUBLE:
        return sizeof(double);
    case FC_HYPER:
        return sizeof(ULONGLONG);
    case FC_ENUM16:
        return sizeof(UINT);
    default:
        ERR("unknown type 0x%x\n", ((const NDR_RANGE *)pFormat)->flags_type & 0xf);
        RpcRaiseException(RPC_X_BAD_STUB_DATA);
    }
  }
  case FC_NON_ENCAPSULATED_UNION:
    pFormat += 2;
    pFormat = SkipConformance(pStubMsg, pFormat);
    pFormat += *(const SHORT*)pFormat;
    return *(const SHORT*)pFormat;
  case FC_IP:
    return sizeof(void *);
  case FC_WSTRING:
    return *(const WORD*)&pFormat[2] * 2;
  default:
    FIXME("unhandled embedded type %02x\n", *pFormat);
  }
  return 0;
}

ULONG ComplexStructSize(PMIDL_STUB_MESSAGE pStubMsg, PFORMAT_STRING pFormat)
{
  PFORMAT_STRING desc;
  ULONG size = 0;

  while (*pFormat != FC_END) {
    switch (*pFormat) {
    case FC_BYTE:
    case FC_CHAR:
    case FC_SMALL:
    case FC_USMALL:
      size += 1;
      break;
    case FC_WCHAR:
    case FC_SHORT:
    case FC_USHORT:
      size += 2;
      break;
    case FC_LONG:
    case FC_ULONG:
    case FC_ENUM16:
    case FC_ENUM32:
    case FC_FLOAT:
      size += 4;
      break;
    case FC_INT3264:
    case FC_UINT3264:
      size += sizeof(INT_PTR);
      break;
    case FC_HYPER:
    case FC_DOUBLE:
      size += 8;
      break;
    case FC_RP:
    case FC_UP:
    case FC_OP:
    case FC_FP:
    case FC_POINTER:
      size += sizeof(void *);
      if (*pFormat != FC_POINTER)
        pFormat += 4;
      break;
    case FC_ALIGNM2:
      align_length(&size, 2);
      break;
    case FC_ALIGNM4:
      align_length(&size, 4);
      break;
    case FC_ALIGNM8:
      align_length(&size, 8);
      break;
    case FC_STRUCTPAD1:
    case FC_STRUCTPAD2:
    case FC_STRUCTPAD3:
    case FC_STRUCTPAD4:
    case FC_STRUCTPAD5:
    case FC_STRUCTPAD6:
    case FC_STRUCTPAD7:
      size += *pFormat - FC_STRUCTPAD1 + 1;
      break;
    case FC_EMBEDDED_COMPLEX:
      size += pFormat[1];
      pFormat += 2;
      desc = pFormat + *(const SHORT*)pFormat;
      size += EmbeddedComplexSize(pStubMsg, desc);
      pFormat += 2;
      continue;
    case FC_PAD:
      break;
    default:
      FIXME("unhandled format 0x%02x\n", *pFormat);
    }
    pFormat++;
  }

  return size;
}

PFORMAT_STRING ComputeConformanceOrVariance(
    MIDL_STUB_MESSAGE *pStubMsg, unsigned char *pMemory,
    PFORMAT_STRING pFormat, ULONG_PTR def, ULONG_PTR *pCount)
{
  BYTE dtype = pFormat[0] & 0xf;
  short ofs = *(const short *)&pFormat[2];
  LPVOID ptr = NULL;
  ULONG_PTR data = 0;

  if (!IsConformanceOrVariancePresent(pFormat)) {
    /* null descriptor */
    *pCount = def;
    goto finish_conf;
  }

  switch (pFormat[0] & 0xf0) {
  case FC_NORMAL_CONFORMANCE:
    TRACE("normal conformance, ofs=%d\n", ofs);
    ptr = pMemory;
    break;
  case FC_POINTER_CONFORMANCE:
    TRACE("pointer conformance, ofs=%d\n", ofs);
    ptr = pStubMsg->Memory;
    break;
  case FC_TOP_LEVEL_CONFORMANCE:
    TRACE("toplevel conformance, ofs=%d\n", ofs);
    if (pStubMsg->StackTop) {
      ptr = pStubMsg->StackTop;
    }
    else {
      /* -Os mode, *pCount is already set */
      goto finish_conf;
    }
    break;
  case FC_CONSTANT_CONFORMANCE:
    data = ofs | ((DWORD)pFormat[1] << 16);
    TRACE("constant conformance, val=%ld\n", data);
    *pCount = data;
    goto finish_conf;
  case FC_TOP_LEVEL_MULTID_CONFORMANCE:
    FIXME("toplevel multidimensional conformance, ofs=%d\n", ofs);
    if (pStubMsg->StackTop) {
      ptr = pStubMsg->StackTop;
    }
    else {
      /* ? */
      goto done_conf_grab;
    }
    break;
  default:
    FIXME("unknown conformance type %x, expect crash.\n", pFormat[0] & 0xf0);
    goto finish_conf;
  }

  switch (pFormat[1]) {
  case FC_DEREFERENCE:
    ptr = *(LPVOID*)((char *)ptr + ofs);
    break;
  case FC_CALLBACK:
  {
    unsigned char *old_stack_top = pStubMsg->StackTop;
    ULONG_PTR max_count, old_max_count = pStubMsg->MaxCount;

    pStubMsg->StackTop = ptr;

    /* ofs is index into StubDesc->apfnExprEval */
    TRACE("callback conformance into apfnExprEval[%d]\n", ofs);
    pStubMsg->StubDesc->apfnExprEval[ofs](pStubMsg);

    pStubMsg->StackTop = old_stack_top;

    /* the callback function always stores the computed value in MaxCount */
    max_count = pStubMsg->MaxCount;
    pStubMsg->MaxCount = old_max_count;
    *pCount = max_count;
    goto finish_conf;
  }
  default:
    ptr = (char *)ptr + ofs;
    break;
  }

  switch (dtype) {
  case FC_LONG:
  case FC_ULONG:
    data = *(DWORD*)ptr;
    break;
  case FC_SHORT:
    data = *(SHORT*)ptr;
    break;
  case FC_USHORT:
    data = *(USHORT*)ptr;
    break;
  case FC_CHAR:
  case FC_SMALL:
    data = *(CHAR*)ptr;
    break;
  case FC_BYTE:
  case FC_USMALL:
    data = *(UCHAR*)ptr;
    break;
  case FC_HYPER:
    data = *(ULONGLONG *)ptr;
    break;
  default:
    FIXME("unknown conformance data type %x\n", dtype);
    goto done_conf_grab;
  }
  TRACE("dereferenced data type %x at %p, got %ld\n", dtype, ptr, data);

done_conf_grab:
  switch (pFormat[1]) {
  case FC_DEREFERENCE: /* already handled */
  case 0: /* no op */
    *pCount = data;
    break;
  case FC_ADD_1:
    *pCount = data + 1;
    break;
  case FC_SUB_1:
    *pCount = data - 1;
    break;
  case FC_MULT_2:
    *pCount = data * 2;
    break;
  case FC_DIV_2:
    *pCount = data / 2;
    break;
  default:
    FIXME("unknown conformance op %d\n", pFormat[1]);
    goto finish_conf;
  }

finish_conf:
  TRACE("resulting conformance is %ld\n", *pCount);

  return SkipConformance(pStubMsg, pFormat);
}

static unsigned int type_stack_size(unsigned char fc)
{
    switch (fc)
    {
    case FC_BYTE:
    case FC_CHAR:
    case FC_SMALL:
    case FC_USMALL:
    case FC_WCHAR:
    case FC_SHORT:
    case FC_USHORT:
    case FC_LONG:
    case FC_ULONG:
    case FC_INT3264:
    case FC_UINT3264:
    case FC_ENUM16:
    case FC_ENUM32:
    case FC_FLOAT:
    case FC_ERROR_STATUS_T:
    case FC_IGNORE:
        return sizeof(void *);
    case FC_DOUBLE:
        return sizeof(double);
    case FC_HYPER:
        return sizeof(ULONGLONG);
    default:
        ERR("invalid base type 0x%x\n", fc);
        RpcRaiseException(RPC_S_INTERNAL_ERROR);
    }
}

PFORMAT_STRING convert_old_args( PMIDL_STUB_MESSAGE pStubMsg, PFORMAT_STRING pFormat,
                                 unsigned int stack_size, BOOL object_proc,
                                 void *buffer, unsigned int size, unsigned int *count )
{
    NDR_PARAM_OIF *args = buffer;
    unsigned int i, stack_offset = object_proc ? sizeof(void *) : 0;

    for (i = 0; stack_offset < stack_size; i++)
    {
        const NDR_PARAM_OI_BASETYPE *param = (const NDR_PARAM_OI_BASETYPE *)pFormat;
        const NDR_PARAM_OI_OTHER *other = (const NDR_PARAM_OI_OTHER *)pFormat;

        if (i + 1 > size / sizeof(*args))
        {
            FIXME( "%u args not supported\n", i );
            RpcRaiseException( RPC_S_INTERNAL_ERROR );
        }

        args[i].stack_offset = stack_offset;
        memset( &args[i].attr, 0, sizeof(args[i].attr) );

        switch (param->param_direction)
        {
        case FC_IN_PARAM_BASETYPE:
            args[i].attr.IsIn = 1;
            args[i].attr.IsBasetype = 1;
            break;
        case FC_RETURN_PARAM_BASETYPE:
            args[i].attr.IsOut = 1;
            args[i].attr.IsReturn = 1;
            args[i].attr.IsBasetype = 1;
            break;
        case FC_IN_PARAM:
            args[i].attr.IsIn = 1;
            args[i].attr.MustFree = 1;
            break;
        case FC_IN_PARAM_NO_FREE_INST:
            args[i].attr.IsIn = 1;
            args[i].attr.IsDontCallFreeInst = 1;
            break;
        case FC_IN_OUT_PARAM:
            args[i].attr.IsIn = 1;
            args[i].attr.IsOut = 1;
            args[i].attr.MustFree = 1;
            break;
        case FC_OUT_PARAM:
            args[i].attr.IsOut = 1;
            break;
        case FC_RETURN_PARAM:
            args[i].attr.IsOut = 1;
            args[i].attr.IsReturn = 1;
            break;
        }
        if (args[i].attr.IsBasetype)
        {
            args[i].u.type_format_char = param->type_format_char;
            stack_offset += type_stack_size( param->type_format_char );
            pFormat += sizeof(NDR_PARAM_OI_BASETYPE);
        }
        else
        {
            args[i].u.type_offset = other->type_offset;
            args[i].attr.IsByValue = is_by_value( &pStubMsg->StubDesc->pFormatTypes[other->type_offset] );
            stack_offset += other->stack_size * sizeof(void *);
            pFormat += sizeof(NDR_PARAM_OI_OTHER);
        }
    }
    *count = i;
    return (PFORMAT_STRING)args;
}

/***********************************************************************
 *           NdrContextHandleMarshall [internal]
 */
static unsigned char *WINAPI NdrContextHandleMarshall(
    PMIDL_STUB_MESSAGE pStubMsg,
    unsigned char *pMemory,
    PFORMAT_STRING pFormat)
{
    TRACE("pStubMsg %p, pMemory %p, type 0x%02x\n", pStubMsg, pMemory, *pFormat);

    if (*pFormat != FC_BIND_CONTEXT)
    {
        ERR("invalid format type %x\n", *pFormat);
        RpcRaiseException(RPC_S_INTERNAL_ERROR);
    }
    TRACE("flags: 0x%02x\n", pFormat[1]);

    if (pStubMsg->IsClient)
    {
        if (pFormat[1] & HANDLE_PARAM_IS_VIA_PTR)
            NdrClientContextMarshall(pStubMsg, *(NDR_CCONTEXT **)pMemory, FALSE);
        else
            NdrClientContextMarshall(pStubMsg, pMemory, FALSE);
    }
    else
    {
        NDR_SCONTEXT ctxt = CONTAINING_RECORD(pMemory, struct _NDR_SCONTEXT, userContext);
        NDR_RUNDOWN rundown = pStubMsg->StubDesc->apfnNdrRundownRoutines[pFormat[2]];
        NdrServerContextNewMarshall(pStubMsg, ctxt, rundown, pFormat);
    }

    return NULL;
}

/***********************************************************************
 *           NdrContextHandleBufferSize [internal]
 */
static void WINAPI NdrContextHandleBufferSize(
    PMIDL_STUB_MESSAGE pStubMsg,
    unsigned char *pMemory,
    PFORMAT_STRING pFormat)
{
    TRACE("pStubMsg %p, pMemory %p, type 0x%02x\n", pStubMsg, pMemory, *pFormat);

    if (*pFormat != FC_BIND_CONTEXT)
    {
        ERR("invalid format type %x\n", *pFormat);
        RpcRaiseException(RPC_S_INTERNAL_ERROR);
    }
    align_length(&pStubMsg->BufferLength, 4);
    safe_buffer_length_increment(pStubMsg, cbNDRContext);
}

/***********************************************************************
 *           NdrRangeMarshall [internal]
 */
static unsigned char *WINAPI NdrRangeMarshall(
    PMIDL_STUB_MESSAGE pStubMsg,
    unsigned char *pMemory,
    PFORMAT_STRING pFormat)
{
    const NDR_RANGE *pRange = (const NDR_RANGE *)pFormat;
    unsigned char base_type;

    TRACE("pStubMsg %p, pMemory %p, type 0x%02x\n", pStubMsg, pMemory, *pFormat);

    if (pRange->type != FC_RANGE)
    {
        ERR("invalid format type %x\n", pRange->type);
        RpcRaiseException(RPC_S_INTERNAL_ERROR);
        return NULL;
    }

    base_type = pRange->flags_type & 0xf;

    return NdrBaseTypeMarshall(pStubMsg, pMemory, &base_type);
}

/***********************************************************************
 *           NdrContextHandleUnmarshall [internal]
 */
static unsigned char *WINAPI NdrContextHandleUnmarshall(
    PMIDL_STUB_MESSAGE pStubMsg,
    unsigned char **ppMemory,
    PFORMAT_STRING pFormat,
    unsigned char fMustAlloc)
{
    TRACE("pStubMsg %p, ppMemory %p, pFormat %p, fMustAlloc %s\n", pStubMsg,
        ppMemory, pFormat, fMustAlloc ? "TRUE": "FALSE");

    if (*pFormat != FC_BIND_CONTEXT)
    {
        ERR("invalid format type %x\n", *pFormat);
        RpcRaiseException(RPC_S_INTERNAL_ERROR);
    }
    TRACE("flags: 0x%02x\n", pFormat[1]);

    if (pStubMsg->IsClient)
    {
        NDR_CCONTEXT *ccontext;
        if (pFormat[1] & HANDLE_PARAM_IS_VIA_PTR)
            ccontext = *(NDR_CCONTEXT **)ppMemory;
        else
            ccontext = (NDR_CCONTEXT *)ppMemory;
        /* [out]-only or [ret] param */
        if ((pFormat[1] & (HANDLE_PARAM_IS_IN|HANDLE_PARAM_IS_OUT)) == HANDLE_PARAM_IS_OUT)
            *ccontext = NULL;
        NdrClientContextUnmarshall(pStubMsg, ccontext, pStubMsg->RpcMsg->Handle);
    }
    else
    {
        NDR_SCONTEXT ctxt;
        ctxt = NdrServerContextNewUnmarshall(pStubMsg, pFormat);
        if (pFormat[1] & HANDLE_PARAM_IS_VIA_PTR)
            *(void **)ppMemory = NDRSContextValue(ctxt);
        else
            *(void **)ppMemory = *NDRSContextValue(ctxt);
    }

    return NULL;
}

/***********************************************************************
 *           NdrBaseTypeUnmarshall [internal]
 */
static unsigned char *WINAPI NdrBaseTypeUnmarshall(
    PMIDL_STUB_MESSAGE pStubMsg,
    unsigned char **ppMemory,
    PFORMAT_STRING pFormat,
    unsigned char fMustAlloc)
{
    TRACE("pStubMsg: %p, ppMemory: %p, type: 0x%02x, fMustAlloc: %s\n", pStubMsg, ppMemory, *pFormat, fMustAlloc ? "true" : "false");

#define BASE_TYPE_UNMARSHALL(type) do { \
        align_pointer(&pStubMsg->Buffer, sizeof(type)); \
        if (!fMustAlloc && !pStubMsg->IsClient && !*ppMemory) \
        { \
            *ppMemory = pStubMsg->Buffer; \
            TRACE("*ppMemory: %p\n", *ppMemory); \
            safe_buffer_increment(pStubMsg, sizeof(type)); \
        } \
        else \
        {  \
            if (fMustAlloc) \
                *ppMemory = NdrAllocate(pStubMsg, sizeof(type)); \
            TRACE("*ppMemory: %p\n", *ppMemory); \
            safe_copy_from_buffer(pStubMsg, *ppMemory, sizeof(type)); \
        } \
    } while (0)

    switch(*pFormat)
    {
    case FC_BYTE:
    case FC_CHAR:
    case FC_SMALL:
    case FC_USMALL:
        BASE_TYPE_UNMARSHALL(UCHAR);
        TRACE("value: 0x%02x\n", **ppMemory);
        break;
    case FC_WCHAR:
    case FC_SHORT:
    case FC_USHORT:
        BASE_TYPE_UNMARSHALL(USHORT);
        TRACE("value: 0x%04x\n", **(USHORT **)ppMemory);
        break;
    case FC_LONG:
    case FC_ULONG:
    case FC_ERROR_STATUS_T:
    case FC_ENUM32:
        BASE_TYPE_UNMARSHALL(ULONG);
        TRACE("value: 0x%08x\n", **(ULONG **)ppMemory);
        break;
   case FC_FLOAT:
        BASE_TYPE_UNMARSHALL(float);
        TRACE("value: %f\n", **(float **)ppMemory);
        break;
    case FC_DOUBLE:
        BASE_TYPE_UNMARSHALL(double);
        TRACE("value: %f\n", **(double **)ppMemory);
        break;
    case FC_HYPER:
        BASE_TYPE_UNMARSHALL(ULONGLONG);
        TRACE("value: %s\n", wine_dbgstr_longlong(**(ULONGLONG **)ppMemory));
        break;
    case FC_ENUM16:
    {
        USHORT val;
        align_pointer(&pStubMsg->Buffer, sizeof(USHORT));
        if (!fMustAlloc && !*ppMemory)
            fMustAlloc = TRUE;
        if (fMustAlloc)
            *ppMemory = NdrAllocate(pStubMsg, sizeof(UINT));
        safe_copy_from_buffer(pStubMsg, &val, sizeof(USHORT));
        /* 16-bits on the wire, but int in memory */
        **(UINT **)ppMemory = val;
        TRACE("value: 0x%08x\n", **(UINT **)ppMemory);
        break;
    }
    case FC_INT3264:
        if (sizeof(INT_PTR) == sizeof(INT)) BASE_TYPE_UNMARSHALL(INT);
        else
        {
            INT val;
            align_pointer(&pStubMsg->Buffer, sizeof(INT));
            if (!fMustAlloc && !*ppMemory)
                fMustAlloc = TRUE;
            if (fMustAlloc)
                *ppMemory = NdrAllocate(pStubMsg, sizeof(INT_PTR));
            safe_copy_from_buffer(pStubMsg, &val, sizeof(INT));
            **(INT_PTR **)ppMemory = val;
            TRACE("value: 0x%08lx\n", **(INT_PTR **)ppMemory);
        }
        break;
    case FC_UINT3264:
        if (sizeof(UINT_PTR) == sizeof(UINT)) BASE_TYPE_UNMARSHALL(UINT);
        else
        {
            UINT val;
            align_pointer(&pStubMsg->Buffer, sizeof(UINT));
            if (!fMustAlloc && !*ppMemory)
                fMustAlloc = TRUE;
            if (fMustAlloc)
                *ppMemory = NdrAllocate(pStubMsg, sizeof(UINT_PTR));
            safe_copy_from_buffer(pStubMsg, &val, sizeof(UINT));
            **(UINT_PTR **)ppMemory = val;
            TRACE("value: 0x%08lx\n", **(UINT_PTR **)ppMemory);
        }
        break;
    case FC_IGNORE:
        break;
    default:
        FIXME("Unhandled base type: 0x%02x\n", *pFormat);
    }
#undef BASE_TYPE_UNMARSHALL

    /* FIXME: what is the correct return value? */

    return NULL;
}

/***********************************************************************
 *           NdrBaseTypeMarshall [internal]
 */
static unsigned char *WINAPI NdrBaseTypeMarshall(
    PMIDL_STUB_MESSAGE pStubMsg,
    unsigned char *pMemory,
    PFORMAT_STRING pFormat)
{
    TRACE("pStubMsg %p, pMemory %p, type 0x%02x\n", pStubMsg, pMemory, *pFormat);

    switch(*pFormat)
    {
    case FC_BYTE:
    case FC_CHAR:
    case FC_SMALL:
    case FC_USMALL:
        safe_copy_to_buffer(pStubMsg, pMemory, sizeof(UCHAR));
        TRACE("value: 0x%02x\n", *pMemory);
        break;
    case FC_WCHAR:
    case FC_SHORT:
    case FC_USHORT:
        align_pointer_clear(&pStubMsg->Buffer, sizeof(USHORT));
        safe_copy_to_buffer(pStubMsg, pMemory, sizeof(USHORT));
        TRACE("value: 0x%04x\n", *(USHORT *)pMemory);
        break;
    case FC_LONG:
    case FC_ULONG:
    case FC_ERROR_STATUS_T:
    case FC_ENUM32:
        align_pointer_clear(&pStubMsg->Buffer, sizeof(ULONG));
        safe_copy_to_buffer(pStubMsg, pMemory, sizeof(ULONG));
        TRACE("value: 0x%08x\n", *(ULONG *)pMemory);
        break;
    case FC_FLOAT:
        align_pointer_clear(&pStubMsg->Buffer, sizeof(float));
        safe_copy_to_buffer(pStubMsg, pMemory, sizeof(float));
        break;
    case FC_DOUBLE:
        align_pointer_clear(&pStubMsg->Buffer, sizeof(double));
        safe_copy_to_buffer(pStubMsg, pMemory, sizeof(double));
        break;
    case FC_HYPER:
        align_pointer_clear(&pStubMsg->Buffer, sizeof(ULONGLONG));
        safe_copy_to_buffer(pStubMsg, pMemory, sizeof(ULONGLONG));
        TRACE("value: %s\n", wine_dbgstr_longlong(*(ULONGLONG*)pMemory));
        break;
    case FC_ENUM16:
    {
        USHORT val = *(UINT *)pMemory;
        /* only 16-bits on the wire, so do a sanity check */
        if (*(UINT *)pMemory > SHRT_MAX)
            RpcRaiseException(RPC_X_ENUM_VALUE_OUT_OF_RANGE);
        align_pointer_clear(&pStubMsg->Buffer, sizeof(USHORT));
        safe_copy_to_buffer(pStubMsg, &val, sizeof(val));
        TRACE("value: 0x%04x\n", *(UINT *)pMemory);
        break;
    }
    case FC_INT3264:
    case FC_UINT3264:
    {
        UINT val = *(UINT_PTR *)pMemory;
        align_pointer_clear(&pStubMsg->Buffer, sizeof(UINT));
        safe_copy_to_buffer(pStubMsg, &val, sizeof(val));
        break;
    }
    case FC_IGNORE:
        break;
    default:
        FIXME("Unhandled base type: 0x%02x\n", *pFormat);
    }

    /* FIXME: what is the correct return value? */
    return NULL;
}

/***********************************************************************
 *           NdrBaseTypeBufferSize [internal]
 */
static void WINAPI NdrBaseTypeBufferSize(
    PMIDL_STUB_MESSAGE pStubMsg,
    unsigned char *pMemory,
    PFORMAT_STRING pFormat)
{
    TRACE("pStubMsg %p, pMemory %p, type 0x%02x\n", pStubMsg, pMemory, *pFormat);

    switch(*pFormat)
    {
    case FC_BYTE:
    case FC_CHAR:
    case FC_SMALL:
    case FC_USMALL:
        safe_buffer_length_increment(pStubMsg, sizeof(UCHAR));
        break;
    case FC_WCHAR:
    case FC_SHORT:
    case FC_USHORT:
    case FC_ENUM16:
        align_length(&pStubMsg->BufferLength, sizeof(USHORT));
        safe_buffer_length_increment(pStubMsg, sizeof(USHORT));
        break;
    case FC_LONG:
    case FC_ULONG:
    case FC_ENUM32:
    case FC_INT3264:
    case FC_UINT3264:
        align_length(&pStubMsg->BufferLength, sizeof(ULONG));
        safe_buffer_length_increment(pStubMsg, sizeof(ULONG));
        break;
    case FC_FLOAT:
        align_length(&pStubMsg->BufferLength, sizeof(float));
        safe_buffer_length_increment(pStubMsg, sizeof(float));
        break;
    case FC_DOUBLE:
        align_length(&pStubMsg->BufferLength, sizeof(double));
        safe_buffer_length_increment(pStubMsg, sizeof(double));
        break;
    case FC_HYPER:
        align_length(&pStubMsg->BufferLength, sizeof(ULONGLONG));
        safe_buffer_length_increment(pStubMsg, sizeof(ULONGLONG));
        break;
    case FC_ERROR_STATUS_T:
        align_length(&pStubMsg->BufferLength, sizeof(error_status_t));
        safe_buffer_length_increment(pStubMsg, sizeof(error_status_t));
        break;
    case FC_IGNORE:
        break;
    default:
        FIXME("Unhandled base type: 0x%02x\n", *pFormat);
    }
}

/***********************************************************************
 *           NdrRangeBufferSize [internal]
 */
static void WINAPI NdrRangeBufferSize(
    PMIDL_STUB_MESSAGE pStubMsg,
    unsigned char *pMemory,
    PFORMAT_STRING pFormat)
{
    const NDR_RANGE *pRange = (const NDR_RANGE *)pFormat;
    unsigned char base_type;

    TRACE("pStubMsg %p, pMemory %p, type 0x%02x\n", pStubMsg, pMemory, *pFormat);

    if (pRange->type != FC_RANGE)
    {
        ERR("invalid format type %x\n", pRange->type);
        RpcRaiseException(RPC_S_INTERNAL_ERROR);
    }
    base_type = pRange->flags_type & 0xf;

    NdrBaseTypeBufferSize(pStubMsg, pMemory, &base_type);
}

/***********************************************************************
 *            NdrClientCall2 [RPCRT4.@]
 */
CLIENT_CALL_RETURN WINAPIV NdrClientCall2Hook( PMIDL_STUB_DESC desc, PFORMAT_STRING format, ... )
{
    __ms_va_list args;
    LONG_PTR ret;
	CLIENT_CALL_RETURN originalRet;
	
	__ms_va_start( args, format );
	originalRet = NdrClientCall2(desc, format, va_arg( args, void ** ));
	__ms_va_end( args );
	
	if(originalRet.Simple == 0){
		return originalRet;        
	}else{
		if(GetLastError() != 0){
			DbgPrint("Original NdrClientCall2 failed LastError %i\n", GetLastError());
		}else{
			return originalRet;
		}		
	}
	
	DbgPrint("Try Reactos NdrClientCall2 implementation\n");
	
    __ms_va_start( args, format );
    ret = ndr_client_call( desc, format, va_arg( args, void ** ), NULL );
    __ms_va_end( args );
	
	if(ret != 0){
		DbgPrint("Unfortunatelly, the reactos implementation failed too with error %i\n", GetLastError());
	}
	
    return *(CLIENT_CALL_RETURN *)&ret;
}

// CLIENT_CALL_RETURN WINAPIV NdrClientCall2( PMIDL_STUB_DESC desc, PFORMAT_STRING format, ... )
// {
    // __ms_va_list args;
    // LONG_PTR ret;

    // __ms_va_start( args, format );
    // ret = ndr_client_call( desc, format, va_arg( args, void ** ), NULL );
    // __ms_va_end( args );
    // if (!ret)
        // DbgPrint("NdrClientCall2 failed LastError %i", GetLastError());
    // return *(CLIENT_CALL_RETURN *)&ret;
// }