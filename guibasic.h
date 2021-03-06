#ifndef GUIBASIC_H
#define GUIBASIC_H

#include <QMainWindow>

#include <QtWidgets>
#include "program.h"


QT_BEGIN_NAMESPACE
namespace Ui { class GuiBasic; }
QT_END_NAMESPACE

class GuiBasic : public QMainWindow
{
    Q_OBJECT //这是一个宏

public:
    GuiBasic(QWidget *parent = nullptr);
    ~GuiBasic();
    static GuiBasic * ui_handle;
    /**Gui提供给其他文件的接口**/
    void print(QString &);//for PRINT
    void set_ques_mark_int();//for INPUT
    void set_ques_mark_str();//for INPUTS
    void show_line(const QString &);//for program.cpp
    void show_var(const QString &);//for evalstate.cpp
    void clear_var();
    QTextCursor get_cursor();//return the cursor point to the end of file
    void syn_tree_display(QString line);//for statement's display_tree
    void error_display(QString err_meg);//for error display using messagebox
    void run_finished();//for Program::run and Program::debug to show that the program finished normally
    
signals:
    void stop_prog_input();
    void input_num(int num);
    void input_str(QString value);
    void quit_app();

private slots:
    void on_btnLoadCode_clicked();

    void on_btnClearCode_clicked();

    void on_btnRunCode_clicked();

    void on_btnDebugCode_clicked();

    void get_help();

    void on_cmdLineEdit_returnPressed();

private:
    Ui::GuiBasic *ui;
    EvalState s;
    Program pro;
    int frequency;

    void LoadFile(const QString &filename);  //按下载入代码按钮加载文件函数
};
#endif // GUIBASIC_H
