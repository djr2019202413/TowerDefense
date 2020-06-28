#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "waypoint.h"
#include "enemy.h"
#include "bullet.h"
#include "audioplayer.h"
#include "plistreader.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtGlobal>
#include <QMessageBox>
#include <QTimer>
#include <QXmlStreamReader>
#include <QtDebug>

#define BUILDA 0
#define BUILDB 1
#define BUILDC 2
#define SELL 3


static const int TowerCostA = 100;
static const int TowerCostB = 200;
static const int TowerCostC = 300;

static const int SellGet = 50;

int Tool;


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, m_waves(0)
	, m_playerHp(5)
    , m_playrGold(1000)
	, m_gameEnded(false)
	, m_gameWin(false)
{
	ui->setupUi(this);

    preLoadWavesInfo();     //敌人每波进攻的信息
    loadTowerPositions();   //塔
    addWayPoints();         //路径

    m_audioPlayer = new AudioPlayer(this);  //音效预备
    m_audioPlayer->startBGM();              //bgm生效

    QTimer *timer = new QTimer(this);   //定时器
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMap())); //定时器重绘地图
    connect(timer, SIGNAL(timeout()), this, SLOT(getTool())); //定时器重绘地图

    timer->start(30);


    QTimer::singleShot(1000, this, SLOT(gameStart())); // 设置1000ms后游戏启动
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::loadTowerPositions()   //建塔点的预设信息
{
	QFile file(":/config/TowersPosition.plist");
    if (!file.open(QFile::ReadOnly | QFile::Text))  //文件不存在时报错,因为直接导致游戏无法进行
	{
        QMessageBox::warning(this, "TowerDefense", "Cannot Open TowersPosition.plist");
		return;
	}

    PListReader reader;
	reader.read(&file);

	QList<QVariant> array = reader.data();
	foreach (QVariant dict, array)
	{
		QMap<QString, QVariant> point = dict.toMap();
        int x = point.value("x").toInt();
		int y = point.value("y").toInt();
        x = x*60 + 10; //因为资源大小的原因,要使建塔点的图标建设在正中央,要小小校正以下,在plist文件中所写还是正常按照坐标运算即可
        y = y*60 + 10;
        m_towerPositionsList.push_back(QPoint(x, y));   //加载建塔点的位置的资源,在plist文件中写的是建塔点的格坐标,如(2格, 2格)->(1*60, 1*60)
	}

	file.close();
}

void MainWindow::paintEvent(QPaintEvent *)  //绘制事件,所有绘制更新都在这里
{
    if (m_gameWin)   //是否已输或胜
    {
        m_gameWin = false;
//        QMessageBox:: StandardButton result= QMessageBox::information(NULL, "游戏结束", "你击败了所有敌人!", QMessageBox::Yes );
//        switch (result)
//        {
//        case QMessageBox::Yes:
//            this->close();
//            break;
//        }

        winwindow = new WinWindow;
        winwindow->show();
        this->close();
    }

    if (m_gameEnded)   //是否已输或胜
    {
        m_gameEnded = false;
        QMessageBox:: StandardButton result= QMessageBox::information(NULL, "游戏结束", "基地装甲被敌人洞穿了!", QMessageBox::Yes );
        switch (result)
        {
        case QMessageBox::Yes:
            this->close();
            break;
        }
    }



    QPixmap cachePix(":/image/Bg.png");     //显示背景图片

	QPainter cachePainter(&cachePix);

    foreach (const TowerPosition &towerPos, m_towerPositionsList)   //绘炮塔可建造位置
		towerPos.draw(&cachePainter);

    foreach (const Tower *tower, m_towersList)  //绘炮塔
		tower->draw(&cachePainter);

    foreach (const WayPoint *wayPoint, m_wayPointsList) //绘路径
		wayPoint->draw(&cachePainter);

    foreach (const Enemy *enemy, m_enemyList)   //绘敌人
		enemy->draw(&cachePainter);

    foreach (const Bullet *bullet, m_bulletList)    //绘炮弹
		bullet->draw(&cachePainter);

    drawWave(&cachePainter);    //文字绘制
	drawHP(&cachePainter);
	drawPlayerGold(&cachePainter);

    QPainter painter(this);
	painter.drawPixmap(0, 0, cachePix);
}

