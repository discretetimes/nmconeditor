import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: networkConnectionsRoot
    title: qsTr("Network Connections")

    // Signal to open the editing page for a new connection
    signal addConnection()
    // Signal to open the editing page for an existing connection
    // Pass the connection data as an argument
    signal modifyConnection(variant connectionData)


    // Placeholder for the list of Ethernet connections.
    // In a real application, this would be a ListModel populated from your backend.
    ListModel {
        id: ethernetConnectionsModel
        ListElement { name: "Connection 1" }
        ListElement { name: "Connection 2" }
        ListElement { name: "Office Network" }
    }

    contentData: ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Label {
            text: qsTr("EtherNet")
            font.bold: true
            Layout.bottomMargin: 5
        }

        ListView {
            id: connectionsListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: ethernetConnectionsModel
            currentIndex: -1 // No item selected initially

            delegate: ItemDelegate {
                width: parent.width
                text: model.name
                highlighted: ListView.isCurrentItem

                onClicked: {
                    connectionsListView.currentIndex = index
                    // In a real app, you'd fetch the full data for 'selectedConnectionData' from your model
                    // For now, just using the name as an example.
                    // networkConnectionsRoot.parent.parent.selectedConnectionData = { "name": model.name, "device": "eth0", "method": "Auto", "staticIPs": [] } // Example data
                    // A better way: expose a function in Main.qml to set this, or have EditingEthernetPage fetch it.
                    // For simplicity here, we'll assume the modify signal carries enough info or EditingEthernetPage handles fetching.
                    console.log("Selected:", model.name)
                }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 10

            Button {
                text: qsTr("Add")
                onClicked: networkConnectionsRoot.addConnection()
            }

            Button {
                id: deleteButton
                text: qsTr("Delete")
                enabled: connectionsListView.currentIndex !== -1
                onClicked: {
                    if (connectionsListView.currentIndex !== -1) {
                        // Implement delete logic here
                        console.log("Deleting:", ethernetConnectionsModel.get(connectionsListView.currentIndex).name)
                        ethernetConnectionsModel.remove(connectionsListView.currentIndex)
                        connectionsListView.currentIndex = -1 // Reset selection
                    }
                }
            }

            Button {
                id: modifyButton
                text: qsTr("Modify")
                enabled: connectionsListView.currentIndex !== -1
                onClicked: {
                    if (connectionsListView.currentIndex !== -1) {
                        // Pass the data of the selected connection
                        // In a real app, this would be a more complex object
                        const selectedData = {
                            name: ethernetConnectionsModel.get(connectionsListView.currentIndex).name,
                            // Example placeholder data, fetch real data from your model
                            device: "eth0", // Placeholder
                            method: "Auto", // Placeholder
                            staticIPs: ["192.168.1.100/24"] // Placeholder
                        };
                        networkConnectionsRoot.modifyConnection(selectedData)
                    }
                }
            }
        }
    }
}
