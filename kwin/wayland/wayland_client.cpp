/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2011 Martin Gräßlin <mgraesslin@kde.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#include "wayland_client.h"
#include "wayland.h"
#include "surface.h"
#include "wayland_bridge.h"
// KWin
#include "paintredirector.h"
// Qt
#include <QtGui/QPainter>
// wayland
#include <wayland-server.h>

namespace KWin
{
namespace Wayland
{
Client::Client(Workspace* ws, Surface* surface)
    : Toplevel(ws)
    , m_surface(surface)
    , m_decorationBridge(new Bridge(this))
    , m_decoration(NULL)
    , m_paintRedirector(NULL)
    , m_borderLeft(0)
    , m_borderRight(0)
    , m_borderTop(0)
    , m_borderBottom(0)
    , m_paddingLeft(0)
    , m_paddingRight(0)
    , m_paddingTop(0)
    , m_paddingBottom(0)
{
    geom = surface->geometry();
    // all Wayland Clients have alpha
    bit_depth = 32;
    setupCompositing();
    updateDecoration(true, true);
    connect(m_surface, SIGNAL(geometryChanged(QRect)), SLOT(setGeometry(QRect)));
    connect(m_surface, SIGNAL(damaged(QRect)), SLOT(surfaceDamaged(QRect)));
}

Client::~Client()
{
    destroyDecoration();
    workspace()->removeWaylandClient(this);
    delete m_decorationBridge;
}

void Client::updateDecoration(bool checkWorkspacePos, bool force)
{
    Q_UNUSED(checkWorkspacePos)
    QRect oldgeom = geometry();
    if (force) {
        destroyDecoration();
    }
    m_decoration = workspace()->createDecoration(m_decorationBridge);
    m_decoration->init();
    // TODO: install an event filter
    m_decoration->borders(m_borderLeft, m_borderRight, m_borderTop, m_borderBottom);
    m_paddingLeft = m_paddingRight = m_paddingTop = m_paddingBottom = 0;
    if (KDecorationUnstable *deco2 = dynamic_cast<KDecorationUnstable*>(m_decoration))
        deco2->padding(m_paddingLeft, m_paddingRight, m_paddingTop, m_paddingBottom);
    XMoveWindow(display(), m_decoration->widget()->winId(), -m_paddingLeft, -m_paddingTop);
    move(QPoint(m_borderLeft, m_borderTop));
    resizeDecoration(geom.size());
    m_paintRedirector = new PaintRedirector(m_decoration->widget());
    connect(m_paintRedirector, SIGNAL(paintPending()), SLOT(repaintDecorationPending()));
    resizeDecorationPixmaps();
    // TODO: inform effect system about changed geometry
    m_decoration->widget()->show();
}

void Client::destroyDecoration()
{
    if (m_decoration) {
        delete m_decoration;
        m_decoration = NULL;
        m_borderLeft = m_borderRight = m_borderTop = m_borderBottom = m_paddingLeft = m_paddingRight = m_paddingTop = m_paddingBottom = 0;
        // TODO: here we need to resize
        delete m_paintRedirector;
        m_paintRedirector = NULL;
        m_decorationPixmapLeft = m_decorationPixmapRight = m_decorationPixmapTop = m_decorationPixmapBottom = QPixmap();
        // TODO: inform effect system about changed geometry
    }
}

void Client::repaintDecorationPending()
{
    // The scene will update the decoration pixmaps in the next painting pass
    // if it has not been already repainted before
    const QRegion r = m_paintRedirector->scheduledRepaintRegion();
    if (!r.isEmpty())
        Workspace::self()->addRepaint(r.translated(x() - m_paddingLeft, y() - m_paddingTop));
}

void Client::resizeDecoration(const QSize& s)
{
    // TODO: move to new parent class
    if (!m_decoration)
        return;
    QSize newSize = s + QSize(m_paddingLeft + m_paddingRight, m_paddingTop + m_paddingBottom);
    QSize oldSize = m_decoration->widget()->size();
    m_decoration->resize(newSize);
    if (oldSize == newSize) {
        QResizeEvent e(newSize, oldSize);
        QApplication::sendEvent(m_decoration->widget(), &e);
    } else { // oldSize != newSize
        resizeDecorationPixmaps();
    }
}

void Client::resizeDecorationPixmaps()
{
    QRect lr, rr, tr, br;
    layoutDecorationRects(lr, tr, rr, br, DecorationRelative);
#define PIXMAP(pix, rect) \
    if (pix.size() != rect.size()) { \
        pix = QPixmap(rect.size()); \
        pix.fill(Qt::transparent); \
    }

    PIXMAP(m_decorationPixmapTop, tr)
    PIXMAP(m_decorationPixmapBottom, br)
    PIXMAP(m_decorationPixmapLeft, lr)
    PIXMAP(m_decorationPixmapRight, rr)

#undef PIXMAP
    triggerDecorationRepaint();
}

void Client::triggerDecorationRepaint()
{
    // TODO: move to new parent class
    if (m_decoration != NULL) {
        m_decoration->widget()->update();
    }
}

void Client::layoutDecorationRects(QRect &left, QRect &top, QRect &right, QRect &bottom, Client::CoordinateMode mode) const
{
    // TODO: move to new parent class
    QRect r = m_decoration->widget()->rect();
    if (mode == WindowRelative)
        r.translate(-m_paddingLeft, -m_paddingTop);

    // TODO: needs strut support
    top = QRect(r.x(), r.y(), r.width(), m_paddingTop + m_borderTop);
    bottom = QRect(r.x(), r.y() + r.height() - m_paddingBottom - m_borderBottom,
                   r.width(), m_paddingBottom + m_borderBottom );
    left = QRect(r.x(), r.y() + top.height(),
                 m_paddingLeft + m_borderLeft, r.height() - top.height() - bottom.height());
    right = QRect(r.x() + r.width() - m_paddingRight - m_borderRight, r.y() + top.height(),
                  m_paddingRight + m_borderRight, r.height() - top.height() - bottom.height());
}

QRegion Client::decorationPendingRegion() const
{
    // TODO: move to new parent class
    if (!m_paintRedirector)
        return QRegion();
    return m_paintRedirector->scheduledRepaintRegion().translated(x() - m_paddingLeft, y() - m_paddingTop);
}

QRect Client::decorationRect() const
{
    // TODO: move to new parent class
    if (m_decoration && m_decoration->widget()) {
        return m_decoration->widget()->rect().translated(-m_paddingLeft, -m_paddingTop);
    } else {
        return QRect(0, 0, width(), height());
    }
}

bool Client::decorationPixmapRequiresRepaint() const
{
    if (!m_paintRedirector)
        return false;
    const QRegion r = m_paintRedirector->pendingRegion();
    return !r.isEmpty();
}

void Client::ensureDecorationPixmapsPainted()
{
    if (!m_paintRedirector)
        return;

    const QRegion r = m_paintRedirector->pendingRegion();
    if (r.isEmpty())
        return;

    QPixmap p = m_paintRedirector->performPendingPaint();

    QRect lr, rr, tr, br;
    layoutDecorationRects(lr, tr, rr, br, DecorationRelative);

    repaintDecorationPixmap(m_decorationPixmapLeft, lr, p, r);
    repaintDecorationPixmap(m_decorationPixmapRight, rr, p, r);
    repaintDecorationPixmap(m_decorationPixmapTop, tr, p, r);
    repaintDecorationPixmap(m_decorationPixmapBottom, br, p, r);
}

void Client::repaintDecorationPixmap(QPixmap& pix, const QRect& r, const QPixmap& src, QRegion reg)
{
    // TODO: move to parent class
    if (!r.isValid())
        return;
    QRect b = reg.boundingRect();
    reg &= r;
    if (reg.isEmpty())
        return;
    QPainter pt(&pix);
    pt.translate(-r.topLeft());
    pt.setCompositionMode(QPainter::CompositionMode_Source);
    pt.setClipRegion(reg);
    pt.drawPixmap(b.topLeft(), src);
    pt.end();
}

bool Client::shouldUnredirect() const
{
    return false;
}

void Client::debug(QDebug& stream) const
{
    Q_UNUSED(stream)
    // TODO implement me
}

QStringList Client::activities() const
{
    return QStringList();
}

int Client::desktop() const
{
    return NET::OnAllDesktops;
}

QRect Client::transparentRect() const
{
    return QRect(clientPos(), clientSize());
}

QSize Client::clientSize() const
{
    return m_clientSize;
}

QPoint Client::clientPos() const
{
    return QPoint(m_borderLeft, m_borderTop);
}

double Client::opacity() const
{
    return 1.0;
}

bool Client::isWayland() const
{
    return true;
}

wl_buffer *Client::buffer()
{
    return m_surface->buffer();
}

void Client::setGeometry(const QRect &geometry)
{
    if (geom == geometry) {
        return;
    }
    addRepaint(geom);
    geom = geometry;
    m_clientSize = geometry.size() - QSize(m_borderLeft + m_borderRight, m_borderTop + m_borderBottom);
    resizeDecoration(geom.size());
    addRepaint(geom);
}
void Client::frameRendered(int timeStamp)
{
    wl_display_post_frame(workspace()->wayland()->display(), m_surface->surface(), timeStamp);
}

void Client::surfaceDamaged(const QRect &damage)
{
    addDamage(damage);
}

void Client::move(int x, int y, ForceGeometry_t force)
{
    move(QPoint(x, y), force);
}

void Client::move(const QPoint &p, ForceGeometry_t force)
{
    if (force == NormalGeometrySet && geom.topLeft() == p)
        return;
    geom.moveTopLeft(p);
    // TODO: there is more in KWin::Client::move
}

} // namespace Wayland
} // namespace KWin