#ifndef HANDWRITTENDATABASES_H
#define HANDWRITTENDATABASES_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

class HandwrittenDatabases {
public:
    HandwrittenDatabases();
    ~HandwrittenDatabases();

    // 公开接口函数
    void createTable(char ins[]);
    void dropTable(char ins[]);
    void listTables();
    void insertData(char ins[]);
    void deleteData(char ins[]);
    void updateData(char ins[]);
    void selectData(char ins[]);

private:
    // 数据结构
    struct SQL {
        string table_name;
        string file_name;
        int index;
    };

    struct TABLE {
        string table_name;
        string file_name;
        string column[50];
        int col_num = 0;
        int row_num = 0;
    };

    // 成员变量
    SQL my_sql[100];
    int my_sql_num = 0;
    TABLE all_table[100];
    int table_num = 0;

    string table_data[100][100];
    int row = 0, col = 0;

    // 私有函数
    void readMySQL();
    void writeMySQL();
    void readAllTables();
    void readTable(int index, const string& filename);
    void writeTable(int index, const string& filename);
    void readTableData(int index, const string& filename);
    void writeTableData(int index, const string& filename);
    void printTable(int index);
    bool isNotEndl(char m);
};

#endif // HANDWRITTENDATABASES_H