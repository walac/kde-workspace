/***************************************************************************
 *   Copyright (C) 2012 by Aleix Pol Gonzalez <aleixpol@kde.org>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

import QtQuick 1.1
import org.kde.runnermodel 0.1 as RunnerModels
import org.kde.plasma.components 0.1
import org.kde.qtextracomponents 0.1

Item {
    width: 400
    height: (inputRow.height+
                ((!contentItem.content || runnerModel.count==0) ? 0 : (contentItem.content.childrenRect.height+20)))
    property string view
    
    id: main
    
    Connections {
        target: app
        onChangeTerm: {
            input.text=term
            input.selectAll()
        }
    }
    
    RunnerModels.RunnerModel { id: runnerModel }
    
    Item {
        id: inputRow
        height: 40
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        
        ToolButton {
            id: configButton
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
            }
            iconSource: "configure"
            onClicked: app.configure()
        }
        
        ToolButton {
            id: viewsButton
            anchors {
                verticalCenter: parent.verticalCenter
                left: configButton.right
            }
            iconSource: "plasma"
            
            onClicked: {
                var views = [ "ResultsPath.qml", "ResultsList.qml", "ResultsGrid.qml"]
                var i = (views.indexOf(view)+1) % views.length
                console.log("laaaaa " + i + " " + views.length)
                view = views[i]
            }
        }
    
        TextField {
            id: input
            anchors {
                left: viewsButton.right
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            onTextChanged: { runnerModel.query = text; contentItem.content.currentIndex = 0 }
            
            onAccepted: {
                runnerModel.run(contentItem.content.currentIndex)
                app.hide()
            }
        }
    }
    
    onViewChanged: {
        var comp=Qt.createComponent(view)
        
        if(comp.status == Component.Ready) {
            if(contentItem.content)
                contentItem.content.destroy()
            try {
                var obj = comp.createObject(main, { model: runnerModel } )
                contentItem.content=obj
            } catch(e) {
                console.log("error: "+e)
            }
        } else {
            console.log("Error loading component:", comp.errorString());
        }
    }
    
    Keys.onPressed: {
        if(event.key == Qt.Key_Tab || event.key == Qt.Key_Backtab) {
            var inc = (event.key == Qt.Key_Backtab) ? -1 : 1;
            contentItem.content.currentIndex = (contentItem.content.currentIndex+inc) % runnerModel.count
            event.accepted=true
        }
    }
    
    Keys.onEscapePressed: {
        app.hide()
        input.text=''
    }
    
    Item {
        id: contentItem
        anchors {
            top: inputRow.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        clip: true
        
        property Item content
        
        onContentChanged: content.anchors.fill=contentItem
        
        Component.onCompleted: {
            view = "ResultsList.qml"
            input.selectAll()
        }
    }
}
