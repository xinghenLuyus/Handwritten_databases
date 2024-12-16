#include<cstdio>
#include<cmath>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
//数据类型标识枚举型 
typedef enum {
	CHAR = 1,
	INT = 2,
	LONG = 3,
	FLOAT = 4,
	DOUBLE = 5,
	BOOL = 6,
	Type = 7  //DataType枚举型标识 
}DataType;
typedef enum {
	less_than = 0,
	greater_than = 1,
	equal_to = 2,
	not_equal_to = 3,
	less_equal = 4,
	greater_equal = 5
}CmpOperator;

//pg_class数据字典表的元组结构体，存储表信息 
typedef struct pg_class_tuple {
	char TableName[20]; //表名 
	int ID;             //表ID 
	int ColNum;         //列数量 
}PC_Tuple;
//pg_attribute数据字典表的元组结构体 ，存储列信息 
typedef struct pg_attribute_tuple {
	char ColName[20]; //列名 
	int BelongToID; //归属表的ID 
	DataType Type;  //数据类型标识 
	int TypeSize;   //数据字节长度 
	int NoOfCol;    //列号 
	bool AbleNull;  //是否可以为空，true为可以为空        ??????????字节数为4，不明原因 ,但是不影响正常读写 
}PA_Tuple;
//数据块头
typedef struct PageHeadData {
	int lower;//空闲开始位置 
	int upper;//空闲结束位置
	int special;//special开始位置 
}PageHeadData;
//元组存储信息
typedef struct Item {
	int tuple_start;  //元组位置 
	int length;       //元组长度 
	bool IsDeleted;   //删除标记 
}Item;
//过滤表达式
typedef struct filter {
	int ColNo;
	CmpOperator CO;
	char* Value;
}Filter;
//其它 
typedef struct Buffer {
	char TypeSetBuffer[50];
	int MemoryLen;
	int CurrentPos;
	bool* pBool;
	int* pInt;
	long* pLong;
	float* pFloat;
	double* pDouble;
	char* pChar;
	int Int;
	long Long;
	float Float;
	double Double;
	bool Bool;
}Buffer;


//标记未使用的表ID，当多次建表时可节省调用查找ID是否重复的函数调用 
int IsIDOccupied = 10000;
//块内排序使用
int SortColFlag;
char* page;
//存储表列信息，可以去掉，我在函数中都作为参数传入，作为全局变量只是偷懒，不在函数中定义
PC_Tuple* PCT;
PA_Tuple* PAT;

#define pg_class "1259"
#define pg_attribute "1249"
#define PAGESIZE 8192
#define SPECIAL 8000

