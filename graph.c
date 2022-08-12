#include <stdio.h>
#include <string.h>

void trim_extension(char *str)
{
	char *b = strstr(str, ".c");
	if (b != NULL)
		*b = '\0';
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
		trim_extension(from);
		trim_extension(to);
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

	return 0;
}
