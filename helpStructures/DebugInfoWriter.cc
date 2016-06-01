#include "DebugInfoWriter.h"

ofstream DebugInfoWriter::theFile;
string DebugInfoWriter::fileName;

DebugInfoWriter::DebugInfoWriter(const string & fName)
{
	fileName = fName;
}

void DebugInfoWriter::setDebugFileName(const string & fName)
{
	fileName = fName;
}

ofstream & DebugInfoWriter::getStream(void)
{
	if (!theFile.is_open())
		theFile.open(fileName.c_str(), ios::app);
	theFile.setf(ios::left);

	return theFile;
}

void DebugInfoWriter::closeStream(void)
{
	if (theFile.is_open())
		theFile.close();
}
