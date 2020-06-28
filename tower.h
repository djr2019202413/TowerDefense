#ifndef TOWER_H
#define TOWER_H

#include <QPoint>
#include <QSize>
#include <QPixmap>
#include <QObject>

class QPainter;
class Enemy;
class MainWindow;
class QTimer;

class Tower : QObject
{
	Q_OBJECT
public:
    Tower(QPoint pos, MainWindow *game, const QPixmap &sprite = QPixmap(":/image/tower.png"));  //构造函数
	~Tower();

    void draw(QPainter *painter) const;
	void checkEnemyInRange();
	void targetKilled();
	void attackEnemy();
	void chooseEnemyForAttack(Enemy *enemy);
	void removeBullet();
    void damageEnemy();
    void lostSightOfEnemy();
    QPoint getPos();

//    void setAttackRange(int a); //攻击范围
//    void setDamage(int a);      //伤害值
//    void setFireRate(int a);  //攻击速度,单位为ms

private slots:
    void shootWeapon(); //射击槽函数

public:
    bool			m_attacking;    // 是否攻击
    int				m_attackRange;	// 代表塔可以攻击到敌人的距离
    int				m_damage;		// 代表攻击敌人时造成的伤害
    int				m_fireRate;		// 代表再次攻击敌人的时间间隔
private:
    qreal			m_rotationSprite;//旋转方位

    Enemy *			m_chooseEnemy;  //Enemy类的一个实例,表示当前攻击目标
	MainWindow *	m_game;
    QTimer *		m_fireRateTimer;//开火定时器

    const QPoint	m_pos;      //位置
    const QPixmap	m_sprite;   //绘制的图标

	static const QSize ms_fixedSize;
};

#endif // TOWER_H
