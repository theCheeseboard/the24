import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.vicr123.Contemporary
import Contemporary

Item {
    id: root

    Grandstand {
        id: grandstand
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        innerTopMargin: SafeZone.top
        z: 20

        text: qsTr("Stopwatches")
        color: Contemporary.calculateLayer(1).value
    }

    StopwatchModel {
        id: model
    }

    Pager {
        anchors.top: grandstand.bottom
        anchors.topMargin: 6
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: SafeZone.bottom

        currentAnimation: Pager.Fade
        currentIndex: model.serviceAvailable ? 1 : 0

        Interstitial {
            anchors.fill: parent
            icon.name: "clock-stopwatch"
            text: qsTr("Hmm...")
            subtitle: qsTr("Looks like the the24 service is not running, and stopwatches won't work without it.")
            visible: !model.serviceAvailable
        }

        ListView {
            id: list
            spacing: 6

            model: model

            ScrollBar.vertical: ScrollBar {
                active: true
            }

            delegate: StopwatchDisplay {
                implicitWidth: list.width
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
                    text: qsTr("Start Stopwatch")
                    icon.name: "list-add"

                    onClicked: model.newStopwatch()
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
}
