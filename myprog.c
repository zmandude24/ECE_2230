#include <stdlib.h>
void fun(void) {
    int *x = (int *) malloc(10 * sizeof(int));
    x[1] = 0;
    if (x[1] == 0) x[0] = 1;
    free(x);
}
int main(void)
{
    fun();
    return 0;
}
