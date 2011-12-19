/*
    Copyright (C) 2011  Lamarque Souza <lamarque@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.qtextracomponents 0.1

PlasmaCore.FrameSvgItem {
    id: button
    property string iconSource
    property alias text: labelElement.text
    property bool smallButton: false
    property bool menu: false
    property ContextMenu contextMenu
    property Item tabStopNext
    property Item tabStopBack

    signal clicked()
    signal pressed()
    signal pressAndHold()

    PlasmaCore.Theme {
        id: theme
    }

    PlasmaCore.SvgItem {
        id: background
        anchors.fill: parent

        svg: PlasmaCore.Svg {
            imagePath: "dialogs/shutdowndialog"
        }
        elementId: "button-normal"
    }

    Text {
        id: labelElement
        font.pixelSize: Math.max(12, theme.defaultFont.pointSize)
        color: theme.textColor
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 5
        }

        onPaintedWidthChanged: {
            button.width = Math.max(button.width, 5 + labelElement.width + 10 + iconElement.width + 5)
        }
    }

    QIconItem {
        id: menuIconElement

        // if textColor is closer to white than to black use "draw-triangle4", which is also close to white.
        // Otherwise use "arrow-down", which is green. I have not found a black triangle icon.
        icon: theme.textColor > "#7FFFFF" ? QIcon("draw-triangle4") : QIcon("arrow-down")

        width: 6
        height: width
        visible: button.menu

        anchors {
            right: iconElement.left
            rightMargin: 2
            bottom: parent.bottom
            bottomMargin: 2
        }
    }

    QIconItem {
        id: iconElement
        icon: QIcon(iconSource)
        width: height
        height: parent.height - 6

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 3
        }
    }

    Component.onCompleted: {
        if (button.focus) {
            background.elementId = button.smallButton ? "button-small-hover" : "button-hover"
        } else {
            background.elementId = button.smallButton ? "button-small-normal" : "button-normal"
        }
    }

    onActiveFocusChanged: {
        if (activeFocus) {
            background.elementId = button.smallButton ? "button-small-hover" : "button-hover"
            //console.log("KSMButton.qml activeFocus "+activeFocus+" "+button.text)
        } else {
            background.elementId = button.smallButton ? "button-small-normal" : "button-normal"
        }
    }

    onTabStopNextChanged: {
        KeyNavigation.tab = tabStopNext
        KeyNavigation.down = tabStopNext
        KeyNavigation.right = tabStopNext
    }

    onTabStopBackChanged: {
        KeyNavigation.backtab = tabStopBack
        KeyNavigation.up = tabStopBack
        KeyNavigation.left = tabStopBack
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Return || event.key == Qt.Key_Enter ||
            event.key == Qt.Key_Space) {
            mouseArea.clicked(null)
        }
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            button.focus = true
            button.clicked()
        }
        onPressed: button.pressed()
        onPressAndHold: button.pressAndHold()
        onEntered: {
            background.elementId = button.smallButton ? "button-small-hover" : "button-hover"
        }
        onExited: {
            if (!button.focus) {
                background.elementId = button.smallButton ? "button-small-normal" : "button-normal"
            }
        }
    }
}
