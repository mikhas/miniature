/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2011 Quim Gil <quimgil@gmail.com>
 *
 *
 * Miniature is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Miniature is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Miniature. If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
    id: appWindow
    initialPage: MainPage { }

    // Funtion to push pages cleanly to the PageStack
    function loadScreen(f_ScreenName) {
        var component = Qt.createComponent(f_ScreenName);
        if (component.status == Component.Ready)
            pageStack.push(component);
        else console.log("Error loading component:", component.errorString())
    }

//    // Function available to go back to a specific page of the stack other that the previous one or the first
//    // (commented because it is not used anywhere yet)
//    // To go back one page just use pageStack.pop()
//    // To go to the initial page just use pageStack.pop(null)
//    function backScreen(b_ScreenName) {
//        var component = Qt.createComponent(b_ScreenName);
//        if (component.status == Component.Ready)
//            pageStack.pop(component);
//        else console.log("Error loading component:", component.errorString())
//    }
}
