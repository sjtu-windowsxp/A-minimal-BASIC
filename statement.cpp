#include "statement.h"
#include "guibasic.h"
#include <QEventLoop>
#include <QDebug>

statement :: statement(){
    /* Empty */
}

statement :: ~statement(){
    /* Empty */
}
/**** LET begin ****/
LETstatement :: LETstatement(QString init_var, expression *init_exp):var(init_var), exp(init_exp){
    type = LET;
}

LETstatement :: ~ LETstatement(){
    delete exp;
}

void LETstatement::execute(EvalState &state){
    display_tree();
    if (exp -> type == strexp) state.setValue(var, ((stringexp*)exp) -> getValue());
    else if (exp -> type == identexp) {
        QString value_var = ((identifierexp*)exp) -> getName();
        QString resStr;
        int res;
        if (state.isDefinedStr(value_var)) {
            resStr = state.getStr(value_var);
            state.setValue(var, resStr);
        }
        else if (state.isDefined(value_var)) {
            res = state.getValue(value_var);
            state.setValue(var,res);
        }
        else throw BasicError("VARIABLE NOT DEFINED");
    }
    else state.setValue(var, exp->eval(state));
}


void LETstatement::display_tree() {
    QString space = "    ";
    QString tree_var = space + var;
    GuiBasic::ui_handle -> syn_tree_display(tree_var);
    exp -> display(space);
}
/**** LET end ****/


/**** REM begin ****/
REMstatement::REMstatement(QString init_anno): annotation(init_anno) {
    type = REM;
}

REMstatement::~REMstatement() {
   /* Empty */
}

void REMstatement::execute(EvalState & state)
{
   display_tree();
}
void REMstatement::display_tree()
{
    QString space = "    ";
    QString tree_anno = space + annotation;
    GuiBasic::ui_handle -> syn_tree_display(tree_anno);
}
/**** REM end ****/

/**** PRINT begin ****/
PRINTstatement::PRINTstatement(expression * init_exp):exp(init_exp) {
    type = PRINT;
}

PRINTstatement::~PRINTstatement()
{
    delete exp;
}

void PRINTstatement::execute(EvalState & state)
{
    display_tree();
    QString res = QString::number(exp->eval(state));
    GuiBasic::ui_handle -> print(res);
}

void PRINTstatement::display_tree()
{
    exp -> display("    ");
}
/**** PRINT end ****/

/**** PRINTF begin ****/
PRINTFstatement::PRINTFstatement(QStringList *init_str_list, QQueue<int> *init_mark, QQueue<expression*> *init_exp_queue):result(init_str_list), mark(init_mark), expQueue(init_exp_queue) {
    type = PRINTF;
}

PRINTFstatement::~PRINTFstatement()
{
    delete result;
    delete mark;
    while(!expQueue -> isEmpty()) delete expQueue -> dequeue();
    delete expQueue;
}

void PRINTFstatement::execute(EvalState & state)
{
    display_tree();
    QStringList *tmpRes = new QStringList;
    QQueue<int> *tmpMar = new QQueue<int>;
    QQueue<expression*> *tmpExp = new QQueue<expression*>;

    QStringList::iterator pResult = result -> begin();
    QQueue<int>::iterator pMark = mark -> begin();
    QQueue<expression*>::iterator pExp = expQueue -> begin();
    while (pResult != result -> end()) {
        tmpRes -> append(*pResult);
        pResult++;
    }
    while(pMark != mark -> end()) {
        int index = *pMark;
        expression* exp = *pExp;
        QString res;
        if (exp -> type == identexp) {
            QString var = ((identifierexp*)exp) -> getName();
            if (state.isDefinedStr(var)) res = state.getStr(var);
            else if (state.isDefined(var)) res = QString::number(state.getValue(var));
            else throw BasicError("VARIABLE NOT DEFINED");
        }
        else if (exp -> type == strexp) res = ((stringexp*)exp) -> getValue();
        else res = QString::number(exp -> eval(state));//constexp and compoundexp
        tmpRes -> replace(index, res);
        pMark++;
        pExp++;
    }
    QString resultStr = tmpRes -> join(' ');
    GuiBasic::ui_handle -> print(resultStr);
    delete tmpRes;
    delete tmpMar;
    delete tmpExp;
}

void PRINTFstatement::display_tree()
{
    QString templeteStr = result -> join(' ');
    GuiBasic::ui_handle -> syn_tree_display("    " + templeteStr);
}

/**** PRINT end ****/

/**** END begin ****/
ENDstatement::ENDstatement() {
   type = END;
}

ENDstatement::~ENDstatement() {
   /* Empty */
}

