#include "HandwrittenDatabases.h"

HandwrittenDatabases::HandwrittenDatabases() : row(0), col(0) {
    readMySQL();
    readAllTables();
}

HandwrittenDatabases::~HandwrittenDatabases() {
    writeMySQL();
}

void HandwrittenDatabases::readMySQL() {
    my_sql_num = 0; //读入新状态前先将原状态归零
    const string my_sql_filename = "my_sql.txt"; //设置数据库状态文件名

    //打开文件，若文件不存在，则创建文件
    ifstream in_file(my_sql_filename, ios::in);

    while (in_file.peek() != EOF)
    {
        in_file >> my_sql[my_sql_num].table_name;
        in_file >> my_sql[my_sql_num].file_name;
        in_file >> my_sql[my_sql_num].index;
        if (isNotEndl(my_sql[my_sql_num].table_name[0]) == 1)
            my_sql_num++;
    }

    in_file.close();
}

void HandwrittenDatabases::writeMySQL() {
    const string my_sql_filename = "my_sql.txt"; //设置数据库状态文件名
    ofstream out_file(my_sql_filename, ios::out);
    for (int i = 0; i < my_sql_num; i++)
    {
        //表名 文件名 下标
        out_file << my_sql[i].table_name << " ";
        out_file << my_sql[i].file_name << " ";
        out_file << my_sql[i].index << endl;
    }
    out_file.close();
}

void HandwrittenDatabases::readAllTables() {
    table_num = 0; //表格数归零

    for (int i = 0; i < my_sql_num; i++)
    {
        //每读一行数据库信息都会得到一张表
        int ind = my_sql[i].index;
        all_table[ind].table_name = my_sql[i].table_name;
        all_table[ind].file_name = my_sql[i].file_name;
        readTable(ind, my_sql[i].file_name);
        if (isNotEndl(all_table[ind].file_name[0]) == 1)
            table_num++;
    }
}

void HandwrittenDatabases::readTable(int index, const string& filename) {
    all_table[index].col_num = 0; //读入表头前先将表格列数归零

    //打开文件，若文件不存在，则创建文件
    ifstream in_file(filename, ios::in);
    string s;
    getline(in_file, s);
    int i = 0;

    //解析表头
    while (i < s.size())
    {
        int k = 0;
        string str;
        if (s[i + k] == ' ') //规避汉字中间的两个空格
            i++;
        while (i + k != s.size() && s[i + k] != ' ')
        {
            str += s[i + k];
            k++;
        }

        int c = all_table[index].col_num;
        all_table[index].column[c] = str;
        all_table[index].col_num++;
        i = i + k + 1;
    }

    in_file.close();
}

void HandwrittenDatabases::writeTable(int index, const string& filename) {
    ofstream out_file(filename, ios::out);
    for (int i = 0; i < all_table[index].col_num; i++)
        out_file << all_table[index].column[i] << " ";
    out_file << endl;
    out_file.close();
}

void HandwrittenDatabases::readTableData(int index, const string& filename) {
    all_table[index].row_num = 0; //读入表头前先将表格行数归零

    //打开文件，若文件不存在，则创建文件
    ifstream in_file(filename, ios::in);

    //先读表头
    string s;
    getline(in_file, s);

    //剩下的是表格数据
    while (in_file.peek() != EOF)
    {
        for (int j = 0; j < all_table[index].col_num; j++)
            in_file >> table_data[all_table[index].row_num][j]; //用二维数组接受表格信息
        if (table_data[all_table[index].row_num][0] >= "0" && table_data[all_table[index].row_num][0] <= "9")
            all_table[index].row_num++;
    }
    in_file.close();
}

void HandwrittenDatabases::writeTableData(int index, const string& filename) {
    ofstream out_file(filename, ios::out);
    //写入表头
    for (int i = 0; i < all_table[index].col_num; i++)
    {
        out_file << all_table[index].column[i] << " ";
    }
    out_file << endl;

    //写入数据
    for (int i = 0; i < all_table[index].row_num; i++)
    {
        for (int j = 0; j < all_table[index].col_num; j++)
        {
            out_file << table_data[i][j] << " ";
        }
        out_file << endl;
    }
    out_file.close();
}

