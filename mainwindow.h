#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "json.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void fatalError(const char *message);
    void updateSymbol();
    void updateError(const char *message);
    void handle(Result *result);

private slots:
    void on_b_load_clicked();

    void on_b_symbol_clicked();

    void on_e_field_textChanged();

private:
    Ui::MainWindow *ui;
    Context ctx;
};

#endif // MAINWINDOW_H
