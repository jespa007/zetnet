#pragma once

namespace zetnet{

class CUri{

public:

	static std::string escape(const string& s, const unsigned char mask=0xff);
	static std::string unescape(const string& s);

};

};
