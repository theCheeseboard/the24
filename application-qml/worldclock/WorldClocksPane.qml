import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.vicr123.Contemporary
import Contemporary

Item {
    WorldClockController {
        id: controller
    }

    Grandstand {
        id: grandstand
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        innerTopMargin: SafeZone.top
        z: 20

        text: qsTr("World Clocks")
        color: Contemporary.calculateLayer(1)
    }

    ListView {
        id: list
        anchors.top: grandstand.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: SafeZone.bottom
        anchors.topMargin: 6
        spacing: 6

        model: controller

        ScrollBar.vertical: ScrollBar {
            active: true
        }

        delegate: Item {
            id: worldClock
            implicitHeight: childrenRect.height
            // implicitHeight: 300
            implicitWidth: list.width

            required property var timezone
            required property bool removable

            WorldClockItemController {
                id: itemController
                timezone: worldClock.timezone
            }

            SwipeDelegate {
                anchors.centerIn: parent

                implicitHeight: worldClockLayer.height
                implicitWidth: 600

                padding: 0

                contentItem: Layer {
                    id: worldClockLayer

                    height: childrenRect.height + 9 + 6 + 9
                    implicitWidth: 600

                    color: Contemporary.calculateLayer(1)

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
                        anchors.right: parent.right
                        anchors.topMargin: 6
                        anchors.leftMargin: 9
                        anchors.rightMargin: 9
                        text: itemController.informationString
                    }

                    Label {
                        id: dateDifferenceLabel
                        anchors.top: timeLabel.top
                        anchors.left: timeLabel.right
                        anchors.right: parent.right
                        anchors.leftMargin: 6

                        text: itemController.dateDifferenceString
                    }
                }

                background: Item {

                }

                swipe.enabled: worldClock.removable

                swipe.left: Label {
                    id: deleteLabel
                    text: qsTr("Remove")
                    color: "white"
                    verticalAlignment: Label.AlignVCenter
                    padding: 12
                    height: parent.height
                    anchors.left: parent.left

                    SwipeDelegate.onClicked: controller.removeClock(worldClock.timezone)

                    background: Rectangle {
                        color: deleteLabel.SwipeDelegate.pressed ? Qt.darker("tomato", 1.1) : "tomato"
                        radius: 4
                    }
                }
            }
        }

        footer: Item {
            implicitHeight: childrenRect.height + 6
            implicitWidth: list.width
            z: 20

            Button {
                anchors.centerIn: parent
                anchors.bottom: parent.top
                implicitWidth: 600
                text: qsTr("Add Clock")
                icon.name: "list-add"
            }
        }
        footerPositioning: ListView.PullBackFooter

        add: Transition {
            SequentialAnimation {
                PropertyAction {
                    property: "x"
                    value: -list.width
                }

                PauseAnimation {
                    duration: 250
                }

                NumberAnimation {
                    properties: "x"
                    duration: 250
                    easing.type: Easing.OutCubic
                }
            }

        }

        addDisplaced: Transition {
            NumberAnimation {
                properties: "y"
                duration: 250
                easing.type: Easing.InOutCubic
            }
        }

        remove: Transition {
            NumberAnimation {
                properties: "x"
                to: list.width
                duration: 250
                easing.type: Easing.InCubic
            }
        }


        removeDisplaced: Transition {
            SequentialAnimation {
                PauseAnimation {
                    duration: 250
                }

                NumberAnimation {
                    properties: "y"
                    duration: 250
                    easing.type: Easing.InOutCubic
                }
            }
        }
    }
}
