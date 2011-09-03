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

    function loadScreen(f_ScreenName)
    {
        var component = Qt.createComponent(f_ScreenName);

        // FIXME: Actually, we are supposed to pop the pages from this very stack
        // whenever we return from a page. Currently, we just keep reloading
        // components ... leads to problems when same page is loaded twice!
        // This hack removes any component with f_ScreenName before the push,
        // ensuring that each page only exists once.
        if (component.status == Component.Ready) {
            pageStack.pop(pageStack.find(function(component) {
                return component.name == "f_ScreenName";
            }));

            pageStack.push(component);
        } else {
            console.log("Error loading component:", component.errorString());
        }
    }

}
