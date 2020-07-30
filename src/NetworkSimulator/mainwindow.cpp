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


    connect(ui->plotBtn, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->startBtn, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->stopBtn, SIGNAL(released()), this, SLOT(buttonPressed()));
    connect(ui->quitBtn, SIGNAL(released()), this, SLOT(buttonPressed()));

     //DISPLAYING RECTANGLE
    scene = new  QGraphicsScene();
    ui->graphicsView->setScene(scene);
    scene->setSceneRect(0, 0, 400, 400);

    QGraphicsRectItem *rect1 = new QGraphicsRectItem();
    rect1->setFlag(QGraphicsItem::ItemIsMovable, true);
    rect1->setRect(10, 50, 100, 50);
    rect1->setBrush(QBrush(Qt::blue));
    rect1->setPen(QPen(QBrush(Qt::black), 2));
    scene->addItem(rect1);

    QGraphicsRectItem *rect2 = new QGraphicsRectItem();
    rect2->setFlag(QGraphicsItem::ItemIsMovable, true);
    rect2->setRect(10, 50, 100, 50);
    rect2->setBrush(QBrush(Qt::yellow));
    rect2->setPen(QPen(QBrush(Qt::white), 2));
    scene->addItem(rect2);

    QGraphicsLineItem *line = new QGraphicsLineItem();
    line->setFlag(QGraphicsItem::ItemIsMovable, true);
    line->setVisible(true);
    line->setPen(QPen(QBrush(Qt::red), 1));
    line->setLine(10, 20, 40, 50);
    scene->addItem(line);

    // DISPLAYING NODES
//    scene = new QGraphicsScene();
//    ui->graphicsView->setScene(scene);
//    scene->addSimpleText("Take care and see you next time");

    // DRAWING a LINE
//    scene = new QGraphicsScene();
//    ui->graphicsView->setScene(scene);





}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::buttonPressed() {
    qDebug() << "signal from pressed button";
    button = new QPushButton();
}






















