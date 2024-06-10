import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import com.vicr123.Contemporary
import Contemporary

Item {
    id: root
    implicitHeight: childrenRect.height

    required property string objectPath

    signal remove(timezone: string)

    StopwatchPaneController {
        id: itemController
        objectPath: root.objectPath
    }

    LayerCalculator {
        id: layer1
        layer: 1
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

                height: childrenRect.height + 9
                implicitWidth: 600

                color: layer1.color

                Button {
                    id: pauseButton
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 9

                    width: pauseButton.height
                    icon.name: itemController.paused ? "media-playback-start" : "media-playback-pause";
                    icon.width: 32
                    icon.height: 32

                    onClicked: itemController.paused = !itemController.paused
                }

                Label {
                    anchors.top: parent.top
                    anchors.left: pauseButton.right
                    anchors.topMargin: 9
                    anchors.leftMargin: 6
                    text: itemController.displayString
                    font.pointSize: 30
                    color: itemController.paused ? Contemporary.disabled(Contemporary.foreground) : Contemporary.foreground
                }

                Button {
                    id: resetButton
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 9
                    icon.name: "view-refresh"
                    flat: true
                    visible: itemController.paused
                    width: resetButton.height

                    onClicked: itemController.reset()
                }
            }

            Menu {
                id: contextMenu

                MenuSection {
                    text: qsTr("For this stopwatch")
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
    }
}
