#include <stdio.h> 

int main() {
    int a = 5; 
    int* b = &a;
    int c = *b;
    printf("%d\n", c);
    c = 10;
    printf("c: %d, a: %d\n", c, a);
    
    return 0;
}