void HandwrittenDatabases::createTable(char ins[]) {
    readMySQL();
    readAllTables();

    //解析指令获取表名，记录到总表格数组中
    string tablename;
    int i = 13; //移动到指令中name的第一位
    while (ins[i] != ' ')
    {
        tablename += ins[i];
        i++;
    }
    all_table[table_num].table_name = tablename;

    i += 1;
    //第一类创建语句
    if (ins[i] == '(')
    {
        //检测表格名是否重复
        for (int j = 0; j < my_sql_num; j++)
        {
            if (my_sql[j].table_name == tablename)
            {
                //表格名已存在
                cout << "TABLE " << tablename << " Already Exists !" << endl;
                return;
            }
        }
        //解析指令获取每一列的属性名，记录到总表格数组中
        i += 1; //移动到指令中column[i]的第一位
        int k = 0;
        string col_name[50];
        col = 0;
        while (true)
        {
            col_name[col] += ins[i + k];
            k++;
            if (ins[i + k] == ',' || ins[i + k] == '，')
            {
                //把每列的名字给到对应表格的column数组中，对应列数+1
                int c = all_table[table_num].col_num;
                all_table[table_num].column[c] = col_name[col];
                all_table[table_num].col_num++; col++;
                //移动至下一个列名的第一位
                i = i + k + 1;
                k = 0;
            }
            else if (ins[i + k] == ')' || ins[i + k] == '）')
            {
                //把每列的名字给到对应表格的column数组中，对应列数+1
                int c = all_table[table_num].col_num;
                all_table[table_num].column[c] = col_name[col];
                all_table[table_num].col_num++; col++;
                i = i + k + 1;
                break;
            }
        }

        //解析指令获取文件名，记录到总表格数组中
        i += 4; //移动到指令中file的第一位
        string filename;
        while (ins[i] != '\0')
        {
            filename += ins[i];
            i++;
        }
        all_table[table_num].file_name = filename;

        //创建表格文件并写入表头
        writeTable(table_num, filename);

        //修改数据库状态
        my_sql[my_sql_num].table_name = tablename;
        my_sql[my_sql_num].file_name = filename;
        my_sql[my_sql_num].index = table_num;
        my_sql_num++;
        writeMySQL(); //更新数据库状态文件

        table_num++; //最后，表格数量加一

        //展示最新创建的表头
        cout << "--------------------" << endl;
        cout << "ID ";
        for (int i = 0; i < all_table[table_num - 1].col_num; i++)
        {
            cout << all_table[table_num - 1].column[i] << " ";
        }
        cout << endl;
        cout << "--------------------" << endl;
    }

    //第二类创建语句（相当于打印已有表格）
    else if (ins[i] == 'F')
    {
        int index;
        string filename;
        bool flag = 1;
        for (int j = 0; j < my_sql_num; j++)
        {
            if (my_sql[j].table_name == tablename)
            {
                //找到了对应的表格
                index = my_sql[j].index;
                filename = my_sql[j].file_name;
                readTableData(index, filename);
                printTable(index);
                flag = 0;
                break;
            }
            else if (my_sql[j].table_name == "None")
            {
                //给无名氏表格命名
                my_sql[j].table_name = tablename;
                writeMySQL();
                index = my_sql[j].index;
                filename = my_sql[j].file_name;
                readTableData(index, filename);
                printTable(index);
                flag = 0;
                break;
            }
        }
        if (flag == 1) //没找到
        {
            cout << "Not found TABLE " << tablename << " !" << endl;
            return;
        }
    }

    else
    {
        cout << "Error instruction!" << endl;
    }
}

void HandwrittenDatabases::dropTable(char ins[]) {
    readMySQL();
    readAllTables();

    //解析指令获取待删除表名
    string tablename;
    int i = 11;
    while (ins[i] != '\0')
    {
        tablename += ins[i];
        i++;
    }

    bool flag = 1;
    for (int j = 0; j < my_sql_num; j++)
    {
        if (my_sql[j].table_name == tablename)
        {
            //找到待删的表格，则进行数据迁移
            for (int k = j; k < my_sql_num - 1; k++)
            {
                my_sql[k].table_name = my_sql[k + 1].table_name;
                my_sql[k].file_name = my_sql[k + 1].file_name;
            }
            table_num--;
            my_sql_num--;
            flag = 0;
            cout << "DROP TABLE SUCCESS !" << endl;
            break;
        }
    }

    if (flag == 1) //没找到
    {
        cout << "Not found TABLE " << tablename << " !" << endl;
        return;
    }

    writeMySQL();//更新数据库状态文件
    readAllTables(); //修改当前表格列表
}

void HandwrittenDatabases::listTables() {
    readMySQL();
    readAllTables();
    //依次读取每张表格的数据
    for (int i = 0; i < table_num; i++)
    {
        readTableData(i, all_table[i].file_name);
    }
    //打印所有表格信息
    cout << "   Total table number:" << table_num << endl;
    cout << " 表格名称 ：（列数，行数）[属性列表]" << endl;
    for (int i = 0; i < table_num; i++)
    {
        cout << "      " << all_table[i].table_name << ":";
        cout << "（" << all_table[i].col_num << "，" << all_table[i].row_num << "）";
        if (all_table[i].col_num == 0)
            cout << "[ ]" << endl;
        else
        {
            cout << "[" << all_table[i].column[0];
            for (int j = 1; j < all_table[i].col_num; j++)
                cout << "，" << all_table[i].column[j];
            cout << "]" << endl;
        }
    }
}

