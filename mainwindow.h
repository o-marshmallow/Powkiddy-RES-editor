#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include "version.h"
#include "resfile.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ResFile *res;

private slots:
    void openFileClicked();
    void imageSelected(int);
    void showAbout();
};

#endif // MAINWINDOW_H
