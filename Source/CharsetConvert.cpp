#include "CharsetConvert.h"
#include "Log/NetLog.h"
#include <string.h>
#include <stdio.h>
#ifdef WIN32
#define		WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <iconv.h>
#endif //WIN32

const int CharsetConvert::UTF8_CODEPAGE_ID = 65001;
const int CharsetConvert::GB18030_CODEPAGE_ID = 54936;

CharsetConvert::CharsetConvert()
{
    //
}

CharsetConvert::~CharsetConvert()
{
    //
}

bool CharsetConvert::UTF16ToMultiByte(unsigned char *pUTF16, int nUTF16Size, unsigned char *pMultiByte, int *pMultiByteSize, int nCodePage)
{
#ifdef WIN32
    int nBytesNeed;
    bool bReturn;

    bReturn = false;
    if (
        (pUTF16 != NULL)
        && (nUTF16Size > 0)
        && (pMultiByte != NULL)
        && (pMultiByteSize != NULL)
        )
    {
        nBytesNeed = WideCharToMultiByte(nCodePage, 0, (LPCWSTR)pUTF16, nUTF16Size/sizeof(WCHAR), NULL, 0, NULL, NULL);
        if (
            (nBytesNeed > 0)
            && (nBytesNeed < *pMultiByteSize)
            )
        {
            *pMultiByteSize = 0;
            nBytesNeed = WideCharToMultiByte(nCodePage, 0, (LPCWSTR)pUTF16, nUTF16Size/sizeof(WCHAR), (LPSTR)pMultiByte, nBytesNeed, NULL, NULL);
            if (nBytesNeed > 0)
            {
                bReturn = true;
                *pMultiByteSize = nBytesNeed;
            }
        }
    }

    return bReturn;

#else
    iconv_t cd;
    char **ppIn;
    char **ppOut;
    size_t nIn;
    size_t nOut;
    size_t nConvert;
    bool bReturn = false;
    size_t nOriginOut;

    if (
        (pUTF16 != NULL)
        && (nUTF16Size > 0)
        && (pMultiByte != NULL)
        && (pMultiByteSize != NULL)
        )
    {
        //cd = iconv_open(GetCharsetName(nCodePage), "UTF-16LE");
        cd = iconv_open(GetCharsetName(nCodePage), "WCHAR_T");
        if(cd != NULL)
        {
            ppIn = (char **)&pUTF16;
            nIn = nUTF16Size;
            nOriginOut = nOut = *pMultiByteSize;
            ppOut = (char **)&pMultiByte;
            nConvert = iconv(cd, ppIn, &nIn, ppOut, &nOut);
            if (nConvert != (size_t)(-1))
            {// convert success
                bReturn = true;
                *pMultiByteSize = nOriginOut-nOut;
            }
            iconv_close(cd);
        }
    }

    return bReturn;

#endif //WIN32
}

bool CharsetConvert::MultiByteToUTF16(unsigned char *pMultiByte, int nMultiByteSize, unsigned char *pUTF16, int *pUTF16Size, int nCodePage)
{
#ifdef WIN32
    int nBytesNeed;
    bool bReturn;

    bReturn = false;
    if (
        (pMultiByte != NULL)
        && (nMultiByteSize > 0)
        && (pUTF16 != NULL)
        && (pUTF16Size != NULL)
        )
    {
        nBytesNeed = MultiByteToWideChar(nCodePage, 0, (LPCSTR)pMultiByte, nMultiByteSize, NULL, 0);
        if (
            (nBytesNeed*sizeof(WCHAR) > 0)
            && ((int)(nBytesNeed*sizeof(WCHAR)) < *pUTF16Size)
            )
        {
            *pUTF16Size = 0;
            nBytesNeed = MultiByteToWideChar(nCodePage, 0, (LPCSTR)pMultiByte, nMultiByteSize, (LPWSTR)pUTF16, nBytesNeed);
            if (nBytesNeed > 0)
            {
                bReturn = true;
                *pUTF16Size = nBytesNeed*sizeof(WCHAR);
            }
        }
    }

    return bReturn;
#else
    iconv_t cd;
    char **ppIn;
    char **ppOut;
    size_t nIn;
    size_t nOut;
    size_t nConvert;
    bool bReturn = false;
    size_t nOriginOut;

    if (
        (pMultiByte != NULL)
        && (nMultiByteSize > 0)
        && (pUTF16 != NULL)
        && (pUTF16Size != NULL)
        )
    {
        cd = iconv_open("WCHAR_T", GetCharsetName(nCodePage));
        //cd = iconv_open("UTF-16LE", GetCharsetName(nCodePage));
        if(cd != NULL)
        {
            ppIn = (char **)&pMultiByte;
            nIn = nMultiByteSize;
            nOriginOut = nOut = *pUTF16Size;
            ppOut = (char **)&pUTF16;
            nConvert = iconv(cd, ppIn, &nIn, ppOut, &nOut);
            if (nConvert != (size_t)(-1))
            {// convert success
                bReturn = true;
                *pUTF16Size = nOriginOut-nOut;
            }
            else
            {
                g_NetLog.Debug("iconv failed\n");
            }
            iconv_close(cd);
        }
        else
        {
            g_NetLog.Debug("iconv_open is null\n");
        }
    }

    return bReturn;

#endif //WIN32
}

const char * CharsetConvert::GetCharsetName(int nCodePage)
{
    if (nCodePage == UTF8_CODEPAGE_ID)
    {
        return  "UTF-8";
    }
    else if (nCodePage == GB18030_CODEPAGE_ID)
    {
        return  "GB18030";
    }

    return  "UTF-8";
}
