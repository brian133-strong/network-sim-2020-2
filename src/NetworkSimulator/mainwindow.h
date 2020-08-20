#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsScene>

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
    void buttonPressed();
    void exitSlot();
    void on_quitBtn_clicked();



    void on_plotBtn_clicked();

    void on_startBtn_clicked();

private:
    Ui::MainWindow *ui;
    QPushButton *button;
    QGraphicsScene *scene;
    QString string;
};
