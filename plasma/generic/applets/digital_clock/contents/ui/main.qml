/*
    Copyright 2013 Heena Mahour <heena393@gmail.com>
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This plasmoid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this plasmoid. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as Components
import org.kde.locale 2.0

Item {

    id: mainWindow
    property int minimumHeight:40
    property int minimumWidth: 40
    property alias timeData: dataSource.data
    property string tz: "Europe/Amsterdam"
    property string textColor
    property string textFont
    property int theight: (width / height > 4) ? height/2 : width/5
    
    Component.onCompleted: {
        plasmoid.setBackgroundHints( 0 )
        plasmoid.addEventListener( 'ConfigChanged', configChanged );
        textColor = plasmoid.readConfig( "textColor" )
        textFont = plasmoid.readConfig( "textFont" )
    }
    
    function configChanged()
    {   
        textColor = plasmoid.readConfig( "textColor" )
        textFont = plasmoid.readConfig( "textFont" )
    }
    
    function setTimeFormat()
    {
        return;
        timeFormat = plasmoid.readConfig( "timeFormat" )
        if( timeFormat == 12 ){
            timeString = (Qt.formatTime( timeData[tz]["Time"],"h:mmap" )).toString().slice(0,-2)
        }
        else
            timeString = (Qt.formatTime( timeData[tz]["Time"],"hh:mm" ))
    }
  
     Components.Label  {
        id: time
        text : (Qt.formatTime( timeData[tz]["Time"],"hh:mm" ))
        font.pointSize: theight
        anchors {
            centerIn: parent
        }
    }

    Components.Label  {
        id: date
        text : timeData[tz]["Timezone City"] + ", " + Qt.formatDate( timeData[tz]["Date"],"dddd, MMM d" )
        anchors {
            top: time.bottom;
            //topMargin: font.pixelSize
            horizontalCenter: time.horizontalCenter
        }
    }
    
    PlasmaCore.DataSource {
        id: dataSource
        engine: "time"
        connectedSources: [tz]
        interval: 2000

//         onDataChanged: {
//             print("data: " + data);
// //             print("data: " + tdata.valueOf());
//             //timeData = data;
//             print("data: " + timeData[tz]);
//
//             for (var d in timeData[tz]) {
//                 print("  d data[" + d +"] = " + timeData[tz][d] + "    ")
//                 for (var e in d) {
//                     print("     e data[" + e +"] = " + d[e])
//                 }
//             }
//             print( " data" + timeData[tz]["Time"]);
//         }
    }
    
    Locale {
        id: locale
    }    
}