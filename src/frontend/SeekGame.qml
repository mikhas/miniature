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

    // Filter & new seek options, all at once.
    Rectangle {
        id: headerBackground
        color: "olivedrab"
        width: parent.width
        height: 70
        anchors.top: parent.top

        Button {
            id: seekTypeChess
            text: typeDialog.model.get(typeDialog.selectedIndex).name
            width: 150
            anchors {
                bottom: headerBackground.bottom
                bottomMargin: 10
                left: parent.left
                leftMargin: 10
            }
            onClicked: {
                typeDialog.open();
            }

            SelectionDialog {
                id: typeDialog
                titleText: "Chess variant"
                model:  ListModel {
                    ListElement { name: "Standard" }
                    ListElement { name: "Crazyhouse" }
                    ListElement { name: "Suicide" }
                }
                selectedIndex: 0
                visualParent: dialogWrapper
            }
        }

        Button {
            id: seekTime
            text: timeDialog.model.get(timeDialog.selectedIndex).name
            width: 50
            anchors {
                bottom: headerBackground.bottom
                bottomMargin: 10
                right: seekIncrement.left
                rightMargin: 15
            }
            onClicked: {
                timeDialog.open();
            }

            SelectionDialog {
                id: timeDialog
                titleText: "Time"
                model:  ListModel { // Is there a convenient number picker component available somewhere?
                    ListElement { name: "*" } // Any time is good
                    ListElement { name: "1" }
                    ListElement { name: "3" }
                    ListElement { name: "5" }
                    ListElement { name: "10" }
                    ListElement { name: "15" }
                    ListElement { name: "30" }
                    ListElement { name: "60" }
                }
                selectedIndex: 3
                visualParent: dialogWrapper
            }
        }

        Button {
            id: seekIncrement
            text: incrementDialog.model.get(incrementDialog.selectedIndex).name
            width: 50
            anchors {
                bottom: headerBackground.bottom
                bottomMargin: 10
                right: seekRated.left
                rightMargin: 20
            }
            onClicked: {
                incrementDialog.open();
            }

            SelectionDialog {
                id: incrementDialog
                titleText: "Increment"
                model:  ListModel { // Is there a convenient number picker component available somewhere?
                    ListElement { name: "*" } // Any increment is good
                    ListElement { name: "0" }
                    ListElement { name: "1" }
                    ListElement { name: "3" }
                    ListElement { name: "6" }
                    ListElement { name: "12" }
                    ListElement { name: "30" }
                }
                selectedIndex: 5
                visualParent: dialogWrapper
            }
        }


        Button {
            id: seekRated
            text: ratedDialog.model.get(ratedDialog.selectedIndex).name
            width: 120
            anchors {
                bottom: headerBackground.bottom
                bottomMargin: 10
                right: parent.right
                rightMargin: 10
            }
            onClicked: {
                ratedDialog.open();
            }

            SelectionDialog {
                id: ratedDialog
                titleText: "Type of match"
                model:  ListModel { // FIXME guest users can only play Unrated, button inactive
                    ListElement { name: "Any" } // Any option is good
                    ListElement { name: "Unrated" }
                    ListElement { name: "Rated" }
                }
                selectedIndex: 1 // Better 2 for registered users
                visualParent: dialogWrapper
            }
        }
    }

    // List of games available according to filters

    Rectangle { // Used to define the size of the dialogs
        id: dialogWrapper
        color:  "transparent"
        anchors.top: headerBackground.bottom
        anchors.bottom: seekToolbar.top
        width: parent.width
        z: 10
    }

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
                id: sortingStyle
                target: gameAdvertisements
                // var flowing = "seekList.positionViewAtEnd()" // FIXME This attempt to keep the highlighted item on view won't work
                // onRowsInserted: flowing // FIXME This attempt to keep the highlighted item on view won't work
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
                            text: model.rating + " " + model.playerName + model.highlighted // FIXME highlighted used for debugging
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
                        id: counterBackground
                        width: seekList.width * 0.2
                        height: parent.height
                        color: "transparent"
                        anchors {
                            verticalCenter: gameData.verticalCenter
                            left: parent.right
                        }

                        Text {
                            id: counter
                            text: index + 1
                            font.bold: true
                            font.pointSize: 40
                            color: "whitesmoke"
                            visible: true
                            anchors.centerIn: parent
                        }

                        ToolIcon { // Confirms the game
                            id: confirmButton
                            iconId: "toolbar-add"
                            visible: false
                            anchors.centerIn: parent
                            onClicked: {
                                console.log(model.playerName)
                                miniature.play(model.id, "Your Name", model.playerName)
                                loadScreen("OnlineBoard.qml")
                            }
                        }
                    }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: gameData
                    onClicked: {
                        console.log(listItem.previouslySelected.state + " " + listItem.previousIndex)
                        // if (listItem.previouslySelected.state === "highlighted") FIXME this should bring previous highlighted back to normal
                        // listItem.previouslySelected.state = ""
                        // seekList.model.set(listItem.previousIndex, { highlighted: false }) FIXME not working either
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
                            target: counter
                            visible: false
                        }
                        StateChangeScript {
                            script: {
                                // seekList.model.set(index, { highlighted: false }) // FIXME not working
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

            ToolButton { // New seek button
                id: newSeek
                text: "New"
                onClicked: {
                    newSeekDialog.open();
                }

                QueryDialog { // New seek dialog
                    id: newSeekDialog
                    titleText: "New seek"
                    message: (
                                 typeDialog.model.get(typeDialog.selectedIndex).name + " " +
                                 timeDialog.model.get(timeDialog.selectedIndex).name + " " +
                                 incrementDialog.model.get(incrementDialog.selectedIndex).name + " " +
                                 ratedDialog.model.get(ratedDialog.selectedIndex).name + "\n" +
                                 "1033 Username" // This still needs the real variables
                                 )
                    acceptButtonText: "Yes"
                    onAccepted: {
                        // FIXME instructions to send a new seek
                    }
                    rejectButtonText: "No"
                    visualParent: dialogWrapper
                }
            }

            ToolIcon { // FIXME Settings page pending
                iconId: "toolbar-view-menu"
            }
        }
    }
}
