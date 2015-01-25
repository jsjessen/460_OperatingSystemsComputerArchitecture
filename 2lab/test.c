#include <stdio.h>

int main()
{
    int arr[10];

    for(int i = 0; i < 10; i++)
        arr[i] = i;

    int a = 2;
    int b = 4;
    int c = 6;

    int* ptr = arr;

    printf("%d\n", *ptr++);
    printf("%d\n", *ptr++);
    printf("%d\n", *ptr++);
}
