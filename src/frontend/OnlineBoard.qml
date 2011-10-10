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

Page {

    // Taken from http://codeaid.net/javascript/convert-seconds-to-hours-minutes-and-seconds-%28javascript%29
    function formatTime(secs) {
            var hr = Math.floor(secs / 3600);
            var min = Math.floor((secs - (hr * 3600))/60);
            var sec = secs - (hr * 3600) - (min * 60);

            if (hr < 10) {hr = "0" + hr; }
            if (min < 10) {min = "0" + min;}
            if (sec < 10) {sec = "0" + sec;}
            if (hr) {hr = "00";}
            return hr + ':' + min + ':' + sec;
    }

    id: ficsBoard
    orientationLock: PageOrientation.LockPortrait

    function gameRated() { // trick to convert int activeGame.rating to the right value
        switch (activeGame.rating) {
        default:
        case Miniature.RatingDisabled:
            return "unrated"

        case Miniature.RatingEnabled:
            return "rated";
        }
    }

    Rectangle {
        id: windowBackground
        color: "lightgray"
        anchors.fill: parent
        z: -10
    }

    // How to define a proper header with platform style?

    Rectangle {
        id: headerBackground
        color: "olivedrab"
        width: parent.width
        height: 70
        anchors.top: parent.top
        z: 30

        Text {
            id: typeChess
            text: "Standard" // FIXME onPress Suicide, Bughouse, etc
            font.family: "Nokia Pure Headline"
            font.pointSize: 28
            color: "white"
            anchors.bottom: headerBackground.bottom
            anchors.bottomMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10
        }

        Text {
            id: timeGame
            text: (activeGame.time / 60).toFixed(0) // Show in minutes.
            font.family: "Nokia Pure Headline"
            font.pointSize: 28
            color: "white"
            anchors.bottom: headerBackground.bottom
            anchors.bottomMargin: 10
            anchors.right: incrementGame.left
            anchors.rightMargin: 15
        }

        Text {
            id: incrementGame
            text: activeGame.timeIncrement
            font.family: "Nokia Pure Headline"
            font.pointSize: 28
            color: "white"
            anchors.bottom: headerBackground.bottom
            anchors.bottomMargin: 10
            anchors.right: ratedGame.left
            anchors.rightMargin: 20
        }


        Text {
            id: ratedGame
            text: gameRated()
            font.family: "Nokia Pure Headline"
            font.pointSize: 28
            color: "white"
            anchors.bottom: headerBackground.bottom
            anchors.bottomMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
        }
    }

    Component {
        id: emptyHeader
        Rectangle {
            height: 20
            width: parent.width
            color: "transparent"
        }
    }

    // Chat area
    Rectangle {
        id: chatBkg
        property bool expanded: false
        property string opponentDeclines: qsTr("FICS: %1 has declined your request.").arg(remoteSide.id)
        color: "black"
        opacity: 0.8
        anchors.top: headerBackground.bottom
        anchors.bottom: opponentZone.top
        anchors.left: parent.left
        anchors.right: parent.right
        z: 20

        ListView {
            id: chatLog
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            clip: true
            header: emptyHeader // empty header to avoid overlap of 1st item with shadow.
            spacing: 0

            model: messageLog

            Connections {
                target: messageLog
                onRowsInserted: chatLog.positionViewAtEnd()
            }

            delegate: Text {
                color: "white"
                font.pointSize: 16
                wrapMode: Text.Wrap
                verticalAlignment: Text.AlignBottom
                text: model.playerName + ": " + model.message
                width: parent.width
                clip: false
            }
        }

        MouseArea {
            id: chatArea
            anchors.fill: parent
            onClicked: {
                if ( chatBkg.state === "" ) chatBkg.state = "expanded"
                else chatBkg.state = ""
            }
        }

        states: [

            State {
                name: "expanded"
                PropertyChanges {
                    target: chatField
                    text: ""
                    visible: true
                    focus: true
                }

                StateChangeScript {
                    // Programmatically setting the focus does not request VKB:
                    script: {
                        chatField.forceActiveFocus()
                        chatField.platformOpenSoftwareInputPanel()
                    }
                }

                PropertyChanges {
                    target: chatInputArea
                    visible: true
                    opacity: 0.8
                }
                PropertyChanges {
                    target: chatButton
                    visible: true
                }
                AnchorChanges {
                    target: chatBkg
                    anchors.bottom: chatInputArea.top
                }
            },

            State {
                name: ""
                AnchorChanges {
                    target: chatBkg
                    anchors.bottom: opponentZone.top
                }
            }

        ]
    }

    Rectangle { // Little effect of shadow in chat
        width: ficsBoard.width
        height: 15
        z: +22
        anchors.top: headerBackground.bottom
        gradient: Gradient {
            GradientStop { position: 0.0; color: "black" }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }

    Rectangle { // Chat input area, only visible when chat is expanded
        id: chatInputArea
        height: 80
        anchors.bottom: userZone.top
        anchors.left: parent.left
        anchors.right: parent.right
        color: "black"
        visible: false
        z: 20

        TextField {
            id: chatField
            text: ""
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: chatButton.left
            anchors.rightMargin: 10
            visible: false
            z: 20

            Keys.onReturnPressed:  {
                miniature.sendMessage(chatField.text)
                chatBkg.state = ""
            }
        }

        Button {
            id: chatButton
            //: Translations shuldn't be longer than "Chat" otherwise they make the button too big.
            text: qsTr("Chat")
            width: 100
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            visible: false
            z: 20

            onClicked: {
                miniature.sendMessage(chatField.text)
                chatBkg.state = ""
                // chatField.focus = false
            }
        }
    }


    // Opponent info
    Rectangle {
        id: opponentZone
        width: parent.width
        height: 40 // FIXME how to make it relative to text height?
        color: remoteSide.color
        anchors.bottom: imageBoard.top

        Text {
            id: opponentInfo
            text: remoteSide.rating + " " + remoteSide.id // FIXME must be changed for rating variable.
            color: localSide.color
            font.pointSize: 16
            font.weight: Font.DemiBold
            anchors.verticalCenter: opponentZone.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
        }

        Text {
            id: opponentTime
            text: formatTime(remoteSide.remainingTimeClock)
            font.pointSize: 24
            font.weight: Font.Bold
            font.family: "Courier"
            color: localSide.color
            anchors.verticalCenter: opponentZone.verticalCenter
            anchors.right: opponentZone.right
            anchors.rightMargin: 30
        }

        Rectangle {
            id: remoteMoves
            color: "cornflowerblue"
            width: parent.width
            height: 5
            anchors.bottom: parent.bottom
            z: 50
            visible: remoteSide.active
        }
    }

    // The board

    Image {
        id: imageBoard
        source: "chessboard.svg"
        anchors.bottom: userZone.top
        width: parent.width
        //        fillMode: Image.PreserveAspectFit // commenting out for MeeGo CE in N900 compatibility

        // 2 rectangles purely decorative to make the board edges look nicer
        Rectangle {
            id: remoteCorners
            color: remoteSide.active ? "cornflowerblue" : remoteSide.color
            width: parent.width
            height:  50
            anchors.top: parent.top
            z: -1
        }
        Rectangle {
            id: localCorners
            color: localSide.active ? "cornflowerblue" : localSide.color
            width: parent.width
            height:  50
            anchors.bottom: parent.bottom
            z: -1
        }
    }

    GridView {
        id: chessboardGrid
        anchors.fill: imageBoard // Just for you to know: this doesn't work in landscape.
        cellWidth: 60 // FIXME better not hardcoded
        cellHeight: 60 // FIXME better not hardcoded
        interactive: false
        model: chessBoard
        delegate: chessboardGridDelegate

        // Drawing the pieces on the board
        Component {
            id: chessboardGridDelegate
            Rectangle {
                id: squareDelegate
                width: 60 // FIXME better not hardcoded
                height: 60 // FIXME better not hardcoded
                color: squareColor
                SequentialAnimation on color {
                    id: wrongTapAnimation
                    running: false
                    ColorAnimation { from: "red"; to: "red"; duration: 50 }
                    ColorAnimation { from: "red"; to: "transparent"; duration: 200; easing.type: Easing.OutQuart }
                }

                Image {
                    source: pieceImage
                    width: 60 // FIXME better not hardcoded
                    height: 60 // FIXME better not hardcoded
                }

                MouseArea {
                    id: squareMouseArea
                    anchors.fill: parent
                    onClicked: {
                        if (!miniature.selectSquare(index)) {
                            wrongTapAnimation.restart()
                        }
                    }
                }
            }
        }
    }


    // User info
    Rectangle {
        id: userZone
        width: parent.width
        height: 40 // FIXME how to make it relative to text height?
        color: localSide.color
        anchors.bottom:  boardToolbar.top

        Text {
            id: userInfo
            text: localSide.rating + " " + localSide.id // FIXME rating must be changed for variables.
            color: remoteSide.color
            font.pointSize: 16
            font.weight: Font.DemiBold
            anchors.verticalCenter: userZone.verticalCenter
            anchors.verticalCenterOffset: 3
            anchors.left: parent.left
            anchors.leftMargin: 10
        }

        Text {
            id: userTime
            text: formatTime(localSide.remainingTimeClock)
            font.pointSize: 24
            font.weight: Font.Bold
            font.family: "Courier"
            color: remoteSide.color
            anchors.verticalCenter: userZone.verticalCenter
            anchors.verticalCenterOffset: 3
            anchors.right: userZone.right
            anchors.rightMargin: 30
        }

        Rectangle {
            id: localMoves
            color: "cornflowerblue"
            width: parent.width
            height: 5
            anchors.top: parent.top
            z: 50
            visible: localSide.active
        }
    }


    // Toolbar at the bottom of the page
    ToolBar {
        id: boardToolbar
        property bool gameEnded: false
        anchors.bottom: parent.bottom
        tools: ToolBarLayout {

            ToolIcon { // Stops the game, can lead to draw,
                id: backIcon
                iconId: "toolbar-back"
                onClicked: {
                    if (boardToolbar.gameEnded == true) pageStack.pop()
                    else exitMenu.open()
                }
            }

            ToolIcon {
                id: confirmButton
                iconId: "toolbar-add"
                visible: miniature.validMove
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    miniature.confirmMove()
                }
            }

            //            ToolIcon { // FIXME Settings page pending
            //                iconId: "toolbar-view-menu"
            //            }
        }
    }

    Menu {
        id: exitMenu
        property string queryTitle: ""
        property string queryMessage: ""
        MenuLayout {
            MenuItem {
                id: menuResign
                text: qsTr("Resign")
                visible: true
                onClicked: {
                    exitMenu.queryTitle = qsTr("Resign?")
                            endQueryDialog.open()
                }
            }
            MenuItem {
                id: menuDraw
                text: qsTr("Draw")
                visible: true
                onClicked: {
                    exitMenu.queryTitle = qsTr("Propose a draw?")
                            exitMenu.queryMessage = qsTr("%1 must accept it, otherwise the game continues").arg(remoteSide.id)
                            endQueryDialog.open()
                }
            }

//            MenuItem {text: qsTr("Adjourn"); onClicked: { // Out of scope in 0.5 - commenting
//                    exitMenu.queryTitle = qsTr("Request to adjourn?")
//                            exitMenu.queryMessage = qsTr("%1 must accept it, otherwise the game continues").arg(remoteSide.id)
//                            endQueryDialog.open() }
//            }
//            MenuItem {text: qsTr("Abort"); onClicked: { // Out of scope in 0.5 - commenting
//                    exitMenu.queryTitle = qsTr("Abort?")
//                            endQueryDialog.open() }
//            }
        }
    }

    // Signals and their dialogs

    Connections {
        id: opponentProposals
        target: miniature
        property string resolution
        property string description
        onRequestReceived: {
            if (proposal == Miniature.ProposedDraw) {
                opponentProposals.resolution = qsTr("Is this a draw?")
                //: %1 is the opponent's name
                opponentProposals.description = qsTr("%1 is proposing to end this game with a draw.").arg(remoteSide.id)
            }
            if (proposal == Miniature.ProposedAdjourn) {
                opponentProposals.resolution = qsTr("Continue another day?")
                //: %1 is the opponent's name
                opponentProposals.description = qsTr("%1 is requesting to adjourn this game.").arg(remoteSide.id)
            }
            if (proposal == Miniature.ProposedAbort) {
                opponentProposals.resolution = qsTr("Scrap this game?")
                //: %1 is the opponent's name
                opponentProposals.description = qsTr("%1 is proposing to abort this game.").arg(remoteSide.id)
            }

            requestsDialog.open()
        }
    }

    Connections {
        id: gameResolutions
        target: miniature
        property string winnerIs: "You lose..."
        property string description
        property string newRatings
        onGameEnded: {
            boardToolbar.gameEnded = true

            // Defining the result string to be used as title
            if (result == Miniature.ResultWhiteWins) {
                if (localSide.color == "#ffffff") gameResolutions.winnerIs = qsTr("You win!")
            }
            if (result == Miniature.ResultBlackWins) {
                if (localSide.color == "#000000") gameResolutions.winnerIs = qsTr("You win!")
            }
            if (result == Miniature.ResultDraw) { gameResolutions.winnerIs = qsTr("Draw") }
            if (result == Miniature.ResultAdjourned) { gameResolutions.winnerIs = qsTr("Game adjourned") }
            // if (result == Miniature.ResultAborted) { gameResolutions.winnerIs = qsTr("Game aborted") }
            if (result == Miniature.ResultUnknown) { gameResolutions.winnerIs = qsTr("What happened?") }

            // Defining the reason string to be used as description
            gameResolutions.description = qsTr("Game ended, reason unknown")

            if (reason == Miniature.ReasonCheckmated) {
                gameResolutions.description = qsTr("Checkmate")
            }
            if (reason == Miniature.ReasonDrawAccepted) {
                //: %1 is the opponent's name
                gameResolutions.description = qsTr("%1 accepts draw").arg(remoteSide.id)
            }
            if (reason == Miniature.ReasonTimeForfeit) {
                gameResolutions.description = qsTr("Out of time!")
            }
            else {
                //: %1 is the opponent's name
                gameResolutions.description = qsTr("%1 vanished").arg(remoteSide.id)
            }

            // Defining the ratings strings to be used in the description

            if (gameRated() == "unrated") { gameResolutions.newRatings = qsTr("No ratings adjustment done") }
            else gameResolutions.newRatings = localSide.id + ": " + localSide.rating + " > " + "FIXME\n" +
            remoteSide.id + ": " + remoteSide.rating + " > " + "FIXME" // We need a values for new ratings

            gameoverDialog.open()

            // Used for debugging game endings
            console.log(Miniature.ResultWhiteWins + " " + Miniature.ResultBlackWins + " " + result + " " + reason + " " + localSide.color)
        }
    }

    Item { // visualParent for end game dialogs
        id: dialogWindow
        opacity: 0.9
        width: parent.width
        anchors.top: opponentZone.top
        anchors.bottom: parent.bottom
    }

    QueryDialog { // Confirmation dialog for end game options selected by the local player
        id: endQueryDialog
        titleText: exitMenu.queryTitle
        acceptButtonText: qsTr("Yes")
        visualParent: dialogWindow
        onAccepted: {
            if (exitMenu.queryTitle == qsTr("Resign?")) miniature.resign()
            if (exitMenu.queryTitle == qsTr("Draw?")) miniature.draw() // FIXME function missing
            if (exitMenu.queryTitle == qsTr("Adjourn?")) miniature.adjourn() // FIXME function missing
            if (exitMenu.queryTitle == qsTr("Abort?")) miniature.abort() // FIXME function missing
            pageStack.pop() // This needs to be removed as part of BMO#12433
        }
        rejectButtonText: qsTr("No")
    }

    QueryDialog { // Confirmation dialog for end game options proposed by the remote player
        id: requestsDialog
        visualParent: dialogWindow
        titleText: opponentProposals.resolution
        message: opponentProposals.description
        acceptButtonText: "Accept"
        onAccepted: {
            if (opponentProposals.resolution == qsTr("Is this a draw?")) miniature.acceptDraw()
            if (opponentProposals.resolution == qsTr("Continue another day?")) miniature.acceptAdjourn()
            if (opponentProposals.resolution == qsTr("Scrap this game?")) miniature.acceptAbort()
        }
        rejectButtonText: "Ignore"
    }

    QueryDialog { // Connection lost
        id: connectionlostDialog
        visualParent: dialogWindow
        titleText: qsTr("Connection lost")
        message:  qsTr("Miniature can't reach the chess server.")
        acceptButtonText: qsTr("Log in again")
        onAccepted: {
            // FIXME with the stored values, can we attempt to re-login and resume a game automatically?
        }
        rejectButtonText: qsTr("Close app")
        onRejected: {
            // FIXME how to close the app?
        }
    }

    QueryDialog { // End of game // FIXME this dialog needs to be nicer and wiser! Not spending extra time now.
        id: gameoverDialog
        visualParent: dialogWindow
        titleText: gameResolutions.winnerIs
        message:  gameResolutions.description + "\n\n" + gameResolutions.newRatings

//        acceptButtonText: qsTr("Rematch") // Out of scope in 0.5
//        onAccepted: { // FIXME match functionality needs to be implemented
//        }
        acceptButtonText: qsTr("Seek games")
        onAccepted:  {
            pageStack.pop()
        }
        rejectButtonText: qsTr("Show board")
    }
}
