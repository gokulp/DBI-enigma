#include <iostream>
#include "Statistics.h"
#include "QueryPlan.h"
#include "Ddl.h"
#include "Errors.h"
#include "ParsedData.h"
#include "ParseTree.h"


using std::cout;

// yyparese method is defined in y.tab.c
extern "C" {
  int yyparse(void);
}

// the list of tables and aliases in the query
extern struct TableList *tables;
// the predicate in the WHERE clause
extern struct AndList *boolean;
// grouping atts (NULL if no grouping)
extern struct NameList *groupingAtts;
// the set of attributes in the SELECT (NULL if no such atts)
extern struct NameList *attsToSelect;
// if there is a distinct in non-aggregate query then this value will be 1
extern int distinctAtts;
// if there is a distinct in aggregate query then this value will be 1
extern int distinctFunc;
// New tabel generated
extern char* newtable;
// Oled tapbe pointer
extern char* oldtable;

extern char* newfile;

extern char* deoutput;
// This function pointer will be null if there is no aggregate
// the aggregate function (NULL if no agg)
extern struct FuncOperator *finalFunction;
extern struct AttrList *newattrs; //Use this to build Attribute* array and record schema

void ParsedData::run() {
    char *fileName = "Statistics.txt";
    Statistics s;
    Ddl d;
    QueryPlan plan(&s);

  while(true) {
    cout << "DBIDemo> ";
    if (yyparse() != 0) {
      cout << "Can't parse your CNF.\n";
      continue;
    }

    //cout << "Filename = " << fileName << endl;
    s.Read(fileName);

    if (newtable) {
        //cout << "Reached here newtable" << endl;
      if (d.createTable()) cout << "Create table " << newtable << std::endl;
      else cout << "Table " << newtable << " already exists." << std::endl;
    } else if (oldtable && newfile) {
    //cout << "Reached here oldtable && newfile" << endl;
      if (d.insertInto()) cout << "Insert into " << oldtable << std::endl;
      else cout << "Insert failed." << std::endl;
    } else if (oldtable && !newfile) {
       // cout << "Reached here oldtable && !newfile" << endl;
      if (d.dropTable()) cout << "Drop table " << oldtable << std::endl;
      else cout << "Table " << oldtable << " does not exist." << std::endl;
    } else if (deoutput) {
    //cout << "Reached here oldtable && deoutput" << endl;
      plan.setOutput(deoutput);
    } else if (attsToSelect || finalFunction) {
      //   cout << "Reached here else" << endl;
      plan.plan();
      cout<<"Printing Query Plan:"<<endl;
      plan.print();
      cout<<"Executing Query..."<<endl;
      plan.execute();
      //cout << "ParsedData :: Execute " << endl;
    }
    clear();
  }
}

// TODO: free lists
void ParsedData::clear() {
  newattrs = NULL;
  finalFunction = NULL;
  tables = NULL;
  boolean = NULL;
  groupingAtts = NULL;
  attsToSelect = NULL;
  newtable = oldtable = newfile = deoutput = NULL;
  distinctAtts = distinctFunc = 0;
  FATALIF (!remove ("*.tmp"), "remove tmp files failed");
}