//元组的输入函数 
void InsertInput()
{
	char* Tuple = NULL, TableName[20];
	bool* IsNull = NULL;
	int ID, InputNum, * ColPos = NULL, i, j, MaxMemoryLen;
	Buffer buffer;

	PCT = (PC_Tuple*)malloc(sizeof(PC_Tuple));

	printf("请输入你要插入的表名称\nInput-->");
	scanf("%s", TableName);
	for (; strnicmp(TableName, "pg_class", 8) == 0 ||
		strnicmp(TableName, "pg_attribute", 12) == 0 ||
		GetTableInfoByName(TableName, PCT) != 1;)
	{
		printf("您要插入的表不存在,请重新输入,quit退出\nInput-->");
		scanf("%s", TableName);
		if (strnicmp(TableName, "quit", 4) == 0)exit(0);
	}

	PAT = (PA_Tuple*)malloc(sizeof(PA_Tuple) * PCT->ColNum);

	if (GetColInfo(PCT->ID, PAT, PCT->ColNum) != PCT->ColNum)
	{
		printf("获取的列数与pg_class中的记录不符,表信息错误\n");
		exit(1);
	}

	//对得到的列数据按照NoOfCol排序 
	qsort(PAT, PCT->ColNum, sizeof(PA_Tuple), Sort_Colnum);

	//标记属性是否已输入 
	ColPos = (int*)malloc(sizeof(int) * PCT->ColNum);
	IsNull = (bool*)malloc(sizeof(bool) * PCT->ColNum);

	for (i = 0, MaxMemoryLen = 0; i < PCT->ColNum; i++)
	{
		MaxMemoryLen += PAT[i].TypeSize;
	}
	Tuple = (char*)malloc(sizeof(char) * MaxMemoryLen);

	for (getchar(), printf("回车键退出，任意键开始插入元组\nInput-->");
		getchar() != '\n';
		printf("回车键退出，任意键开始插入元组\nInput-->"))
	{
		fflush(stdin);
	errorloop: printf("CHAR=1,INT=2,LONG=3,FLOAT=4,DOUBLE=5,[0/1]BOOL=6\n");
		for (i = 0; i < PCT->ColNum; i++)
			printf("[%3d.%20s 类型：%2d 是否可以为空：%2d]\n",
				PAT[i].NoOfCol, PAT[i].ColName, PAT[i].Type, PAT[i].AbleNull);
		printf("\n");

		for (i = 0, buffer.MemoryLen = 0; i < PCT->ColNum; i++)
		{

			//初始化Tuple空间 
			if (i == 0)memset(Tuple, '\0', buffer.MemoryLen);
			//检测各值是否可以为空，可以则让用户输入该值是否空 ，不可则跳过 
			if (PAT[i].AbleNull == true)
			{
				printf("该属性值可为空，是否空[0/1]\nInput-->");
				scanf("%d", &buffer.Int);
				if (buffer.Int == 0)IsNull[i] = false;
				else if (buffer.Int == 1)
				{
					IsNull[i] = true;
					continue;
				}
				else
				{
					printf("%s 非法输入\n", PAT[i].ColName);
					goto errorloop;
				}
			}
			else if (PAT[i].AbleNull == false)
			{
				IsNull[i] = false;
			}
			printf("%s:", PAT[i].ColName);
			switch (PAT[i].Type)
			{
			case BOOL:  scanf("%d", &buffer.Int);
				if (buffer.Int == 0)*((bool*)(Tuple + buffer.MemoryLen)) = false;
				else if (buffer.Int == 1)*((bool*)(Tuple + buffer.MemoryLen)) = true;
				else
				{
					printf("%s 非法输入\n", PAT[i].ColName);
					goto errorloop;
				}
				buffer.MemoryLen += sizeof(bool);
				break;
			case DOUBLE:scanf("%lf", &buffer.Double);
				memcpy((Tuple + buffer.MemoryLen), &buffer.Double, sizeof(double));
				buffer.MemoryLen += sizeof(double);
				break;
			case FLOAT: scanf("%f", &buffer.Float);
				memcpy((Tuple + buffer.MemoryLen), &buffer.Float, sizeof(float));
				buffer.MemoryLen += sizeof(float);
				break;
			case LONG:  scanf("%ld", &buffer.Long);
				memcpy((Tuple + buffer.MemoryLen), &buffer.Long, sizeof(long));
				buffer.MemoryLen += sizeof(long);
				break;
			case INT:   scanf("%d", &buffer.Int);
				memcpy((Tuple + buffer.MemoryLen), &buffer.Int, sizeof(int));
				buffer.MemoryLen += sizeof(int);
				break;
			case CHAR:	scanf("%s", buffer.TypeSetBuffer);
				if ((buffer.Int = strlen(buffer.TypeSetBuffer)) >= PAT[i].TypeSize)
				{
					printf("%s输入的字符串溢出\n", PAT[i].ColName);
					goto errorloop;
				}
				strcpy((Tuple + buffer.MemoryLen), buffer.TypeSetBuffer);
				buffer.MemoryLen += sizeof(char) * (buffer.Int + 1);
				break;
			default:    printf("错误"); i--;
			}
			printf("位置：%d\n", buffer.MemoryLen);
		}
		insert(Tuple, IsNull, PCT->ID, PCT->ColNum, sizeof(char) * buffer.MemoryLen);
		printf("插入成功!\n");
		fflush(stdin);
	}
	free(PCT);
	free(PAT);
	free(Tuple);
	free(ColPos);
}

//通过表名获取表信息 
int GetTableInfoByName(char* TableName, PC_Tuple* PCT)
{
	int status = 0;
	FILE* fp = NULL;
	PC_Tuple temp;
	int i;
	//读写二进制文件 
	if ((fp = fopen(pg_class, "rb")) == NULL)
	{
		printf("1259打开失败:GetTableInfoByName()\n");
		exit(1);
	}

	//循环遍历pg_class文件，直到找到ID对应的表信息，若直到文件尾仍然未找到，则退出循环 
	for (i = 0; feof(fp) == 0; i++)
	{
		fseek(fp, sizeof(PC_Tuple) * i, SEEK_SET);
		if (fread(&temp, sizeof(PC_Tuple), 1, fp) == 0)break;
		if (strnicmp(TableName, temp.TableName, 20) == 0)
		{
			*PCT = temp;
			status = 1;
			break;
		}
	}
	if (fclose(fp) != 0)
	{
		printf("1259关闭失败:GetTableInfoByName()\n");
		exit(1);
	}

	return status;
}