void HandwrittenDatabases::insertData(char ins[]) {
    readMySQL();
    readAllTables();

    //解析指令获取表名
    string tablename;
    int i = 12; //移动到指令中name的第一位
    while (ins[i] != ' ')
    {
        tablename += ins[i];
        i++;
    }
    all_table[table_num].table_name = tablename;

    //根据表名打开对应文件，读取表格数据
    int index;
    string filename;
    bool flag = 1;
    for (int j = 0; j < my_sql_num; j++)
    {
        if (my_sql[j].table_name == tablename)
        {
            //找到了对应的表格
            index = my_sql[j].index;
            filename = my_sql[j].file_name;
            readTableData(index, filename);
            flag = 0;
            break;
        }
    }
    if (flag == 1) //没找到
    {
        cout << "Not found TABLE " << tablename << " !" << endl;
        return;
    }

    i += 1;
    //第一类插入语句
    if (ins[i] == 'V')
    {
        i += 8; //i移动到value[i]的第一位
        int k = 0;
        string col_value[50];
        int num = 0;
        col = 0, row = all_table[index].row_num;
        while (true)
        {
            col_value[num] += ins[i + k];
            k++;
            if (ins[i + k] == ',' || ins[i + k] == '，')
            {
                //把每列的值给到对应表格的值数组中，对应列数+1
                table_data[row][col] = col_value[num];
                num++; col++;
                //移动至下一个值
                i = i + k + 1;
                k = 0;
            }
            else if (ins[i + k] == ')' || ins[i + k] == '）')
            {
                //把每列的值给到对应表格的值数组中，对应列数+1
                table_data[row][col] = col_value[num];
                num++; col++;
                i = i + k + 1;
                break;
            }
        }
        cout << "INSERT SUCCESS !" << endl;
        all_table[index].row_num++;
        printTable(index);

        //修改表格文件
        writeTableData(index, filename);
    }

    //第二类插入语句
    else if (ins[i] == '(')
    {
        //获取给定列名称和添加的值
        i += 1; //i移动到column[i]的第一位
        int k = 0;
        string cols[50]; //暂存column[i]的值
        int num = 0; //给定值的个数
        while (true)
        {
            cols[num] += ins[i + k];
            k++;
            if (ins[i + k] == ',' || ins[i + k] == '，')
            {
                num++;
                //移动至下一个值
                i = i + k + 1;
                k = 0;
            }
            else if (ins[i + k] == ')' || ins[i + k] == '）')
            {
                num++;
                i = i + k + 1;
                break;
            }
        }

        i += 9; //i移动到value[i]的第一位
        k = 0;
        string value[50]; //暂存value[i]的值
        num = 0; //给定值的个数
        while (true)
        {
            string col_value;
            value[num] += ins[i + k];
            k++;
            if (ins[i + k] == ',' || ins[i + k] == '，')
            {
                num++;
                //移动至下一个值
                i = i + k + 1;
                k = 0;
            }
            else if (ins[i + k] == ')' || ins[i + k] == '）')
            {
                num++;
                i = i + k + 1;
                break;
            }
        }

        col = 0, row = all_table[index].row_num;
        for (int m = 0; m < all_table[index].col_num; m++)
        {
            bool tag = 1; //表示是否为缺省
            for (int n = 0; n < num; n++)
            {
                if (all_table[index].column[m] == cols[n])
                {
                    table_data[row][m] = value[n];
                    tag = 0;
                    break;
                }
            }
            if (tag == 1)
                table_data[row][m] = "空"; //缺省值默认为空
        }

        cout << "INSERT SUCCESS !" << endl;
        all_table[index].row_num++;
        printTable(index);

        writeTableData(index, filename);
    }
    else {
        cout << "Error instruction!" << endl;
    }
}

void HandwrittenDatabases::deleteData(char ins[]) {
    readMySQL();
    readAllTables();

    //第一类删除语句
    if (ins[7] == 'F')
    {
        //解析指令获取表名
        string tablename;
        int i = 12; //移动到指令中name的第一位
        while (ins[i] != ' ')
        {
            tablename += ins[i];
            i++;
        }
        all_table[table_num].table_name = tablename;

        //读取表格数据 
        int index;
        string filename;
        bool flag = 1;
        for (int j = 0; j < my_sql_num; j++)
        {
            if (my_sql[j].table_name == tablename)
            {
                //找到了对应的表格
                index = my_sql[j].index;
                filename = my_sql[j].file_name;
                readTableData(index, filename);
                flag = 0;
                break;
            }
        }

        if (flag == 1) //没找到
        {
            cout << "Not found TABLE " << tablename << " !" << endl;
            return;
        }

        i += 7; //i移动到待删除的column第一位
        string column;
        while (ins[i] != ' ')
        {
            column += ins[i];
            i++;
        }

        i += 3; //i移动到待删除的value第一位
        string value;
        while (ins[i] != '\0')
        {
            value += ins[i];
            i++;
        }

        col = all_table[index].col_num, row = all_table[index].row_num;

        //先找到删除的列的下标
        int column_index = -1;
        for (int k = 0; k < col; k++)
        {
            if (all_table[index].column[k] == column)
            {
                column_index = k;
                break;
            }
        }
        if (column_index == -1)
        {
            cout << "Error instruction!" << endl;
            return;
        }

        bool tag = 1;
        //再逐行搜索满足条件的记录
        for (int k = 0; k < row; k++)
        {
            if (table_data[k][column_index] == value)
            {
                tag = 0;
                //找到待删行，进行数据迁移
                for (int m = k; m < row - 1; m++)
                {
                    for (int n = 0; n < col; n++)
                    {
                        table_data[m][n] = table_data[m + 1][n];
                    }
                }
                break;
            }
        }
        if (tag == 1)
        {
            cout << "Error instruction!" << endl;
            return;
        }

        else
        {
            all_table[index].row_num--;

            cout << "DELETE SUCCESS !" << endl;
            printTable(index);

            writeTableData(index, filename);
        }
    }

    //第二类删除语句
    else if (ins[7] == '*' && ins[9] == 'F')
    {
        //解析指令获取表名
        string tablename;
        int i = 14; //移动到指令中name的第一位
        while (ins[i] != '\0')
        {
            tablename += ins[i];
            i++;
        }
        all_table[table_num].table_name = tablename;

        //读取表格数据 
        int index;
        string filename;
        bool flag = 1;
        for (int j = 0; j < my_sql_num; j++)
        {
            if (my_sql[j].table_name == tablename)
            {
                //找到了对应的表格
                index = my_sql[j].index;
                filename = my_sql[j].file_name;
                readTableData(index, filename);
                flag = 0;
                break;
            }
        }
        if (flag == 1) //没找到
        {
            cout << "Not found TABLE " << tablename << " !" << endl;
            return;
        }

        all_table[index].row_num = 0; //删除所有行
        cout << "DELETE SUCCESS !" << endl;
        printTable(index);

        writeTableData(index, filename);
    }
    else {
        cout << "Error instruction!" << endl;
    }
}

