import QtQuick 1.1
import com.meego 1.0

// Pending: proper parenting between groups of items

Page {
    id: ficsBoard

    Rectangle {
        id: windowBackground
        // color: "palegoldenrod"
        anchors.fill: parent
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
        width: parent.width
        height:  130 // FIXME should be automatic based on space available
        color:  "lightgray"
        anchors.top: headerBackground.bottom


        Text {
            id: chatLog
            text: "Here goes the chat log. Tapping opens a semi-transparent window with more text."
            font.pointSize: 16
            anchors.top: chatBkg.top
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 10
            width: parent.width
            clip: true
            wrapMode: Text.Wrap
        }

        TextField {
            id: chatField
            anchors.bottom: chatBkg.bottom
            anchors.bottomMargin: 10
            anchors.left: chatBkg.left
            anchors.leftMargin: 10
            anchors.right: chatButton.left
            anchors.rightMargin: 10
        }

        Button {
            id: chatButton
            text: "Chat"
            width: 100
            anchors.right: chatBkg.right
            anchors.rightMargin: 10
            anchors.bottom: chatBkg.bottom
            anchors.bottomMargin: 10
        }
    }


    // Opponent info
    Rectangle {
        id: opponentZone
        width: parent.width
        height: 40 // FIXME how to make it relative to text height?
        color: "black" // or white, whatever color is playing
        anchors.top: chatBkg.bottom
    }

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
        text: "06:55:12" // or mm:ss if it's <  1h
        color: "white" // or black, opposite of opponentZone
        font.pointSize: 26
        font.weight: Font.Bold
        anchors.verticalCenter: opponentZone.verticalCenter
        anchors.right: opponentZone.right
        anchors.rightMargin: 10
    }

    // The board

    Image {
        id: imageBoard
        source: "chessboard.svg"
        width: ficsBoard.width
        fillMode: Image.PreserveAspectFit
        anchors.top: opponentZone.bottom
    }

    // User info
    Rectangle {
        id: userZone
        width: parent.width
        height: 40 // FIXME how to make it relative to text height?
        color: "white" // or black, whatever color is playing
        anchors.top:  imageBoard.bottom
    }

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
        text: "07:35:42" // or mm:ss if it's <  1h
        color: "black" // or white, opposite of opponentZone
        font.pointSize: 26
        font.weight: Font.Bold
        anchors.verticalCenter: userZone.verticalCenter
        anchors.right: userZone.right
        anchors.rightMargin: 10
    }

    // Not sure why this toolbar is not placed automatically in the bottom - getting rid of it and putting the icons alone.
    //ToolBarLayout {
    //    id: boardTools


        // The 1st icon is used to stop the game, can lead to draw,
        ToolIcon {
            id: backIcon
            iconId: "toolbar-back";
            anchors.bottom: ficsBoard.bottom
            anchors.left: ficsBoard.left
            onClicked: {
                loadScreen("MainPage.qml")
        }
        }


        // The 2nd icon is used to confirm the move after the piece has been placed.
        ToolButton {
            text: "Confirm";
            anchors.verticalCenter: backIcon.verticalCenter
            anchors.horizontalCenter: ficsBoard.horizontalCenter
        }

        // Placeholder icon for settings etc
        ToolIcon {
            iconId: "toolbar-view-menu"
            anchors.bottom: ficsBoard.bottom
            anchors.right: ficsBoard.right
        }

        // }

}


