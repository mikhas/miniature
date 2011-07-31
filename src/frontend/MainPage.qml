import QtQuick 1.1
import com.meego 1.0

Page {
    id: ficsLogin

    // Generic app window style


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
    }

    Text {
        id: poormansHeader
        text: "Let's go freechess.org!"
        font.family: "Nokia Pure Headline"
        font.pointSize: 28
        color: "white"
        anchors.bottom: headerBackground.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
    }


    Text {
        id: ficsIntro
        text: "The Free Internet Chess Server (aka FICS) is a great place to play. (Join as guest to see the seek game UI. Log in to see the board UI)"
        font.pointSize: 16
        width: parent.width
        anchors.top: ficsLogin.top
        anchors.topMargin: 100
        anchors.left: parent.left
        anchors.leftMargin: 10
        clip: true
        wrapMode: Text.Wrap
    }

    Button {
        id: anonymousButton
        text: "Join as guest"
        anchors.top: ficsLogin.top
        anchors.topMargin: 200
        anchors.horizontalCenter: ficsLogin.horizontalCenter
        onClicked: {
            loadScreen("SeekGame.qml")
        }
    }

    TextField {
        id: idForm
        placeholderText: "username"
        anchors.top:  anonymousButton.bottom
        anchors.topMargin: 70
        anchors.left: anonymousButton.left
        anchors.right: anonymousButton.right
    }
    TextField {
        id: idPassword
        placeholderText: "password"
        echoMode: TextInput.Password
        anchors.top:  idForm.bottom
        anchors.topMargin: 5
        anchors.left: anonymousButton.left
        anchors.right: anonymousButton.right
    }
    Button {
        id: loginButton
        text: "Log in"
        anchors.top:  idPassword.bottom
        anchors.topMargin: 10
        anchors.left: anonymousButton.left
        anchors.right: anonymousButton.right
        onClicked: {
            loadScreen("OnlineBoard.qml")
    }

    // tools: gameTypes - still figuring out navigation

    }
}
