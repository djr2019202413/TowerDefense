#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QPixmap>

class WayPoint;
class QPainter;
class MainWindow;
class Tower;

class Enemy : public QObject
{
	Q_OBJECT
public:
    Enemy(WayPoint *startWayPoint, MainWindow *game, const QPixmap &sprite = QPixmap(":/image/enemy.png"));
	~Enemy();

    void draw(QPainter *painter) const; //绘制函数
    void move();        //移动
    void getDamage(int damage); //被打击
    void getRemoved();          //死亡移除
    void getAttacked(Tower *attacker);  //进入打击圈
    void gotLostSight(Tower *attacker); //超出打击圈
    QPoint pos() const;         //位置

    int				m_maxHp;    //总血量
    int				m_currentHp;    //剩余血量
    qreal			m_walkingSpeed; //前进速度

public slots:
    void doActivate();      //开始行动

private:
    bool			m_active;   //是否开始行动
//    int				m_maxHp;    //总血量
//    int				m_currentHp;    //剩余血量
//    qreal             m_walkingSpeed; //前进速度
    qreal			m_rotationSprite; //旋转角

    QPoint			m_pos;  //坐标点
    WayPoint *		m_destinationWayPoint;  //前进点
    MainWindow *	m_game; //游戏窗口
    QList<Tower *>	m_attackedTowersList;   //在射程内的塔

	const QPixmap	m_sprite;
	static const QSize ms_fixedSize;
};



#endif // ENEMY_H
