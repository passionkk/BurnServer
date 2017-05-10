#include "stdafx.h"
#include "CharsetConvertSTD.h"
#include "CharsetConvert.h"

const int CharsetConvertSTD::MULTIPLIER = 16;

CharsetConvertSTD::CharsetConvertSTD()
{
    //
}

CharsetConvertSTD::~CharsetConvertSTD()
{
    //
}

CharsetStream CharsetConvertSTD::ConstructCharsetStream(const unsigned char *pData, int nDataSize)
{
    CharsetStream   strData;

    if (
        (pData != NULL)
        && (nDataSize > 0)
        )
    {
        strData.resize(nDataSize, 0);
        memcpy(&strData[0], pData, nDataSize);
    }

    return strData;
}

std::string CharsetConvertSTD::CharsetStreamToString(CharsetStream strData)
{
    std::string strReturn;

    if (strData.size() > 0)
    {
        char *pData = new char[strData.size()+1];
        if (pData != NULL)
        {
            memset(pData, 0, strData.size()+1);
            memcpy(pData, &strData[0], strData.size());
            strReturn = std::string(pData);
            delete []pData;
        }
    }

    return strReturn;
}

bool CharsetConvertSTD::UTF16ToUTF8(CharsetStream strUTF16, CharsetStream &strUTF8)
{
    bool bReturn = false;
    int nUTF8Size = strUTF16.size()*MULTIPLIER;
    unsigned char *pUTF8;;

    if (strUTF16.size() > 0)
    {
        pUTF8 = new unsigned char[nUTF8Size];
        if (pUTF8 != NULL)
        {
            memset(pUTF8, 0, nUTF8Size);
            if (CharsetConvert::UTF16ToMultiByte((unsigned char *)&strUTF16[0], strUTF16.size(), 
                pUTF8, &nUTF8Size, CharsetConvert::UTF8_CODEPAGE_ID))
            {
                strUTF8.resize(nUTF8Size, 0);
                memcpy(&strUTF8[0], pUTF8, nUTF8Size);
                bReturn = true;
            }
            delete []pUTF8;
        }
    }

    return bReturn;
}

bool CharsetConvertSTD::UTF16ToGB18030(CharsetStream strUTF16, CharsetStream &strGB18030)
{
    bool bReturn = false;
    int nGB18030Size = strUTF16.size()*MULTIPLIER;
    unsigned char *pGB18030;

    if (strUTF16.size() > 0)
    {
        pGB18030 = new unsigned char[nGB18030Size];
        if (pGB18030 != NULL)
        {
            memset(pGB18030, 0, nGB18030Size);
            if (CharsetConvert::UTF16ToMultiByte((unsigned char *)&strUTF16[0], strUTF16.size(), 
                pGB18030, &nGB18030Size, CharsetConvert::GB18030_CODEPAGE_ID))
            {
                strGB18030.resize(nGB18030Size, 0);
                memcpy(&strGB18030[0], pGB18030, nGB18030Size);
                bReturn = true;
            }
            delete []pGB18030;
        }
    }

    return bReturn;
}

bool CharsetConvertSTD::UTF8ToUTF16(CharsetStream strUTF8, CharsetStream &strUTF16)
{
    bool bReturn = false;
    int nUTF16Size = strUTF8.size()*MULTIPLIER;
    unsigned char *pUTF16;

    if (strUTF8.size() > 0)
    {
        pUTF16 = new unsigned char[nUTF16Size];
        if (pUTF16 != NULL)
        {
            memset(pUTF16, 0, nUTF16Size);
            if (CharsetConvert::MultiByteToUTF16((unsigned char *)&strUTF8[0], strUTF8.size(),
                pUTF16, &nUTF16Size, CharsetConvert::UTF8_CODEPAGE_ID))
            {
                strUTF16.resize(nUTF16Size, 0);
                memcpy(&strUTF16[0], pUTF16, nUTF16Size);
                bReturn = true;
            }
            delete []pUTF16;
        }
    }

    return bReturn;
}

bool CharsetConvertSTD::UTF8ToGB18030(CharsetStream strUTF8, CharsetStream &strGB18030)
{
    CharsetStream   strUTF16;
    if (UTF8ToUTF16(strUTF8, strUTF16))
    {
        if (UTF16ToGB18030(strUTF16, strGB18030))
        {
            return true;
        }
    }

    return false;
}

bool CharsetConvertSTD::GB18030ToUTF16(CharsetStream strGB18030, CharsetStream &strUTF16)
{
    bool bReturn = false;
    int nUTF16Size = strGB18030.size()*MULTIPLIER;
    unsigned char *pUTF16;

    if (strGB18030.size() > 0)
    {
        pUTF16 = new unsigned char[nUTF16Size];
        if (pUTF16 != NULL)
        {
            memset(pUTF16, 0, nUTF16Size);
            if (CharsetConvert::MultiByteToUTF16((unsigned char *)&strGB18030[0], strGB18030.size(),
                pUTF16, &nUTF16Size, CharsetConvert::GB18030_CODEPAGE_ID))
            {
                strUTF16.resize(nUTF16Size, 0);
                memcpy(&strUTF16[0], pUTF16, nUTF16Size);
                bReturn = true;
            }
            delete []pUTF16;
        }
    }

    return bReturn;
}

bool CharsetConvertSTD::GB18030ToUTF8(CharsetStream strGB18030, CharsetStream &strUTF8)
{
    CharsetStream   strUTF16;
    if (GB18030ToUTF16(strGB18030, strUTF16))
    {
        if (UTF16ToUTF8(strUTF16, strUTF8))
        {
            return true;
        }
    }

    return false;
}
