import QtQuick 1.1
import com.meego 1.0

Page {
    id: ficsSeekGame
    orientationLock: PageOrientation.LockPortrait

    // Generic app window style

    Rectangle {
        id: windowBackground
        color: "lightgray"
        anchors.fill: parent
        z: -10
    }

    // The seek/filter options are integrated in the UI already
    Rectangle {
        id: headerBackground
        color: "olivedrab"
        width: parent.width
        height: 70
        anchors.top: parent.top

        Button {
            id: seekTypeChess
            text: "Standard" // FIXME onPress Suicide, Bughouse, etc
            width: 150
            anchors.bottom: headerBackground.bottom
            anchors.bottomMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10
        }

        Button {
            id: seekTime
            text: "-" // FIXME onPress number picker
            width: 50
            anchors.bottom: headerBackground.bottom
            anchors.bottomMargin: 10
            anchors.right: seekIncrement.left
            anchors.rightMargin: 15
        }

        Button {
            id: seekIncrement
            text: "-" // FIXME onPress number picker
            width: 50
            anchors.bottom: headerBackground.bottom
            anchors.bottomMargin: 10
            anchors.right: seekRated.left
            anchors.rightMargin: 20
        }

        // FIXME guest users can only play Unrated, button inactive
        Button {
            id: seekRated
            text: "Unrated" // FIXME onPress Unrated / Any
            width: 120
            anchors.bottom: headerBackground.bottom
            anchors.bottomMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
        }
    }

    // List of games available according to filters
   Rectangle {
        id: seekListWindow
        color:  "lightgray"
        anchors {
            top: headerBackground.bottom
            bottom: seekToolbar.top
            left: parent.left
            leftMargin: 15
            right: parent.right
            rightMargin: 10
        }

        ListView { // FIXME ideally the list should have always the most recent item visible
            id: seekList
            anchors.fill: parent
            clip: true
            spacing: 10 // FIXME an empty header of 20 pixels needs to be created to separate 1st item from shadow
            model: gameAdvertisements
            delegate:

                Item {
                id: listItem
                height: 80
                width: parent.width * 0.8 // a "+" for confirming is meant to fill the remaining 0.2

                Rectangle {
                    id: gameData
                    color: "olivedrab"
                    anchors.fill: parent
                    radius: 10

                    Rectangle {
                        id: nonselectedEffect
                        anchors.fill: parent
                        color: "white"
                        opacity: 0.4
                        radius: 10
                    }

                    Text {
                        id: typeChess
                        text: "Standard" // FIXME the right model* variable goes here
                        font.family: "Nokia Pure Headline"
                        font.pointSize: 22
                        color: "black"
                        anchors {
                            bottom: parent.bottom
                            bottomMargin: 40
                            left: parent.left
                            leftMargin: 10
                        }
                    }

                    Text {
                        id: timeGame
                        text: model.time
                        font.family: "Nokia Pure Headline"
                        font.pointSize: 22
                        color: "black"
                        anchors {
                            right: incrementGame.left
                        rightMargin: 15
                        bottom: parent.bottom
                        bottomMargin: 40
                        }
                    }

                    Text {
                        id: incrementGame
                        text: model.increment
                        font.family: "Nokia Pure Headline"
                        font.pointSize: 22
                        color: "black"
                        anchors {
                            right: ratedGame.left
                        rightMargin: 22
                        bottom: parent.bottom
                        bottomMargin: 40
                        }
                    }


                    Text {
                        id: ratedGame
                        text: model.rated ? "rated" : "unrated"
                        font.family: "Nokia Pure Headline"
                        font.pointSize: 22
                        color: "black"
                        anchors {
                            right: parent.right
                       rightMargin: 10
                        bottom: parent.bottom
                        bottomMargin: 40
                        }
                    }


                    // Opponent info
                    Rectangle {
                        id: opponentData
                        width: parent.width - 10
                        height: 35 // FIXME how to make it relative to text height?
                        color: "lightgrey" // FIXME also white or black if the opponent has defined piece color
                        anchors {
                            bottom: gameData.bottom
                        bottomMargin: 5
                        horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            id: opponentInfo
                            text: model.rating + " " + model.playerName // FIXME Ugly way while figuring out tables
                            color: "black" // FIXME or white if opponentData.color == "black"
                            font.pointSize: 16
                            font.weight: Font.DemiBold
                            anchors {
                                verticalCenter: opponentData.verticalCenter
                            left: parent.left
                            leftMargin: 10
                            }
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        onClicked: {
                            confirmGame.open();
                        }

                        // Confirm game dialog - TO BE REPLACED by "+" button next to selected item
                        QueryDialog {
                            id: confirmGame
                            titleText: "Ready to play?"
                            message: model.rating + " " + model.playerName + " " + model.time + " " + model.increment
                            acceptButtonText: "Yes"
                            onAccepted: {
                                loadScreen("OnlineBoard.qml") // FIXME here would start the real game
                            }
                            rejectButtonText: "No"
                        }
                    }

                }
            }
        }
    }

    Rectangle { // Little effect of shadow in list
        width: ficsSeekGame.width
        height: 15
        z: +2
        anchors.top: headerBackground.bottom
        gradient: Gradient {
            GradientStop { position: 0.0; color: "black" }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }



    // Toolbar at the bottom of the page - probably this needs to be part of a stacked page
    ToolBar {
        id: seekToolbar
        anchors.bottom: parent.bottom
        tools: ToolBarLayout {
            ToolIcon {
                id: backIcon
                iconId: "toolbar-back";
                onClicked: {
                    loadScreen("MainPage.qml") // FIXME a dialog of stopping options to be plugged here
                }
            }

            ToolIcon { // FIXME Settings page pending
                iconId: "toolbar-view-menu"
            }
        }
    }
}