void ENDstatement::execute(EvalState & state)
{
    display_tree();
    state.setPC(EvalState::HALT);
}

void ENDstatement::display_tree(){
    /*** Empty ***/
}
/**** END end ****/

/**** INPUT begin ****/
INPUTstatement::INPUTstatement(QString init_name, int num):var(init_name), num(num) {
    type = INPUT;
}

INPUTstatement::~INPUTstatement()
{
    /* Empty */
}
void INPUTstatement::display_tree()
{
    QString syn_var = "    " + var;
    GuiBasic::ui_handle -> syn_tree_display(syn_var);
}

void INPUTstatement::get_input(int var)
{
    num = var;
}

void INPUTstatement::begin_loop()
{
    QEventLoop loop;
    connect(GuiBasic::ui_handle, SIGNAL(input_num(int)), &loop, SLOT(quit()));
    loop.exec();
}

void INPUTstatement::execute(EvalState & state)
{
    display_tree();
    connect(GuiBasic::ui_handle, SIGNAL(input_num(int)), this, SLOT(get_input(int)));
    connect(GuiBasic::ui_handle, SIGNAL(stop_prog_input()), this, SLOT(begin_loop()));
    GuiBasic::ui_handle -> set_ques_mark_int();
    state.setValue(var, num);
    disconnect(GuiBasic::ui_handle, SIGNAL(input_num(int)), this, SLOT(get_input(int)));
    disconnect(GuiBasic::ui_handle, SIGNAL(stop_prog_input()), this, SLOT(begin_loop()));
}
/**** INPUT end ****/


/**** GOTO begin ****/
GOTOstatement::GOTOstatement(int ln):line_number(ln){
    type = GOTO;
}

GOTOstatement::~GOTOstatement(){
    /* Empty */
}

void GOTOstatement::execute(EvalState &state)
{
    display_tree();
    state.setPC(line_number);
}

void GOTOstatement::display_tree()
{
    QString space = "    ";
    QString tree_dis = space + QString::number(line_number);
    GuiBasic::ui_handle -> syn_tree_display(tree_dis);
}
/**** GOTO end ****/

/**** IF begin ****/
IFstatement::IFstatement(int ln, QString op, expression * exp1,expression * exp2):line_number(ln),op(op) ,exp1(exp1), exp2(exp2){
    type = IF;
}

IFstatement::~IFstatement() {
    delete exp1;
    delete exp2;
}

void IFstatement::execute(EvalState & state)
{
    display_tree();
    int val_exp1 = exp1 -> eval(state);
    int val_exp2 = exp2 -> eval(state);
    bool res = false;
    if (op == '>') {
        res = val_exp1 > val_exp2;
    }
    else if (op == '<') {
        res = val_exp1 < val_exp2;
    }
    else if (op == '=') {
        res = val_exp1 == val_exp2;
    }
    if (res) {
        state.setPC(line_number);
    }
}

void IFstatement::display_tree()
{
    QString space = "    ";
    exp1 -> display(space);
    GuiBasic::ui_handle -> syn_tree_display(space + op);
    exp2 -> display(space);
    GuiBasic::ui_handle -> syn_tree_display(space + QString::number(line_number));
}
/**** IF end ****/

/**** INPUTS begin ****/
INPUTSstatement::INPUTSstatement(QString varTmp, QString value):var(varTmp),value(value)
{
    type = INPUTS;
}

INPUTSstatement::~INPUTSstatement()
{
    /* Empty */
}

void INPUTSstatement::execute(EvalState &state)
{
    display_tree();
    connect(GuiBasic::ui_handle, SIGNAL(input_str(QString)), this, SLOT(get_input(QString)));
    connect(GuiBasic::ui_handle, SIGNAL(stop_prog_input()), this, SLOT(begin_loop()));
    GuiBasic::ui_handle -> set_ques_mark_str();
    state.setValue(var, value);
    disconnect(GuiBasic::ui_handle, SIGNAL(input_str(QString)), this, SLOT(get_input(QString)));
    disconnect(GuiBasic::ui_handle, SIGNAL(stop_prog_input()), this, SLOT(begin_loop()));
}

void INPUTSstatement::display_tree()
{
    QString syn_var = "    " + var;
    GuiBasic::ui_handle -> syn_tree_display(syn_var);
}

void INPUTSstatement::begin_loop()
{
    QEventLoop loop;
    connect(GuiBasic::ui_handle, SIGNAL(input_str(QString)), &loop, SLOT(quit()));
    loop.exec();
}

void INPUTSstatement::get_input(QString var)
{
    value = var;
}
/**** INPUTS end ****/
