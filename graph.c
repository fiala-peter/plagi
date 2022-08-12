#include <stdio.h>
#include <string.h>

int strmod(char *str)
{
	char *b = strstr(str, ".c");
	if (b != NULL)
		*b = '\0';
	b = strchr(str, '.');
	if (b != NULL)
	{
		*b = *(b + 1);
		*(b + 1) = '\0';
	}
	int a;
	sscanf(str, "%d", &a);
	return a;
}

int main(int argc, char *argv[])
{
	FILE *fin, *fout;

	if (argc < 3)
	{
		printf("Usage: %s input output list\n", argv[0]);
		return 1;
	}

	int limit = 90;

	fin = fopen(argv[1], "r");
	fout = fopen(argv[2], "w");

	enum
	{
		BUFSIZE = 512
	};
	char linebuf[BUFSIZE];

	fprintf(fout, "graph G {\n");

	// int list[1000] = {0};
	while (fgets(linebuf, BUFSIZE, fin) != NULL)
	{
		char from[BUFSIZE], to[BUFSIZE];
		char *color;
		int percent;
		int cnt = sscanf(linebuf, "%s consists for %d %% of %s material", from, &percent, to);
		// int ifrom = strmod(from);
		// int ito = strmod(to);
		// if (percent > 50)
		// {
		// 	list[ifrom]++;
		// 	list[ito]++;
		// }
		if (percent > 70)
			color = "red";
		else if (percent > 50)
			color = "blue";
		else if (percent > 30)
			color = "green";
		if (cnt == 3 && percent > limit)
			fprintf(fout, "\t%s -- %s[label=%2d,color=\"%s\"];\n", from, to, percent, color);
	}

	fprintf(fout, "}\n");

	fclose(fin);
	fclose(fout);

	// fout = fopen(argv[3], "w");
	// for (int i = 0; i < 1000; i++)
	// 	if (list[i] != 0)
	// 		fprintf(fout, "%d\n", i);
	// fclose(fout);

	return 0;
}
