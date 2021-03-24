#ifndef EXP_H
#define EXP_H

#include "evalstate.h"
#include <QString>

class expression
{
public:
    expression();
    virtual ~expression();
    virtual int eval(EvalState & state) = 0;
};

class constantexp: public expression{
public:
    constantexp(int val);
    virtual int eval(EvalState & state);
private:
    int value;
};

class identifierexp : public expression {
public:
    identifierexp(QString id);
    virtual int eval(EvalState & state);
private:
    QString name;
};

class compoundexp: public expression {
public:
    compoundexp(QString _op, expression * _lhs, expression * _rhs);
    virtual ~compoundexp();
    expression *getLHS();
    expression *getRHS();
private:
    QString op;
    expression *lhs, *rhs;
};
#endif // EXP_H
