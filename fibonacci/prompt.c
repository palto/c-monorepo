#include <stdio.h>
#include "prompt.h"

int prompt_for_number()
{
    int number = -1;
    while (number < 0)
    {
        fprintf(stderr, "How many numbers to generate?: ");
        char input[20];
        if (!fgets(input, sizeof(input), stdin))
        {
            return 0;
        }
        int retval = sscanf(input, "%d", &number);
        if (retval != 1)
        {
            fprintf(stderr, "Numbers only!\n");
        }
        else if (number < 0)
        {
            fprintf(stderr, "Enter positive number please!\n");
        }
        else
        {
            break;
        }
    }
    return number;
}