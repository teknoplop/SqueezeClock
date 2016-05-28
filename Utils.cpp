#include "Utils.h"

// http://stackoverflow.com/a/14530993
std::string 
Utils::urldecode( const char *src )
{
    std::string dst;
         char a, b;
        while (*src) {
                if ((*src == '%') &&
                    ((a = src[1]) && (b = src[2])) &&
                    (isxdigit(a) && isxdigit(b))) {
                        if (a >= 'a')
                                a -= 'a'-'A';
                        if (a >= 'A')
                                a -= ('A' - 10);
                        else
                                a -= '0';
                        if (b >= 'a')
                                b -= 'a'-'A';
                        if (b >= 'A')
                                b -= ('A' - 10);
                        else
                                b -= '0';
                        dst += char( 16*a+b );
                        src+=3;
                } else {
                        dst+= char( *src++ );
                }
        }
    return dst;
}
