import QtQuick 1.1
import com.meego 1.0

Page {
    id: ficsSeekGame

    // Generic app window style


    Rectangle {
        id: windowBackground
        // color: "palegoldenrod"
        anchors.fill: parent
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
            font.family: "Nokia Pure Headline"
            font.pointSize: 28
            width: 150
            anchors.bottom: headerBackground.bottom
            anchors.bottomMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10
        }

        Button {
            id: seekTime
            text: "10" // FIXME onPress number picker
            font.family: "Nokia Pure Headline"
            font.pointSize: 28
            width: 50
            anchors.bottom: headerBackground.bottom
            anchors.bottomMargin: 10
            anchors.right: seekIncrement.left
            anchors.rightMargin: 15
        }

        Button {
            id: seekIncrement
            text: "05" // FIXME onPress number picker
            font.family: "Nokia Pure Headline"
            font.pointSize: 28
            width: 50
            anchors.bottom: headerBackground.bottom
            anchors.bottomMargin: 10
            anchors.right: seekRated.left
            anchors.rightMargin: 20
        }

        // FIXME guest users can only play Unrated, button inactive
        Button {
            id: seekRated
            text: "Rated" // FIXME onPress Unrated / Any
            font.family: "Nokia Pure Headline"
            font.pointSize: 28
            width: 100
            anchors.bottom: headerBackground.bottom
            anchors.bottomMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
        }
    }

    // List of games available according to filters
    // FIXME there is a weird effect that makes list items sometimes visible at top bottom, out of the item area.
    Item {
        id: seekListWindow
        width: parent.width
        anchors.top: headerBackground.bottom
        anchors.topMargin: 50
        anchors.bottom: backIcon.top // FIXME a tool rectangle needs to be defined
        anchors.bottomMargin: 50

        ListView {
            id: seekList
            anchors.fill: seekListWindow
            model: gameAdvertisements
            delegate: Item { // feels weird that a delegate is called like this, jus copied from Qt examples
                id: listItem
                height: 88
                width: parent.width

                Row {
                    anchors.fill: parent

                    Column {
                        anchors.verticalCenter: parent.verticalCenter

                        Label {
                            id: opponentTextEntry
                            text: model.rating + " " + model.playerName + " " + model.time + " " + model.increment
                            font.pointSize: 36 // FIXME no idea why this font isn't bigger
                        }

                        Label {
                            id: opponentTextSubEntry
                            text: model.rated ? "rated" : "unrated"
                            font.pointSize: 22
                        }
                    }
                }

                Image {
                    source: "image://theme/icon-m-common-drilldown-arrow" + (theme.inverted ? "-inverse" : "")
                    anchors.right: parent.right;
                    anchors.verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: listItem
                    onClicked: {
                        loadScreen("OnlineBoard.qml") // FIXME must open confirm dialog
                    }
                }
            }
        }

    }

    // Not sure why this toolbar is not placed automatically in the bottom - getting rid of it and putting the icons alone.
    //ToolBarLayout {
    //    id: boardTools


        ToolIcon {
            id: backIcon
            iconId: "toolbar-back";
            anchors.bottom: ficsSeekGame.bottom
            anchors.left: ficsSeekGame.left
            onClicked: {
                loadScreen("MainPage.qml")
        }
        }

        // TO BE DECIDED The 2nd icon is used to confirm the move after the piece has been placed.
        // ToolButton {
        //    text: "Confirm";
        //    anchors.verticalCenter: backIcon.verticalCenter
        //    anchors.horizontalCenter: ficsSeekGame.horizontalCenter
        //}

        // Placeholder icon for settings etc
        ToolIcon {
            iconId: "toolbar-view-menu"
            anchors.bottom: ficsSeekGame.bottom
            anchors.right: ficsSeekGame.right
        }

        // }

}

