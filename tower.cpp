/***  攻击塔的实现
*
*
*/

#include "tower.h"
#include "enemy.h"
#include "bullet.h"
#include "mainwindow.h"
#include "utility.h"
#include <QPainter>
#include <QColor>
#include <QTimer>
#include <QVector2D>
#include <QtMath>

const QSize Tower::ms_fixedSize(100, 100); //设定炮台长宽尺寸,满足旋转要求


Tower::Tower(QPoint pos, MainWindow *game, const QPixmap &sprite/* = QPixmap(":/image/tower.png"*/)
	: m_attacking(false)
    , m_attackRange(100) //攻击范围
    , m_damage(10)      //伤害值
    , m_fireRate(1000)  //攻击速度,单位为ms
    , m_rotationSprite(0.0)
	, m_chooseEnemy(NULL)
	, m_game(game)
	, m_pos(pos)
	, m_sprite(sprite)
{
	m_fireRateTimer = new QTimer(this);
	connect(m_fireRateTimer, SIGNAL(timeout()), this, SLOT(shootWeapon()));
}

Tower::~Tower()
{
	delete m_fireRateTimer;
	m_fireRateTimer = NULL;
}

void Tower::checkEnemyInRange()
{
	if (m_chooseEnemy)
	{
		// 这种情况下,需要旋转炮台对准敌人
		// 向量标准化
		QVector2D normalized(m_chooseEnemy->pos() - m_pos);
		normalized.normalize();
		m_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x())) - 90;

		// 如果敌人脱离攻击范围
		if (!collisionWithCircle(m_pos, m_attackRange, m_chooseEnemy->pos(), 1))
			lostSightOfEnemy();
	}
	else
	{
		// 遍历敌人,看是否有敌人在攻击范围内
		QList<Enemy *> enemyList = m_game->enemyList();
		foreach (Enemy *enemy, enemyList)
		{
			if (collisionWithCircle(m_pos, m_attackRange, enemy->pos(), 1))
			{
				chooseEnemyForAttack(enemy);
				break;
			}
		}
	}
}

void Tower::draw(QPainter *painter) const   //绘制炮台
{
	painter->save();
    QPen pen;           //绘制炮台攻击范围圈
    //pen.setColor(Qt::green);
    pen.setColor(QColor(140,250,140));
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);    //开启抗锯齿
    // 绘制攻击范围
	painter->drawEllipse(m_pos, m_attackRange, m_attackRange);

	// 绘制偏转坐标,由中心+偏移=左上
	static const QPoint offsetPoint(-ms_fixedSize.width() / 2, -ms_fixedSize.height() / 2);
	// 绘制炮塔并选择炮塔
	painter->translate(m_pos);
	painter->rotate(m_rotationSprite);
	painter->drawPixmap(offsetPoint, m_sprite);
	painter->restore();
}

void Tower::attackEnemy()   //攻击敌人
{
	m_fireRateTimer->start(m_fireRate);
}

void Tower::chooseEnemyForAttack(Enemy *enemy)  //选定一个攻击目标
{
	m_chooseEnemy = enemy;
	attackEnemy();
	m_chooseEnemy->getAttacked(this);
}

void Tower::shootWeapon()   //射击
{
	Bullet *bullet = new Bullet(m_pos, m_chooseEnemy->pos(), m_damage, m_chooseEnemy, m_game);
	bullet->move();
	m_game->addBullet(bullet);
}

void Tower::targetKilled()  //目标敌人挂了
{
    if (m_chooseEnemy)      //若此时有选定目标,则清空
		m_chooseEnemy = NULL;

    m_fireRateTimer->stop();    //停止开火,即不开炮弹
    m_rotationSprite = 0.0;     //方向归位
}

void Tower::lostSightOfEnemy()  //射程内无目标
{
    m_chooseEnemy->gotLostSight(this);
	if (m_chooseEnemy)
		m_chooseEnemy = NULL;

    m_fireRateTimer->stop();    //不开火
    m_rotationSprite = 0.0;     //归位
}

QPoint Tower::getPos()
{
    return m_pos;
}

