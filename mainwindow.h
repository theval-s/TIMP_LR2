#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QProcess>
#include <QInputDialog>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_directory_button_clicked();

    void on_name_button_clicked();

private:
    void closeEvent (QCloseEvent *event);
    Ui::MainWindow *ui;
    void check_trial();
    void start_uninstalling();
    bool successful = false;
};
#endif // MAINWINDOW_H
