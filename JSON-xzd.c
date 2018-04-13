#include "JSON-xzd.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Test_Pointer(pointer) ((pointer == NULL) ? exit(EXIT_FAILURE) : (void)0)

/* JSON¶ÁÈ¡Æ÷ */
void Reader_JSON(char filename[], Table * table)
{
	assert(filename != NULL && table != NULL);

	FILE * file = NULL;
	fopen_s(&file, filename, "r");
	Test_Pointer(file);

	int length = 0;
	while (fgetc(file) != EOF)
	{
		length++;
	}
	table->text = (char *)malloc(sizeof(char) * (length + 1));
	Test_Pointer(table->text);

	rewind(file);
	fread(table->text, length, 1, file);
	table->text[length] = '\0';
	fclose(file);
}

/* JSONÓÅ»¯Æ÷ */
void Optimizer_JSON(Table * table)
{
	assert(table != NULL && table->text != NULL);

	char * text = table->text;
	char * temp = (char *)malloc(sizeof(char) * strlen(text));
	Test_Pointer(temp);

	int index = -1;
	int length = 0;
	while (text[++index] != '\0')
	{
		if (text[index] == '{' || text[index] == '}'
			|| text[index] == '[' || text[index] == ']'
			|| text[index] == ':' || text[index] == ','
			|| isalnum(text[index]) || text[index] == '.'
			|| text[index] == '+' || text[index] == '-')
		{
			temp[length++] = text[index];
		}
		else if (text[index] == '"')
		{
			do
			{
				temp[length++] = text[index++];
			} while (text[index] != '"');
			temp[length++] = text[index];
		}
	}
	temp[length] = '\0';

	realloc(text, sizeof(char) * (length + 1));
	strcpy_s(text, length + 1, temp);
	free(temp);
}

/* JSONÉ¨ÃèÆ÷ */
void Scanner_JSON(Table * table)
{
	assert(table != NULL && table->text != NULL);

	char * text = table->text;
	table->sign = (enum JSON_Sign *)malloc(sizeof(enum JSON_Sign) * strlen(text));
	Test_Pointer(table->sign);
	
	enum JSON_Sign * sign = table->sign;
	int index = -1;
	while (text[++index] != '\0')
	{
		switch (text[index])
		{
		case '{':
			sign[index] = JSON_Sign_ObjectBegin;
			break;
		case '}':
			sign[index] = JSON_Sign_ObjectEnd;
			break;
		case '[':
			sign[index] = JSON_Sign_ArrayBegin;
			break;
		case ']':
			sign[index] = JSON_Sign_ArrayEnd;
			break;
		case '"':
			sign[index] = JSON_Sign_StringBegin;
			int length = 0;
			while (text[++index] != '"')
			{
				sign[index] = JSON_Sign_NameText;
				length++;
			}
			sign[index] = JSON_Sign_StringEnd;
			if (text[index + 1] != ':')
			{
				for (int i = -1; i <= length; i++)
				{
					sign[index - length + i] = JSON_Sign_ValueText;
				}
			}
			break;
		case ':':
			sign[index] = JSON_Sign_NameSeparator;
			break;
		case ',':
			sign[index] = JSON_Sign_ValueSeparator;
			break;
		default:
			sign[index] = JSON_Sign_ValueText;
			break;
		}
	}
}

