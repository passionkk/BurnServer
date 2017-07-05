#ifndef _CHARSET_CONVERTSTD_H_
#define _CHARSET_CONVERTSTD_H_

#include <vector>
#include <string>

typedef std::vector<unsigned char> CharsetStream;

class CharsetConvertSTD
{
public:
    static CharsetStream ConstructCharsetStream(const unsigned char *pData, int nDataSize);
    static std::string CharsetStreamToString(CharsetStream strData);
public:
    static bool UTF16ToUTF8(CharsetStream strUTF16, CharsetStream &strUTF8);
    static bool UTF16ToGB18030(CharsetStream strUTF16, CharsetStream &strGB18030);
    static bool UTF8ToUTF16(CharsetStream strUTF8, CharsetStream &strUTF16);
    static bool UTF8ToGB18030(CharsetStream strUTF8, CharsetStream &strGB18030);
    static bool GB18030ToUTF16(CharsetStream strGB18030, CharsetStream &strUTF16);
    static bool GB18030ToUTF8(CharsetStream strGB18030, CharsetStream &strUTF8);
private:
    CharsetConvertSTD();
    ~CharsetConvertSTD();
private:
    static const int MULTIPLIER;
};

#endif //_CHARSET_CONVERTSTD_H_
