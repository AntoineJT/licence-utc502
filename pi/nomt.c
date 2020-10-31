#include <stdio.h>
#include <stdlib.h>

#define MULTITHREAD 1

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        printf("Usage: %s [intervals]\n"
               "Computes a pi estimation\n", argv[0]);
        return EXIT_FAILURE;
    }

    // atoi -> interprets a str to int
    int intervals = atoi(argv[1]);
    double width = 1.0 / intervals;

    double sum = 0;

    for (int i = 0; i < intervals; ++i)
    {
        double x = (i + 0.5) * width;
        sum += 4.0 / (1.0 + x * x);
    }

    sum *= width;
    printf("Pi estimation - %10.9f\n", sum);

    return EXIT_SUCCESS;
}
