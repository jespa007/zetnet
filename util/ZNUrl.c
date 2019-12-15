#include "zetnet.h"

#define R URL_CHAR_RESERVED
#define U URL_CHAR_UNSAFE
#define E URL_CHAR_ESC
#define A URL_CHAR_AUTH
#define P URL_CHAR_PATH
#define Q URL_CHAR_QUERY
#define F URL_CHAR_FRAGMENT
#define RU R|U
#define X U|E|A|P|Q|F

#define url_char_test(c, mask) (url_char_table[(unsigned char)(c)] & (mask))


/**
 * Url escaping
 */
enum {
	URL_CHAR_RESERVED    = 0x1,
	URL_CHAR_UNSAFE        = 0x2,
	URL_CHAR_ESC        = 0x4,
	URL_CHAR_AUTH        = 0x8,
	URL_CHAR_PATH        = 0x10,
	URL_CHAR_QUERY        = 0x20,
	URL_CHAR_FRAGMENT    = 0x40
};




/*
 * For authority we have to escape / ? #
 * For path: ?#
 * For query: #
 */
static const unsigned char url_char_table[256] =
{
  X,  X,  X,  X,   X,  X,  X,  X,     /* NUL SOH STX ETX  EOT ENQ ACK BEL */
  X,  X,  X,  X,   X,  X,  X,  X,     /* BS  HT  LF  VT   FF  CR  SO  SI  */
  X,  X,  X,  X,   X,  X,  X,  X,     /* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
  X,  X,  X,  X,   X,  X,  X,  X,     /* CAN EM  SUB ESC  FS  GS  RS  US  */
  X,  0,  X, RU|A|P|Q, R, X, R, 0,    /* SP  !   "   #    $   %   &   '   */
  0,  0,  0,  R,   R,  0,  0,  R|A,   /* (   )   *   +    ,   -   .   /   */
  0,  0,  0,  0,   0,  0,  0,  0,     /* 0   1   2   3    4   5   6   7   */
  0,  0, RU,  R,   X,  R,  X,  R|A|P, /* 8   9   :   ;    <   =   >   ?   */
 RU,  0,  0,  0,   0,  0,  0,  0,     /* @   A   B   C    D   E   F   G   */
  0,  0,  0,  0,   0,  0,  0,  0,     /* H   I   J   K    L   M   N   O   */
  0,  0,  0,  0,   0,  0,  0,  0,     /* P   Q   R   S    T   U   V   W   */
  0,  0,  0, RU,   X, RU,  X,  0,     /* X   Y   Z   [    \   ]   ^   _   */
  X,  0,  0,  0,   0,  0,  0,  0,     /* `   a   b   c    d   e   f   g   */
  0,  0,  0,  0,   0,  0,  0,  0,     /* h   i   j   k    l   m   n   o   */
  0,  0,  0,  0,   0,  0,  0,  0,     /* p   q   r   s    t   u   v   w   */
  0,  0,  0,  X,   X,  X,  0,  X,     /* x   y   z   {    |   }   ~   DEL */

  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,

  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
};


char digit_to_xnum(char c)
{
	if( c >= 0 && c <= 15 ){
		return ("0123456789ABCDEF"[(int)c]);
	}

	fprintf(stderr,"digit outside [0,15] range\n");

	return 0;
	/*else
		throw std::logic_error("digit outside [0,15] range");*/
}

char xdigit_to_num(char c) throw(std::logic_error)
{
	if( ! isxdigit(c) ){
		fprintf(stderr,"not an hex digit\n");
		return 0;
	}
	return(c < 'A' ? c - '0' : toupper(c) - 'A' + 10);
}

char x2digits_to_num(char c1, char c2) throw(std::logic_error)
{
	if( ! isxdigit(c1) || ! isxdigit(c2) ){
		fprintf(stderr,"not an hex digit\n");
		return 0;
	}
	return((xdigit_to_num(c1) << 4) + xdigit_to_num(c2));
}

char * ZNURL_Escape(const char * s, const unsigned char mask)
{
	static result[4096]={0};
	if(s.empty())
		return s;
	std::string result;
	std::string::size_type reserve_ahead = s.size();
	for(std::string::const_iterator j = s.begin(); j != s.end(); ++j)  {
		if( url_char_test(*j, mask) ) {
			if(*j == '%' && (j+1) != s.end() && (j+2) != s.end()
				&& isxdigit(*(j+1)) && isxdigit(*(j+2)) ) {
			} else {
				reserve_ahead+=2; // escaping one, augments it by two more
			}
		}
	}
	result.reserve(reserve_ahead);
	for(std::string::const_iterator i = s.begin(); i != s.end(); ++i) {
		if( url_char_test(*i, mask) ) {
			//cout << "test: " << *i << hex <<(int) *i << endl;
			if(*i == '%' && (i+1) != s.end() && (i+2) != s.end()
				&& isxdigit(*(i+1)) && isxdigit(*(i+2)) ) {
				// this is a valid escaped sequence, don't escape the %
				result += *i;
				result += *(i+1);
				result += *(i+2);
				i += 2;
			} else {
				char c1,c2;
				//cout << " (*i) >> 4:" << hex << ((*i) >> 4) <<endl;
				c1 = digit_to_xnum((unsigned char)(*i) >> 4);
				c2 = digit_to_xnum( (*i) & 0xf);
				result += "%";
				result += c1;
				result += c2;
			}
		} else {
			result += (*i);
		}
	}
	result.reserve(); // shrink to fit
	return result;
}


char * ZNUrl_Unescape(const char * s)
{
	static result[4096]={0};
	// avoid copying if there's nothing to unescape
	if( s.empty() || s.find('%') == std::string::npos )
		return s;
	std::string result;
	result.reserve(s.size());
	for(std::string::const_iterator i = s.begin(); i != s.end(); ++i) {
		if( *i == '%' && (i+1) != s.end() && (i+2) != s.end()
		&& isxdigit(*(i+1)) && isxdigit(*(i+2)) //
		&& ((*(i+1)) != 0 || (*(i+2)) != 0)) { // can't unescape null

			char c = x2digits_to_num(*(i+1),*(i+2));
			result += c;
			i += 2; // increment the iterator
		} else
			result += (*i);
	}
	result.reserve(); // shrink to fit
	return result;
}




