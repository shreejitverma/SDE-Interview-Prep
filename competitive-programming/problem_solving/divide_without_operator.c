// Given two integers, you have to divide without using the division operator.
#include <stdio.h>

struct answer {
	int remainder;
	int quotient;
};

struct answer divide(int a, int b) {
	struct answer result;

	if (b == 0) {
		result.quotient = 0;
		result.remainder = 0;
		return result;
	}
	while (a >= b) {
		result.quotient++;
		a = a - b;
	}
	result.remainder = a;
	return result;
}

int main() {
	struct answer res = divide(10, 2);
	printf("Divide answer: %d %d\n", res.quotient, res.remainder);
	res = divide(100, 7);
	printf("Divide answer: %d %d\n", res.quotient, res.remainder);
}