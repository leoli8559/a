#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <umsdUtil.h>
#ifdef OS_WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int boolarrayToInt(MSD_BOOL array[], int size)
{
    int result = 0;
    for (int i = 0; i < size; i++) {
        if (array[i]) {
            result |= (1 << i);
        }
    }
    return result;
}

int numberIsInArr(int* arr, int arrSize, int num)
{
    for (int i = 0; i < arrSize; i++) {
        if (arr[i] == num) {
            return i;
        }
    }
    return -1;
}



