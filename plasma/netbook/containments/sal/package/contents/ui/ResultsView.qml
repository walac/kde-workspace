/******************************************************************************
 *  Copyright (C) 2012 by Shaun Reich <shaun.reich@blue-systems.com>          *
 *                                                                            *
 *  This library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as published  *
 *  by the Free Software Foundation; either version 2 of the License or (at   *
 *  your option) any later version.                                           *
 *                                                                            *
 *  This library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 *  Library General Public License for more details.                          *
 *                                                                            *
 *  You should have received a copy of the GNU Lesser General Public License  *
 *  along with this library; see the file COPYING.LIB.                        *
 *  If not, see <http://www.gnu.org/licenses/>.                               *
 *****************************************************************************/

import Qt 4.7
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.qtextracomponents 0.1 as QtExtra

Item {
    id: main

    Flickable {
        id: resultsFlickable

        anchors.fill: parent

        interactive: true
        clip: true
        contentHeight: 1000
        contentWidth: 1000

        Flow {
            anchors.fill: parent

            move: Transition {
                PropertyAnimation {
                    properties: "x,y"
                    easing.type: Easing.InOutQuad
                }
            }

            Repeater {
                model: 5000

                QtExtra.QIconItem {
                    id: normalIcon1
                    width: 100
                    height: 100
                    icon: "system-shutdown"
                }
            }
        }
    }
}