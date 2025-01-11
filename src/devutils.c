#include <stdio.h>
#include "../include/devutils.h"


void printArray(char** arr, int size) {
		for (int i = 0; i < size; i++) {
				printf("\"%s\" ", arr[i]);
		}
		printf("\n");
}
