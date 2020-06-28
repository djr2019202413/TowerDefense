#include "winwindow.h"
#include "ui_winwindow.h"
//游戏结束时蹦出的win界面
WinWindow::WinWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WinWindow)
{
    ui->setupUi(this);

    ui->setupUi(this);
    connect(this->ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(quit()));


        ui->welcomeBox->setPixmap(QPixmap(":/image/win.png"));//加载到Label标签
        ui->welcomeBox->show();//显示
}

WinWindow::~WinWindow()
{
    delete ui;
}

void WinWindow::quit()
{
    this->close();

}
