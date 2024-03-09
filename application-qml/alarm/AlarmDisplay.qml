import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.vicr123.Contemporary
import Contemporary

Item {
    id: root
    implicitHeight: childrenRect.height

    required property string objectPath

    signal remove(timezone: string)

    AlarmPaneController {
        id: itemController
        objectPath: root.objectPath
    }

    SwipeDelegate {
        id: swipeDelegate
        anchors.centerIn: parent

        implicitHeight: worldClockLayer.height
        implicitWidth: 600

        padding: 0

        contentItem: MouseArea {
            acceptedButtons: Qt.RightButton

            Layer {
                id: worldClockLayer

                height: childrenRect.height + 9 + 6 + 9
                implicitWidth: 600

                color: Contemporary.calculateLayer(1)

                Switch {
                    id: activeSwitch
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 9
                    checked: itemController.active

                    onCheckedChanged: itemController.active = activeSwitch.checked
                }

                ColumnLayout {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 9
                    anchors.leftMargin: 9

                    Label {
                        id: timeLabel
                        text: itemController.offsetString
                        color: itemController.active ? Contemporary.foreground : Contemporary.disabled(Contemporary.foreground)

                        font.pointSize: 30
                    }

                    Label {
                        id: snoozeLabel
                        text: itemController.snoozeOffsetString
                        visible: itemController.snoozeOffset >= 0
                    }

                    Label {
                        id: repeatLabel
                        text: itemController.repeatString
                        visible: itemController.repeat !== 0
                        color: Contemporary.disabled(Contemporary.foreground)
                    }
                }
            }

            Menu {
                id: contextMenu

                MenuSection {
                    text: qsTr("For this alarm")
                }

                MenuItem {
                    text: qsTr("Edit")
                    icon.name: "edit-rename"
                    // onTriggered: itemController.remove()
                }

                MenuItem {
                    text: qsTr("Remove")
                    icon.name: "list-remove"
                    onTriggered: itemController.remove()
                }
            }

            onClicked: contextMenu.popup()
        }

        background: Item {

        }

        swipe.left: SwipeAction {
            id: deleteAction
            text: qsTr("Remove")
            icon.name: "list-remove"
            height: parent.height
            anchors.left: parent.left
            leftSide: true
            destructive: true

            SwipeDelegate.onClicked: itemController.remove()
        }

        swipe.right: SwipeAction {
            id: editAction
            text: qsTr("Edit")
            icon.name: "edit-rename"
            height: parent.height
            anchors.right: parent.right

            SwipeDelegate.onClicked: () => {
                swipeDelegate.swipe.close()
            }
        }
    }
}