void HandwrittenDatabases::updateData(char ins[]) {
    readMySQL();
    readAllTables();

    //解析指令获取表名
    string tablename;
    int i = 7; //移动到指令中name的第一位
    while (ins[i] != ' ')
    {
        tablename += ins[i];
        i++;
    }
    all_table[table_num].table_name = tablename;

    //根据表名打开对应文件，读取表格数据
    int index;
    string filename;
    bool flag = 1;
    for (int j = 0; j < my_sql_num; j++)
    {
        if (my_sql[j].table_name == tablename)
        {
            //找到了对应的表格
            index = my_sql[j].index;
            filename = my_sql[j].file_name;
            readTableData(index, filename);
            flag = 0;
            break;
        }
    }
    if (flag == 1) //没找到
    {
        cout << "Not found TABLE " << tablename << " !" << endl;
        return;
    }

    i += 5; //移动到column i第一位
    int k = 0;
    int tag = 0;
    string s[50];
    string column[50], value[50];
    int num = 0;
    while (true)
    {
        while (ins[i + k] != ',' && ins[i + k] != '\0' && ins[i + k] != 'W')
        {
            s[num] += ins[i + k];
            k++;
        }

        if (ins[i + k] == ',')
        {
            num++;
            i = i + k + 2;
            k = 0;
        }

        else if (ins[i + k] == '\0')
        {
            tag = 1; //第一类更新语句
            num++;
            break;
        }

        else if (ins[i + k] == 'W')
        {
            tag = 2; //第二类更新语句
            num++;
            //最后一句多了一个空格，删去之
            string temp = s[num - 1].substr(0, s[num - 1].size() - 1);
            s[num - 1] = temp;
            i = i + k + 6; //i 移动到column首位
            break;
        }
    }

    //逐句再次解析
    for (int j = 0; j < num; j++)
    {
        int cur = 0;
        while (s[j][cur] != ' ')
            cur++;
        column[j] = s[j].substr(0, cur);
        cur += 3;
        value[j] = s[j].substr(cur, s[j].size());
    }

    if (tag == 1)
    {
        //对表格每一列进行遍历，搜索column[]数组判断是否需要更改
        for (int j = 0; j < all_table[index].col_num; j++)
        {
            for (int n = 0; n < num; n++)
            {
                if (all_table[index].column[j] == column[n])
                {
                    //更改这一列每一行的值
                    for (int m = 0; m < all_table[index].row_num; m++)
                    {
                        //m是表格的某一行，j是表格的某一列，n是对应要修改的值
                        table_data[m][j] = value[n];
                    }
                    break;
                }
            }
        }
        cout << "UPDATE SUCCESS !" << endl;
        printTable(index);

        //修改表格文件
        writeTableData(index, filename);
    }

    else if (tag == 2)
    {
        //记录限制条件 column = value
        string clmn, val;
        while (ins[i] != ' ')
        {
            clmn += ins[i];
            i++;
        }
        i += 3;
        while (ins[i] != '\0')
        {
            val += ins[i];
            i++;
        }

        //找到对应的列的下标
        int column_index = -1;
        for (k = 0; k < all_table[index].col_num; k++)
        {
            if (all_table[index].column[k] == clmn)
            {
                column_index = k;
                break;
            }
        }
        if (column_index == -1)
        {
            cout << "Error instruction!" << endl;
            return;
        }

        //对表格每一列进行遍历，搜索column[]数组判断是否需要更改
        for (int j = 0; j < all_table[index].col_num; j++)
        {
            for (int n = 0; n < num; n++)
            {
                if (all_table[index].column[j] == column[n])
                {
                    //只更改这一列满足限制条件的行的值
                    for (int m = 0; m < all_table[index].row_num; m++)
                    {
                        //判断是否满足条件
                        if (table_data[m][column_index] == val)
                        {
                            //m是表格的某一行，j是表格的某一列，n是对应要修改的值
                            table_data[m][j] = value[n];
                        }
                    }
                    break;
                }
            }
        }
        cout << "UPDATE SUCCESS !" << endl;
        printTable(index);

        //修改表格文件
        writeTableData(index, filename);
    }

    else
    {
        cout << "Error instruction!" << endl;
    }
}