//获取具有空闲空间的页
int GetThePageHaveEnoughSpace(char* Page, int Size, FILE* fp)
{
	int i, status = -1;
	for (i = 0; feof(fp) == 0; i++)
	{
		fseek(fp, PAGESIZE * i, SEEK_SET);
		if (fread(Page, PAGESIZE, 1, fp) == 0)break;
		if (((((PageHeadData*)Page)->upper) - (((PageHeadData*)Page)->lower)) >= Size)
		{
			status = i;
			return status;
		}
	}
	status = -i;
	return status;
}

//按ID检测表是否存在，如存在则返回表信息，返回值为1找到，0为未找到 
int GetTableInfo(int ID, PC_Tuple* PCT)
{
	int status = 0;
	FILE* fp = NULL;
	PC_Tuple temp;
	int i;
	//读写二进制文件 
	if ((fp = fopen(pg_class, "rb")) == NULL)
	{
		printf("1259打开失败:GetTableInfo()\n");
		exit(1);
	}

	//循环遍历pg_class文件，直到找到ID对应的表信息，若直到文件尾仍然未找到，则退出循环 
	for (i = 0; feof(fp) == 0; i++)
	{
		fseek(fp, sizeof(PC_Tuple) * i, SEEK_SET);
		if (fread(&temp, sizeof(PC_Tuple), 1, fp) == 0)break;
		if (temp.ID == ID)
		{
			*PCT = temp;
			status = 1;
			break;
		}
	}
	if (fclose(fp) != 0)
	{
		printf("1259关闭失败:GetTableInfo()\n");
		exit(1);
	}
	return status;
}

//按ID检测表属性，返回表属性列 ，返回值为找到的列数 
int GetColInfo(int ID, PA_Tuple* PAT, int ColNum)
{
	FILE* fp = NULL;
	PA_Tuple temp;
	int i, count;

	if ((fp = fopen(pg_attribute, "rb")) == NULL)
	{
		if (fclose(fp) != 0)
		{
			printf("1249关闭失败:GetColInfo()\n");
			exit(1);
		}
	}
	for (i = 0, count = 0; feof(fp) == 0; i++)
	{
		fseek(fp, sizeof(PA_Tuple) * i, SEEK_SET);
		if (fread(&temp, sizeof(PA_Tuple), 1, fp) == 0)break;
		if (temp.BelongToID == ID)
		{
			PAT[count] = temp;
			count++;
			if (count == ColNum)break;
		}
	}
	if (fclose(fp) != 0)
	{
		printf("1249关闭失败:GetColInfo()\n");
		exit(1);
	}
	return count;
}

//qsort()函数调用 
int Sort_Colnum(const void* elem1, const void* elem2)
{
	PA_Tuple* pac1 = (PA_Tuple*)elem1;
	PA_Tuple* pac2 = (PA_Tuple*)elem2;
	return (pac1->NoOfCol) - (pac2->NoOfCol);
}

//扩展页 
int extend(FILE* fp)
{
	char* p = NULL;
	int status;
	p = (char*)malloc(PAGESIZE);
	memset(p, '\0', PAGESIZE);
	init_Page(p);

	fseek(fp, 0, SEEK_END);
	if (fwrite(p, PAGESIZE, 1, fp) == 0)
	{
		printf("创建新页失败：extend（）\n");
	}
	free(p);

}

//初始化PAGE 
void init_Page(char* PAGE)
{
	PageHeadData* PHD = NULL;
	PHD = (PageHeadData*)malloc(sizeof(PageHeadData));
	PHD->lower = sizeof(PageHeadData);
	PHD->special = SPECIAL;
	PHD->upper = SPECIAL;
	memcpy(PAGE, PHD, sizeof(PageHeadData));
	free(PHD);
}

