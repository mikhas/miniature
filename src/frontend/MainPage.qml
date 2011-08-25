import QtQuick 1.1
import com.nokia.meego 1.0

Page { // FIXME how to make the height fixed so the virtual keyboard doesn't push it up?
    id: ficsLogin
    orientationLock: PageOrientation.LockPortrait

    // Generic app window style

   Rectangle {
        id: windowBackground
        color: "lightgray"
        anchors.fill: parent
    }

    Image {
        source: "miniatureBIG.png"
        x: -110
        y: 100
        height: 900
        fillMode: Image.PreserveAspectCrop
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

    Button {
        id: anonymousButton
        text: "Join as guest"
        anchors.top: headerBackground.bottom
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

    Text {
        id: ficsDescription
        text: "freechess.org"
        font.pointSize: 20
        font.underline: true
        color: "white"
        height: 40
        verticalAlignment: Text.AlignVCenter
        anchors {
            top: registerButton.bottom
            topMargin: 200
            horizontalCenter: parent.horizontalCenter
        }
        MouseArea {
            anchors.fill: parent
            onClicked: Qt.openUrlExternally("http://freechess.org/") }
    }

//    Button { // Provisional button to access OnlineBoard for testing purposes
//        id: testingButton
//        text:  "Testing: OnlineBoard"
//        opacity: 0.3
//        anchors.top: registerButton.bottom
//        anchors.topMargin: 100
//        anchors.horizontalCenter: parent.horizontalCenter
//        onClicked: loadScreen("OnlineBoard.qml")
//    }


}


