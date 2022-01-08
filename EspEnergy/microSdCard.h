#ifndef microSdCard_h
#define microSdCard_h

#include <mySD.h>

void writeToSd(String path, String toWrite);
void printDirectory(File dir, int numTabs);
//String readFromSd(String password);


#endif
