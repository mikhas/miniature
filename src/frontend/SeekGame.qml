import QtQuick 1.1
import com.nokia.meego 1.0
import org.maemo.miniature 1.0

Page {
    id: ficsSeekGame
    orientationLock: PageOrientation.LockPortrait

    Connections {
        target: miniature
        onGameStarted: loadScreen("OnlineBoard.qml")
    }

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
                    ListElement { name: "0" }
                    ListElement { name: "1" }
                    ListElement { name: "2" }
                    ListElement { name: "3" }
                    ListElement { name: "4" }
                    ListElement { name: "5" }
                    ListElement { name: "7" }
                    ListElement { name: "10" }
                    ListElement { name: "15" }
                    ListElement { name: "20" }
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
                    ListElement { name: "5" }
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
        anchors.bottom: userZone.top
        width: parent.width
        z: 10
    }

    Rectangle {
        id: seekListWindow
        color:  "lightgray"
        anchors {
            top: headerBackground.bottom
            bottom: userZone.top
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

            model: availableSeeks

            delegate: Item {
                id: listItem
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
                        color: "lightgrey"
                        opacity: 0.6
                        radius: 10
                        z: 20
                        visible: model.highlighted
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
                            text: model.rating + " " + model.playerName
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

                    MouseArea {
                        anchors.fill:  parent
                        onClicked: {
                            miniature.toggleGameAdvertisementHighlighting(model.id)
                            miniature.play(model.id) // FIXME here goes an instruction to send the response to the selected challenge
                            // FIXME note that selecting a new seek implies that the old response is cancelled/withdrawn
                        }
                    }
                }

                Rectangle {
                    id: spinnerBackground
                    width: (parent.width * 0.25) + 10
                    height: 80
                    color: "transparent"
                    visible: model.highlighted
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        rightMargin: -((parent.width * 0.25) + 10)
                    }

                    Connections {
                        target: miniature
                        onSeekCancelled: {
                            visible = false
                            spinner.running = false
                            model.remove(model.id)
                        }

                        onGameStarted: {
                            visible = false
                            spinner.running = false
                            model.remove(model.id)
                        }
                    }

                    BusyIndicator {
                        id: spinner
                        anchors.centerIn: parent
                        running: model.highlighted
                    }
                }
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

    Rectangle { // counter of games listed
        id: counterBackground
        width: 80
        height: 80
        color: "transparent"
        anchors {
            top: headerBackground.bottom
            topMargin: 20
            right: parent.right
            rightMargin: 10
        }

        Text {
            id: counter
            text: seekList.count
            font.bold: true
            font.pointSize: 40
            color: "whitesmoke"
            anchors.centerIn: parent
        }
    }

    // New seek item.
    Rectangle {
        id: newgameArea
        height: 90
        width:  parent.width
        color: "cornflowerblue"
        anchors.bottom: seekToolbar.top
        visible: false
        z: 10

        Rectangle {
            id: newgameData
            color: "olivedrab"
            width: (parent.width - 25) * 0.8
            height: 80
            radius: 10
            anchors {
                bottom: parent.bottom
                bottomMargin: 5
                left: parent.left
                leftMargin: 15
            }

            Text {
                id: newtypeChess
                text: "Standard"
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
                id: newtimeGame
                text: timeDialog.model.get(timeDialog.selectedIndex).name
                font.family: "Nokia Pure Headline"
                font.pointSize: 22
                color: "black"
                anchors {
                    right: newincrementGame.left
                    rightMargin: 15
                    bottom: parent.bottom
                    bottomMargin: 40
                }
            }

            Text {
                id: newincrementGame
                text: incrementDialog.model.get(incrementDialog.selectedIndex).name
                font.family: "Nokia Pure Headline"
                font.pointSize: 22
                color: "black"
                anchors {
                    right: newratedGame.left
                    rightMargin: 22
                    bottom: parent.bottom
                    bottomMargin: 40
                }
            }

            Text {
                id: newratedGame
                text: ratedDialog.model.get(ratedDialog.selectedIndex).name
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

            Rectangle {
                id: newplayerData
                width: parent.width - 10
                height: 35 // FIXME how to make it relative to text height?
                color: "lightgrey"
                anchors {
                    bottom: newgameData.bottom
                    bottomMargin: 5
                    horizontalCenter: parent.horizontalCenter
                }

                Text {
                    id: newplayerInfo
                    text: "++++" + " " + localSide.id // FIXME rating must be changed for a variable.
                    color: "black"
                    font.pointSize: 16
                    font.weight: Font.DemiBold
                    anchors {
                        verticalCenter: newplayerData.verticalCenter
                        left: parent.left
                        leftMargin: 10
                    }
                }
            }

            MouseArea {
                anchors.fill:  parent
                onClicked: {
                    miniature.play(seekList.highId)
                }
            }
        }

        Rectangle {
            id: newcounterBackground
            width: 80
            height: 80
            color: "transparent"
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 10
            }

            Connections {
                target: miniature
                onGameStarted: {
                    visible = false
                    newSpinner.running = false
                }
            }


            BusyIndicator {
                id: newSpinner
                anchors.centerIn: parent
                running: newgameArea.visible
            }

            MouseArea {
                anchors.fill:  parent
                onClicked: {
                    miniature.play(seekList.highId)
                }
            }
        }
    }

    // Local user info
    Rectangle {
        id: separator
        height: 1
        width:  parent.width
        color: "cornflowerblue"
        anchors.bottom: userZone.top
    }

    Rectangle {
        id: userZone
        width: parent.width
        height: 40 // FIXME how to make it relative to text height?
        color: "lightgrey"
        anchors.bottom:  seekToolbar.top

        Text {
            id: userInfo
            text: "++++" + " " + localSide.id // FIXME rating must be changed for variable.
            font.pointSize: 16
            font.weight: Font.DemiBold
            anchors.verticalCenter: userZone.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 30
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
                    miniature.logout()
                    loadScreen("MainPage.qml")
                }
            }

            ToolIcon { // New seek button
                id: newSeek
                anchors.horizontalCenter: parent.horizontalCenter
                iconId: "toolbar-add"
                onClicked: {
                    newSeekDialog.open();
                }

                QueryDialog { // New seek dialog
                    id: newSeekDialog
                    titleText: "New seek"
                    message: (
                                 "Standard" + " " +
                                 timeDialog.model.get(timeDialog.selectedIndex).name + " " +
                                 incrementDialog.model.get(incrementDialog.selectedIndex).name + " " +
                                 ratedDialog.model.get(ratedDialog.selectedIndex).name + "\n" +
                                 localSide.id
                                 )
                    acceptButtonText: "Yes"
                    onAccepted: {
                        miniature.seek(timeDialog.model.get(timeDialog.selectedIndex).name,
                                       incrementDialog.model.get(incrementDialog.selectedIndex).name,
                                       ratedDialog.model.get(ratedDialog.selectedIndex).name,
                                       "Auto")
                        newgameArea.visible = true
                        separator.anchors.top = newgameArea.top
                        // FIXME instructions to send a new seek to the backend.
                        // We are supporting one seek at a time at this point. If the user sends a new seek the old one is cancelled.
                    }
                    rejectButtonText: "No"
                    visualParent: dialogWrapper
                }
            }

            //            ToolIcon { // FIXME Settings page pending
            //                iconId: "toolbar-view-menu"
            //            }
        }
    }
}
