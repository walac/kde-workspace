#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>
#include <QGLWidget>
#include <QGLWidget>
#include <KWin>

#include "widgets/pushbutton.h"
#include "widgets/lineedit.h"

#include "clock.h"
#include "desktop.h"
#include "desktop.moc"

Desktop::Desktop(QWidget *parent)
    : QGraphicsView(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Desktop);
    //setViewport(new QGLWidget  ( QGLFormat(QGL::StencilBuffer | QGL::AlphaChannel)   ));

    int primaryScreen = QApplication::desktop()->primaryScreen();
    QRect desktopSize = QApplication::desktop()->screenGeometry(primaryScreen);
    setGeometry(desktopSize);

    m_graphicsScene = new QGraphicsScene(desktopSize);
    m_graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(m_graphicsScene);
    setRenderHint(QPainter::Antialiasing, false);
    setDragMode(QGraphicsView::RubberBandDrag);
    setCacheMode(QGraphicsView::CacheBackground);
    setInteractive(true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Give it some silly default background
    QPixmap tile(100, 100);
    tile.fill(Qt::white);
    QPainter pt(&tile);
    QColor color(220, 220, 220);
    pt.fillRect(0, 0, 50, 50, color);
    pt.fillRect(50, 50, 50, 50, color);
    pt.end();
    setBackground(tile);

    // Make us legit via KWin
    KWin::setType(winId(), NET::Desktop);
    KWin::setState(winId(), NET::SkipPager);
    KWin::setOnAllDesktops(winId(), true);

        
    //[mX] strange observation.. 
        //try the following loop..takes only 19 - 21 MB so strange! 
    
    int count=0;
    // Tmp
    for (int i = 0; i < 20; i++)
    {
/*
        Plasma::Clock *testClock = new Plasma::Clock;
        m_graphicsScene->addItem(testClock);

        Plasma::LineEdit *testLineEdit = new Plasma::LineEdit;
        m_graphicsScene->addItem(testLineEdit);
*/
    for (int j = 0 ; j < 20 ; j++) {

        Plasma::PushButton *testButton = new Plasma::PushButton;
        testButton->setText(QString::number(i));
        testButton->moveBy(j*testButton->width()/2, i*testButton->height()/2);
        m_graphicsScene->addItem(testButton);
        count++;
     //   int x = (qrand() % 275);
      //  int y = (qrand() % 275);
        qDebug() << "moving to: " << count ;
        }
    }
}

Desktop::~Desktop()
{
}

void Desktop::setBackground(const QString &path)
{
    QPixmap image(path);
    setBackground(image);
}

void Desktop::setBackground(const QPixmap &image)
{
    resetCachedContent();
    setBackgroundBrush(image);
}

