#pragma once

#define MAX_NAME_LENGTH 256 //最大键长

/* 符号 */
enum JSON_Sign
{
	JSON_Sign_NameText, //键文本
	JSON_Sign_ValueText, //值文本
	JSON_Sign_NameSeparator, //冒号
	JSON_Sign_ValueSeparator, //逗号
	JSON_Sign_StringBegin, //左双引号
	JSON_Sign_StringEnd, //右双引号
	JSON_Sign_ArrayBegin, //左中括号
	JSON_Sign_ArrayEnd, //右中括号
	JSON_Sign_ObjectBegin, //左大括号
	JSON_Sign_ObjectEnd //右大括号
};

/* 错误 */
enum JSON_Error
{
	JSON_Error_System, //系统错误
	JSON_Error_File, //文件错误
	JSON_Error_Syntax, //语法错误
	JSON_Error_Semantic, //语义错误
	JSON_Error_Unknown //未知错误
};

/* 类型 */
enum JSON_Type
{
	JSON_Type_Null, //空类型
	JSON_Type_Boolean, //布尔型
	JSON_Type_Number, //数值型
	JSON_Type_String, //字符串型
	JSON_Type_Array, //数组型
	JSON_Type_Object //对象型
};

/* 数据 */
union JSON_Value
{
	_Bool boolean; //布尔值
	char * number; //数值
	char * string; //字符串
	struct Data_List * array; //数组
	struct Data_List * object; //对象
};

/* 符号表 */
typedef struct Sign_Table
{
	char * text;
	enum JSON_Sign * sign;
}Table;

/* 语法树 */
typedef struct Syntax_Tree
{
	char name[MAX_NAME_LENGTH];
	char * value;
	struct Syntax_Tree * child;
	struct Syntax_Tree * brother;
}*Tree;

/* 数据表 */
typedef struct Data_List
{
	char name[MAX_NAME_LENGTH]; //键
	enum JSON_Type type; //类型
	union JSON_Value value; //值
	struct Data_List * next; //后继
}*JSON;

/* JSON读取器 */
void Reader_JSON(char filename[], Table * table);

/* JSON优化器 */
void Optimizer_JSON(Table * table);

/* JSON扫描器 */
void Scanner_JSON(Table * table);

/* JSON分析器 */
int Parser_JSON(Table table, Tree * tree, int begin_index, int end_index);

/* JSON生成器 */
void Generator_JSON(Tree tree, JSON * json);

/* JSON构造器 */
JSON Constructor_JSON(char name[], enum JSON_Type type, union JSON_Value value, JSON next);

/* JSON析构器 */
void Destructor_JSON(JSON * json);

/* JSON输出器 */
void Printer_JSON(JSON json, int tab);

/* JSON编码器 */
void Encoder_JSON(JSON json, char filename[]);

/* JSON处理器 */
void Processor_JSON(_Bool state, enum JSON_Error error);