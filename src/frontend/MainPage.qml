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
import org.maemo.miniature 1.0

Page { // FIXME how to make the height fixed so the virtual keyboard doesn't push it up?
    id: ficsLogin
    orientationLock: PageOrientation.LockPortrait

    // Generic app window style

    Connections {
        target: miniature
        onLoginFailed: {
            busyLogin.visible = false
            busyLogin.running = false
            loginErrorBkgAnimation.restart()
            loginErrorAnimation.restart()
            idForm.text = ""
            idForm.focus = true
            idPassword.text = ""
        }

        onLoginSucceeded: {
            busyLogin.visible = false
            busyLogin.running = false
            loadScreen("SeekGame.qml")
        }
    }

    Rectangle {
        id: windowBackground
        color: "lightgray"
        anchors.fill: parent
    }

    Image {
        source: "miniatureBIG.png"
        x: -110
        y: 100
        height: 900
        fillMode: Image.PreserveAspectCrop
    }

    // How to define a proper header with platform style?

    Rectangle {
        id: headerBackground
        color: "olivedrab"
        width: parent.width
        height: 70
        anchors.top: parent.top
        z: 25

        BusyIndicator {
            id: busyLogin
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            // FIXME ideally this should be color white but I can't find the way to go away from default grey
            visible: false
            running: false
        }
    }

    Text {
        id: poormansHeader
        //: only to be translated in non-Latin writing languages requiring it
        text: "Miniature"
        font.family: "Nokia Pure Headline"
        font.pointSize: 28
        color: "white"
        anchors.bottom: headerBackground.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        z: 30
    }

    Text {
        id: poormansHeaderVersion
        //: only to be translated in non-Latin writing languages requiring it
        text: "0.5"
        font.family: "Nokia Pure Headline"
        font.pointSize: 28
        color: "white"
        anchors.bottom: headerBackground.bottom
        anchors.bottomMargin: 10
        anchors.left: poormansHeader.right
        anchors.leftMargin: 15
        visible: false
        z: 30
    }

    Button {
        id: anonymousButton
        text: qsTr("Join as guest")
        anchors.top: headerBackground.bottom
        anchors.topMargin: 50
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            busyLogin.visible = true
            busyLogin.running = true
            miniature.login("guest", "")
        }
    }

    // Login error message
    Rectangle {
        id: loginErrorBkg
        color: "transparent"
        radius: 10
        anchors {
            top: idForm.top
            topMargin: -40
            bottom: loginButton.bottom
            bottomMargin: -10
            left: anonymousButton.left
            leftMargin: -10
            right: anonymousButton.right
            rightMargin: -10
        }
        SequentialAnimation on color {
            id: loginErrorBkgAnimation
            running: false
            ColorAnimation { from: "red"; to: "red"; duration: 1000 }
            ColorAnimation { from: "red"; to: "transparent"; duration: 1000; easing.type: Easing.OutQuart }
        }
    }

    Text {
        id: loginError
        text:  qsTr("Wrong")
        font.pointSize: 20
        color: "transparent"
        anchors {
            top: loginErrorBkg.top
            topMargin: 8
            horizontalCenter: parent.horizontalCenter
        }
        SequentialAnimation on color {
            id: loginErrorAnimation
            running: false
            ColorAnimation { from: "white"; to: "white"; duration: 1000 }
            ColorAnimation { from: "white"; to: "transparent"; duration: 1000; easing.type: Easing.OutQuart }
        }
    }

    // Login form
    TextField {
        id: idForm
        text: miniature.storedUsername
        placeholderText: qsTr("username")
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        anchors.top:  anonymousButton.bottom
        anchors.topMargin: 70
        anchors.left: anonymousButton.left
        anchors.right: anonymousButton.right
        Keys.onReturnPressed: {
            idPassword.focus = true
        }
    }

    TextField {
        id: idPassword
        text: miniature.storedPassword
        placeholderText: qsTr("password")
        echoMode: TextInput.Password
        anchors.top:  idForm.bottom
        anchors.topMargin: 5
        anchors.left: anonymousButton.left
        anchors.right: anonymousButton.right
        Keys.onReturnPressed: {
            busyLogin.visible = true
            busyLogin.running = true
            miniature.login(idForm.text, idPassword.text)
        }
    }

    Button {
        id: loginButton
        text: qsTr("Log in")
        anchors.top:  idPassword.bottom
        anchors.topMargin: 10
        anchors.left: anonymousButton.left
        anchors.right: anonymousButton.right
        onClicked: {
            busyLogin.visible = true
            busyLogin.running = true
            miniature.login(idForm.text, idPassword.text)
        }
    }

    Button {
        id: registerButton
        text: qsTr("Register online")
        anchors.top:  loginButton.bottom
        anchors.topMargin: 70
        anchors.left: anonymousButton.left
        anchors.right: anonymousButton.right
        onClicked: { Qt.openUrlExternally("http://www.freechess.org/cgi-bin/Register/FICS_register.cgi") }
    }

