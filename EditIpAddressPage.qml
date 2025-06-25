import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    property string connectionUuid
    property var connectionDetails: ({})

    title: "Edit Addresses for " + (connectionDetails.name || "")

    ListModel {
        id: addressModel
    }

    Component.onCompleted: {
        connectionDetails = networkModel.getConnectionDetails(connectionUuid)
        // Set the initial index of the ComboBox
        methodComboBox.currentIndex = connectionDetails.ipv4Method === 1 ? 0 : 1;
        for (var i = 0; i < connectionDetails.addresses.length; ++i) {
            addressModel.append({ "address": connectionDetails.addresses[i] })
        }
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent

        Label { text: "IPv4 Method:" }
        ComboBox {
            id: methodComboBox
            Layout.fillWidth: true
            model: ["Automatic (DHCP)", "Manual"]
        }


        ListView {
            id: addressListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: addressModel
            currentIndex: -1
            // Enable the list only when the method is 'Manual'
            enabled: methodComboBox.currentIndex === 1

            delegate: TextField {
                width: parent.width
                text: model.address
                placeholderText: enabled ? "e.g., 192.168.1.100/24" : "Not applicable"
                readOnly: !enabled
                onEditingFinished: {
                    model.address = text
                }
            }
        }

        RowLayout {
            // Disable add/remove buttons if not in Manual mode
            enabled: methodComboBox.currentIndex === 1
            Button {
                text: qsTr("Add Address")
                onClicked: {
                    addressModel.append({ "address": "" })
                }
            }
            Button {
                text: qsTr("Remove Selected")
                enabled: addressListView.currentIndex !== -1
                onClicked: {
                    if (addressListView.currentIndex !== -1) {
                        addressModel.remove(addressListView.currentIndex)
                    }
                }
            }
        }
    }

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Save | DialogButtonBox.Cancel

        onAccepted: { // Save button clicked
            var newAddresses = []
            if(methodComboBox.currentIndex === 1) { // Only save addresses if method is Manual
                for (var i = 0; i < addressModel.count; i++) {
                    var item = addressModel.get(i)
                    if (item.address !== "") {
                        newAddresses.push(item.address)
                    }
                }
            }

            var newSettings = {
                // Get the new method from the ComboBox (1 for Auto, 2 for Manual)
                "ipv4Method": methodComboBox.currentIndex === 0 ? 1 : 2,
                "addresses": newAddresses
            }

            networkModel.updateConnection(connectionUuid, newSettings)
            stackView.pop()
        }
        onRejected: { // Cancel button clicked
            stackView.pop()
        }
    }
}
