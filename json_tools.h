#ifndef JSON_TOOLS_H_DEFINED
#define JSON_TOOLS_H_DEFINED

/**
 * @brief trim a string from leading white spaces
 * 
 * @param str the input string
 * @return char* pointer to the trimmed string
 */
char *trim(char *str);

/**
 * @brief Remove starting and ending quotation marks from a string
 * 
 * @param str the input string
 * @return char* pointer to the trimmed string
 */
char *remove_quote(char *str);


/**
 * @brief Find escape sequence in a string and return the appropriate character
 * 
 * @param s the string
 * @param c[out] the returned character
 * @return int number of characters interpreted from the string
 */
int escape(char const *str, char *c);

#ifdef JSON_TOOLS_DEFINITIONS

char *trim(char *str)
{
	while (*str != '\0' && isspace(*str))
		str++;
	return str;
}

char *remove_quote(char *str)
{
	char *end = str + strlen(str);
	// find first quotation mark from the end
	while (end != str && *end != '\"')
		end--;
	*end = '\0';
	// find first quotation mark from the beginning
	while (*str != '\0' && *str != '\"')
		str++;
	if (*str == '\0')
		return str;
	return str + 1;
}


int escape(char const *str, char *c)
{
	if (str[0] == '\0')
	{
		*c = '\\';
		return 1;
	}
	// decode conventional C escapes
	char arr[][2] = {
		{'n', '\n'},
		{'t', '\t'},
		{'\"', '\"'},
		{'\'', '\''},
		{'\\', '\\'},
		{0, 0}
	};
	for (int i = 0; arr[i][0] != 0; i++)
	{
		if (str[1] == arr[i][0])
		{
			*c = arr[i][1];
			return 2;
		}
	}
	// decode unicode sequences \uxxxx
	if (str[1] == 'u')
	{
		char unicode[4 + 1] = {0};
		strncpy(unicode, str + 2, 4);
		unsigned code;
		sscanf(unicode, "%x", &code);
		struct
		{
			unsigned code;
			char c;
		} data[] = {
			{0x00c1, 'A'}, // Á
			{0x00c9, 'E'}, // É
			{0x00c1, 'I'}, // Í
			{0x00d3, 'O'}, // Ó
			{0x00d6, 'O'}, // Ö
			{0x00da, 'U'}, // Ú
			{0x00dc, 'U'}, // Ü
			{0x00e1, 'a'}, // á
			{0x00e9, 'e'}, // é
			{0x00ed, 'i'}, // í
			{0x00f3, 'o'}, // ó
			{0x00f6, 'o'}, // ö
			{0x00fc, 'u'}, // ú
			{0x00fa, 'u'}, // ü
			{0x0150, 'O'}, // Ő
			{0x0151, 'o'}, // ő
			{0x0170, 'U'}, // Ű
			{0x0171, 'u'}, // ű
			{0x0, '\0'}};
		for (int q = 0; data[q].c != '\0'; q++)
			if (code == data[q].code)
			{
				*c = data[q].c;
				return 6;
			}
	}
	// could not decode anything
	*c = '\\';
	return 1;
}

#endif // JSON_TOOLS_DEFINITIONS

#endif
