/*
 *   Copyright 2011 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.draganddrop 1.0
import org.kde.qtextracomponents 0.1

PlasmaCore.FrameSvgItem {
    id: background
    width: list.width / Math.floor(list.width / 180)
    height: list.height
    imagePath: "widgets/tasks"
    prefix: Current? "focus" : "normal"

    ListView.onRemove: SequentialAnimation {
        PropertyAction {
            target: background
            property: "ListView.delayRemove"
            value: true
        }
        NumberAnimation {
            target: background
            property: "y"
            to: background.height
            duration: 250
            easing.type: Easing.InOutQuad
        }
        PropertyAction {
            target: background
            property: "ListView.delayRemove"
            value: false
        }
    }

    ListView.onAdd: NumberAnimation {
        target: background
        property: "y"
        from: -background.height
        to: 0
        duration: 250
        easing.type: Easing.InOutQuad
    }

    Behavior on x {
        NumberAnimation { duration: 250 }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            var activityId = model["DataEngineSource"]
            var service = activitySource.serviceForSource(activityId)
            var operation = service.operationDescription("setCurrent")
            service.startOperationCall(operation)
        }
    }

    QIconItem {
        id: iconWidget
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: background.margins.left
            topMargin: background.margins.top
            bottomMargin: background.margins.bottom
        }
        width: theme.hugeIconSize
        height: width
        icon: QIcon(Icon)
    }
    QPixmapItem {
        anchors.fill: iconWidget
        pixmap: Icon ? undefined : activityManager.pixmapForActivity(model["DataEngineSource"])
    }
    QIconItem {
        width: theme.mediumIconSize
        height: width
        anchors.centerIn: iconWidget
        icon: QIcon("media-playback-start")
        visible: model["State"] != "Running"
    }
    Column {
        anchors {
            left: iconWidget.right
            right: parent.right
            verticalCenter: parent.verticalCenter

            leftMargin: background.margins.left
            rightMargin: background.margins.right
        }
        PlasmaComponents.Label {
            id: titleText
            color: theme.textColor
            text: Name
            anchors.left: parent.left
            anchors.right: parent.right
            horizontalAlignment: Text.AlignHCenter
            height: paintedHeight
            clip: true
            wrapMode: Text.Wrap
        }
        Row {
            id: buttonsRow
            visible: model["State"] == "Running"
            anchors.horizontalCenter: parent.horizontalCenter

            PlasmaComponents.ToolButton {
                id: configureButton
                iconSource: "configure"
            }
            PlasmaComponents.ToolButton {
                iconSource: "media-playback-stop"
                onClicked: {
                    var activityId = model["DataEngineSource"]
                    var service = activitySource.serviceForSource(activityId)
                    var operation = service.operationDescription("stop")
                    service.startOperationCall(operation)
                }
            }
        }
        PlasmaComponents.ToolButton {
            visible: model["State"] != "Running"
            iconSource: "edit-delete"
            text: i18n("Delete")
            width: Math.min(implicitWidth, parent.width)
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
