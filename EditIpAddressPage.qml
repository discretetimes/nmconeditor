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
        if (connectionDetails.ipv4Method === 0) {
            methodComboBox.currentIndex = 0
        } else {
            methodComboBox.currentIndex = 1
        }
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
            // currentIndex: -1
            enabled: methodComboBox.currentIndex === 1

            // --- FIX START: Replace TextField delegate with ItemDelegate ---
            delegate: ItemDelegate {
                width: parent.width
                // Visually indicate when the item is selected
                highlighted: ListView.isCurrentItem

                TextField {
                    id: addressField
                    // Anchor the text field within the delegate
                    anchors.fill: parent
                    text: model.address
                    placeholderText: parent.enabled ? "e.g., 192.168.1.100/24" : "Not applicable"
                    // Make the text field transparent so the highlight shows through
                    background: null
                    onEditingFinished: {
                        model.address = text
                    }
                }

                onClicked: {
                    // This is the crucial part that was missing.
                    // It tells the ListView which item is now current.
                    addressListView.currentIndex = index
                }
            }
            // --- FIX END ---
        }

        RowLayout {
            enabled: methodComboBox.currentIndex === 1
            Button {
                text: qsTr("Add Address")
                onClicked: {
                    addressModel.append({ "address": "" })
                }
            }
            Button {
                text: qsTr("Remove Selected")
                // This enabled condition will now work correctly.
                // enabled: addressListView.currentIndex !== -1
                onClicked: {
                    // if (addressListView.currentIndex !== -1) {
                        addressModel.remove(addressListView.currentIndex)
                    // }
                }
            }
        }
    }

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Save | DialogButtonBox.Cancel
        onAccepted: {
            var newAddresses = []
            if(methodComboBox.currentIndex === 1) {
                for (var i = 0; i < addressModel.count; i++) {
                    var item = addressModel.get(i)
                    if (item.address !== "") {
                        newAddresses.push(item.address)
                    }
                }
            }
            var newMethod;
            if (methodComboBox.currentIndex === 0) {
                newMethod = 0;
            } else {
                newMethod = 2;
            }
            var newSettings = {
                "ipv4Method": newMethod,
                "addresses": newAddresses
            }
            networkModel.updateConnection(connectionUuid, newSettings)
            stackView.pop()
        }
        onRejected: {
            stackView.pop()
        }
    }
}