void MainWindow::mousePressEvent(QMouseEvent *event)    //鼠标事件，按下时可能会进行建塔工作
{
    QPoint pressPos = event->pos();            //使用pressPos记录鼠标按下的点
    auto it = m_towerPositionsList.begin();    //迭代器，首位不等时开始迭代
	while (it != m_towerPositionsList.end())
	{
        if (canBuyTower(TowerCostA) && it->containPoint(pressPos) && !it->hasTower() && Tool == BUILDA) //条件判断，canBuyTower是资金足够，containPoint是放在指定位置了，hasTower是判断该位置是否已有塔
		{
            m_audioPlayer->playSound(TowerPlaceSound); //建塔音效
            m_playrGold -= TowerCostA;   //扣资金
            it->setHasTower();          //设置该位置的有塔

            Tower *tower = new Tower(it->centerPos(), this,QPixmap(":/image/tower_a.png"));    //建塔

            tower->m_attackRange=100;
            tower->m_damage=10;
            tower->m_fireRate=1000;

            m_towersList.push_back(tower);                      //丢入List
			update();
			break;
		}



        if (canBuyTower(TowerCostB) && it->containPoint(pressPos) && !it->hasTower() && Tool == BUILDB) //条件判断，canBuyTower是资金足够，containPoint是放在指定位置了，hasTower是判断该位置是否已有塔
        {
            m_audioPlayer->playSound(TowerPlaceSound); //建塔音效
            m_playrGold -= TowerCostB;   //扣资金
            it->setHasTower();          //设置该位置的有塔

            Tower *tower = new Tower(it->centerPos(), this,QPixmap(":/image/tower_b.png"));    //建塔

            tower->m_attackRange=120;
            tower->m_damage=20;
            tower->m_fireRate=800;

            m_towersList.push_back(tower);                      //丢入List
            update();
            break;
        }


        if (canBuyTower(TowerCostC) && it->containPoint(pressPos) && !it->hasTower() && Tool == BUILDC) //条件判断，canBuyTower是资金足够，containPoint是放在指定位置了，hasTower是判断该位置是否已有塔
        {
            m_audioPlayer->playSound(TowerPlaceSound); //建塔音效
            m_playrGold -= TowerCostC;   //扣资金
            it->setHasTower();          //设置该位置的有塔

            Tower *tower = new Tower(it->centerPos(), this,QPixmap(":/image/tower_c.png"));    //建塔

            tower->m_attackRange=150;
            tower->m_damage=30;
            tower->m_fireRate=600;

            m_towersList.push_back(tower);                      //丢入List
            update();
            break;
        }


//卖塔有bug,注释掉了

//        if (it->containPoint(pressPos) && it->hasTower() && Tool == SELL) //条件判断，canBuyTower是资金足够，containPoint是放在指定位置了，hasTower是判断该位置是否已有塔
//        {
//            m_audioPlayer->playSound(TowerPlaceSound); //建塔音效
//            m_playrGold += SellGet;   //得资金

//            it->setHasTower(false);          //设置该位置的无塔

//            for (auto i = 0; i < m_towersList.size(); ++i)
//            {
//                ;
//                if (m_towersList[i]->getPos() == it->centerPos())
//                {
//                    m_towersList.removeAt(i);
//                    break;
//                }

//                update();
//                break;
//            }
//        }


		++it;  
    }
}

bool MainWindow::canBuyTower(int price) const    //资金够不够买塔的判断
{
    if (m_playrGold >= price)
		return true;
	return false;
}

void MainWindow::drawWave(QPainter *painter)    //以下三个draw用于绘制游戏状态数据
{
    painter->setPen(QPen(Qt::blue));
    QFont font;
    font.setPixelSize(30);
    painter->setFont(font);
    painter->drawText(QRect(510, 10, 610, 110), QString(tr("第 %1 轮进攻")).arg(m_waves + 1));
}

void MainWindow::drawHP(QPainter *painter)
{
    painter->setPen(QPen(Qt::blue));
    QFont font;
    font.setPixelSize(30);
    painter->setFont(font);
    painter->drawText(QRect(10, 10, 110, 110), QString(tr("血量 : %1")).arg(m_playerHp));
}

void MainWindow::drawPlayerGold(QPainter *painter)
{
    painter->setPen(QPen(Qt::blue));
    QFont font;
    font.setPixelSize(30);
    painter->setFont(font);
    painter->drawText(QRect(210, 10, 310, 110), QString(tr("剩余资金 : %1")).arg(m_playrGold)); //(200, 5, 200, 25)
}

void MainWindow::doGameOver()
{
	if (!m_gameEnded)
	{
		m_gameEnded = true;
		// 此处应该切换场景到结束场景
	}
}

void MainWindow::awardGold(int gold)    //杀敌赚取资金
{
    m_playrGold += gold;
	update();
}

AudioPlayer *MainWindow::audioPlayer() const
{
	return m_audioPlayer;
}

void MainWindow::addWayPoints() //敌人行进路线
{
//由于游戏背景大小为960*480,(每个小格为60*60,则为16*8格),每个行进拐点为(横格位*60+30, 纵格位*60+30),加30是为了矫正到格子中心点

    //以下为一个逆序链表
    WayPoint *wayPoint1 = new WayPoint(QPoint(930, 210));
    m_wayPointsList.push_back(wayPoint1);

    WayPoint *wayPoint2 = new WayPoint(QPoint(690, 210));
    m_wayPointsList.push_back(wayPoint2);
    wayPoint2->setNextWayPoint(wayPoint1);

    WayPoint *wayPoint3 = new WayPoint(QPoint(690, 330));
    m_wayPointsList.push_back(wayPoint3);
    wayPoint3->setNextWayPoint(wayPoint2);

    WayPoint *wayPoint4 = new WayPoint(QPoint(270, 330));
    m_wayPointsList.push_back(wayPoint4);
    wayPoint4->setNextWayPoint(wayPoint3);

    WayPoint *wayPoint5 = new WayPoint(QPoint(270, 210));
    m_wayPointsList.push_back(wayPoint5);
    wayPoint5->setNextWayPoint(wayPoint4);

    WayPoint *wayPoint6 = new WayPoint(QPoint(30, 210));
    m_wayPointsList.push_back(wayPoint6);
    wayPoint6->setNextWayPoint(wayPoint5);
}

