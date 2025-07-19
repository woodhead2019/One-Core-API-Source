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

#include <main.h>

static const INPUT_MESSAGE_SOURCE msg_source_unavailable = { IMDT_UNAVAILABLE, IMO_UNAVAILABLE };

/***********************************************************************
 *		GetCurrentInputMessageSource (USER32.@)
 */
BOOL WINAPI GetCurrentInputMessageSource(PINPUT_MESSAGE_SOURCE MessageSource)
{
    if (!MessageSource) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    
    // Most OSS applications I have seen just check this for touch input so it should be fine
    MessageSource->DeviceType = IMDT_UNAVAILABLE;
    // To be better compatible with applications. The only real uses of OriginId I have seen are for not processing injected input
    // (or game anticheat) so IMO_HARDWARE is the most compatible with those applications
    MessageSource->OriginId = IMO_HARDWARE;
    return TRUE;
}