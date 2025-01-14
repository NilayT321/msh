#ifndef DIRNAVIGATING_H


void gotoHome(char* wd, int* wd_end);
void cd(char** argv, int argc, char* wd, int* wd_end);
void toParentDirectory(char** argv, char* wd, int* wd_end);
void resizeWD(char* wd, int* wd_end);
void updateWD(char** argv, char* wd, int* wd_end);

#endif
