#include "stdafx.h"
#include <string>
#include <iostream>

#include "MyDebug.h"

// Custom debugger that prints messages on a txt file

TxtDebugFile::TxtDebugFile(std::string const &txtPath, bool overwrite) {
	if (overwrite == false) {
		this->inFile.open(txtPath, std::ofstream::app);
	}
	else
	{
		this->inFile.open(txtPath);
	}
	if (!inFile) {
		std::cout << "Unable to open file datafile.txt" << '\n';
		return;
	}
	
}

void TxtDebugFile::WriteOnTxt(std::string text) {
	try
	{
		this->inFile << text << '\n';
	}
	catch (const std::exception&)
	{
		std::cout << "Error when trying to write on txt file. " << '\n';
	}
	
}