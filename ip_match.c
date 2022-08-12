/**
 * @file ip_match.c
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief Match IP addresses of students
 * @version 0.1
 * @date 2020-11-14
 * 
 * @copyright Copyright (c) 2020
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JSON_TOOLS_DEFINITIONS
#include "json_tools.h"

enum
{
	NUM_EVENTS = 6,
	MAX_LINE = 1024,
	BUFSIZE = MAX_LINE 
};

typedef struct l
{
	char name[100];
	struct
	{
		char event[100], ip[100];
	} events[NUM_EVENTS];
	struct l *next;
} list_elem, *list_ptr;

list_ptr new_elem(char *name)
{
	list_ptr p = malloc(sizeof(list_elem));
	p->next = NULL;
	strcpy(p->name, name);
	for (int i = 0; i < NUM_EVENTS; i++)
	{
		strcpy(p->events[i].event, "");
		strcpy(p->events[i].ip, "-");
	}
	return p;
}

void add_to_list(list_ptr list, char *name, char *event, char *ip, int n)
{
	list_ptr p = list;
	while (p->next != NULL && strcmp(p->next->name, name) < 0)
		p = p->next;
	if (p->next == NULL || strcmp(p->next->name, name) != 0)
	{
		list_ptr q = p->next;
		p->next = new_elem(name);
		p->next->next = q;
	}
	strcpy(p->next->events[n].event, event);
	strcpy(p->next->events[n].ip, ip);
}

void process(list_ptr list, char *fname, int num_file)
{
	FILE *f = fopen(fname, "r");
	if (f == NULL)
	{
		fprintf(stderr, "Could not open file %s\n", fname);
		return;
	}

	char time[BUFSIZE] = "";
	char name[BUFSIZE] = "";
	char event[BUFSIZE] = "";
	char category[BUFSIZE] = "";
	char ip[BUFSIZE] = "";

	int cntr = 0;
	char line[MAX_LINE];
	while (fgets(line, MAX_LINE, f) != NULL)
	{
		char *to[] = {time, name, NULL, event, NULL, category, NULL, NULL, ip};
		char *str = trim(line);
		if (str != NULL && str[0] == '\"')
		{
			if (to[cntr] != NULL)
				strcpy(to[cntr], remove_quote(str));
			cntr++;
		}
		else
		{
			cntr = 0;
			if (strcmp(category, "Quiz attempt started") == 0)
				add_to_list(list, name, event, ip, num_file);
		}
	}

	fclose(f);
}

void print_same(list_ptr list)
{
	for (; list != NULL; list = list->next)
	{
		int i;
		for (i = 2; i < 4; i++)
			if (strcmp(list->events[i].ip, list->events[1].ip) != 0)
				break;
		if (i != 4)
			continue;
		printf("%-16s\t%s\n", list->name, list->events[1].ip);
	}
}


void print_list(list_ptr list)
{
	for (; list != NULL; list = list->next)
	{
		printf("%-30s", list->name);
		for (int i = 0; i < NUM_EVENTS; i++)
			printf("%16s", list->events[i].ip);
		printf("\n");
	}
}

void print_cross(list_ptr list, char *events[])
{
	if (list == NULL)
		return;
	for (list_ptr p = list; p->next != NULL; p = p->next)
	{
		for (list_ptr q = p->next; q != NULL; q = q->next)
		{
			for (int i = 0; i < NUM_EVENTS; i++)
			{
				if (strcmp(p->events[i].ip, "-") == 0)
					continue;

				for (int j = 0; j < NUM_EVENTS; j++)
				{
					if (strcmp(q->events[j].ip, "-") == 0)
						continue;

					if (strcmp(p->events[i].ip, q->events[j].ip) == 0)
						printf("%-16s : %s - %s --- %s - %s\n", p->events[i].ip, p->name, events[i], q->name, events[j]);
				}
			}
		}
	}
}

void delete_list(list_ptr list)
{
	while (list != NULL)
	{
		list_ptr p = list->next;
		free(list);
		list = p;
	}
}

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("Usage: %s json1 json2 json3 ...\n", argv[0]);
		return 0;
	}

	list_ptr list = new_elem("SENTINEL");

	for (int i = 1; i < argc; i++)
	{
		char *fname = argv[i];
		process(list, fname, i - 1);
	}

	printf("IP-címek\n");
	print_same(list->next);
	printf("\n");

	printf("IP-címek\n");
	print_list(list->next);
	printf("\n");

	printf("IP-egyezések:\n");
	print_cross(list->next, argv + 1);
	printf("\n");

	delete_list(list);

	return 0;
}
