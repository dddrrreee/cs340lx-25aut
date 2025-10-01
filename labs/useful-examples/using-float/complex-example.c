// trivial example of using floating point + our simple math
// library.
#include "rpi.h"
#include "rpi-math.h"
#include <complex.h>

void notmain(void) {
    double complex c = CMPLX(2,3);
    double complex squared = c * c;
    double complex scaled = c * 2;

    printk("complex: (%f,%f)^2 = (%f,%f).\n", 
            creal(c), cimag(c),
            creal(squared), cimag(squared));

    printk("complex: 2*(%f,%f) = (%f,%f).\n", 
            creal(c), cimag(c),
            creal(scaled), cimag(scaled));
}
