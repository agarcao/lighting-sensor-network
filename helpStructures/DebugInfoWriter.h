#ifndef DEBUGINFOWRITER_H_
#define DEBUGINFOWRITER_H_

#include <iostream>
#include <fstream>
using namespace std;

class DebugInfoWriter {
 private:
	static ofstream theFile;
	static string fileName;

 public:
	DebugInfoWriter(const string & fName);
	static void setDebugFileName(const string & fName);
	static ofstream & getStream(void);
	static void closeStream(void);
};

#endif				/*DEBUGINFOWRITER_H_ */
