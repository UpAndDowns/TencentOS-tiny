/*
 * Copyright (c) 2017-2019 Tencent Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "qcloud.h"

char *LITE_format_string(const char *fmt, ...)
{
#define TEMP_STRING_MAXLEN      (512)

    va_list         ap;
    char           *tmp = NULL;
    char           *dst;
    int             rc = -1;

    va_start(ap, fmt);
    tmp = osal_malloc(TEMP_STRING_MAXLEN);
    if(NULL == tmp)
    {
        return NULL;
    }
    memset(tmp, 0, TEMP_STRING_MAXLEN);
    rc = osal_vsnprintf(tmp, TEMP_STRING_MAXLEN, fmt, ap);
    va_end(ap);
    LITE_ASSERT(tmp);
    LITE_ASSERT(rc < 1024);

    dst = LITE_strdup(tmp);
    osal_free(tmp);

    return dst;

#undef TEMP_STRING_MAXLEN
}

char *LITE_format_nstring(const int len, const char *fmt, ...)
{
    va_list         ap;
    char           *tmp = NULL;
    char           *dst;
    int             rc = -1;

    va_start(ap, fmt);
    tmp = osal_malloc(len+2);
    if(NULL == tmp)
    {
        return NULL;
    }
    memset(tmp, 0, len+2);
    rc = osal_vsnprintf(tmp, len+1, fmt, ap);
    va_end(ap);
    LITE_ASSERT(tmp);
    LITE_ASSERT(rc < 1024);

    dst = osal_malloc(len + 1);
    osal_snprintf(dst, (len + 1), "%s", tmp);
    osal_free(tmp);

    return dst;
}

char *LITE_strdup(const char *src)
{
    int             len = 0;
    char           *dst = NULL;

    if (!src) {
        return NULL;
    }
    len = strlen(src) + 1;
    if (len > 1024) {
        QCLOUD_LOG_E("Too long string to duplicate, abort! len = %d", len);
        return NULL;
    }

    dst = (char *)osal_malloc(sizeof(char) * len);
    if (!dst) {
        return NULL;
    }
    strncpy(dst, src, len);

    return dst;
}

void LITE_hexbuf_convert(unsigned char *digest, char *out, int in_len, int uppercase)
{
    static char    *zEncode[] = {"0123456789abcdef", "0123456789ABCDEF"};
    int             j = 0;
    int             i = 0;
    int             idx = uppercase ? 1 : 0;

    for (i = 0; i < in_len; i ++) {
        int         a = digest[i];

        out[j++] = zEncode[idx][(a >> 4) & 0xf];
        out[j++] = zEncode[idx][a & 0xf];
    }
}

static uint8_t _hexval_of_char(char hex)
{
    if (LITE_isdigit(hex)) {
        return (hex - '0');
    }
    if (hex >= 'a' && hex <= 'f') {
        return (hex - 'a' + 10);
    }
    if (hex >= 'A' && hex <= 'F') {
        return (hex - 'A' + 10);
    }

    return 0;
}

void LITE_hexstr_convert(char *hexstr, uint8_t *out_buf, int in_len)
{
    int             i = 0;
    uint8_t         ch0, ch1;

    if (in_len % 2 != 0) {
        QCLOUD_LOG_E("hexstr length (%d) is not even", in_len);
        return;
    }

    while (i < in_len) {
        ch0 = _hexval_of_char((char)hexstr[2 * i]);
        ch1 = _hexval_of_char((char)hexstr[2 * i + 1]);
        out_buf[i] = (ch0 << 4 | ch1);
        i++;
    }
}

void LITE_replace_substr(char originalString[], char key[], char swap[])
{
    int         lengthOfOriginalString, lengthOfKey, lengthOfSwap, i, j, flag;
    char        tmp[512];

    lengthOfOriginalString = strlen(originalString);
    lengthOfKey = strlen(key);
    lengthOfSwap = strlen(swap);

    for (i = 0; i <= lengthOfOriginalString - lengthOfKey; i++) {
        flag = 1;
        for (j  = 0; j < lengthOfKey; j++) {
            if (originalString[i + j] != key[j]) {
                flag = 0;
                break;
            }
        }

        if (flag) {
            strcpy(tmp, originalString);
            strcpy(&tmp[i], swap);
            strcpy(&tmp[i + lengthOfSwap], &originalString[i  + lengthOfKey]);
            strcpy(originalString, tmp);
            i += lengthOfSwap - 1;
            lengthOfOriginalString = strlen(originalString);
        }
    }
}
