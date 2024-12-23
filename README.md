# Handwritten_databases

代码通过文件操作以及处理，实现了一个简易的数据库，可以实现增删改查等功能。

## 1. 项目简介

`Handwritten_databases` 是一个用 C++ 编写的简易数据库系统。该系统通过文件操作来存储和管理数据，支持基本的 SQL 操作，如创建表、插入数据、删除数据、更新数据和查询数据。

## 2. 功能特性

- **创建表**：支持创建包含多个列的表，并将表结构保存到文件中。
- **插入数据**：支持向表中插入数据，并将数据保存到文件中。
- **删除数据**：支持从表中删除指定条件的数据。
- **更新数据**：支持更新表中符合条件的数据。
- **查询数据**：支持查询表中的数据，并显示查询结果。
- **删除表**：支持删除表及其数据文件。

## 3. 使用方法

### 3.1 编译和运行

1. 克隆项目到本地：

        git clone https://github.com/xinghenLuyus/Handwritten_databases.git
        cd Handwritten_databases

2. 使用 Visual Studio 打开项目文件，并编译项目。

3. 运行生成的可执行文件。

### 3.2 指令格式

1. **创建表**：
    ```sql
    createTable(CREATE TABLE <表名> (<列1>, <列2>, ...) TO <文件名>）
    createTable(CREATE TABLE <表名> FROM <文件名>)
    ```
1. **删除表**：
    ```sql
    dropTable(DROP TABLE <表名>)
    ```
1. **列出表**：
    ```sql
    listTables(LIST TABLES)
    ```
1. **插入数据**：
    ```sql
    insertData(INSERT INTO <表名> VALUES (<值1>, <值2>, ...))
    insertData(INSERT INTO <表名> (<列1>, <列2>, ...) VALUES (<值1>, <值2>, ...))
    ```
1. **删除数据**：
    ```sql
    deleteData(DELETE FROM <表名> WHERE <列n>=<值n>)
    deleteData(DELETE * FROM <表名>)
    ```
1. **更新数据**：
    ```sql
    updateData(UPDATE <表名> SET <列1>=<值1>, <列2>=<值2>, ...)
    updateData(UPDATE <表名> SET <列1>=<值1>, <列2>=<值2>, ... WHERE <列n>=<值n>)
    ```
1. **查询数据**：
    ```sql
    selectData(SELECT * FROM <表名>)
    selectData(SELECT <列1>, <列2>, ... FROM <表名>)
    selectData(SELECT * FROM <表名> ORDER BY <列1>, <列2>, ...ASC/DESC)
                                                     // ASC: 升序，DESC: 降序
    selectData(SELECT <列1>, <列2>, ... FROM <表名> WHERE <列n>=<值n>)
    selectData(SELECT <列1>, <列2>, ... FROM <表名> TO <文件名>)
    ```


### 3.3 示例指令

##### 以下是一些示例指令，展示了如何使用该数据库系统：
```sql
    createTable(CREATE TABLE Student (学号,姓名,专业) TO student.txt)
    insertData(INSERT INTO Student VALUES (170000001,张三,计算机科学与技术))
    insertData(INSERT INTO Student VALUES (170000002,李四,金融系)")
    insertData(INSERT INTO Student (学号,姓名) VALUES (170000003,王二))
    insertData(INSERT INTO Student (学号,姓名,专业) VALUES (170000004,刘五,微电子))
    selectData(SELECT * FROM Student" "UPDATE Student SET 学号 = 170000000, 专业 = 计算机科学与技术 WHERE 姓名 = 张三)
    selectData(SELECT * FROM Student" "DELETE FROM Student WHERE 姓名 = 张三)
    selectData(SELECT * FROM Student" "DROP TABLE Student)
```

### 3.4 代码示例

##### 以下是一个简单的 `main.cpp` 文件示例，展示了如何使用 `HandwrittenDatabases` 类：
```cpp
#include "HandwrittenDatabases.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    HandwrittenDatabases db;
    cout << "\033[33m----->\033[32mWelcome \033[35mto \033[36mmy \033[31mhand \033[32mSQL!\033[33m<-----\033[0m" << endl;

    // 示例指令
    char com1[] = "CREATE TABLE Student (学号,姓名,专业) TO student.txt";
    char ins[100];
    strcpy_s(ins, sizeof(ins), com1);

    cout << "Executing: " << ins << endl;

    // CREATE TABLE 创建表格功能
    if (strncmp(ins, "CREATE", 6) == 0) {
        db.createTable(ins);
    }
    // 其他指令处理...

    cout << "\033[33m-----\033[32mQuit \033[31mhand \033[36mSQL!\033[33m-----\033[0m" << endl;

    return 0;
}
```
## 4. 文件结构
```
Handwritten_databases/ 
    ├── HandwrittenDatabases.h 
    ├── HandwrittenDatabases.cpp 
    ├── main.cpp (测试文件)
    ├── README.md 
    └── ...
```