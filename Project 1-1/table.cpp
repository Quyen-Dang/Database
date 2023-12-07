#include <vector>
#include <fstream>
#include <cmath>
#include <sstream>
#include <algorithm>

#include "query.h"
#include "table.h"

unordered_map<string, Table *> Table::tables;
class Query;

void Table::readTableFromCSV(const string &file)
{
    ifstream csv_file;
    csv_file.open(file);
    int table_name_index = file.find_last_of('.');
    auto table_name = file.substr(0, table_name_index);
    auto extention = file.substr(table_name_index + 1);
    for (auto &c : extention)
        c = toupper(c);
    if (extention != "CSV")
        throw MyException("This is not CSV file " + file);
    vector<vector<string>> rows;
    string line, word;

    while (getline(csv_file, line))
    {
        stringstream ss(line);
        vector<string> row;
        while (getline(ss, word, ','))
        {
            word.erase(remove(word.begin(), word.end(), '\r'), word.end());
            row.push_back(word);
        }
        rows.push_back(row);
    }

    Table *table_t = new Table(table_name);

    table_t->setTableAttributes(rows[0], rows[1]);

    for (uint32_t i = 2; i < rows.size(); i++)
    {
        Row *row_r = new Row(table_t, rows[i]);
        table_t->rows.push_back(row_r);
    }
    tables[table_name] = table_t;
}

ostream &operator<<(ostream &ost, const Table &table)
{
    for (auto attName : table.attributes)
        ost << attName + ",";
    ost << endl;

    for (auto attName : table.datatype_list)
        ost << attName + ",";
    ost << endl;

    for (auto row : table.rows)
    {
        for (auto value : row->row_data)
            ost << value + ",";
        ost << endl;
    }
    ost << table.rows.size() << " rows" << endl;
    return ost;
}

const void *Row::getValue(const string &attName) const
{
    if (table_p->attribute_index.find(attName) == table_p->attribute_index.end())
        return nullptr;
    if (table_p->attribute_datatype[attName] == BOOL)
    {
        if (row_data[table_p->attribute_index[attName]] == "TRUE")
            return (void *)new bool(true);
        else
            return (void *)new bool(false);
    }
    if (table_p->attribute_datatype[attName] == INT)
    {
        return (void *)new int32_t(atoi(row_data[table_p->attribute_index[attName]].c_str()));
    }
    return (void *)new char(atoi(row_data[table_p->attribute_index[attName]].c_str()));
}

Table *Table::runQuery(Query &q) const
{
    Table *table = new Table();
    vector<string> q_attNames = q.getAttributesToReturn();
    vector<string> q_datatype;
    if (q_attNames[0] == "*")
        table->setTableAttributes(this->attributes, datatype_list);
    else
    {
        for (auto q_attName : q_attNames)
        {
            auto it = attribute_datatype.find(q_attName);
            if (it->second == BOOL)
                q_datatype.push_back("BOOL");
            else if (it->second == INT)
                q_datatype.push_back("INT");
            else
                q_datatype.push_back("STRING");
        }
        table->setTableAttributes(q_attNames, q_datatype);
    }
    for (auto row : rows)
    {
        const bool *c = reinterpret_cast<const bool *>(q.getCondition()->evaluate(*row));
        if (*(bool *)(c))
        {
            vector<string> row_data;
            for (auto q_attName : table->attributes)
            {
                auto it = attribute_index.find(q_attName);
                row_data.push_back(row->row_data[it->second]);
            }
            Row *q_row = new Row(table, row_data);
            table->rows.push_back(q_row);
        }
    }
    return table;
}

void Table::setAttributeDatatype(const string &att_name, const string &att_type)
{
    datatype_t datatype;
    if (att_type == "BOOL")
        datatype = BOOL;
    else if (att_type == "INT")
        datatype = INT;
    else if (att_type == "STRING")
        datatype = STRING;
    else
    {
        throw MyException("Wrong datatype " + att_type);
    }

    if (attribute_datatype.find(att_name) != attribute_datatype.end())
        throw MyException("Duplicate attribute " + att_name);

    attribute_datatype[att_name] = datatype;
}

void Table::setTableAttributes(const vector<string> &at, const vector<string> &dt)
{
    attribute_num = at.size();
    if (attribute_num != dt.size())
        throw MyException("Wrong CSV database file format");

    for (uint16_t i = 0; i < attribute_num; i++)
    {
        if (at[i] == "")
            throw MyException("No attribute");
        attributes.push_back(at[i]);
        setAttributeDatatype(at[i], dt[i]);
        datatype_list.push_back(dt[i]);
        attribute_index[at[i]] = i;
    }
}

datatype_t Table::getAttributeType(const string &att_name) const
{
    auto it = attribute_datatype.find(att_name);
    if (it == attribute_datatype.end())
        return UNKNOWN;
    return it->second;
}

Row::Row(Table *t, vector<string> data) : table_p(t)
{
    if (data.size() != t->attribute_num)
        throw MyException("Wrong row attibute number");

    row_data = data;
    for (uint16_t i = 0; i < t->attribute_num; i++)
    {
        if (t->datatype_list[i] == "BOOL")
        {
            for (auto &c : row_data[i])
                c = toupper(c);
            if (row_data[i] != "TRUE" && row_data[i] != "FALSE")
                throw MyException(row_data[i] + " is not BOOL value");
        }
        else if (t->datatype_list[i] == "INT")
        {
            try
            {
                std::atoi(row_data[i].c_str());
            }
            catch (...)
            {
                throw MyException(row_data[i] + " is not INT value");
            }
        }
    }
}
