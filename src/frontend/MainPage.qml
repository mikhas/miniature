import QtQuick 1.1
import com.meego 1.0

Page { // FIXME how to make the height fixed so the virtual keyboard doesn't push it up?
    id: ficsLogin
    orientationLock: PageOrientation.LockPortrait

    // Generic app window style


    Rectangle {
        id: windowBackground
        color: "#020167"
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
        text: "Play online - FICS"
        font.family: "Nokia Pure Headline"
        font.pointSize: 28
        color: "white"
        anchors.bottom: headerBackground.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
    }

Image {
        id: ficsBanner
        source: "FicsBanner.jpg"
        width: parent.width // - 10
        fillMode: Image.PreserveAspectFit
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: headerBackground.bottom

        MouseArea {
            anchors.fill: parent
            onClicked: Qt.openUrlExternally("http://freechess.org/")
        }
    }

    Rectangle {
        id: whiteBackground
        width: parent.width * 0.8
        color: "white"
        radius: 20
        anchors.right: parent.right
        anchors.top: ficsBanner.bottom
        anchors.bottom: ficsFooter.top

        Rectangle { // Just a patch to cover the top right rounded corner
            color: "white"
            width: 50
            height: 50
            anchors.top: parent.top
            anchors.right: parent.right
        }

        Button {
            id: anonymousButton
            text: "Join as guest"
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                miniature.login("guest", "")
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
            z: 10
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
                miniature.login(idForm.text, idPassword.text)
                loadScreen("SeekGame.qml")
            }
        }

        Button {
            id: registerButton
            text: "Register online"
            anchors.top:  loginButton.bottom
            anchors.topMargin: 70
            anchors.left: anonymousButton.left
            anchors.right: anonymousButton.right
            onClicked: { Qt.openUrlExternally("http://www.freechess.org/cgi-bin/Register/FICS_register.cgi") }
        }

        Button { // Provisional button to access OnlineBoard for testing purposes
            id: testingButton
            text:  "Testing: OnlineBoard"
            opacity: 0.3
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: loadScreen("OnlineBoard.qml")
        }
    }

    Text {
        id: ficsFooter
        text: "info: freechess.org"
        font.pointSize: 20
        font.bold: true
        color: "white"
        height:  40
        verticalAlignment: Text.AlignVCenter
        anchors {
            bottom: parent.bottom
            bottomMargin: 10
            horizontalCenter: whiteBackground.horizontalCenter
        }

        MouseArea {
            anchors.fill: parent
            onClicked: Qt.openUrlExternally("http://freechess.org/") }
    }
}

