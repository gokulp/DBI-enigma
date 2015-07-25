#include <iostream>

#include "Errors.h"
#include "Interpreter.h"
#include "ParseTree.h"
#include "Statistics.h"
#include "QueryPlan.h"
#include "Ddl.h"

using std::cout;

extern "C" {
  int yyparse(void);     // defined in y.tab.c
}

extern struct FuncOperator *finalFunction; // the aggregate function (NULL if no agg)
extern struct TableList *tables; // the list of tables and aliases in the query
extern struct AndList *boolean; // the predicate in the WHERE clause
extern struct NameList *groupingAtts; // grouping atts (NULL if no grouping)
extern struct NameList *attsToSelect; // the set of attributes in the SELECT (NULL if no such atts)
extern int distinctAtts; // 1 if there is a DISTINCT in a non-aggregate query
extern int distinctFunc;  // 1 if there is a DISTINCT in an aggregate query
extern char* newtable;
extern char* oldtable;
extern char* newfile;
extern char* deoutput;
extern struct AttrList *newattrs; //Use this to build Attribute* array and record schema

void Interpreter::run() {
  char *fileName = "Statistics.txt";
  /*  Statistics s;

  Ddl d;
  QueryPlan plan(&s); */
      Statistics s;
  Ddl d;
  QueryPlan plan(&s);
  while(true) {
    cout << "SimpleDB> ";
    if (yyparse() != 0) {
      cout << "Can't parse your CNF.\n";
      continue;
    }

    //cout << "Filename = " << fileName << endl;
    s.Read(fileName);



    if (newtable) {
        //cout << "Rohit Reached here newtable" << endl;
      if (d.createTable()) cout << "Create table " << newtable << std::endl;
      else cout << "Table " << newtable << " already exists." << std::endl;
    } else if (oldtable && newfile) {
    //cout << "Rohit Reached here oldtable && newfile" << endl;
      if (d.insertInto()) cout << "Insert into " << oldtable << std::endl;
      else cout << "Insert failed." << std::endl;
    } else if (oldtable && !newfile) {
       // cout << "Rohit Reached here oldtable && !newfile" << endl;
      if (d.dropTable()) cout << "Drop table " << oldtable << std::endl;
      else cout << "Table " << oldtable << " does not exist." << std::endl;
    } else if (deoutput) {
    //cout << "Rohit Reached here oldtable && deoutput" << endl;
      plan.setOutput(deoutput);
    } else if (attsToSelect || finalFunction) {
      //   cout << "Rohit Reached here else" << endl;
      plan.plan();
      plan.print();
      plan.execute();
      //cout << "Interpreter :: Execute " << endl;
    }
    clear();
  }
}

// TODO: free lists
void Interpreter::clear() {
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