/* JSON·ÖÎöÆ÷ */
int Parser_JSON(Table table, Tree * tree, int begin_index, int end_index)
{
	assert(tree != NULL && begin_index >= 0 && begin_index < end_index);

	char * text = table.text;
	enum JSON_Sign * sign = table.sign;
	*tree = (Tree)malloc(sizeof(struct Syntax_Tree));
	Test_Pointer(*tree);
	(*tree)->name[0] = '\0';
	(*tree)->value = NULL;
	(*tree)->child = NULL;
	(*tree)->brother = NULL;

	int index = begin_index;
	while (index < end_index)
	{
		int length = 0;
		int i = 0;
		switch (sign[index])
		{
		case JSON_Sign_ObjectBegin:
			do
			{
				if (sign[index + length] == JSON_Sign_ObjectBegin)
				{
					i++;
				}
				else if (sign[index + length] == JSON_Sign_ObjectEnd)
				{
					i--;
				}
				length++;
			} while (i != 0);
			index += length;
			if (index - length + 1 < index - 1)
			{
				Parser_JSON(table, &(*tree)->child, index - length + 1, index - 1);
			}
			else
			{
				(*tree)->value = (char *)malloc(sizeof(char) * 5);
				Test_Pointer((*tree)->value);
				strcpy_s((*tree)->value, 5, "null");
			}
			break;
		case JSON_Sign_ArrayBegin:
			do
			{
				if (sign[index + length] == JSON_Sign_ArrayBegin)
				{
					i++;
				}
				else if (sign[index + length] == JSON_Sign_ArrayEnd)
				{
					i--;
				}
				length++;
			} while (i != 0);
			index += length;
			if (index - length + 1 < index - 1)
			{
				Parser_JSON(table, &((*tree)->child), index - length + 1, index - 1);
			}
			else
			{
				(*tree)->value = (char *)malloc(sizeof(char) * 5);
				Test_Pointer((*tree)->value);
				strcpy_s((*tree)->value, 5, "null");
			}
			break;
		case JSON_Sign_StringBegin:
			while (sign[++index] != JSON_Sign_StringEnd)
			{
				(*tree)->name[i++] = text[index];
			}
			(*tree)->name[i] = '\0';
			index++;
			break;
		case JSON_Sign_ValueSeparator:
			index = Parser_JSON(table, &((*tree)->brother), index + 1, end_index);
			index++;
			break;
		case JSON_Sign_ValueText:
			while (sign[index + ++length] == JSON_Sign_ValueText)
				;
			(*tree)->value = (char *)malloc(sizeof(char) * (length + 1));
			Test_Pointer((*tree)->value);
			while (i < length)
			{
				(*tree)->value[i++] = text[index++];
			}
			(*tree)->value[i] = '\0';
			break;
		default:
			index++;
			break;
		}
	}
	return index;
}

/* JSONÉú³ÉÆ÷ */
void Generator_JSON(Tree tree, JSON * json)
{
	enum JSON_Type type;
	union JSON_Value value;
	JSON next = NULL;

	if (tree->value != NULL)
	{
		if (strcmp(tree->value,"null") == 0)
		{
			type = JSON_Type_Null;
			value.object = NULL;
		}
		else if (strcmp(tree->value, "true") == 0)
		{
			type = JSON_Type_Boolean;
			value.boolean = true;
		}
		else if (strcmp(tree->value, "false") == 0)
		{
			type = JSON_Type_Boolean;
			value.boolean = false;
		}
		else if (tree->value[0] == '"' && tree->value[strlen(tree->value) - 1] == '"')
		{
			type = JSON_Type_String;
			value.string = (char *)malloc(sizeof(char) * (strlen(tree->value) + 1));
			strcpy_s(value.string, strlen(tree->value) + 1, tree->value);
		}
		else
		{
			type = JSON_Type_Number;
			value.number = (char *)malloc(sizeof(char) * (strlen(tree->value) + 1));
			strcpy_s(value.number, strlen(tree->value) + 1, tree->value);
		}
	}

	if (tree->value == NULL && tree->child != NULL)
	{
		if (tree->child->name[0] == '\0')
		{
			type = JSON_Type_Array;
			Generator_JSON(tree->child, &value.array);
		}
		else
		{
			type = JSON_Type_Object;
			Generator_JSON(tree->child, &value.object);
		}
	}

	if (tree->brother != NULL)
	{
		Generator_JSON(tree->brother, &next);
	}

	*json = Constructor_JSON(tree->name, type, value, next);
}

/* JSON¹¹ÔìÆ÷ */
JSON Constructor_JSON(char name[], enum JSON_Type type, union JSON_Value value, JSON next)
{
	assert(name != NULL && (type == JSON_Type_Null || type == JSON_Type_Boolean || type == JSON_Type_Number
		|| type == JSON_Type_String || type == JSON_Type_Array || type == JSON_Type_Object));

	JSON json = (JSON)malloc(sizeof(struct Data_List));
	Test_Pointer(json);
	strcpy_s(json->name, strlen(name) + 1, name);
	json->type = type;
	json->value = value;
	json->next = next;
	return json;
}

