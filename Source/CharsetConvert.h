#ifndef _CHARSET_CONVERT_H_
#define _CHARSET_CONVERT_H_

class CharsetConvert
{
public:
    static bool UTF16ToMultiByte(unsigned char *pUTF16, int nUTF16Size, unsigned char *pMultiByte, int *pMultiByteSize, int nCodePage);
    static bool MultiByteToUTF16(unsigned char *pMultiByte, int nMultiByteSize, unsigned char *pUTF16, int *pUTF16Size, int nCodePage);
    static const char *GetCharsetName(int nCodePage);
private:
    CharsetConvert();
    ~CharsetConvert();
public:
    static const int UTF8_CODEPAGE_ID;
    static const int GB18030_CODEPAGE_ID;
};

#endif//_CHARSET_CONVERT_H_
