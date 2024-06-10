import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.vicr123.Contemporary
import Contemporary

Item {
    WorldClockController {
        id: controller
    }

    LayerCalculator {
        id: layer1
        layer: 1
    }

    Grandstand {
        id: grandstand
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        innerTopMargin: SafeZone.top
        z: 20

        text: qsTr("World Clock")
        color: layer1.color
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

        delegate: WorldClockDisplay {
            implicitWidth: list.width
            onRemove: (timezone) => controller.removeClock(timezone)
        }

        footer: Item {
            implicitHeight: childrenRect.height + 6
            implicitWidth: list.width
            z: 20

            Button {
                id: addClockButton
                anchors.centerIn: parent
                anchors.bottom: parent.top
                implicitWidth: 600
                text: qsTr("Add Clock")
                icon.name: "list-add"

                AddClockPopup {
                    id: addClockPopup
                    x: Math.round((parent.width - width) / 2)
                    y: -height
                    width: 400
                    height: 400

                    onAdd: (timezone) => controller.addClock(timezone)
                }

                onClicked: addClockPopup.open()
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
