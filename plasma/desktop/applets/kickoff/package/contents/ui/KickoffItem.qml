/*
    Copyright (C) 2011  Martin Gräßlin <mgraesslin@kde.org>

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
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.qtextracomponents 0.1
import org.kde.draganddrop 1.0

PlasmaComponents.ListItem {
    id: listItem
    enabled: true
    checked: ListView.isCurrentItem
    height: Math.max(elementIcon.height, titleElement.paintedHeight + subTitleElement.paintedHeight)  + 10

    property bool modelChildren: hasModelChildren

    function activate() {
        if (hasModelChildren) {
            listItem.ListView.view.addBreadcrumb(listItem.ListView.view.model.modelIndex(index), display);
            listItem.ListView.view.model.rootIndex = listItem.ListView.view.model.modelIndex(index);
        } else {
            launcher.openUrl(model["url"]);
            plasmoid.hidePopup();
        }
    }

    /*
     * context menu items
     */
    PlasmaComponents.MenuItem {
        id: addToFavorites
        text: "Add to favorites"
        onClicked: {
            console.log(model["url"]);
            listItem.ListView.view.parent.favoritesModel.add(model["url"]);
        }
    }
    PlasmaComponents.MenuItem {
        id: removeFromFavorites
        text: "Remove from favorites"
        onClicked: {
            listItem.ListView.view.model.remove(model["url"]);
        }
    }

    Component.onCompleted: {
        if (root.state == "APPLICATIONS") {
            contextMenu.addMenuItem(addToFavorites);
        } else if (root.state == "NORMAL") {
            if (listItem.ListView.view.model == listItem.ListView.view.favoritesModel)
                contextMenu.addMenuItem(removeFromFavorites)
            else if (listItem.ListView.view.model == listItem.ListView.view.recentlyUsedModel)
                contextMenu.addMenuItem(addToFavorites);
        } else if (root.state == "SEARCH") {
            contextMenu.addMenuItem(addToFavorites);
        }
    }

    QIconItem {
        id: elementIcon
        icon: decoration
        width: 32
        height: 32
        anchors {
            left: parent.left
        }
    }
    PlasmaComponents.Label {
        id: titleElement
        text: {
            if (root.state == "APPLICATIONS") {
                if (hasModelChildren) {
                    return display;
                } else {
                    // TODO: games should always show the by name
                    return root.showAppsByName ? subtitle : display;
                }
            } else {
                return display;
            }
        }
        anchors {
            top: parent.top
            left: elementIcon.right
            right: parent.right
            leftMargin: 5
        }
    }
    PlasmaComponents.Label {
        id: subTitleElement
        text: {
            if (root.state == "APPLICATIONS") {
                if (hasModelChildren) {
                    return "";
                } else {
                    return root.showAppsByName ? display : subtitle;
                }
            } else {
                return subtitle;
            }
        }
        visible: listItem.ListView.isCurrentItem
        opacity: 0.6
        font.pointSize: theme.smallestFont.pointSize
        elide: Text.ElideMiddle
        anchors {
            left: elementIcon.right
            right: parent.right
            bottom: parent.bottom
            top: titleElement.bottom
            leftMargin: 5
        }
    }

    PlasmaComponents.ContextMenu {
        id: contextMenu
        visualParent: listItem
    }

    DragArea {
        anchors.fill: parent
        supportedActions: Qt.MoveAction | Qt.LinkAction
            mimeData {
                url: model["url"]
                source: parent
                text: index
            }
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onEntered: {
                listItem.ListView.view.currentIndex = index;
            }
            onClicked: {
                if (mouse.button == Qt.LeftButton)
                    activate();
                else if (mouse.button == Qt.RightButton) {
                    contextMenu.open();
                }
            }
        }
    }
    DropArea {
        anchors.fill: parent
        onDrop: {
            listItem.ListView.view.model.dropMimeData(event.mimeData.text, event.mimeData.urls, index, 0);
        }
    }
}
