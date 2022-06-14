#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <iostream>
#include "json.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow),ctx(Context()){
    ui->setupUi(this);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::fatalError(const char *message){
    std::cerr << message << '\n';
    QApplication::exit(1);
}

void MainWindow::updateSymbol(){
    if (ctx.message() != nullptr) {
        QString message = *ctx.message()
                ? QString::asprintf("(%lu; %lu): %s", ctx.line(), ctx.symbol(), ctx.message())
                : QString::asprintf("(%lu; %lu)", ctx.line(), ctx.symbol());
        ui->l_symbol->setText(message);
        ui->b_symbol->setEnabled(true);
    } else {
        ui->l_symbol->setText("Valid JSON");
        ui->b_symbol->setEnabled(false);
    }
}

void MainWindow::updateError(const char *message) {
    ui->l_symbol->setText("");
    ui->b_symbol->setEnabled(false);
    ui->l_path->setText(message);
}

void MainWindow::handle(Result *result){
    if (result == nullptr)
        fatalError("Allocation failure");

    const ErrorResult *error;
    const LoadResult *load;

    if ((error = dynamic_cast<const ErrorResult *>(result))) {
        if (error->fatal)
            fatalError(error->message);
        else
            updateError(error->message);
    } else if ((load = dynamic_cast<const LoadResult *>(result))) {
        ui->e_field->setPlainText(load->text());
    } else if (dynamic_cast<const ValidateResult *>(result) ||
               dynamic_cast<const SuccessResult *>(result)) {
        updateSymbol();
    }

    delete result;
}

void MainWindow::on_b_load_clicked()
{
    std::string path = QFileDialog::getOpenFileName().toStdString();
    LoadOperation operation = LoadOperation(path.data());
    handle(ctx.run(operation));
}

void MainWindow::on_b_symbol_clicked()
{
    QTextCursor cursor = ui->e_field->textCursor();
    cursor.setPosition(ctx.index());
    ui->e_field->setTextCursor(cursor);
    ui->e_field->setFocus();
}

void MainWindow::on_e_field_textChanged()
{
    std::string text = ui->e_field->toPlainText().toStdString();
    ValidateOperation operation = ValidateOperation(text.data());
    handle(ctx.run(operation));
}
