/*
 * Copyright 2014 Henri Verbeet for CodeWeavers
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

#define D2D1_INIT_GUID
#include "stdio.h"

#include "xmllite.h"
#include "wine/list.h"
#include "math.h"

WINE_DECLARE_DEBUG_CHANNEL(winediag);
WINE_DEFAULT_DEBUG_CHANNEL(d2d);

typedef struct D2D_MATRIX_3X2_F
{
    union
    {
        struct
        {
            float m11;
            float m12;
            float m21;
            float m22;
            float dx;
            float dy;
        };

        struct
        {
            float _11, _12;
            float _21, _22;
            float _31, _32;
        };

        float m[3][2];
    };
} D2D_MATRIX_3X2_F;

typedef D2D_MATRIX_3X2_F D2D1_MATRIX_3X2_F;

float WINAPI D2D1ComputeMaximumScaleFactor(const D2D1_MATRIX_3X2_F *matrix)
{
    const float (*m)[2] = matrix->m;
    float a1, a2, c;

    //TRACE("matrix %p.\n", matrix);

    /* 2x2 matrix, _31 and _32 are ignored. */
    a1 = m[0][0] * m[0][0] + m[1][0] * m[1][0];
    a2 = m[0][1] * m[0][1] + m[1][1] * m[1][1];
    c = m[0][0] * m[0][1] + m[1][0] * m[1][1];

    /* Maximum scale factor of matrix M refers to maximum value of |Mv|/|v| over all vectors v, where |.| is
     * vector length. That is defined as matrix spectral norm. Spectral norm equals to the maximum of the
     * singular values s1, s2 for 2x2 matrix M.
     * s_i^2 = e_i where e_i (e1, e2) are eigenvalues of (transpose(M) * M)
     * e1 + e2 = trace(transpose(M) * M) = a1 + a2
     * e1 * e2 = det(transpose(M) * M) = a1 * a2 - c ^ 2. */
    return sqrtf(0.5f * (a1 + a2 + sqrtf((a1 - a2) * (a1 - a2) + 4 * c * c)));
}
