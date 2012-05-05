/*
 * Copyright 2007 Aaron Seigo <aseigo@kde.org
 * Copyright (C) 2012 Shaun Reich <shaun.reich@blue-systems.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FULLVIEW_H
#define FULLVIEW_H

#include <KMainWindow>

#include <Plasma/Applet>

#include <Plasma/Corona>

#include <QGraphicsView>

class QTimer;

namespace Plasma
{
    class AccessAppletJob;
class PushButton;
}

class FullView : public QGraphicsView
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.salViewer")

public:
    explicit FullView(const QString &formfactor = "planar", const QString &location = "floating", bool persistentConfig = false, QWidget *parent = 0);
    ~FullView();

public Q_SLOTS:
    void toggle(int screen);
    void setContainment(Plasma::Containment *containment);
    void updateGeometry();

protected:
    void showEvent(QShowEvent *event);
    virtual void focusInEvent(QFocusEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void closeEvent(QCloseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
private:

    Plasma::Corona *m_corona;
    Plasma::Containment *m_containment;
    Plasma::Applet *m_applet;
    Plasma::PushButton *m_closeButton;
};

#endif