//    Button { // Toggle between testing mode and real mode - to be commented in releases
//        id: testingButton
//        text:  checked ? qsTr("TESTING MODE ON")
//                       : qsTr("TESTING MODE OFF")
//        checkable: true
//        checked: miniature.mode == Miniature.TestFicsMode
//        opacity: 0.8
//        anchors.top: aboutButton.bottom
//        anchors.topMargin: 40
//        anchors.horizontalCenter: parent.horizontalCenter
//        onClicked: {
//            miniature.setMode(testingButton.checked ? Miniature.TestFicsMode
//                                                    : Miniature.FicsMode)
//        }
//    }

    Rectangle {
        id: aboutMiniature
        width: parent.width
        anchors.top: headerBackground.bottom
        anchors.bottom: parent.bottom
        color: "black"
        visible: false

        Image {
            id: board
            source: "chessboard.svg"
            width: parent.width * 0.8
            height: parent.width * 0.8
            anchors.top: parent.top
            anchors.topMargin: -(board.width / 2)
            anchors.horizontalCenter: parent.horizontalCenter

            Image {
                source: "miniatureMedium.png"
                width: 150
                height: 150
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
            }
        }

        Text {
            id: chatLog
            width: parent.width
            anchors.top: board.bottom
            anchors.topMargin: 30
            color: "white"
            text: qsTr("Miniature is free software developed openly by volunteers and distributed with the GPLv2 license. ") +
                  qsTr("We welcome your feedback and contributions!\n\n") +
                  qsTr("Special thanks to Colin M.L. Burnett for his free chess set used by Wikipedia.\n\n") +
                  qsTr("(C) 2011 Michael Hasselmann & Quim Gil")
            font.pointSize: 16
            horizontalAlignment: Text.AlignHCenter
            clip: true
            // lineHeight: 1.2 // commented to keep compatibility with Maemo
            wrapMode: Text.Wrap
        }

        Text {
            id: miniatureMail
            text: "info@miniature-chess.org"
            font.pointSize: 20
            font.underline: true
            color: "white"
            height: 40
            anchors {
                bottom: miniatureURL.top
                bottomMargin: 15
                horizontalCenter: parent.horizontalCenter
            }
            MouseArea {
                anchors.fill: parent
                onClicked: Qt.openUrlExternally("mailto:info@miniature-chess.org") }
        }

        Text {
            id: miniatureURL
            text: "miniature-chess.org"
            font.pointSize: 20
            font.underline: true
            color: "white"
            height: 40
            anchors {
                bottom: parent.bottom
                bottomMargin: 10
                horizontalCenter: parent.horizontalCenter
            }
            MouseArea {
                anchors.fill: parent
                onClicked: Qt.openUrlExternally("http://miniature-chess.org") }
        }
    }

    Button {
        id: aboutButton
        checkable: true
        //: Stands for "information". Translations must have just one character. In case of doubt just leave the original "i".
        text: qsTr("i")
        width: 50
        anchors {
            top: registerButton.bottom
            topMargin: 70
            horizontalCenter: parent.horizontalCenter
        }
        onClicked: {
            if (aboutButton.checked == true) {
                aboutMiniature.visible = true
                poormansHeaderVersion.visible = true }
            else { aboutMiniature.visible = false
                poormansHeaderVersion.visible = false }
        }
    }
}


