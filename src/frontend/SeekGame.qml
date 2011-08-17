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
            bottomMargin: 10
            left: parent.left
            leftMargin: 15
            right: parent.right
            rightMargin: 10
        }

        ListView {
            id: seekList
            anchors.fill: parent
            clip: true
            header: seekListHeader // empty header to avoid overlap of 1st item with shadow.
            spacing: 10

            model: gameAdvertisements

            Connections {
                target: gameAdvertisements
                onRowsInserted: seekList.positionViewAtEnd()
            }

            delegate:

                Item {
                id: listItem
                property Item previouslySelected: gameData
                property int previousIndex // Only used for debugging
                height: 80
                width: parent.width * 0.8

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
                        color: model.color == "auto" ? "lightgrey"
                                                     : model.color
                        anchors {
                            bottom: gameData.bottom
                            bottomMargin: 5
                            horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            id: opponentInfo
                            text: model.rating + " " + model.playerName // FIXME Ugly way while figuring out tables
                            color: model.color == "black" ? "white"
                                                          : "black"
                            font.pointSize: 16
                            font.weight: Font.DemiBold
                            anchors {
                                verticalCenter: opponentData.verticalCenter
                                left: parent.left
                                leftMargin: 10
                            }
                        }
                    }

                    Rectangle {
                        id: counter
                        width: seekList.width * 0.2
                        height: parent.height
                        color: "transparent"
                        anchors {
                            verticalCenter: gameData.verticalCenter
                            left: parent.right
                            // leftMargin: 20
                        }

                        Text {
                            text: index + 1
                            font.bold: true
                            font.pointSize: 40
                            color: "whitesmoke"
                            anchors.centerIn: parent
                        }
                    }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: gameData
                    onClicked: {
                        console.log(listItem.previouslySelected.state + " " + listItem.previousIndex)
                        //                        if (listItem.previouslySelected.state === "highlighted") FIXME this should bring previous highlighted back to normal
                        //                            listItem.previouslySelected.state = ""
                        if (listItem.state === "highlighted")
                            listItem.state = ""
                        else listItem.state = "highlighted"
                    }
                }
                states: [
                    State {
                        name: "highlighted"
                        PropertyChanges {
                            target: confirmButton
                            visible:true
                        }
                        PropertyChanges {
                            target: nonselectedEffect
                            visible: false
                        }
                        PropertyChanges {
                            target:  listItem
                            width: parent.width
                        }
                        StateChangeScript {
                            script: {
                                listItem.previouslySelected = listItem
                                listItem.previousIndex = index // only used for debugging
                            }
                        }
                    }
                ]

            }
        }
    }

    Component {
        id: seekListHeader
        Rectangle {
            height: 20
            width: parent.width
            color: "transparent"
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
                    loadScreen("MainPage.qml")
                }
            }

            ToolIcon { // Confirms the game
                id: confirmButton
                iconId: "toolbar-add"
                visible: false
                onClicked: {
                    miniature.play(model.id, "Your Name", model.playerName)
                    loadScreen("OnlineBoard.qml") // FIXME here would start the real game
                }
            }


            ToolIcon { // FIXME Settings page pending
                iconId: "toolbar-view-menu"
            }
        }
    }
}
