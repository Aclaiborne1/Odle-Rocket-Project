#ifndef OpenLogH
#define OpenLogH

void write_data(float time, int altitude, int Xgees, int Ygees, int Zgees);
void display_data(String fileName);
void setupOpenLog(void);
void createFile(char *fileName);
void deleteFile(char *fileName);
void cleanup(void);
void closeOpenLog(void);

#endif
