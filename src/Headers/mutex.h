#ifdef MUTEX_H
#define MUTEX_H

#include "structs.h"

mutex* chooseMutex(char* arg1);

void initialize_mutex();

#endif