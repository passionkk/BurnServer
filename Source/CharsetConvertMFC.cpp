#include "CharsetConvertMFC.h"
#include "CharsetConvertSTD.h"

CharsetConvertMFC::CharsetConvertMFC()
{

}

CharsetConvertMFC::~CharsetConvertMFC()
{

}

CStringA CharsetConvertMFC::CharsetStreamToCStringA(CharsetStream strData)
{
    CStringA strReturn;

    if (strData.size() > 0)
    {
        char *pData = new char[strData.size()+1];
        if (pData != NULL)
        {
            memset(pData, 0, strData.size()+1);
            memcpy(pData, &strData[0], strData.size());
            strReturn = CStringA(pData);
            delete []pData;
        }
    }

    return strReturn;
}

CStringW CharsetConvertMFC::CharsetStreamToCStringW(CharsetStream strData)
{
    CStringW strReturn;

    if (strData.size() > 0)
    {
        wchar_t *pData = (wchar_t*)new char[strData.size()+sizeof(wchar_t)];
        if (pData != NULL)
        {
            memset(pData, 0, strData.size()+sizeof(wchar_t));
            memcpy(pData, &strData[0], strData.size());
            strReturn = CStringW(pData);
            delete []pData;
        }
    }

    return strReturn;
}

CStringA CharsetConvertMFC::UTF16ToUTF8(CStringW strUTF16)
{
    CharsetStream strUTF8;

    CharsetConvertSTD::UTF16ToUTF8(CharsetConvertSTD::ConstructCharsetStream((unsigned char *)strUTF16.GetBuffer(), 
        strUTF16.GetLength()*sizeof(wchar_t)), strUTF8);

    return CharsetStreamToCStringA(strUTF8);
}

std::string CharsetConvertMFC::UTF16ToUTF8StdString(CStringW strUTF16)
{
    return std::string(UTF16ToUTF8(strUTF16).GetBuffer());
}

CStringA CharsetConvertMFC::UTF16ToGB18030(CStringW strUTF16)
{
    CharsetStream strGB18030;

    CharsetConvertSTD::UTF16ToGB18030(CharsetConvertSTD::ConstructCharsetStream((unsigned char *)strUTF16.GetBuffer(), 
        strUTF16.GetLength()*sizeof(wchar_t)), strGB18030);

    return CharsetStreamToCStringA(strGB18030);

}

CStringW CharsetConvertMFC::UTF8ToUTF16(CStringA strUTF8)
{
    CharsetStream strUTF16;

    CharsetConvertSTD::UTF8ToUTF16(CharsetConvertSTD::ConstructCharsetStream((unsigned char *)strUTF8.GetBuffer(),
        strUTF8.GetLength()), strUTF16);

    return CharsetStreamToCStringW(strUTF16);
}

CStringA CharsetConvertMFC::UTF8ToGB18030(CStringA strUTF8)
{
    CharsetStream strGB18030;

    CharsetConvertSTD::UTF8ToGB18030(CharsetConvertSTD::ConstructCharsetStream((unsigned char *)strUTF8.GetBuffer(),
        strUTF8.GetLength()), strGB18030);

    return CharsetStreamToCStringA(strGB18030);
}

CStringW CharsetConvertMFC::GB18030ToUTF16(CStringA strGB18030)
{
    CharsetStream strUTF16;

    CharsetConvertSTD::GB18030ToUTF16(CharsetConvertSTD::ConstructCharsetStream((unsigned char *)strGB18030.GetBuffer(),
        strGB18030.GetLength()), strUTF16);

    return CharsetStreamToCStringW(strUTF16);
}

CStringA CharsetConvertMFC::GB18030ToUTF8(CStringA strGB18030)
{
    CharsetStream strUTF8;

    CharsetConvertSTD::GB18030ToUTF8(CharsetConvertSTD::ConstructCharsetStream((unsigned char *)strGB18030.GetBuffer(),
        strGB18030.GetLength()), strUTF8);

    return CharsetStreamToCStringA(strUTF8);
}
