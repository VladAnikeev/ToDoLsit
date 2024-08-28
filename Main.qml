import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Universal

ApplicationWindow {
    width: 640
    height: 600
    visible: true
    title: qsTr("Todo List")

    Universal.theme: Universal.Light
    Universal.accent: Universal.Violet

    TodoList {
        anchors.centerIn: parent
    }
    onClosing: {
        todoList.save()
    }
}
