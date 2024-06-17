#ifndef IMP_CODEGEN
#define IMP_CODEGEN

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>

#include "imp.hh"
#include "imp_visitor.hh"
#include "environment.hh"

class ImpCodeGen : public ImpVisitor {
    int variablesToAllocate = 0;
public:
  void codegen(Program*, string outfname, int variablesToAllocateIn = 0);
  void visit(Program*);
  void visit(Body*);
  void visit(VarDecList*);
  void visit(VarDec*);
  void visit(StatementList*);
  void visit(AssignStatement*);
  void visit(PrintStatement*);
  void visit(IfStatement*);
  void visit(WhileStatement*);
  void visit(DoWhileStatement*);

  
  int visit(BinaryExp* e);
  int visit(NumberExp* e);
  int visit(IdExp* e);
  int visit(ParenthExp* e);
  int visit(CondExp* e);

private:
  std::ostringstream code;
  string nolabel;
  int current_label;
  Environment<int> direcciones;
  int siguiente_direccion;
  void codegen(string label, string instr);
  void codegen(string label, string instr, int arg);
  void codegen(string label, string instr, string jmplabel);
  string next_label();
};


#endif