void MainWindow::getHpDamage(int damage/* = 1*/)
{
	m_audioPlayer->playSound(LifeLoseSound);
	m_playerHp -= damage;
	if (m_playerHp <= 0)
		doGameOver();
}

void MainWindow::removedEnemy(Enemy *enemy) //
{
	Q_ASSERT(enemy);

	m_enemyList.removeOne(enemy);
	delete enemy;

    if (m_enemyList.empty())    //此波敌人打完了
	{
        ++m_waves;              //敌人波数+1
        if (!loadWave())        //如果波数已尽头,则胜利
		{
			m_gameWin = true;
			// 游戏胜利转到游戏胜利场景

		}
	}
}

void MainWindow::removedBullet(Bullet *bullet)
{
	Q_ASSERT(bullet);

	m_bulletList.removeOne(bullet);
	delete bullet;
}

void MainWindow::addBullet(Bullet *bullet)
{
	Q_ASSERT(bullet);

	m_bulletList.push_back(bullet);
}

void MainWindow::updateMap()    //更新地图函数,在定时器里调用
{
	foreach (Enemy *enemy, m_enemyList)
		enemy->move();
    foreach (Tower *tower, m_towersList)    //塔检测敌人是否还在范围内
		tower->checkEnemyInRange();
	update();
}

void MainWindow::preLoadWavesInfo()
{
	QFile file(":/config/Waves.plist");
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, "TowerDefense", "Cannot Open TowersPosition.plist");
		return;
	}

	PListReader reader;
	reader.read(&file);

	// 获取波数信息
    m_wavesInfo = reader.data();
    file.close();
}

bool MainWindow::loadWave() //读取每波敌人信息
{
    if (m_waves >= m_wavesInfo.size())
		return false;

	WayPoint *startWayPoint = m_wayPointsList.back();
	QList<QVariant> curWavesInfo = m_wavesInfo[m_waves].toList();

//	for (int i = 0; i < curWavesInfo.size(); ++i)
//	{
//		QMap<QString, QVariant> dict = curWavesInfo[i].toMap();
//		int spawnTime = dict.value("spawnTime").toInt();

//        Enemy *enemy = new Enemy(startWayPoint, this);
//		m_enemyList.push_back(enemy);
//		QTimer::singleShot(spawnTime, enemy, SLOT(doActivate()));
//	}


    for (int i = 0; i < curWavesInfo.size(); ++i)
    {
        QMap<QString, QVariant> dict = curWavesInfo[i].toMap();
        int spawnTime = dict.value("spawnTime").toInt();

        if(m_waves==0)
        {
            Enemy *enemy = new Enemy(startWayPoint, this, QPixmap(":/image/enemy_a.png"));
            enemy->m_maxHp = 50;
            enemy->m_currentHp = 50;
            enemy->m_walkingSpeed = 1.0;
            m_enemyList.push_back(enemy);
            QTimer::singleShot(spawnTime, enemy, SLOT(doActivate()));
        }

        if(m_waves==1)
        {
            Enemy *enemy = new Enemy(startWayPoint, this, QPixmap(":/image/enemy_b.png"));
            enemy->m_maxHp = 100;
            enemy->m_currentHp = 100;
            enemy->m_walkingSpeed = 2.0;
            m_enemyList.push_back(enemy);
            QTimer::singleShot(spawnTime, enemy, SLOT(doActivate()));
        }

        if(m_waves==2)
        {
            Enemy *enemy = new Enemy(startWayPoint, this, QPixmap(":/image/enemy_c.png"));
            enemy->m_maxHp = 150;
            enemy->m_currentHp = 150;
            enemy->m_walkingSpeed = 4.0;
            m_enemyList.push_back(enemy);
            QTimer::singleShot(spawnTime, enemy, SLOT(doActivate()));
        }

    }


	return true;
}

QList<Enemy *> MainWindow::enemyList() const
{
	return m_enemyList;
}

void MainWindow::gameStart()
{
	loadWave();
}


void MainWindow::getTool()
{
    if(this->ui->Tower_a->isDown())
    {
        Tool = BUILDA;
    }
    else if(this->ui->Tower_b->isDown())
    {
        Tool = BUILDB;
    }
    else if(this->ui->Tower_c->isDown())
    {
        Tool = BUILDC;
    }
    else if(this->ui->Sell->isDown())
    {
        Tool = SELL;
    }
}
