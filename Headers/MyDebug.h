#pragma once
#ifndef _FSTREAM_
#include <fstream>
#endif // _fstream_


class TxtDebugFile {
	std::ofstream inFile;
	public:
	TxtDebugFile(std::string const &txtPath, bool overwrite);
	void WriteOnTxt(std::string text);
};
