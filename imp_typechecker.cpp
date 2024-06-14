#include <algorithm>
#include "imp_typechecker.hh"

ImpTypeChecker::ImpTypeChecker() {

}

void ImpTypeChecker::typecheck(Program* p) {
  env.clear();
  p->accept(this);
  cout << "Type checking successful." << endl;
  return;
}

void ImpTypeChecker::visit(Program* p) {
  p->body->accept(this);
  return;
}

void ImpTypeChecker::visit(Body* b) {
  env.add_level();
  b->var_decs->accept(this);
  b->slist->accept(this);
  env.remove_level();
  return;
}

void ImpTypeChecker::visit(VarDecList* decs) {
  list<VarDec*>::iterator it;
  for (it = decs->vdlist.begin(); it != decs->vdlist.end(); ++it) {
    (*it)->accept(this);
  }
  return;
}

void ImpTypeChecker::visit(VarDec* vd) {
    string declarationType = vd->type;
    transform(declarationType.begin(), declarationType.end(), declarationType.begin(), ::tolower);
    ImpType type;
    if (declarationType == "int") {
        type = TINT;
    } else if (declarationType == "bool") {
        type = TBOOL;
    } else {
        cout << "Invalid type: " << declarationType << endl;
        exit(0);
    }
  list<string>::iterator it;
  for (it = vd->vars.begin(); it != vd->vars.end(); ++it) {
     env.add_var(*it, type);
  }
  return;
}


void ImpTypeChecker::visit(StatementList* s) {
  list<Stm*>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it) {
    (*it)->accept(this);
  }
  return;
}

void ImpTypeChecker::visit(AssignStatement* s) {
  ImpType type = s->rhs->accept(this);
    ImpType varType = env.lookup(s->id);
    if (varType != type) {
        cout << "Type error in assignment statement" << endl;
        exit(0);
    }

  return;
}

void ImpTypeChecker::visit(PrintStatement* s) {
  ImpType t = s->e->accept(this);
  if (t != TINT && t != TBOOL) {
    cout << "Type error in print statement" << endl;
    exit(0);
  }
  return;
}

void ImpTypeChecker::visit(IfStatement* s) {
  ImpType condType = s->cond->accept(this);
    if (condType != TBOOL) {
        cout << "Type error in if statement: Condition has to be bool." << endl;
        exit(0);
    }

  s->tbody->accept(this);
  if (s->fbody != NULL)
    s->fbody->accept(this);
  return;
}

void ImpTypeChecker::visit(WhileStatement* s) {
  ImpType tcond = s->cond->accept(this);
  if (tcond != TBOOL) {
    cout << "Type error in while statement: Condition has to be bool." << endl;
    exit(0);
  }
  s->body->accept(this);
 return;
}

ImpType ImpTypeChecker::visit(BinaryExp* e) {
  ImpType t1 = e->left->accept(this);
  ImpType t2 = e->right->accept(this);
  ImpType result;
  switch(e->op) {
  case AND:
  case OR:
      if (t1 == TBOOL && t2 == TBOOL) {
          result = TBOOL;
      } else {
          cout << "Type error in AND / OR binary expression" << endl;
          exit(0);
      }
    break;
  case PLUS:
  case MINUS:
  case MULT:
  case DIV:
  case EXP:
      if (t1 == TINT && t2 == TINT) {
          result = TINT;
      } else {
          cout << "Type error in arithmetic binary expression" << endl;
          exit(0);
      }
    break;
  case LT:
  case LTEQ:
  case EQ:
      if (t1 == TINT && t2 == TINT) {
          result = TBOOL;
      } else {
          cout << "Type error in comparison binary expression" << endl;
          exit(0);
      }
    break;
  }
  return result;
}

ImpType ImpTypeChecker::visit(NumberExp* e) {
  ImpType t = TINT;
  return t;
}

ImpType ImpTypeChecker::visit(IdExp* e) {
    ImpType t = env.lookup(e->id);
  return t;
}

ImpType ImpTypeChecker::visit(ParenthExp* ep) {
  return ep->e->accept(this);
}

ImpType ImpTypeChecker::visit(CondExp* e) {
  ImpType btype = e->cond->accept(this);
  if (btype != TBOOL) {
    cout << "Type error in conditional expression: Condition has to be boolean." << endl;
    exit(0);
  }

  ImpType ttype = e->etrue->accept(this);
  ImpType ftype = e->efalse->accept(this);

  if (ttype != ftype) {
    cout << "Type error in conditional expression: Branches have to have the same type." << endl;
    exit(0);
  }
    return ttype;
}

