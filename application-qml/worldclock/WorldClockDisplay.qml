import QtQuick
import QtQuick.Controls
import com.vicr123.Contemporary
import Contemporary

Item {
    id: worldClock
    implicitHeight: childrenRect.height

    required property var timezone
    required property bool removable

    signal remove(timezone: string)

    WorldClockItemController {
        id: itemController
        timezone: worldClock.timezone
    }

    LayerCalculator {
        id: layer1
        layer: 1
    }

    SwipeDelegate {
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

                color: layer1.color

                Label {
                    id: timeLabel
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 9
                    anchors.leftMargin: 9
                    text: itemController.currentDateTimeString

                    font.pointSize: 30
                }

                Label {
                    anchors.top: timeLabel.bottom
                    anchors.left: parent.left
                    anchors.right: clock.left
                    anchors.topMargin: 6
                    anchors.leftMargin: 9
                    anchors.rightMargin: 9
                    text: itemController.informationString
                }

                Label {
                    id: dateDifferenceLabel
                    anchors.top: timeLabel.top
                    anchors.left: timeLabel.right
                    anchors.right: clock.left
                    anchors.leftMargin: 6

                    text: itemController.dateDifferenceString
                }

                AnalogueClock {
                    id: clock
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.topMargin: 9
                    anchors.bottomMargin: 9
                    anchors.leftMargin: 9
                    anchors.rightMargin: 9
                    width: clock.height

                    time: itemController.currentTime
                    backgroundColor: layer1.color
                    accentColor: Contemporary.accent
                }
            }

            Menu {
                id: worldClockContextMenu

                MenuSection {
                    text: qsTr("For this clock")
                }

                MenuItem {
                    text: qsTr("Remove")
                    icon.name: "list-remove"
                    onTriggered: worldClock.remove(worldClock.timezone)
                }
            }

            onClicked: worldClock.removable && worldClockContextMenu.popup()
        }

        background: Item {

        }

        swipe.enabled: worldClock.removable

        swipe.left: SwipeAction {
            id: deleteLabel
            text: qsTr("Remove")
            icon.name: "list-remove"
            height: parent.height
            anchors.left: parent.left
            leftSide: true
            destructive: true

            SwipeDelegate.onClicked: worldClock.remove(worldClock.timezone)
        }
    }
}
