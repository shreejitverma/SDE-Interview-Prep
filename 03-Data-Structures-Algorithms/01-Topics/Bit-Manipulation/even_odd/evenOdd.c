/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

#include <stdio.h>
int main()
{
	int n;
	printf("Enter any number: ");
	scanf("%d", &n);
	(n&1)?printf("Number is Odd.\n"):printf("Number is even.\n");
}