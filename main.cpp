#include "HandwrittenDatabases.h"
#include <iostream>
#include <string>
#include <cstring> // 包含头文件

using namespace std;

int main() {
    HandwrittenDatabases db;
    cout << "\033[33m----->\033[32mWelcome \033[35mto \033[36mmy \033[31mhand \033[32mSQL!\033[33m<-----\033[0m" << endl;

    // 示例指令
    const char* commands[] = {
        "CREATE TABLE Student (学号,姓名,专业) TO student.txt",
        "INSERT INTO Student VALUES (170000001,张三,计算机科学与技术)",
        "INSERT INTO Student VALUES (170000002,李四,金融系)",
        "INSERT INTO Student (学号,姓名) VALUES (170000003,王二)",
        "INSERT INTO Student (学号,姓名,专业) VALUES (170000004,刘五,微电子)",
        "SELECT * FROM Student",
        "UPDATE Student SET 学号 = 170000000, 专业 = 计算机科学与技术 WHERE 姓名 = 张三",
        "SELECT * FROM Student",
        "DELETE FROM Student WHERE 姓名 = 张三",
        "SELECT * FROM Student",
        "DROP TABLE Student"
    };

    char ins[200];

    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i) {
        strcpy_s(ins, sizeof(ins), commands[i]);

        cout << "（sql)==>" << ins << endl;

        // CREATE TABLE 创建表格功能
        if (strncmp(ins, "CREATE", 6) == 0) {
            db.createTable(ins);
        }
        // DROP TABLE 删除表格功能
        else if (strncmp(ins, "DROP", 4) == 0) {
            db.dropTable(ins);
        }
        // TABLE LIST 查看所有表格
        else if (strncmp(ins, "TABLE", 5) == 0) {
            db.listTables();
        }
        // INSERT INTO 插入数据
        else if (strncmp(ins, "INSERT", 6) == 0) {
            db.insertData(ins);
        }
        // DELETE 删除数据
        else if (strncmp(ins, "DELETE", 6) == 0) {
            db.deleteData(ins);
        }
        // UPDATE 更新数据
        else if (strncmp(ins, "UPDATE", 6) == 0) {
            db.updateData(ins);
        }
        // SELECT 选择数据
        else if (strncmp(ins, "SELECT", 6) == 0) {
            db.selectData(ins);
        }
        // ERROR 指令错误
        else {
            cout << "\033[31mError instruction!\033[0m" << endl;
        }
		cout << endl << endl;
    }

    cout << "\033[33m-----\033[32mQuit \033[31mhand \033[36mSQL!\033[33m-----\033[0m" << endl;

    return 0;
}