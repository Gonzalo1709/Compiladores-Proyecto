#include "imp_codegen.hh"

void ImpCodeGen::codegen(string label, string instr) {
  if (label !=  nolabel)
    code << label << ": ";
  code << instr << endl;
}

void ImpCodeGen::codegen(string label, string instr, int arg) {
  if (label !=  nolabel)
    code << label << ": ";
  code << instr << " " << arg << endl;
}

void ImpCodeGen::codegen(string label, string instr, string jmplabel) {
  if (label !=  nolabel)
    code << label << ": ";
  code << instr << " " << jmplabel << endl;
}

string ImpCodeGen::next_label() {
  string l = "L";
  string n = to_string(current_label++);
  l.append(n);
  return l;
}

void ImpCodeGen::codegen(Program* p, string outfname) {
  nolabel = "";
  current_label = 0;
  siguiente_direccion = 0;
  p->accept(this);
  ofstream outfile;
  outfile.open(outfname);
  outfile << code.str();
  outfile.close();

  return;
}

void ImpCodeGen::visit(Program* p) {
    // Movimos el alloc al vardeclist para que se haga cada vez que se declaran variables
    // Esto evita que se hagan allocs a variables que no son usadas
    // Por ejemplo en scopes a los que no se entran en if - else
  p->body->accept(this);
  codegen(nolabel, "halt");
  return;
}

void ImpCodeGen::visit(Body * b) {
  direcciones.add_level();  
  b->var_decs->accept(this);
  b->slist->accept(this);
  // Se libera la memoria de las variables declaradas en el scope actual
    siguiente_direccion -= direcciones.variablesInCurrentLevel();
  direcciones.remove_level();
  return;
}

void ImpCodeGen::visit(VarDecList* s) {
  list<VarDec*>::iterator it;
  int mem = 0;
  for (it = s->vdlist.begin(); it != s->vdlist.end(); ++it) {
      (*it)->accept(this);
      mem += (*it)->vars.size();
  }
  if (mem > 0) {
    codegen(nolabel, "alloc", mem);
  }
  return;
}
			  
void ImpCodeGen::visit(VarDec* vd) {
  list<string>::iterator it;
  for (it = vd->vars.begin(); it != vd->vars.end(); ++it){
    direcciones.add_var(*it, ++siguiente_direccion);
  }
  return;
}

void ImpCodeGen::visit(StatementList* s) {
  list<Stm*>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it) {
    (*it)->accept(this);
  }
  return;
}

void ImpCodeGen::visit(AssignStatement* s) {
  s->rhs->accept(this);
  codegen(nolabel, "store", direcciones.lookup(s->id));
  return;
}

void ImpCodeGen::visit(PrintStatement* s) {
  s->e->accept(this);
    codegen(nolabel, "print");
  return;
}

void ImpCodeGen::visit(IfStatement* s) {
  string l1 = next_label();
  string l2 = next_label();
  
  s->cond->accept(this);

  codegen(nolabel, "jmpz", l1);

  s->tbody->accept(this);

  codegen(nolabel, "goto", l2);
  codegen(l1, "skip");
  if (s->fbody!=NULL) {
    s->fbody->accept(this);
  }
  codegen(l2, "skip");


  return;
}

void ImpCodeGen::visit(WhileStatement* s) {
  string l1 = next_label();
  string l2 = next_label();

  direcciones.add_level(); // Se añade para que las variables declaradas dentro del while no sean accesibles fuera
  s->body->var_decs->accept(this); // Se acepta antes de la ejecución del while para que se haga el alloc
  codegen(l1, "skip");
  s->cond->accept(this);
  codegen(nolabel, "jmpz", l2);

  s->body->slist->accept(this); // Solo el slist porque el vdlist se tiene que aceptar antes para el alloc
  codegen(nolabel, "goto", l1);
  codegen(l2, "skip");
    // Tenemos que liberar los espacios en memoria de las variables declaradas dentro del while
    // Esto lo podemos hacer contando la cantidad de variables declaradas en el while y liberando esa cantidad de espacios
    siguiente_direccion -= direcciones.variablesInCurrentLevel();
    direcciones.remove_level();
  return;
}

void ImpCodeGen::visit(DoWhileStatement* s) {
    string l1 = next_label();
  string l2 = next_label();

  direcciones.add_level();
  s->body->var_decs->accept(this);
  codegen(l1, "skip");
  s->body->slist->accept(this);
  s->cond->accept(this);
  codegen(nolabel, "jmpz", l2);
  codegen(nolabel, "goto", l1);
  codegen(l2, "skip");
    siguiente_direccion -= direcciones.variablesInCurrentLevel();
    direcciones.remove_level();
  return;
}

int ImpCodeGen::visit(BinaryExp* e) {
  e->left->accept(this);
  e->right->accept(this);
  string op = "";
  switch(e->op) {
  case PLUS: op =  "add"; break;
  case MINUS: op = "sub"; break;
  case MULT:  op = "mul"; break;
  case DIV:  op = "div"; break;
  case LT:  op = "lt"; break;
  case LTEQ: op = "le"; break;
  case EQ:  op = "eq"; break;
  default: cout << "binop " << Exp::binopToString(e->op) << " not implemented" << endl;
  }
  codegen(nolabel, op);

  return 0;
}

int ImpCodeGen::visit(NumberExp* e) {
    codegen(nolabel, "push", e->value);
  return 0;
}

int ImpCodeGen::visit(IdExp* e) {
    codegen(nolabel, "load", direcciones.lookup(e->id));
  return 0;
}

int ImpCodeGen::visit(ParenthExp* ep) {
  ep->e->accept(this);
  return 0;
}

int ImpCodeGen::visit(CondExp* e) {
  string l1 = next_label();
  string l2 = next_label();
 
  e->cond->accept(this);

  codegen(nolabel, "jmpz", l1);
  e->etrue->accept(this);

  codegen(nolabel, "goto", l2);
  codegen(l1, "skip");
  e->efalse->accept(this);
  codegen(l2, "skip");

  return 0;
}
