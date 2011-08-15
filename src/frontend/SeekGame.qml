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
            delegate: Item {
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
                        confirmGame.open();
                    }

                    // Confirm game dialog
                    QueryDialog {
                        id: confirmGame
                        // icon: "" FIXME anybody volunteers an battling image for this dialog?
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