//插入函数，用于插入表信息，第一个参数为插入内容，第二个为要插入表的ID
//第三个为插入列个数，第四个为单个元组的尺寸 
int insert(void* Tuple, bool* IsNull, int ID, int InputNum, int Size)
{
	FILE* fp = NULL;
	int i, j, status, ThePageFind;
	char* PAGE = NULL;
	char str[10];
	Item item;
	PageHeadData* PHD = NULL;

	if (ID == 1259)
	{
		if ((fp = fopen(pg_class, "ab+")) == NULL)
		{
			printf("1259打开失败:insert()\n");
			exit(1);
		}
		if (fwrite((PC_Tuple*)Tuple, Size, 1, fp) != 1)
		{
			printf("1259写入失败：insert()\n");
			exit(1);
		}
		if (fclose(fp) != 0)
		{
			printf("1259关闭失败:insert()\n");
			exit(1);
		}
	}
	else if (ID == 1249)
	{
		if ((fp = fopen(pg_attribute, "ab+")) == NULL)
		{
			printf("1249打开失败:insert()\n");
			exit(1);
		}
		if (fwrite((PA_Tuple*)Tuple, Size, 1, fp) != 1)
		{
			printf("1249写入失败：insert()\n");
			exit(1);
		}
		if (fclose(fp) != 0)
		{
			printf("1249关闭失败:insert()\n");
			exit(1);
		}
	}
	//任意表插入 
	else
	{
		if ((fp = fopen(itoa(ID, str, 10), "rb+")) == NULL)
		{
			printf("%d 打开失败：insert()\n", ID);
			exit(1);
		}
		PAGE = (char*)malloc(PAGESIZE);
		PHD = (PageHeadData*)malloc(sizeof(PageHeadData));
		fseek(fp, 0, SEEK_END);
		if (ftell(fp) == 0)
		{
			printf("PAGE数据消失，请检查文件是否丢失，为保证运行，将开辟新页！\n");
			extend(fp);
		}
		//获取有空闲空间的Page，正数返回值的为第几个page页，负数返回值为找不到，其绝对值为总页数 
		if ((ThePageFind = GetThePageHaveEnoughSpace(PAGE, Size + sizeof(Item) + sizeof(bool) * InputNum, fp)) < 0)
		{
			extend(fp);
		}
		else
		{
			item.IsDeleted = false;
			item.length = Size;

			PHD = (PageHeadData*)PAGE;
			PHD->upper = PHD->upper - sizeof(char) * item.length - sizeof(bool) * InputNum;
			PHD->special = SPECIAL;
			item.tuple_start = PHD->upper;
			memcpy((PAGE + PHD->lower), &item, sizeof(Item));
			PHD->lower += sizeof(Item);
			memcpy((PAGE + PHD->upper), IsNull, sizeof(bool) * InputNum);
			memcpy(PAGE + PHD->upper + sizeof(bool) * InputNum, (char*)Tuple, sizeof(char) * Size);
			memcpy(PAGE, PHD, sizeof(PageHeadData));

			fseek(fp, ThePageFind * PAGESIZE, SEEK_SET);
			if (fwrite(PAGE, PAGESIZE, 1, fp) != 1)
			{
				printf("%d 写入失败:insert()", ID);
				exit(1);
			}
			if ((fclose(fp)) != 0)
			{
				printf("%d 关闭失败：insert()\n", ID);
				exit(1);
			}
		}
		free(PAGE);
	}
	return 0;
}

//char()输入处理，忽略大小写 
int DealWithCharInput(char* pInputChar, int n)
{
	int i, count = 0;
	int Return = 0;
	if ((pInputChar[0] == 'C' || pInputChar[0] == 'c') &&
		(pInputChar[1] == 'H' || pInputChar[1] == 'h') &&
		(pInputChar[2] == 'A' || pInputChar[2] == 'a') &&
		(pInputChar[3] == 'R' || pInputChar[3] == 'r'))
	{
		if (pInputChar[4] == '(')
		{
			for (i = 5; pInputChar[i] >= '0' && pInputChar[i] <= '9' && i < n; i++)count++;
			for (i = 5; pInputChar[i] >= '0' && pInputChar[i] <= '9' && i < n; i++)
			{
				Return += (pInputChar[i] - '0') * pow(10, count - 1);
				count--;
			}
			return Return;
		}
		else if (pInputChar[4] == '\0' || pInputChar[4] == '\n')
			return 1;
	}
	else return 0;
}

