import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.vicr123.Contemporary
import Contemporary

Item {
    Grandstand {
        id: grandstand
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        innerTopMargin: SafeZone.top
        z: 20

        text: qsTr("Alarms")
        color: Contemporary.calculateLayer(1)
    }

    AlarmModel {
        id: model
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

        model: model

        ScrollBar.vertical: ScrollBar {
            active: true
        }

        delegate: AlarmDisplay {
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
                text: qsTr("Add Alarm")
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
