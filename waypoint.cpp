/*** 这个类是敌人行进路径的设计
*/



#include "waypoint.h"
#include <QPainter>
#include <QColor>

WayPoint::WayPoint(QPoint pos)
	: m_pos(pos)
	, m_nextWayPoint(NULL)
{
}

void WayPoint::setNextWayPoint(WayPoint *nextPoint)
{
	m_nextWayPoint = nextPoint;
}

WayPoint* WayPoint::nextWayPoint() const
{
	return m_nextWayPoint;
}

const QPoint WayPoint::pos() const
{
	return m_pos;
}

void WayPoint::draw(QPainter *painter) const
{
    painter->save();    //该painter设定为一个子进程,会用restore回退
    painter->setPen(Qt::red);
    painter->drawEllipse(m_pos, 4, 4);  //拐弯点画圆圈

	if (m_nextWayPoint)
        painter->drawLine(m_pos, m_nextWayPoint->m_pos);    //各路径点之间画线
	painter->restore();
}
