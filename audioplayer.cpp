/*** 用于播放bgm和音效的AudioPlayer类
*
*
*/

#include "audioplayer.h"
#include <QDir>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDebug>
#include <string>

#define MYPATH "C:/Users/dell/Desktop/TowerDefense/TowerDefense/"       //使用直接路径读取音乐文件,MYPATH是工程目录的直接路径，注意加/与后来的Qstring连接


AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
    , m_backgroundMusic(NULL)
{
    // 背景音乐
    QUrl backgroundMusicUrl = QUrl::fromLocalFile(QString(MYPATH)+QString("music/bgm.mp3"));    //bgm音乐位置
    if (QFile::exists(backgroundMusicUrl.toLocalFile()))    //若文件存在, 那就播放
    {
        m_backgroundMusic = new QMediaPlayer(this); //实例化为m_backgroundMusic
        QMediaPlaylist *backgroundMusicList = new QMediaPlaylist();

        QMediaContent media(backgroundMusicUrl);
        backgroundMusicList->addMedia(media);
        backgroundMusicList->setCurrentIndex(0);

        backgroundMusicList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop); // 背景音乐循环播放
        m_backgroundMusic->setPlaylist(backgroundMusicList);

    }
}

void AudioPlayer::startBGM()
{
    if (m_backgroundMusic)
        m_backgroundMusic->play();

}

void AudioPlayer::playSound(SoundType soundType)    //音效播放
{
	static const QUrl mediasUrls[] =
	{
        QUrl::fromLocalFile(QString(MYPATH)+QString("music/tower_place.wav")),   //四种音效
        QUrl::fromLocalFile(QString(MYPATH)+QString("music/life_lose.wav")),
        QUrl::fromLocalFile(QString(MYPATH)+QString("music/laser_shoot.wav")),
        QUrl::fromLocalFile(QString(MYPATH)+QString("music/enemy_destroy.wav"))
	};
    static QMediaPlayer player;

    if (QFile::exists(mediasUrls[soundType].toLocalFile()))
    {
        player.setMedia(mediasUrls[soundType]);
        player.play();
    }
}
