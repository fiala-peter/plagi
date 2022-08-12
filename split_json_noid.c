/**
 * @file split_json.c
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief Split json encoded solution file into separate c files
 * @version 0.1
 * @date 2020-11-14
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_json.h"

#define JSON_TOOLS_DEFINITIONS
#include "json_tools.h"

/**
 * @brief split the json syntax tree into separate C files
 * 
 * @param st json syntax tree 
 */
void parse_and_split(syntax_tree st)
{
	// traverse tree children
	for (syntax_tree *it = syntax_tree_first_child(st); it != NULL; it = syntax_tree_next_sibling(it))
	{
		syntax_tree c = *it;
		/*
		// only accepted solutions are considered
		if (syntax_tree_get_field(c, "accepted")->type != syntax_true)
			continue;
		*/

		// extract attributes from the node
		char const *student_id = syntax_tree_get_field(c, "student_id")->data;
		char const *time = syntax_tree_get_field(c, "time")->data;
		char const *solution = syntax_tree_get_field(c, "solution")->data;

		// generate C file name from student id
		char fname[10];
		sprintf(fname, "%s.c", student_id);
		FILE *fout = fopen(fname, "w");
		if (fout == NULL)
		{
			fprintf(stderr, "Could not create file %s\n", fname);
			continue;
		}
		// print solution time into file
		fprintf(fout, "// Solution submitted by %s at %s\n", student_id, time);
		// write solution into solution file with escaping
		char const *content = solution;
		while (*content != '\0')
		{
			if (*content == '\\')
			{
				char c;
				content += escape(content, &c);
				fputc(c, fout);
			}
			else
				fputc(*content++, fout);
		}
		if (fclose(fout) != 0)
			fprintf(stderr, "Could not create file %s\n", fname);
	}
}

int main(int argc, char *argv[])
{
	char *fname;
	if (argc < 2)
	{
		fprintf(stderr, "Did not provide enough arguments\n");
		printf("Usage: %s json_fname\n", argv[0]);
		return 1;
	}
	else
		fname = argv[1];

	FILE *fin = fopen(fname, "r");
	if (fin == NULL)
	{
		fprintf(stderr, "Could not open input file %s\n", fname);
		return 1;
	}

	// parse json in two steps: (1) token list (2) syntax tree
	token_list tl = token_list_read_from_file(fin);
	fclose(fin);
	
	token_list end;
	syntax_tree st = parse_json(tl, &end);
	token_list_delete(tl);

	// split the syntax tree into separate C files
	parse_and_split(st);
	syntax_tree_delete(st);

	return 0;
}