void HandwrittenDatabases::selectData(char ins[]) {
    readMySQL();
    readAllTables();

    if (ins[7] == '*')
    {
        //解析指令获取表名
        string tablename;
        int i = 14; //移动到指令中name的第一位
        while (ins[i] != '\0' && ins[i] != ' ')
        {
            tablename += ins[i];
            i++;
        }

        //根据表名打开对应文件，读取表格数据
        int index;
        string filename;
        bool flag = 1;
        for (int j = 0; j < my_sql_num; j++)
        {
            if (my_sql[j].table_name == tablename)
            {
                //找到了对应的表格
                index = my_sql[j].index;
                filename = my_sql[j].file_name;
                readTableData(index, filename);
                flag = 0;
                break;
            }
        }
        if (flag == 1) //没找到
        {
            cout << "Not found TABLE " << tablename << " !" << endl;
            return;
        }

        //从TABLE name里选择所有列展示，即展示整个TABLE
        if (ins[i] == '\0')
        {
            printTable(index);
        }

        //对返回的查询结果按某些列进行排序展示
        else if (ins[i] == ' ' && ins[i + 1] == 'O')
        {
            i += 10; //移动到 column i 第一位
            string column[50];
            int num = 0;
            int k = 0;
            while (true)
            {
                while (ins[i + k] != ',' && ins[i + k] != ' ')
                {
                    column[num] += ins[i + k];
                    k++;
                }

                if (ins[i + k] == ',')
                {
                    num++;
                    i = i + k + 1;
                    k = 0;
                }

                else if (ins[i + k] == ' ')
                {
                    num++;
                    i = i + k + 1; //i 移动到ASC|DESC首位
                    break;
                }
            }

            //找到对应的列的下标
            int column_index = -1;
            for (k = 0; k < all_table[index].col_num; k++)
            {
                //支持按某一列的值排序
                if (all_table[index].column[k] == column[0])
                {
                    column_index = k;
                    break;
                }
            }
            if (column_index == -1)
            {
                cout << "Error instruction!" << endl;
                return;
            }

            if (ins[i] == 'A') //升序
            {
                for (int m = 0; m < all_table[index].row_num - 1; m++)
                {
                    for (int n = 0; n < all_table[index].row_num - m - 1; n++)
                    {
                        if (table_data[n][column_index] > table_data[n + 1][column_index])
                        {
                            //交换两行元素
                            for (int tt = 0; tt < all_table[index].col_num; tt++)
                            {
                                string temp = table_data[n][tt];
                                table_data[n][tt] = table_data[n + 1][tt];
                                table_data[n + 1][tt] = temp;
                            }
                        }
                    }
                }
            }

            else if (ins[i] == 'D') //降序
            {
                for (int m = 0; m < all_table[index].row_num - 1; m++)
                {
                    for (int n = 0; n < all_table[index].row_num - m - 1; n++)
                    {
                        if (table_data[n][column_index] < table_data[n + 1][column_index])
                        {
                            //交换两行元素
                            for (int tt = 0; tt < all_table[index].col_num; tt++)
                            {
                                string temp = table_data[n][tt];
                                table_data[n][tt] = table_data[n + 1][tt];
                                table_data[n + 1][tt] = temp;
                            }
                        }
                    }
                }
            }

            else
            {
                cout << "Error instruction!" << endl;
                return;
            }

            //打印
            //打印表头
            cout << "----------------------------------------" << endl;
            cout << "ID ";
            for (int j = 0; j < all_table[index].col_num; j++)
            {
                cout << all_table[index].column[j] << " ";
            }
            cout << endl;
            cout << "----------------------------------------" << endl;

            //打印数据
            for (int m = 0; m < all_table[index].row_num; m++)
            {
                cout << m + 1 << " ";
                for (int n = 0; n < all_table[index].col_num; n++)
                {
                    cout << table_data[m][n] << " ";
                }
                cout << endl;
                cout << "----------------------------------------" << endl;
            }
        }

        else if (ins[i] == ' ' && ins[i + 1] == 'W')
        {
            i += 7; //i 移动到column第一位
            //记录限制条件 column = value
            string clmn, val;
            while (ins[i] != ' ')
            {
                clmn += ins[i];
                i++;
            }
            i += 1;

            // column = value
            if (ins[i] == '=')
            {
                i += 2;
                while (ins[i] != '\0' && ins[i] != ' ')
                {
                    val += ins[i];
                    i++;
                }

                //找到对应的列的下标
                int column_index = -1;
                for (int k = 0; k < all_table[index].col_num; k++)
                {
                    if (all_table[index].column[k] == clmn)
                    {
                        column_index = k;
                        break;
                    }
                }
                if (column_index == -1)
                {
                    cout << "Error instruction!" << endl;
                    return;
                }

                if (ins[i] == '\0')
                {
                    //打印
                    //打印表头
                    cout << "----------------------------------------" << endl;
                    cout << "ID ";
                    for (int j = 0; j < all_table[index].col_num; j++)
                    {
                        cout << all_table[index].column[j] << " ";
                    }
                    cout << endl;
                    cout << "----------------------------------------" << endl;

                    //打印数据
                    for (int m = 0; m < all_table[index].row_num; m++)
                    {
                        if (table_data[m][column_index] == val)
                        {
                            cout << m + 1 << " ";
                            for (int n = 0; n < all_table[index].col_num; n++)
                            {
                                cout << table_data[m][n] << " ";
                            }
                            cout << endl;
                            cout << "----------------------------------------" << endl;
                        }
                    }
                }

                else
                {
                    i += 4; //移动到新表格文件名第一位
                    string new_filename;
                    while (ins[i] != '\0')
                    {
                        new_filename += ins[i];
                        i++;
                    }

                    //打印
                    //打印表头
                    cout << "----------------------------------------" << endl;
                    cout << "ID ";
                    for (int j = 0; j < all_table[index].col_num; j++)
                    {
                        cout << all_table[index].column[j] << " ";
                    }
                    cout << endl;
                    cout << "----------------------------------------" << endl;

                    //打印数据
                    for (int m = 0; m < all_table[index].row_num; m++)
                    {
                        if (table_data[m][column_index] == val)
                        {
                            cout << m + 1 << " ";
                            for (int n = 0; n < all_table[index].col_num; n++)
                            {
                                cout << table_data[m][n] << " ";
                            }
                            cout << endl;
                            cout << "----------------------------------------" << endl;
                        }
                    }

                    //创建新表写入文件
                    ofstream out_file(new_filename, ios::out);
                    //写入表头
                    for (int j = 0; j < all_table[index].col_num; j++)
                    {
                        out_file << all_table[index].column[j] << " ";
                    }
                    out_file << endl;

                    //写入数据
                    for (int m = 0; m < all_table[index].row_num; m++)
                    {
                        if (table_data[m][column_index] == val)
                        {
                            for (int n = 0; n < all_table[index].col_num; n++)
                            {
                                out_file << table_data[m][n] << " ";
                            }
                            out_file << endl;
                        }
                    }
                    out_file.close();

                    //修改数据库状态
                    my_sql[my_sql_num].table_name = "None";
                    my_sql[my_sql_num].file_name = new_filename;
                    my_sql[my_sql_num].index = table_num;
                    my_sql_num++;
                    writeMySQL(); //更新数据库状态文件

                }
            }

            // column > value
            else if (ins[i] == '>')
            {
                i += 2;
                while (ins[i] != '\0')
                {
                    val += ins[i];
                    i++;
                }

                //找到对应的列的下标
                int column_index = -1;
                for (int k = 0; k < all_table[index].col_num; k++)
                {
                    if (all_table[index].column[k] == clmn)
                    {
                        column_index = k;
                        break;
                    }
                }
                if (column_index == -1)
                {
                    cout << "Error instruction!" << endl;
                    return;
                }

                //打印
                //打印表头
                cout << "----------------------------------------" << endl;
                cout << "ID ";
                for (int j = 0; j < all_table[index].col_num; j++)
                {
                    cout << all_table[index].column[j] << " ";
                }
                cout << endl;
                cout << "----------------------------------------" << endl;

                //打印数据
                for (int m = 0; m < all_table[index].row_num; m++)
                {
                    if (table_data[m][column_index] > val)
                    {
                        cout << m + 1 << " ";
                        for (int n = 0; n < all_table[index].col_num; n++)
                        {
                            cout << table_data[m][n] << " ";
                        }
                        cout << endl;
                        cout << "----------------------------------------" << endl;
                    }
                }
            }

            // column < value ，其中value=max(学号)
            else if (ins[i] == '<')
            {
                i += 2;

                //找到对应的列的下标
                int column_index = -1;
                for (int k = 0; k < all_table[index].col_num; k++)
                {
                    if (all_table[index].column[k] == clmn)
                    {
                        column_index = k;
                        break;
                    }
                }
                if (column_index == -1)
                {
                    cout << "Error instruction!" << endl;
                    return;
                }

                //找学号最大值
                string max = table_data[0][column_index];
                for (int j = 0; j < all_table[index].row_num; j++)
                {
                    if (table_data[j][column_index] > max)
                    {
                        max = table_data[j][column_index];
                    }
                }

                //打印
                //打印表头
                cout << "----------------------------------------" << endl;
                cout << "ID ";
                for (int j = 0; j < all_table[index].col_num; j++)
                {
                    cout << all_table[index].column[j] << " ";
                }
                cout << endl;
                cout << "----------------------------------------" << endl;

                //打印数据
                for (int m = 0; m < all_table[index].row_num; m++)
                {
                    if (table_data[m][column_index] < max)
                    {
                        cout << m + 1 << " ";
                        for (int n = 0; n < all_table[index].col_num; n++)
                        {
                            cout << table_data[m][n] << " ";
                        }
                        cout << endl;
                        cout << "----------------------------------------" << endl;
                    }
                }
            }

            else
            {
                cout << "Error instruction!" << endl;
                return;
            }
        }

        else
        {
            cout << "Error instruction!" << endl;
        }
    }

    else if (ins[7] == 'D')
    {
        string column[50];
        int num = 0;
        int i = 16; //移动到 column i 第一位
        int k = 0;
        while (true)
        {
            while (ins[i + k] != ',' && ins[i + k] != ' ')
            {
                column[num] += ins[i + k];
                k++;
            }

            if (ins[i + k] == ',')
            {
                num++;
                i = i + k + 1;
                k = 0;
            }

            else if (ins[i + k] == ' ')
            {
                num++;
                i = i + k + 6; //i 移动到name首位
                break;
            }
        }

        //解析表名
        string tablename;
        while (ins[i] != '\0' && ins[i] != ' ')
        {
            tablename += ins[i];
            i++;
        }

        //根据表名打开对应文件，读取表格数据
        int index;
        string filename;
        bool tag = 1;
        for (int j = 0; j < my_sql_num; j++)
        {
            if (my_sql[j].table_name == tablename)
            {
                //找到了对应的表格
                index = my_sql[j].index;
                filename = my_sql[j].file_name;
                readTableData(index, filename);
                tag = 0;
                break;
            }
        }
        if (tag == 1) //没找到
        {
            cout << "Not found TABLE " << tablename << " !" << endl;
            return;
        }

        bool flag[50]; //给每一列做标记
        for (int j = 0; j < all_table[index].col_num; j++)
        {
            flag[j] = 0;
        }
        for (int j = 0; j < all_table[index].col_num; j++)
        {
            for (k = 0; k < num; k++)
            {
                if (all_table[index].column[j] == column[k])
                {
                    flag[j] = 1;
                    break;
                }
            }
        }

        //打印表头
        cout << "----------------------------------------" << endl;
        cout << "ID ";
        for (int j = 0; j < all_table[index].col_num; j++)
        {
            if (flag[j] == 1)
                cout << all_table[index].column[j] << " ";
        }
        cout << endl;
        cout << "----------------------------------------" << endl;

        //打印数据
        for (int m = 0; m < all_table[index].row_num; m++)
        {
            bool print = 1; //判断要不要打印 
            for (int n = 0; n < all_table[index].col_num; n++)
            {
                if (flag[n] == 1)
                {
                    for (int a = 0; a < m; a++)
                    {
                        if (table_data[a][n] == table_data[m][n])
                        {
                            print = 0;
                            break;
                        }
                    }
                }
            }

            if (print == 1)
            {
                cout << m + 1 << " ";
                for (int n = 0; n < all_table[index].col_num; n++)
                {
                    if (flag[n] == 1)
                        cout << table_data[m][n] << " ";
                }
                cout << endl;
                cout << "----------------------------------------" << endl;
            }
        }

    }

    else if (ins[7] == 'M')
    {
        int i = 11;
        string column;
        while (ins[i] != ')')
        {
            column += ins[i];
            i++;
        }

        //解析指令获取表名
        i += 7;
        string tablename;
        while (ins[i] != '\0')
        {
            tablename += ins[i];
            i++;
        }

        //根据表名打开对应文件，读取表格数据
        int index;
        string filename;
        bool flag = 1;
        for (int j = 0; j < my_sql_num; j++)
        {
            if (my_sql[j].table_name == tablename)
            {
                //找到了对应的表格
                index = my_sql[j].index;
                filename = my_sql[j].file_name;
                readTableData(index, filename);
                flag = 0;
                break;
            }
        }
        if (flag == 1) //没找到
        {
            cout << "Not found TABLE " << tablename << " !" << endl;
            return;
        }


        //找到对应的列的下标
        int column_index = -1;
        for (int k = 0; k < all_table[index].col_num; k++)
        {
            if (all_table[index].column[k] == column)
            {
                column_index = k;
                break;
            }
        }
        if (column_index == -1)
        {
            cout << "Error instruction!" << endl;
            return;
        }

        //找最大值
        int maxline = 0;
        string max = table_data[0][column_index];
        for (int j = 0; j < all_table[index].row_num; j++)
        {
            if (table_data[j][column_index] > max)
            {
                max = table_data[j][column_index];
                maxline = j;
            }
        }

        //打印
        //打印表头
        cout << "----------------------------------------" << endl;
        cout << "ID ";
        for (int j = 0; j < all_table[index].col_num; j++)
        {
            cout << all_table[index].column[j] << " ";
        }
        cout << endl;
        cout << "----------------------------------------" << endl;

        //打印数据
        cout << maxline + 1 << " ";
        for (int n = 0; n < all_table[index].col_num; n++)
        {
            cout << table_data[maxline][n] << " ";
        }
        cout << endl;
        cout << "----------------------------------------" << endl;

    }

    else
    {
        string column[50];
        int num = 0;
        int i = 7; //移动到 column i 第一位
        int k = 0;
        while (true)
        {
            while (ins[i + k] != ',' && ins[i + k] != ' ')
            {
                column[num] += ins[i + k];
                k++;
            }

            if (ins[i + k] == ',')
            {
                num++;
                i = i + k + 1;
                k = 0;
            }

            else if (ins[i + k] == ' ')
            {
                num++;
                i = i + k + 6; //i 移动到name首位
                break;
            }
        }

        //解析表名
        string tablename;
        while (ins[i] != '\0' && ins[i] != ' ')
        {
            tablename += ins[i];
            i++;
        }
        all_table[table_num].table_name = tablename;

        //根据表名打开对应文件，读取表格数据
        int index;
        string filename;
        bool tag = 1;
        for (int j = 0; j < my_sql_num; j++)
        {
            if (my_sql[j].table_name == tablename)
            {
                //找到了对应的表格
                index = my_sql[j].index;
                filename = my_sql[j].file_name;
                readTableData(index, filename);
                tag = 0;
                break;
            }
        }
        if (tag == 1) //没找到
        {
            cout << "Not found TABLE " << tablename << " !" << endl;
            return;
        }

        bool flag[50]; //给每一列做标记
        for (int j = 0; j < all_table[index].col_num; j++)
        {
            flag[j] = 0;
        }
        for (int j = 0; j < all_table[index].col_num; j++)
        {
            for (k = 0; k < num; k++)
            {
                if (all_table[index].column[j] == column[k])
                {
                    flag[j] = 1;
                    break;
                }
            }
        }

        //从TABLE name里选择若干列展示
        if (ins[i] == '\0')
        {
            //打印表头
            cout << "----------------------------------------" << endl;
            cout << "ID ";
            for (int j = 0; j < all_table[index].col_num; j++)
            {
                if (flag[j] == 1)
                    cout << all_table[index].column[j] << " ";
            }
            cout << endl;
            cout << "----------------------------------------" << endl;

            //打印数据
            for (int m = 0; m < all_table[index].row_num; m++)
            {
                cout << m + 1 << " ";
                for (int n = 0; n < all_table[index].col_num; n++)
                {
                    if (flag[n] == 1)
                        cout << table_data[m][n] << " ";
                }
                cout << endl;
                cout << "----------------------------------------" << endl;
            }
        }

        //从TABLE name里选择若干列展示
        else if (ins[i] == ' ' && ins[i + 1] == 'W')
        {
            i += 7; //移动到column首位
            //记录限制条件 column = value
            string clmn, val;
            while (ins[i] != ' ')
            {
                clmn += ins[i];
                i++;
            }
            i += 3;
            while (ins[i] != '\0')
            {
                val += ins[i];
                i++;
            }

            //找到对应的列的下标
            int column_index = -1;
            for (k = 0; k < all_table[index].col_num; k++)
            {
                if (all_table[index].column[k] == clmn)
                {
                    column_index = k;
                    break;
                }
            }
            if (column_index == -1)
            {
                cout << "Error instruction!" << endl;
                return;
            }

            //打印表头
            cout << "----------------------------------------" << endl;
            cout << "ID ";
            for (int j = 0; j < all_table[index].col_num; j++)
            {
                if (flag[j] == 1)
                    cout << all_table[index].column[j] << " ";
            }
            cout << endl;
            cout << "----------------------------------------" << endl;

            //打印数据
            for (int m = 0; m < all_table[index].row_num; m++)
            {
                if (table_data[m][column_index] == val)
                {
                    cout << m + 1 << " ";
                    for (int n = 0; n < all_table[index].col_num; n++)
                    {
                        if (flag[n] == 1)
                            cout << table_data[m][n] << " ";
                    }
                    cout << endl;
                    cout << "----------------------------------------" << endl;
                }
            }
        }

        else
        {
            cout << "Error instruction!" << endl;
        }
    }
}

void HandwrittenDatabases::printTable(int index) {
    cout << "----------------------------------------" << endl;
    cout << "ID ";
    for (int i = 0; i < all_table[index].col_num; i++){
        cout << all_table[index].column[i] << " ";
    }
    cout << endl;
    cout << "----------------------------------------" << endl;
    for (int i = 0; i < all_table[index].row_num; i++){
        cout << i + 1 << " ";
        for (int j = 0; j < all_table[index].col_num; j++)
            cout << table_data[i][j] << " ";
        cout << endl;
        cout << "----------------------------------------" << endl;
    }
}

bool HandwrittenDatabases::isNotEndl(char m) {
    if (((m >= 'a') && (m <= 'z')) || ((m >= 'A') && (m <= 'Z')))
        return true;
    return false;
}