//通过名字显示表的全部内容 
void ShowTable()
{
	int i, j, k, ColPos;
	FILE* fp = NULL;
	Buffer buffer;
	char* PAGE = NULL;
	PageHeadData* PHD = NULL;
	Item* item = NULL;
	bool* IsNull = NULL;
	char TableName[20];

	PAGE = (char*)malloc(sizeof(char) * PAGESIZE);
	item = (Item*)malloc(sizeof(Item));
	PCT = (PC_Tuple*)malloc(sizeof(PC_Tuple));

	printf("请输入你要查看的表名称\nInput-->");
	scanf("%s", TableName);
	for (; strnicmp(TableName, "pg_class", 8) == 0 ||
		strnicmp(TableName, "pg_attribute", 12) == 0 ||
		GetTableInfoByName(TableName, PCT) != 1;)
	{
		printf("您要查看的表不存在,请重新输入,quit退出\nInput-->");
		scanf("%s", TableName);
		if (strnicmp(TableName, "quit", 4) == 0)exit(0);
	}

	IsNull = (bool*)malloc(sizeof(bool) * PCT->ColNum);
	PAT = (PA_Tuple*)malloc(sizeof(PA_Tuple) * PCT->ColNum);

	if (GetColInfo(PCT->ID, PAT, PCT->ColNum) != PCT->ColNum)
	{
		printf("列信息不全：%d\n", PCT->ID);
		exit(1);
	}

	//给取出的列排序//便于解释 
	qsort(PAT, PCT->ColNum, sizeof(PA_Tuple), Sort_Colnum);

	if ((fp = fopen(itoa(PCT->ID, buffer.TypeSetBuffer, 10), "rb")) == NULL)
	{
		printf("%d 打开失败：ShowTable()\n", PCT->ID);
		exit(1);
	}
	//获取每个块 
	for (i = 0; feof(fp) == 0; i++)
	{
		fseek(fp, PAGESIZE * i, SEEK_SET);
		if (fread(PAGE, PAGESIZE, 1, fp) == 0)break;
		PHD = (PageHeadData*)PAGE;

		//每个元组获取 
		for (k = 0;
			(sizeof(PageHeadData) + sizeof(Item) * k) < PHD->lower;
			k++)
		{
			memcpy(item, (Item*)(PAGE + sizeof(PageHeadData) + sizeof(Item) * k), sizeof(Item));

			//检测是否已被删除 
			if (item->IsDeleted == true)continue;
			memcpy(IsNull, (bool*)(PAGE + item->tuple_start), PCT->ColNum * sizeof(bool));
			buffer.pChar = (char*)malloc(sizeof(char) * (item->length));
			memcpy(buffer.pChar, (PAGE + item->tuple_start), PCT->ColNum * sizeof(bool));

			memcpy(buffer.pChar, (PAGE + item->tuple_start + PCT->ColNum * sizeof(bool)), sizeof(char) * item->length);
			//输出每个属性 

			for (j = 0, ColPos = 0; j < PCT->ColNum; j++)
			{
				printf("%d    :", ColPos);
				if (IsNull[j] == true)
				{
					printf("%s[null] ", PAT[j].ColName);
					continue;
				}
				else
				{
					switch (PAT[j].Type)
					{
					case BOOL:  if (*((int*)(buffer.pChar + ColPos)) == 1)printf("%s[true] ", PAT[j].ColName);
							 else printf("%s[true] ", PAT[j].ColName);
						ColPos += sizeof(bool);
						break;
					case DOUBLE:buffer.pDouble = (double*)(buffer.pChar + ColPos);
						printf("%s[%lf] ", PAT[j].ColName, *buffer.pDouble);
						ColPos += sizeof(double);
						break;
					case FLOAT: buffer.pFloat = (float*)(buffer.pChar + ColPos);
						printf("%s[%f] ", PAT[j].ColName, *buffer.pFloat);
						ColPos += sizeof(float);
						break;
					case LONG:  buffer.pLong = (long*)(buffer.pChar + ColPos);
						printf("%s[%ld] ", PAT[j].ColName, *buffer.pLong);
						ColPos += sizeof(long);
						break;
					case INT:   buffer.pInt = (int*)(buffer.pChar + ColPos);
						printf("%s[%d] ", PAT[j].ColName, *buffer.pInt);
						ColPos += sizeof(int);
						break;
					case CHAR:  printf("%s[%s] ", PAT[j].ColName, (buffer.pChar + ColPos));
						ColPos += (strlen(buffer.pChar + ColPos) + 1);
						break;
					default:    printf("错误"); i--;
					}
				}

			}
			printf("\n");
			free(buffer.pChar);
		}
	}
	free(IsNull);
	free(PAT);
	free(PCT);
	free(PAGE);

	//free(PHD);        //不明白为什么会卡死？？ 不需要申请空间，内部地址包含在page，导致重复free 
	if ((fclose(fp)) != 0)
	{
		printf("%d 关闭失败：ShowTable()\n", PCT->ID);
		exit(1);
	}
}

