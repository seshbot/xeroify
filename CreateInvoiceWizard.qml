import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import BusyBot 1.0

Page {
    id: root
    property Order order

    signal backButtonPressed()

    property var checkedLineItems: []
    function lineItemChecked(lineItem) {
        var idx = checkedLineItems.indexOf(lineItem)
        if (idx < 0) {
            checkedLineItems.push(lineItem)
        }
        handleCheckedLineItemsUpdated()
    }
    function lineItemUnchecked(lineItem) {
        var idx = checkedLineItems.indexOf(lineItem)
        if (idx >= 0) {
            checkedLineItems.splice(idx, 1)
        }
        handleCheckedLineItemsUpdated()
    }
    function handleCheckedLineItemsUpdated() {
        createButton.enabled = checkedLineItems.length > 0
    }

    header: RowLayout {
        Button {
            text: qsTr('Back')
            onClicked: root.backButtonPressed()
        }
        Button {
            id: createButton
            Layout.alignment: Qt.AlignRight
            enabled: root.checkedLineItems.length > 0
            text: qsTr('Create')
        }
    }

    ColumnLayout {
        anchors.fill: parent

        GroupBox {
            Layout.fillWidth: true
            title: qsTr('Shopify Order')
            OrderSummary {
                width: parent.width
                order: root.order
            }
        }

        GroupBox {
            Layout.fillWidth: true
            title: qsTr('Customer')
            RowLayout {
                width: parent.width
                Label {
                    Layout.fillWidth: true
                    text: root.order.customer.email
                }
                Label {
                    color: 'red'
                    text: qsTr('Not Found')
                }
                Button {
                    Layout.alignment: Qt.AlignRight
                    text: qsTr('Create')
                }
            }
        }

        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            title: qsTr('Line Items')

            ListView {
                id: ordersListView
                anchors.fill: parent

                property var selectedLineItem

                model: root.order.lineItems
                delegate: CheckDelegate {
                    width: parent.width
                    height: 60
                    padding: 6
                    highlighted: ordersListView.selectedLineItem === modelData
                    LineItemSummary {
                        width: parent.width - 50
                        lineItem: modelData
                    }
                    onClicked: {
                        ordersListView.selectedLineItem = modelData
                    }
                    onCheckStateChanged: {
                        switch (checkState) {
                        case Qt.Unchecked: root.lineItemUnchecked(modelData); break
                        default: root.lineItemChecked(modelData); break
                        }
                    }
                }
            }
        }
    }
}
