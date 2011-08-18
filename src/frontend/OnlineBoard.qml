import QtQuick 1.1
import com.meego 1.0

// Pending: proper parenting between groups of items

Page {
    id: ficsBoard
    orientationLock: PageOrientation.LockPortrait

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
            text: "10"
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
            text: "05"
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
            text: "Rated"
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
        color:  "lightgray"
        anchors.top: headerBackground.bottom
        anchors.bottom: opponentZone.top
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10

        Text {
            id: chatLog
            font.pointSize: 16
            anchors.fill: parent
            clip: true
            wrapMode: Text.Wrap
            verticalAlignment: Text.AlignBottom

            // FIXME this needs to be the real log from FICS
            property string liveLog:
                ":FICS - game25 Garry vs Me 10 05 Rated started.\n:Me - Hi from $PLACE! I'm playing with my $DEVICE - sorry if I don't talk too much...\n:Garry - Hi from Baku. No problem, this is my first game here. Please have some mercy with me!"
            text: chatLog.liveLog
        }
    }

    Rectangle { // Little effect of shadow in chat
        width: ficsBoard.width
        height: 15
        z: +2
        anchors.top: headerBackground.bottom
        gradient: Gradient {
            GradientStop { position: 0.0; color: "black" }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }


    // Opponent info
    Rectangle {
        id: opponentZone
        width: parent.width
        height: 40 // FIXME how to make it relative to text height?
        color: "black" // or white, whatever color is playing
        anchors.bottom: imageBoard.top

        Text {
            id: opponentInfo
            text: "1333" + " " + "Opponent Name" // FIXME Ugly way while figuring out tables
            color: "white" // or black, opposite of opponentZone
            font.pointSize: 16
            font.weight: Font.DemiBold
            anchors.verticalCenter: opponentZone.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
        }

        Text {
            id: opponentTime
            text: " " + "06:55:12" + " " // or mm:ss if it's <  1h
            color: "white" // or black, opposite of opponentZone
            font.pointSize: 24
            font.weight: Font.Bold
            anchors.verticalCenter: opponentZone.verticalCenter
            anchors.right: opponentZone.right
            anchors.rightMargin: 10

            Rectangle {
                id: opponentTimeHighlight
                anchors.fill: parent
                z: -1
                color: "transparent"
                border.color: "white" // Same as opponentTime.color
                border.width: 1
                radius: 2
                smooth: true
                visible: false
            }
        }
    }

    // The board

    Image {
        id: imageBoard
        source: "chessboard.svg"
        anchors.bottom: userZone.top
        width: parent.width
        fillMode: Image.PreserveAspectFit

        // 2 rectangles purely decorative to make the board edges look nicer
        Rectangle {
            color: "black" // FIXME needs to be always == opponent's color
            width: parent.width
            height:  50
            anchors.top: parent.top
            z: -1
        }
        Rectangle {
            color: "white" // FIXME needs to be always == user's color
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

        // Creating variables to detect valid piece moves and clean unused squares
        Item {
            id: checkMove
            property string colorPlaying: "white" // Teaching the program who starts the Chess game.
            property bool isValid: true // Poorman's Validator, always judges 'true'. The real one needs to plug here.
            property int fromIndex: -1
            property string fromKing // only needed for castling
            property int toIndex
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
                    source: piece
                    width: 60 // FIXME better not hardcoded
                    height: 60 // FIXME better not hardcoded
                }

                MouseArea {
                    id: squareMouseArea
                    anchors.fill: parent
                    onClicked:
                    { // console.log(squareDelegate.state); Used to debug board effects

                        // Cleaning previous state of the squareDelegate tapped
                        if (squareDelegate.state !== "")
                            squareDelegate.state = ""

                        // This is a tap with no piece already selected
                        if (checkMove.fromIndex === -1) {

                            // The tap is on an empty square or on a figure of wrong color
                            if (piece == "emptysquare.png" || pieceColor !== checkMove.colorPlaying)
                                squareDelegate.state = "wrongTap"

                            // The tap is on a figure of right color
                            else {

                                // The validator doesn't approve the piece selection - FIXME to be plugged
                                if (checkMove.isValid === false)
                                    squareDelegate.state = "wrongTap"

                                // The validator approves the piece selection - FIXME to be plugged
                                else squareDelegate.state = "pieceSelected"
                            }
                        }

                        // This is tap with a piece already selected
                        else {

                            // The tap is on another piece of the same color
                            if (pieceColor === checkMove.colorPlaying) {

                                // The tap is a castling // FIXME must check with mikhas what he wants to check here
                                if ((checkMove.fromKing === "whiteKing") && (checkMove.fromIndex === 60) && (piece === "white/rook.png") && (index === 63 || 56))
                                    squareDelegate.state = "pieceMoving"

                                // No castling & the validator doesn't approve the piece selection - FIXME to be plugged
                                else if (checkMove.isValid === false)
                                    squareDelegate.state = "wrongTap"

                                // No castling & the validator approves the piece selection - FIXME to be plugged
                                else squareDelegate.state = "pieceSelected"
                            }

                            // The tap is on an empty square or a piece of another color
                            else

                                // The validator doesn't approve the piece selection - FIXME to be plugged
                                if (checkMove.isValid === false)
                                    squareDelegate.state = "wrongTap"

                            // The validator approves the piece selection - FIXME to be plugged
                                else squareDelegate.state = "pieceMoving"
                        }
                    }
                }

                states: [

                    State {
                        name: "cleaningSquares"
                        StateChangeScript {
                            script: {
                                if (chessboardGrid.model.get(checkMove.toIndex).squareColor !== "transparent")
                                    chessboardGrid.model.set(checkMove.toIndex, {squareColor: "transparent"})
                            }
                        }
                    },

                    State {
                        name: "pieceSelected"
                        extend: 'cleaningSquares'
                        StateChangeScript {
                            script: {
                                checkMove.fromKing = figure // Needed only to detect castling
                                if (chessboardGrid.model.get(checkMove.fromIndex).squareColor !== "transparent")
                                    chessboardGrid.model.set(checkMove.fromIndex, {squareColor: "transparent"})
                            }
                        }
                        StateChangeScript {
                            script: {
                                chessboardGrid.model.set(index, {squareColor: "blue"})
                                checkMove.fromIndex = index
                            }
                        }
                    },

                    State {
                        name: "wrongTap"
                        extend: 'cleaningSquares'
                        PropertyChanges {
                            target: wrongTapAnimation
                            running: true
                        }
                    },

                    State {
                        name: "pieceMoving"
                        extend: 'cleaningSquares'
                        StateChangeScript {
                            script: {
                                chessboardGrid.model.set(index, {squareColor: "green"})
                                checkMove.toIndex = index
                            }
                        }
                        PropertyChanges {
                            target: confirmButton
                            visible: true
                        }
                    }
                ]
            }
        }

        model: BoardModel {}
        delegate: chessboardGridDelegate
    }


    // User info
    Rectangle {
        id: userZone
        width: parent.width
        height: 40 // FIXME how to make it relative to text height?
        color: "white" // or black, whatever color is playing
        anchors.bottom:  boardToolbar.top

        Text {
            id: userInfo
            text: "1333" + " " + "User Name" // FIXME Ugly way while figuring out tables
            color: "black" // or white, opposite of opponentZone
            font.pointSize: 16
            font.weight: Font.DemiBold
            anchors.verticalCenter: userZone.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10

        }

        Text {
            id: userTime
            text: " " + "07:35:42" + " " // or mm:ss if it's <  1h
            color: "black" // or white, opposite of opponentZone
            font.pointSize: 24
            font.weight: Font.Bold
            anchors.verticalCenter: userZone.verticalCenter
            anchors.right: userZone.right
            anchors.rightMargin: 10

            Rectangle {
                id: userTimeHighlight
                anchors.fill: parent
                z: -1
                color: "transparent"
                border.color: "black" // same as userTime.text
                border.width: 1
                radius: 2
                smooth: true
                visible: true
            }
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
                        MenuItem {text: "Convenient: Back to MainPage"; onClicked: loadScreen("MainPage.qml") }
                    }
                }
            }


            ToolIcon { // Confirms the move. Visible only when fromIndex && toIndex are selected.
                id: confirmButton
                iconId: "toolbar-add"
                visible: false
                onClicked: {
                    chessboardGrid.model.set(checkMove.toIndex, { // moving the piece, removing blue from square
                                             figure: chessboardGrid.model.get(checkMove.fromIndex).figure,
                                             piece: chessboardGrid.model.get(checkMove.fromIndex).piece,
                                             pieceColor: chessboardGrid.model.get(checkMove.fromIndex).pieceColor,
                                             squareColor: "transparent"})
                    chessboardGrid.model.set(checkMove.fromIndex, { // clearing the square of origin
                                             figure: "emptySquare",
                                             piece: "emptysquare.png",
                                             pieceColor: "empty",
                                             squareColor: "transparent"})
                    checkMove.fromIndex = -1 // back to pre-move conditions
                    confirmButton.visible = false
                    if (checkMove.colorPlaying === "white") // turn for the other player
                        checkMove.colorPlaying = "black"
                    else checkMove.colorPlaying = "white"
                    if (userTimeHighlight.visible === true)
                    { opponentTimeHighlight.visible = true ; userTimeHighlight.visible = false}
                    else { opponentTimeHighlight.visible = false ; userTimeHighlight.visible = true}
                }
            }

            ToolIcon { // FIXME Settings page pending
                iconId: "toolbar-view-menu"
            }
        }
    }

    // Dialogs

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
            loadScreen("SeekGame.qml")
        }
    }
}

