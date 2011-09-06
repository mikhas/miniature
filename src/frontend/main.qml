import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
    id: appWindow
    initialPage: MainPage { }

    // Funtion to push pages cleanly to the PageStack
    function loadScreen(f_ScreenName) {
        var component = Qt.createComponent(f_ScreenName);
        if (component.status == Component.Ready)
            pageStack.push(component);
        else console.log("Error loading component:", component.errorString())
    }

//    // Function available to go back to a specific page of the stack other that the previous one or the first
//    // (commented because it is not used anywhere yet)
//    // To go back one page just use pageStack.pop()
//    // To go to the initial page just use pageStack.pop(null)
//    function backScreen(b_ScreenName) {
//        var component = Qt.createComponent(b_ScreenName);
//        if (component.status == Component.Ready)
//            pageStack.pop(component);
//        else console.log("Error loading component:", component.errorString())
//    }
}
