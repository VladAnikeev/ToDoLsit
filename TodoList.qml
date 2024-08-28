import QtQuick 2.12
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Universal 2.12

import Todo 1.0

ColumnLayout {
    property bool statusChanged: false

    function isValidInput(input) {
        var regex = /^(0[1-9]|[12][0-9]|3[01])\/(0[1-9]|1[0-2])\/(19|20)\d{2}$/
        return regex.test(input)
    }

   
    Frame {
        Layout.fillWidth: true

        background: Rectangle {
            color: "transparent"
            border.color: "#AA00FF"
            radius: 2
        }
        ListView {
            implicitWidth: 560
            implicitHeight: 380
            clip: true 
            anchors.fill: parent
            spacing: 10

            ScrollBar.vertical: ScrollBar {
                active: true
            }
            
            model: TodoModel {
                list: todoList
            }

         
            delegate: ColumnLayout {
                width: parent ? parent.width : 0
                RowLayout {
                    width: parent ? parent.width : 0 
                    TextField {
                        id: txtId
                        text: model.id
                        width: 40
                        visible: false
                    }
                    CheckBox {
                        checked: model.done
                        onClicked: {
                            model.done = checked
                            statusChanged = true
                        }
                    }
                    TextField {
                        id: txtHeading
                        Layout.fillWidth: true
                        text: model.heading
                        placeholderText: "Заголовок"
                        onEditingFinished: model.heading = text
                        onTextEdited: {
                            statusChanged = true
                            model.heading = text
                        }
                    }

                    TextField {
                        id: txtDate
                        Layout.preferredWidth: 80
                        text: model.date

                        placeholderText: "dd/mm/yyyy"

                        onEditingFinished: model.date = text
                        padding: 5

                        onTextChanged: {
                            if (!isValidInput(txtDate.text)) {
                                txtDate.color = "red" 
                            } else {
                                txtDate.color = "black" 
                                statusChanged = true
                                model.date = text
                            }
                        }
                    }
                }
                TextArea {
                    Layout.fillWidth: true
                    id: txtDescription
                    placeholderText: "Описание"
                    padding: 15
                    text: model.description
                    onEditingFinished: model.description = text
                    onTextChanged: {
                        statusChanged = true
                        model.description = text
                    }
                }
            }
        }
    }

    Frame {
        Layout.fillWidth: true

        background: Rectangle {
            color: "transparent"
            border.color: "#AA00FF"
            radius: 2
        }

        ColumnLayout {
            width: parent ? parent.width : 0
            RowLayout {

                width: parent ? parent.width : 0
                Text {
                    Layout.fillWidth: true
                    text: "Шаблон поиска"
                }
                Text {
                    Layout.preferredWidth: 80
                    text: "От"
                }
                Text {
                    Layout.preferredWidth: 75
                    text: "До"
                }
            }

            RowLayout {
                width: parent ? parent.width : 0
                CheckBox {
                    id: findCheckBox
                }
                TextField {
                    id: findHeading
                    placeholderText: "Заголовок задачи"
                    Layout.fillWidth: true
                }
                TextField {
                    id: findFromDate
                    Layout.preferredWidth: 80
                    placeholderText: "dd/mm/yyyy"
                    padding: 5

                    onTextChanged: {
                        if (!isValidInput(findFromDate.text)) {
                            findFromDate.color = "red" 
                        } else {
                            findFromDate.color = "black"
                            statusChanged = true
                        }
                    }
                }
                TextField {
                    id: findBeforeDate
                    Layout.preferredWidth: 80
                    placeholderText: "dd/mm/yyyy"
                    padding: 5

                    onTextChanged: {
                        if (!isValidInput(findBeforeDate.text)) {
                            findBeforeDate.color = "red"
                        } else {
                            findBeforeDate.color = "black"
                            statusChanged = true
                        }
                    }
                }
            }
            TextArea {
                id: findTextArea
                Layout.fillWidth: true
                placeholderText: "Описание"
                padding: 15
                visible: true
            }
        }
    }

    RowLayout {
        Button {
            text: qsTr("Добавить элемент")
            onClicked: {
                statusChanged = true
                todoList.append_task()
            }
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Удалить элемент")
            onClicked: todoList.remove_completed_tasks()
            Layout.fillWidth: true
        }
        Button {
            text: qsTr("Поиск")
            onClicked: todoList.find(findCheckBox.checked, findHeading.text,
                                     findTextArea.text, findFromDate.text,
                                     findBeforeDate.text)
            Layout.fillWidth: true
        }
        Button {
            text: qsTr("Сброс")
            onClicked: todoList.reset()
            Layout.fillWidth: true
        }
    }
}
