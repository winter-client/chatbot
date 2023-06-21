// C program to create loading bar
#include <stdio.h>
#include <windows.h>

// Function to creating loading bar
void loading_bar()
{
    int i, j, k;
    // Position Saving Text
    printf("Saving");

    // Print how many dots behind Saving
    for (i = 1; i <= 6; i++)
    {
        // Time delay to print
        for (j = 0; j < 10000000; j++)
        {
            k = j;
        }
        printf(".");
    }
}
