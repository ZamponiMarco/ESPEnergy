#ifndef microSdCard_h
#define microSdCard_h

#include <mySD.h>

void initializeSd();
void writeToSd(String path, String toWritePassword, String toWriteUsername);
void printDirectory(File dir, int numTabs);
//String readFromSd(String password);


#endif
