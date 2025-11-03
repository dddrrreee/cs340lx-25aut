#include "rpi.h"

int sum_range(int start, int end) {
    int sum = 0;
    int i;

    for (i = start; i <= end; i++) {
        sum += i;
    }

    return sum;
}

void notmain(void) {
    int result = sum_range(1, 5);
    printk("Sum from 1 to 5 is: %d\n", result);
}
