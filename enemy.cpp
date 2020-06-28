/*** 这个类是关于敌人的
*
*
*/

#include "enemy.h"
#include "waypoint.h"
#include "tower.h"
#include "utility.h"
#include "mainwindow.h"
#include "audioplayer.h"
#include <QPainter>
#include <QColor>
#include <QDebug>
#include <QMatrix>
#include <QVector2D>
#include <QtMath>

static const int Health_Bar_Width = 20; //血槽宽

const QSize Enemy::ms_fixedSize(52, 52);     //敌人图标大小

Enemy::Enemy(WayPoint *startWayPoint, MainWindow *game, const QPixmap &sprite/* = QPixmap(":/image/enemy.png")*/)
    : QObject(0)
    , m_active(false)   //默认不使能(出发)
    , m_maxHp(150)       //总血量
    , m_currentHp(m_maxHp)   //目前血量
    , m_walkingSpeed(1.0)   //前进速度
    , m_rotationSprite(0.0) //前进角度
    , m_pos(startWayPoint->pos())
	, m_destinationWayPoint(startWayPoint->nextWayPoint())
	, m_game(game)
    , m_sprite(sprite)  //绘制图标
{
}


Enemy::~Enemy()
{
	m_attackedTowersList.clear();
	m_destinationWayPoint = NULL;
	m_game = NULL;
}

void Enemy::doActivate()    //敌人使能
{
    m_active = true;    //设定开始移动
}

void Enemy::move()      //敌人移动
{
    if (!m_active)  //首先要保证该敌人已经启动
		return;

    if (collisionWithCircle(m_pos, 1, m_destinationWayPoint->pos(), 1)) // 敌人抵达了一个路径点(路径点,即m_destinationWayPoint设置见主文件)
	{

        if (m_destinationWayPoint->nextWayPoint())  //还有下一个路径拐点
		{
			m_pos = m_destinationWayPoint->pos();
			m_destinationWayPoint = m_destinationWayPoint->nextWayPoint();
		}
        else    //没有路径拐点了,表示到了终点,即进入基地
		{
			m_game->getHpDamage();
			m_game->removedEnemy(this);
			return;
		}
	}

    QPoint targetPoint = m_destinationWayPoint->pos();	// 还在前往航点的路上, 目标为路径拐点的坐标

    // 未来修改这个可以添加移动状态,加快,减慢,m_walkingSpeed是基准值


    qreal movementSpeed = m_walkingSpeed;       //行进速度
    QVector2D normalized(targetPoint - m_pos);  //向量
    normalized.normalize();                     //向量标准化
    m_pos = m_pos + normalized.toPoint() * movementSpeed; //敌人坐标为当前坐标加上前进向量*速度


    // 默认图片向左,以左为其前进方向
    m_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x()));   // 若图片朝右可以+180进行调整
}

void Enemy::draw(QPainter *painter) const   //绘制敌人
{
	if (!m_active)
		return;

	painter->save();

    QPoint healthBarPoint = m_pos + QPoint(-Health_Bar_Width / 2 - 5 , -ms_fixedSize.height() / 3 - 10 );    //血槽绘制位置,加减数字是为了校正
	// 绘制血条
	painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::red); //红血槽为底色
	QRect healthBarBackRect(healthBarPoint, QSize(Health_Bar_Width, 2));
	painter->drawRect(healthBarBackRect);

    painter->setBrush(Qt::green);
    QRect healthBarRect(healthBarPoint, QSize((double)m_currentHp / m_maxHp * Health_Bar_Width, 2));    //按照当前血量比例绘制剩余血量,对红底色的血槽用绿色进行覆盖
	painter->drawRect(healthBarRect);

	// 绘制偏转坐标,由中心+偏移=左上
	static const QPoint offsetPoint(-ms_fixedSize.width() / 2, -ms_fixedSize.height() / 2);
	painter->translate(m_pos);
	painter->rotate(m_rotationSprite);
	// 绘制敌人
	painter->drawPixmap(offsetPoint, m_sprite);

	painter->restore();
}

void Enemy::getRemoved()    //敌人死亡移除
{
    if (m_attackedTowersList.empty())   //范围内攻击塔若空,即敌人不在塔射程内,跳出函数体
		return;

    foreach (Tower *attacker, m_attackedTowersList)
		attacker->targetKilled();

    m_game->removedEnemy(this); // 通知game,此敌人已经阵亡
}

void Enemy::getDamage(int damage)   //敌人受到攻击
{
    m_game->audioPlayer()->playSound(LaserShootSound); //射击音效
    m_currentHp -= damage;  //扣血


    if (m_currentHp <= 0)   // 阵亡,需要进行移除
	{
        m_game->audioPlayer()->playSound(EnemyDestorySound);    //播放敌人死亡音效
        m_game->awardGold(200);     //资金增加
        getRemoved();               //敌人移除
	}
}

void Enemy::getAttacked(Tower *attacker)    //  敌人进入了攻击射程
{
    m_attackedTowersList.push_back(attacker);   //增加攻击塔
}


void Enemy::gotLostSight(Tower *attacker)   // 敌人超出了攻击射程
{
    m_attackedTowersList.removeOne(attacker);   //移除攻击塔
}

QPoint Enemy::pos() const   //返回敌人的坐标点
{
	return m_pos;
}

