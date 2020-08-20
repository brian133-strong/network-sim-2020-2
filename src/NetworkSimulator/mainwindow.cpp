#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPushButton>
#include <QGraphicsRectItem>
#include <QVector>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Network Simulator"); // title


    //connect(ui->plotBtn, SIGNAL(released()), this, SLOT(buttonPressed()));
    //connect(ui->startBtn, SIGNAL(released()), this, SLOT(buttonPressed()));
    //connect(ui->stopBtn, SIGNAL(released()), this, SLOT(buttonPressed()));
    //connect(ui->quitBtn, SIGNAL(released()), this, SLOT(buttonPressed()));

     //DISPLAYING RECTANGLE
    scene = new  QGraphicsScene();
    ui->graphicsView->setScene(scene);
    scene->setSceneRect(0, 0, 400, 400);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::buttonPressed() {
    qDebug() << "signal from pressed button";
}


void MainWindow::on_quitBtn_clicked()
{
    QObject::connect(ui->quitBtn, SIGNAL(clicked()), this, SLOT(exitSlot()));
}

void MainWindow::exitSlot() {
    MainWindow::close();
}

void MainWindow::on_plotBtn_clicked() // draws a node everytime its clicked
{
    QGraphicsEllipseItem *item = new QGraphicsEllipseItem();
    item->setFlag(QGraphicsItem::ItemIsMovable, true);
    item->setRect(0, 0, 30, 30);
    item->setVisible(true);
    item->setBrush(QBrush(Qt::blue));
    item->setPen(QPen(QBrush(Qt::white), 4));
    scene->addItem(item);
}


void MainWindow::on_startBtn_clicked() // draws a link but the link does not connect nodes at yet
{
    QGraphicsLineItem *line = new QGraphicsLineItem();
    line->setFlag(QGraphicsItem::ItemIsMovable, true);
    line->setVisible(true);
    line->rotation();
    line->setPen(QPen(QBrush(Qt::red), 2));
    line->setLine(0, 0, 100, 100);
    scene->addItem(line);

}
