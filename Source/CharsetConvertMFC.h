#ifndef _CHARSET_CONVERTMFC_H_
#define _CHARSET_CONVERTMFC_H_

#include "CharsetConvertSTD.h"

class CharsetConvertMFC
{
public:
    static CStringA CharsetStreamToCStringA(CharsetStream strData);
    static CStringW CharsetStreamToCStringW(CharsetStream strData);
public:
    static CStringA UTF16ToUTF8(CStringW strUTF16);
    static std::string UTF16ToUTF8StdString(CStringW strUTF16);
    static CStringA UTF16ToGB18030(CStringW strUTF16);
    static CStringW UTF8ToUTF16(CStringA strUTF8);
    static CStringA UTF8ToGB18030(CStringA strUTF8);
    static CStringW GB18030ToUTF16(CStringA strGB18030);
    static CStringA GB18030ToUTF8(CStringA strGB18030);
private:
    CharsetConvertMFC();
    ~CharsetConvertMFC();
};

#endif //_CHARSET_CONVERTMFC_H_
