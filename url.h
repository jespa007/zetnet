#pragma once

namespace zetnet{

	namespace url{
		std::string escape(const std::string& s, const unsigned char mask=0xff);
		std::string unescape(const std::string& s);
	};

};
