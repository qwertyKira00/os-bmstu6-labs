#include <stdio.h>
#include <fcntl.h>
#define ALPHABET "Abcdefghijklmnopqrstuvwxyz"
#define ALPHABET_SIZE 26

int main()
{
	FILE *fd1 = fopen("proc3_alphabet.txt", "w");
	FILE *fd2 = fopen("proc3_alphabet.txt", "w");

	for (int i = 0; i < ALPHABET_SIZE; i++)
	{
		if (ALPHABET[i] % 2)
			fprintf(fd1, "%c", ALPHABET[i]);
		else
			fprintf(fd2, "%c", ALPHABET[i]);
	}

	fclose(fd1);
	fclose(fd2);

	return 0;
}
