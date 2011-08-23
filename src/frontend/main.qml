import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
    id: appWindow
    initialPage: MainPage { }

    // Still figuring out the app navigation. We need icons to replace these buttons.

    // ToolBarLayout {
    //    id: gameTypes
    //    visible: false

        // ToolIcon { iconId: "toolbar-back"; }
        // ToolButton { text: "Online"; }
        // ToolButton { text: "P2P"; }
        // ToolButton { text: "F2F"; }
        // ToolIcon { iconId: "toolbar-view-menu" }

    // Borrowed function to jump from one qml file to another. There must be a simpler way?

    function loadScreen(f_ScreenName) {
        var component = Qt.createComponent(f_ScreenName)

        if (component.status == Component.Ready)
        {
            pageStack.push(component);
        }
        else
        {
            console.log("Error loading component:", component.errorString());
        }
    }

}
