#include "welcomewindow.h"
#include "ui_welcomewindow.h"
#include "mainwindow.h"
//开始界面
WelcomeWindow::WelcomeWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WelcomeWindow)
{

    ui->setupUi(this);
    connect(this->ui->startButton,SIGNAL(clicked(bool)),this,SLOT(start()));//和开始游戏的按钮连起来
    connect(this->ui->quitButton,SIGNAL(clicked(bool)),this,SLOT(quit()));//和退出游戏的按钮连起来


        ui->welcomeBox->setPixmap(QPixmap(":/image/welcome.jpg"));//加载到Label标签
        ui->welcomeBox->show();//显示

}


WelcomeWindow::~WelcomeWindow()
{
    delete ui;
}

void WelcomeWindow::quit()
{
    this->close();

}


void WelcomeWindow::start(){
     mainwindow = new MainWindow;
     mainwindow->show();
     this->close();
}
