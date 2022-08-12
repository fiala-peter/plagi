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

// if this macro is defined then the C files are named using prog1_ids instead of neptun codes
// #define CONVERT_NEPTUN_TO_PROGID

#ifdef CONVERT_NEPTUN_TO_PROGID

/**
 * @brief structure containing neptun code and prog1id pairs
 */
typedef struct
{
	char neptun[6 + 1];
	double id;
} student_id_t;

/**
 * @brief Read neptun-prog1id pairs from file
 *
 * @param fname the file name
 * @param n[out] number of elements read
 * @return student_id_t* array of elements
 */
student_id_t *read_ids(char *fname, size_t *n)
{
	FILE *f = fopen(fname, "r");
	if (f == NULL)
	{
		fprintf(stderr, "Could not open file %s for writing", fname);
		return NULL;
	}

	student_id_t *p = malloc(400 * sizeof(student_id_t));
	size_t i = 0;
	while (i < 400 && fscanf(f, "%s%lf", p[i].neptun, &p[i].id) == 2)
		i++;
	*n = i;
	fclose(f);
	return realloc(p, *n * sizeof(student_id_t));
}

/**
 * @brief Convert Neptun code to prog1 id
 *
 * @param neptun the neptun code (null-terminated)
 * @param ids array of code-id pairs
 * @param n number of array elements
 * @return double prog1_id. 0.0 is returned if not found
 */
double neptun2id(char const *neptun, student_id_t ids[], size_t n)
{
	for (size_t i = 0; i < n; i++)
		if (strcmp(ids[i].neptun, neptun) == 0)
			return ids[i].id;
	return 0.0;
}

#endif

/**
 * @brief parse the json file and split it into separate C solutions
 *
 * @param st json syntax tree
 * @param ids array containing neptun and prog1id pairs
 * @param nids size of array
 */
#ifdef CONVERT_NEPTUN_TO_PROGID
void parse_and_split(syntax_tree st, student_id_t ids[], size_t nids)
#else
void parse_and_split(syntax_tree st)
#endif
{
	// traverese tree children
	for (syntax_tree *it = syntax_tree_first_child(st); it != NULL; it = syntax_tree_next_sibling(it))
	{
		/*
		// only accepted solutions are considered
		if (syntax_tree_get_field(*it, "accepted")->type != syntax_true)
			continue;
		*/

		char const *student_id = syntax_tree_get_field(*it, "student_id")->data;
		char const *time = syntax_tree_get_field(*it, "time")->data;
		char const *solution = syntax_tree_get_field(*it, "solution")->data;

		char fname[10], identifier[8];
#ifdef CONVERT_NEPTUN_TO_PROGID
		double prog1id = neptun2id(student_id, ids, nids);
		if (prog1id == 0.0)
			continue;
		sprintf(identifier, "%02.1f", prog1id);
#else
		sprintf(identifier, "%s", student_id);
#endif
		sprintf(fname, "%s.c", identifier);

		FILE *fout = fopen(fname, "w");
		if (fout == NULL)
		{
			fprintf(stderr, "Could not create file %s\n", fname);
			continue;
		}
		fprintf(fout, "// Solution submitted by %s at %s\n", identifier, time);
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
#ifdef CONVERT_NEPTUN_TO_PROGID
	if (argc < 3)
	{
		fprintf(stderr, "Did not provide enough arguments\n");
		printf("Usage: %s json_fname prog1idname\n", argv[0]);
		return 1;
	}
	char *fname = argv[1];
	char *dbname = argv[2];
#else
	if (argc < 2)
	{
		fprintf(stderr, "Did not provide enough arguments\n");
		printf("Usage: %s json_fname\n", argv[0]);
		return 1;
	}
	char *fname = argv[1];
#endif

	// parse json into syntax tree in two steps: (1) token list (2) syntax tree
	FILE *fin = fopen(fname, "r");
	if (fin == NULL)
	{
		fprintf(stderr, "Could not open input file %s\n", fname);
		return 1;
	}
	token_list tl = token_list_read_from_file(fin);
	fclose(fin);
	token_list end;
	syntax_tree st = parse_json(tl, &end);
	token_list_delete(tl);

#ifdef CONVERT_NEPTUN_TO_PROGID
	// read neptun-prog1id-pairs
	size_t nids;
	student_id_t *ids = read_ids(dbname, &nids);
	// split the syntax tree into separate C files
	parse_and_split(st, ids, nids);
	syntax_tree_delete(st);
	free(ids);
#else
	// split the syntax tree into separate C files
	parse_and_split(st);
	syntax_tree_delete(st);
#endif

	return 0;
}