/* JSONÎö¹¹Æ÷ */
void Destructor_JSON(JSON * json)
{
	assert(json != NULL);

	if ((*json)->next != NULL)
	{
		Destructor_JSON(&((*json)->next));
	}

	switch ((*json)->type)
	{
	case JSON_Type_Number:
		free((*json)->value.number);
		(*json)->value.number = NULL;
		break;
	case JSON_Type_String:
		free((*json)->value.string);
		(*json)->value.string = NULL;
		break;
	case JSON_Type_Array:
		Destructor_JSON(&((*json)->value.array));
		break;
	case JSON_Type_Object:
		Destructor_JSON(&((*json)->value.object));
		break;
	default:
		break;
	}

	free(*json);
	*json = NULL;
}

/* JSONÊä³öÆ÷ */
void Printer_JSON(JSON json, int tab)
{
	assert(json != NULL && tab >= 0);

	for (int i = 0; i < tab; i++)
	{
		printf("  ");
	}

	if (json->name[0] != '\0')
	{
		printf("\"%s\": ", json->name);
	}

	switch (json->type)
	{
	case JSON_Type_Null:
		printf("null");
		break;
	case JSON_Type_Boolean:
		if (json->value.boolean)
		{
			printf("true");
		}
		else
		{
			printf("false");
		}
		break;
	case JSON_Type_Number:
		printf(json->value.number);
		break;
	case JSON_Type_String:
		printf(json->value.string);
		break;
	case JSON_Type_Array:
		printf("[\n");
		Printer_JSON(json->value.array, tab + 1);
		for (int i = 0; i < tab; i++)
		{
			printf("  ");
		}
		printf("]");
		break;
	case JSON_Type_Object:
		printf("{\n");
		Printer_JSON(json->value.object, tab + 1);
		for (int i = 0; i < tab; i++)
		{
			printf("  ");
		}
		printf("}");
		break;
	default:
		break;
	}

	if (json->next != NULL)
	{
		printf(",\n");
		Printer_JSON(json->next, tab);
	}
	else
	{
		printf("\n");
	}
}

static void Encoder(JSON json, FILE * file, int tab)
{
	assert(json != NULL && file != NULL && tab >= 0);

	for (int i = 0; i < tab; i++)
	{
		fprintf(file, "  ");
	}

	if (json->name[0] != '\0')
	{
		fprintf(file, "\"%s\": ", json->name);
	}

	switch (json->type)
	{
	case JSON_Type_Null:
		fprintf(file, "null");
		break;
	case JSON_Type_Boolean:
		if (json->value.boolean)
		{
			fprintf(file, "true");
		}
		else
		{
			fprintf(file, "false");
		}
		break;
	case JSON_Type_Number:
		fprintf(file, "%s", json->value.number);
		break;
	case JSON_Type_String:
		fprintf(file, "%s", json->value.string);
		break;
	case JSON_Type_Array:
		fprintf(file, "[\n");
		Encoder(json->value.array, file, tab + 1);
		for (int i = 0; i < tab; i++)
		{
			fprintf(file, "  ");
		}
		fprintf(file, "]");
		break;
	case JSON_Type_Object:
		fprintf(file, "{\n");
		Encoder(json->value.object, file, tab + 1);
		for (int i = 0; i < tab; i++)
		{
			fprintf(file, "  ");
		}
		fprintf(file, "}");
		break;
	default:
		break;
	}

	if (json->next != NULL)
	{
		fprintf(file, ",\n");
		Encoder(json->next, file, tab);
	}
	else
	{
		fprintf(file, "\n");
	}
}

/* JSON±àÂëÆ÷ */
void Encoder_JSON(JSON json, char filename[])
{
	assert(filename != NULL);

	FILE * file;
	fopen_s(&file, filename, "w");
	Test_Pointer(file);
	Encoder(json, file, 0);
	fclose(file);
}