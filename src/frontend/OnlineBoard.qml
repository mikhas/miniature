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
    id: ficsBoard
    orientationLock: PageOrientation.LockPortrait

    // property int rated: activeGame.rating
    function gameRated() { // trick to convert int activeGame.rating to the right value
        if (activeGame.rating === 0) return "unrated"
        else return "rated"
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
            text: activeGame.time
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


    // Chat area
    Rectangle {
        id: chatBkg
        property bool expanded: false
        color: "black"
        opacity: 0.8
        anchors.top: headerBackground.bottom
        anchors.bottom: opponentZone.top
        anchors.left: parent.left
        anchors.right: parent.right
        z: 20

        Text { // It would be cool to have different text colors for FICS, user and opponent - inspired in IRC & Eboard's console.
            id: chatLog
            color: "white"
            font.pointSize: 16
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            clip: true
            //            lineHeight: 1.5 // // commenting out for MeeGo CE in N900 compatibility
            wrapMode: Text.Wrap
            verticalAlignment: Text.AlignBottom

            // FIXME this needs to be the real log from FICS
            property string liveLog:
                "Creating: Garry (1333) (++++) Me (1333) unrated standard 10 05\n> Hi from $PLACE! I'm playing with my $DEVICE - sorry if I don't talk too much...\nGarry: Hi from Baku. No problem, this is my first game here. Please have some mercy with me!"
            text: chatLog.liveLog
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
                    visible: true
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
            }
        ]

        transitions: [
            Transition {
                ParallelAnimation  {
                    PropertyAnimation {
                        property: "opacity"; from: 0; to: 0.8
                        easing.type: Easing.OutQuint
                    }
                    AnchorAnimation { easing.type: Easing.OutQuint }
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
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: chatButton.left
            anchors.rightMargin: 10
            visible: false
            z: 20
        }

        Button {
            id: chatButton
            text: "Chat"
            width: 100
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            visible: false
            z: 20
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
            text: "++++" + " " + remoteSide.id // FIXME must be changed for rating variable.
            color: localSide.color
            font.pointSize: 16
            font.weight: Font.DemiBold
            anchors.verticalCenter: opponentZone.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
        }

        Text { // I guess there is a way not to have two timers defined in a single file but here we go for now.
            id: opponentTime
            property int milliseconds: 1000 * 60 * 0.2 // ten minutes, FIXME needs to be a real variable
            property string time: getTime()
            property int increment

            function getTime() {
                var minutes = Math.floor(milliseconds / 1000 / 60)
                var seconds = Math.floor((milliseconds-minutes*1000*60) / 1000)
                if (milliseconds > 10000) opponentTime.color = localSide.color
                else opponentTime.color = "red"
                if (milliseconds < 0) return "..."
                else return "00:" + (minutes < 10 ? "0" + minutes : minutes) + ":" + (seconds < 10 ? "0" + seconds : seconds)
            }
            text: time
            font.pointSize: 24
            font.weight: Font.Bold
            font.family: "Courier"
            anchors.verticalCenter: opponentZone.verticalCenter
            anchors.right: opponentZone.right
            anchors.rightMargin: 30

            Timer  {
                id: opponentTimer
                interval: 1000; running: false; repeat: true;
                onTriggered: opponentTime.milliseconds -= 1000
            }
        }

        Rectangle {
            id: remoteMoves
            color: "cornflowerblue"
            width: parent.width
            height: 5
            anchors.bottom: parent.bottom
            z: 50
            visible: false
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
            color: remoteSide.color
            width: parent.width
            height:  50
            anchors.top: parent.top
            z: -1
        }
        Rectangle {
            id: localCorners
            color: localSide.color
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


        // Creating variables to detect valid piece moves and clean unused squares
        Item {
            id: checkMove
            property int moveNumber: 0 // Used by the timer to know when to start
        }

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
            text: "++++" + " " + localSide.id // FIXME rating must be changed for variables.
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
            property int milliseconds: 1000 * 60 * 0.2 // ten minutes, FIXME needs to be a real variable
            property string time: getTime()
            property int increment

            function getTime() {
                var minutes = Math.floor(milliseconds / 1000 / 60)
                var seconds = Math.floor((milliseconds-minutes*1000*60) / 1000)
                if (milliseconds > 10000) userTime.color = remoteSide.color
                else userTime.color = "red"
                if (milliseconds < 0) return "...";
                else return "00:" + (minutes < 10 ? "0" + minutes : minutes) + ":" + (seconds < 10 ? "0" + seconds : seconds)}

            text: time
            font.pointSize: 24
            font.weight: Font.Bold
            font.family: "Courier"
            anchors.verticalCenter: userZone.verticalCenter
            anchors.verticalCenterOffset: 3
            anchors.right: userZone.right
            anchors.rightMargin: 30

            Timer  {
                id: userTimer
                interval: 1000; running: false; repeat: true;
                onTriggered: userTime.milliseconds -= 1000
            }
        }

        Rectangle {
            id: localMoves
            color: "cornflowerblue"
            width: parent.width
            height: 5
            anchors.top: parent.top
            z: 50
            visible: false
        }
    }


    // Toolbar at the bottom of the page
    ToolBar {
        id: boardToolbar
        anchors.bottom: parent.bottom
        tools: ToolBarLayout {

            ToolIcon { // Stops the game, can lead to draw,
                id: backIcon
                iconId: "toolbar-back";
                onClicked: {
                    exitMenu.open()
                }

                Menu {
                    id: exitMenu
                    MenuLayout {
                        MenuItem {text: "Resign"; onClicked: resignDialog.open() }
                        MenuItem {text: "Propose a draw"; onClicked: proposedrawDialog.open() }
                        MenuItem {text: "Request to adjourn"; onClicked: requestadjournDialog.open() }
                        MenuItem {text: "Testing: accept draw"; onClicked: acceptdrawDialog.open() }
                        MenuItem {text: "Testing: accept adjourn"; onClicked: acceptadjournDialog.open() }
                        MenuItem {text: "Testing: end game"; onClicked: gameoverDialog.open() }
                        MenuItem {text: "Testing: connection lost"; onClicked: connectionlostDialog.open() }
                        MenuItem {text: "Convenient: Back to MainPage"; onClicked: {
                                miniature.logout()
                                pageStack.pop(null)
                            }}
                    }
                }
            }


            ToolIcon { // Confirms the move. Visible only when lastIndex && toIndex are selected.
                id: confirmButton
                iconId: "toolbar-add"
                visible: miniature.validMove
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    miniature.confirmMove()

                    checkMove.moveNumber += 1 // timer switcher FIXME the opponent moves will be signaled by the backend and this needs to reflect it
                    if (checkMove.moveNumber === 3) { userTime.increment = 5000 ; opponentTime.increment = 5000 } // FIXME increments must be substituted by variables
                    if (checkMove.moveNumber > 1)
                    { if (userTimer.running === true)
                        { userTime.milliseconds += userTime.increment
                            userTimer.running = false
                            opponentTimer.running = true
                            localCorners.color = localSide.color
                            remoteCorners.color = "cornflowerblue"
                            localMoves.visible = false
                            remoteMoves.visible = true
                        }
                        else { opponentTime.milliseconds += opponentTime.increment
                            opponentTimer.running = false
                            userTimer.running = true
                            remoteCorners.color = remoteSide.color
                            localCorners.color = "cornflowerblue"
                            remoteMoves.visible = false
                            localMoves.visible = true
                        }
                    }
                }
            }

            //            ToolIcon { // FIXME Settings page pending
            //                iconId: "toolbar-view-menu"
            //            }
        }
    }

    // Signals and their dialogs

    Connections {
        id: gameResolutions
        target: miniature
        property string winnerIs
        onGameEnded: {

            if (result == Miniature.ResultWhiteWins) { gameResolutions.winnerIs = "White wins" }
            if (result == Miniature.ResultBlackWins) { gameResolutions.winnerIs = "Black wins" }
            if (result == Miniature.ResultDraw) { gameResolutions.winnerIs = "Draw" }
            if (result == Miniature.ResultAdjourned) { gameResolutions.winnerIs = "Game adjourned" }
            if (result == Miniature.ResultUnknown) { gameResolutions.winnerIs = "Game aborted" }

            if (reason == Miniature.ReasonForfeitByDisconnect || Miniature.ReasonAbortedByDisconnect || Miniature.ReasonAdjournedByDisconnect)
            {remotedisconnectsDialog.open()}
           }
    }

    QueryDialog { // Confirm resign
        id: resignDialog
        titleText: "Resign?"
        acceptButtonText: "Yes"
        onAccepted: {
            // FIXME instructions to send a resign command
        }
        rejectButtonText: "No"
    }

    QueryDialog { // Propose draw
        id: proposedrawDialog
        titleText: "Propose a draw?"
        message:  "Needs to be accepted by $OPPONENT, otherwise the game continues." // FIXME real variable for $OPPONENT
        acceptButtonText: "Yes"
        onAccepted: {
            // FIXME instructions to send a draw request command
            // mikas: if a user proposes again a draw, he will just see the FICS message in the chat/console.
        }
        rejectButtonText: "No"
    }

    QueryDialog { // Request adjourn
        id: requestadjournDialog
        titleText: "Request to adjourn?"
        message:  "Needs to be accepted by $OPPONENT, otherwise the game continues." // FIXME real variable for $OPPONENT
        acceptButtonText: "Yes"
        onAccepted: {
            // FIXME instructions to send an adjourn request command
            // mikas: if a user requests to adjournagain, he will just see the FICS message in the chat/console.
        }
        rejectButtonText: "No"
    }

    QueryDialog { // Accept draw?
        id: acceptdrawDialog
        titleText: "Is this a draw?"
        message:  "$OPPONENT is proposing to end this game with a draw." // FIXME real variable for $OPPONENT
        acceptButtonText: "Accept"
        onAccepted: {
            // FIXME instructions to send an accept draw command
            // FIXME: if $OPPONENT requests a draw while a request already exists, this dialog shouldn't show up again.
        }
        rejectButtonText: "Ignore"
    }

    QueryDialog { // Accept adjourn?
        id: acceptadjournDialog
        titleText: "See you later?"
        message:  "$OPPONENT is requesting to adjourn this game." // FIXME real variable for $OPPONENT
        acceptButtonText: "Accept"
        onAccepted: {
            // FIXME instructions to send an accept draw command
            // FIXME: if $OPPONENT requests to adjourn while a request already exists, this dialog shouldn't show up again.
        }
        rejectButtonText: "Ignore"
    }

    QueryDialog { // End of game // FIXME this dialog needs to be nicer and wiser! Not spending extra time now.
        id: gameoverDialog
        titleText: "You won!" // FIXME or "Sorry...", depending.
        message:  "$whitePlayer: 1033 >> 1026\n$blackPlayer: 1033 >> 1041\nWhat's next?" // FIXME real variables
        acceptButtonText: "Challenge $OPPONENT"
        onAccepted: {
            // FIXME instructions to send a challenge to OPPONENT with the same parameters as the previous game
        }
        rejectButtonText: "Find other players"
        onRejected: {
            pageStack.pop()
        }
    }

    QueryDialog { // Connection lost
        id: connectionlostDialog
        titleText: "Lost connection"
        message:  "You have lost the connection to the server." // FIXME real variables
        acceptButtonText: "Log in again"
        onAccepted: {
            // FIXME with the stored values, can we attempt to re-login and resume a game automatically?
        }
        rejectButtonText: "Close app"
        onRejected: {
            // FIXME how to close the app?
        }
    }

    QueryDialog { // End of game // FIXME this dialog needs to be nicer and wiser! Not spending extra time now.
        id: remotedisconnectsDialog
        titleText: "Bye bye!"
        message:  remoteSide.id + " vanished.\n\nResult: " + gameResolutions.winnerIs
        acceptButtonText: "OK"
        onAccepted: {
            pageStack.pop()
        }
    }
}
