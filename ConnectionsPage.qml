import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    title: qsTr("Network Connections")

    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: networkListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: networkModel
            currentIndex: -1
            delegate: ItemDelegate {
                width: parent.width
                text: model.name
                highlighted: ListView.isCurrentItem

                onClicked: {
                    networkListView.currentIndex = index
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            Button {
                text: qsTr("Add")
                onClicked: {
                    // Logic to add a new connection
                // networkModel.createWiredConnection(test4, enp7s18u5u3c2)
                    // stackView.push("EditEthernetPage.qml", { "connectionUuid": selectedUuid })
                    stackView.push("AddConnectionPage.qml")
                }
            }
            Button {
                text: qsTr("Modify")
                enabled: networkListView.currentIndex != -1
                onClicked: {
                    var selectedUuid = networkListView.model.get(networkListView.currentIndex).uuid;
                    // stackView.push("EditingEthernetPage.qml", { "connectionUuid": selectedUuid })
                    stackView.push("EditIpAddressPage.qml", { "connectionUuid": selectedUuid })
                }
            }
            Button {
                text: qsTr("Delete")
                enabled: networkListView.currentIndex != -1
                onClicked: {
                    networkModel.remove(networkListView.currentIndex)
                }
            }
        }
    }
}
