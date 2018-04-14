#pragma once

#define MAX_NAME_LENGTH 256 //������

/* ���� */
enum JSON_Sign
{
	JSON_Sign_NameText, //���ı�
	JSON_Sign_ValueText, //ֵ�ı�
	JSON_Sign_NameSeparator, //ð��
	JSON_Sign_ValueSeparator, //����
	JSON_Sign_StringBegin, //��˫����
	JSON_Sign_StringEnd, //��˫����
	JSON_Sign_ArrayBegin, //��������
	JSON_Sign_ArrayEnd, //��������
	JSON_Sign_ObjectBegin, //�������
	JSON_Sign_ObjectEnd //�Ҵ�����
};

/* ���� */
enum JSON_Error
{
	JSON_Error_System, //ϵͳ����
	JSON_Error_File, //�ļ�����
	JSON_Error_Syntax, //�﷨����
	JSON_Error_Semantic, //�������
	JSON_Error_Unknown //δ֪����
};

/* ���� */
enum JSON_Type
{
	JSON_Type_Null, //������
	JSON_Type_Boolean, //������
	JSON_Type_Number, //��ֵ��
	JSON_Type_String, //�ַ�����
	JSON_Type_Array, //������
	JSON_Type_Object //������
};

/* ���� */
union JSON_Value
{
	_Bool boolean; //����ֵ
	char * number; //��ֵ
	char * string; //�ַ���
	struct Data_List * array; //����
	struct Data_List * object; //����
};

/* ���ű� */
typedef struct Sign_Table
{
	char * text;
	enum JSON_Sign * sign;
}Table;

/* �﷨�� */
typedef struct Syntax_Tree
{
	char name[MAX_NAME_LENGTH];
	char * value;
	struct Syntax_Tree * child;
	struct Syntax_Tree * brother;
}*Tree;

/* ���ݱ� */
typedef struct Data_List
{
	char name[MAX_NAME_LENGTH]; //��
	enum JSON_Type type; //����
	union JSON_Value value; //ֵ
	struct Data_List * next; //���
}*JSON;

/* JSON��ȡ�� */
void Reader_JSON(char filename[], Table * table);

/* JSON�Ż��� */
void Optimizer_JSON(Table * table);

/* JSONɨ���� */
void Scanner_JSON(Table * table);

/* JSON������ */
int Parser_JSON(Table table, Tree * tree, int begin_index, int end_index);

/* JSON������ */
void Generator_JSON(Tree tree, JSON * json);

/* JSON������ */
JSON Constructor_JSON(char name[], enum JSON_Type type, union JSON_Value value, JSON next);

/* JSON������ */
void Destructor_JSON(JSON * json);

/* JSON����� */
void Printer_JSON(JSON json, int tab);

/* JSON������ */
void Encoder_JSON(JSON json, char filename[]);

/* JSON������ */
void Processor_JSON(_Bool state, enum JSON_Error error);