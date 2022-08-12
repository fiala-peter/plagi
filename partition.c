#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

enum
{
	LINE_SIZE = 1024,
	MAX_NODE = 1000,
	MAX_EDGE = 1000
};

typedef struct
{
	char neptun[7];
	int color;
} node_t;

node_t nodes[MAX_NODE];
int num_nodes = 0;

int edges[MAX_EDGE][2];
int num_edges = 0;

int neptun_to_id(char *neptun)
{
	for (int i = 0; i < num_nodes; ++i)
		if (strcmp(neptun, nodes[i].neptun) == 0)
			return i;
	strcpy(nodes[num_nodes].neptun, neptun);
	num_nodes++;
	return num_nodes - 1;
}

void add_edge(int from, int to)
{
	edges[num_edges][0] = from;
	edges[num_edges][1] = to;
	num_edges++;
}

void color_neighbors(int src)
{
	int c = nodes[src].color;
	for (int e = 0; e < num_edges; e++)
	{
		int pair;
		if (edges[e][0] == src)
			pair = edges[e][1];
		else if (edges[e][1] == src)
			pair = edges[e][0];
		else
			continue;
		if (nodes[pair].color == 0)
		{
			nodes[pair].color = c;
			color_neighbors(pair);
		}
	}
}

int color(void)
{
	int c = 0;
	for (int i = 0; i < num_nodes; ++i)
	{
		if (nodes[i].color == 0)
		{
			c++;
			nodes[i].color = c;
			color_neighbors(i);
		}
	}
	return c;
}

void sort_files(char *sol_name, int num_colors)
{
	char *from_path = "./";

	for (int c = 1; c <= num_colors; c++)
	{
		char to_path[210];
		sprintf(to_path, "%s/part%02d", from_path, c);

		mkdir(to_path, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH);

		for (int i = 0; i < num_nodes; ++i)
			if (nodes[i].color == c)
			{
				char command[500];
				sprintf(command, "cp %s/%s.c %s/%s.c", from_path, nodes[i].neptun, to_path, nodes[i].neptun);
				system(command);
			}
	}
}

// partition solution_name
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Usage: partition solution_name\n");
		return 0;
	}

	char *solution_name = argv[1];
	char gv_name[30];
	sprintf(gv_name, "%s.gv", solution_name);

	FILE *gv = fopen(gv_name, "r");
	char line[LINE_SIZE];
	while (fgets(line, LINE_SIZE, gv) != NULL)
	{
		char from_neptun[7], to_neptun[7];
		if (sscanf(line, "%6s -- %6s", from_neptun, to_neptun) == 2)
			add_edge(neptun_to_id(from_neptun), neptun_to_id(to_neptun));
	}
	fclose(gv);

	int num_colors = color();

	sort_files(solution_name, num_colors);

	return 0;
}
