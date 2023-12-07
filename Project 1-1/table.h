#ifndef __TABLE_H
#define __TABLE_H

#include <string>
#include <iostream>
#include <unordered_map>
#include <map>

using namespace std;

enum datatype_t { UNKNOWN, BOOL, INT, STRING };

class Table;
class Row;
class Query;

class Row
{
private:
  Table *table_p;

public:
  vector<string> row_data;

  //Creates a dummy row with no data
  Row() {}
  ~Row() {}
  Row(Table *tb, vector<string> data);

  //Returns the value of the given attribute for this row
  const void *getValue(const string &attName) const;
};

//Prints the given Table to an output stream
//Format is the same as the CSV file
ostream &operator<<(ostream &, const Table &);

struct MyException : public std::exception
{
  std::string s;
  MyException(std::string ss) : s(ss) {}
  ~MyException() throw() {} // Updated
  const char *what() const throw() { return s.c_str(); }
};

// *** Add the line below to table.cpp ***
//unordered_map<string, Table*> Table::tables;
//Initializes static data member

class Table
{
private:
  static unordered_map<string, Table *> tables;
  string name;

public:
  map<string, datatype_t> attribute_datatype;
  vector<string> attributes;
  uint16_t attribute_num;
  vector<string> datatype_list;
  vector<Row *> rows;
  map<string, uint16_t> attribute_index;

  void setTableAttributes(const vector<string> &at, const vector<string> &dt);
  void setAttributeDatatype(const string &at_name, const string &dt);

  //Constructs a Table based on the data in the given CSV file
  //Name of Table should be the string before ".csv"
  static void readTableFromCSV(const string &file);

  //Constructs a Table based on the data in the file (name + ".dat")
  static Table *getTableByName(const string &name)
  {
    return tables.count(name) ? tables[name] : nullptr;
  }

  //Returns the datatype of the given attribute
  datatype_t getAttributeType(const string &attName) const;

  //Runs the given query on this Table, and returns a new Table with the result
  //The rows of the new Table should be the ones that cause q.getCondition()->getBoolValue(row) to return true
  //The attributes should be the ones returned by q.getAttributesToReturn(), unless
  // q.getAttributesToReturn returns the single attribute "*" (new table has all attributes)
  Table *runQuery(Query &q) const;

  static void printTableNames()
  {
    for (auto &p : tables)
      cout << p.first << '\n';
  }
  //Returns this table's name
  string getName() const
  {
    return name;
  }

  Table(string &name) {}
  Table() {}
  //Removes this table from the map when deleted
  ~Table()
  {
    if (tables.count(name) > 0)
    {
      for (auto row : rows)
        delete row;
      tables.erase(name);
    }
  }
};

#endif //__TABLE_H