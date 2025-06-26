import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    title: qsTr("Network Connections")

    // // --- Add this block to handle the signal from the C++ model ---
    // Connections {
    //     target: networkModel
    //     // This function is automatically called when the connectionsChanged() signal is emitted
    //     function onConnectionsChanged() {
    //         // A simple and effective way to force the ListView to update
    //         // is to briefly detach and re-attach its model.
    //         var m = networkListView.model;
    //         networkListView.model = null;
    //         networkListView.model = m;
    //     }
    // }

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
                text: qsTr("Refresh")
                onClicked: {
                    // This calls the existing C++ refresh() function,
                    // which resets the model and updates the view.
                    networkModel.refresh()
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
