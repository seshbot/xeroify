import QtQuick 2.7
import QtQuick.Controls 1.4 as OldControls
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Loader {
    id: root

    property OrderBook orderBook
    property Order selectedOrder
    property string message

    Component {
        id: loadedComponent

        RowLayout {
            id: orderBookRow
            property var checkedOrders: []
            function orderChecked(order) {
                var idx = checkedOrders.indexOf(order)
                if (idx < 0) {
                    checkedOrders.push(order)
                }
            }
            function orderUnchecked(order) {
                var idx = checkedOrders.indexOf(order)
                if (idx >= 0) {
                    checkedOrders.splice(idx, 1)
                }
            }

            Page {
                id: nav
                Layout.preferredWidth: 350
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent

                    ListView {
                        id: orderListView
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        property string filterText: ''

                        function getFilteredOrders(needle) {
                            if (!needle) return orderBook.orders;
                            return orderBook.orders.filter(function(e) {
                                if (e.name.toLowerCase().indexOf(needle) >= 0) return true
                                if (e.customer && e.customer.defaultAddress) {
                                    var address = e.customer.defaultAddress
                                    if (address.name.toLowerCase().indexOf(needle) >= 0) return true
                                }
                                if (e.name.toLowerCase().indexOf(needle) >= 0) return true
                                return false
                            })
                        }

                        function updateFilterText(text) {
                            filterText = text
                            orderListView.model = getFilteredOrders(text)
                        }

                        Keys.onPressed: {
                            if (event.key === Qt.Key_Escape) {
                                updateFilterText('')
                            }
                        }

                        header: RowLayout {
                            width: orderListView.width
                            ComboBox {
                                textRole: 'name'
                                currentIndex: orderBook.filter
                                model: ListModel {
                                    ListElement { name: qsTr('All Orders') }
                                    ListElement { name: qsTr('Unfulfilled') }
                                    ListElement { name: qsTr('Payment Pending') }
                                }
                                onActivated: {
                                    switch(index) {
                                    case 0: orderBook.filter = OrderBook.FILTER_ALL; break;
                                    case 1: orderBook.filter = OrderBook.FILTER_UNFULFILLED; break;
                                    case 2: orderBook.filter = OrderBook.FILTER_PAYMENT_PENDING; break;
                                    }
                                }
                            }

                            TextField {
                                id: filterTextField
                                text: orderListView.filterText
                                Layout.fillWidth: true
                                placeholderText: qsTr('Search In Page')
                                onAccepted: orderListView.updateFilterText(text)
                            }
                        }

                        width: parent.width
                        clip: true
                        model: orderBook.orders
                        delegate: CheckDelegate {
                            width: parent.width
                            padding: 6
                            highlighted: selectedOrder === modelData
                            OrderDelegateContentItem {
                                width: parent.width - 50
                                order: modelData
                            }
                            onClicked: {
                                selectedOrder = modelData
                            }
                            onCheckStateChanged: {
                                switch (checkState) {
                                case Qt.Unchecked: orderBookRow.orderUnchecked(modelData); break
                                default: orderBookRow.orderChecked(modelData); break
                                }
                            }
                        }
                        ScrollIndicator.vertical: ScrollIndicator { }
                    }
                }
            } // nav bar

            // details page
            Page {
                Layout.fillHeight: true
                Layout.fillWidth: true
                header: Pane {
                    padding: 0
                    Button {
                        anchors.right: parent.right
                        text: qsTr('Create Invoices')
                        onClicked: {
                            createInvoicesWizard.orders = orderBookRow.checkedOrders
                            createInvoicesWizard.open()
                        }
                    }
                }

                Loader {
                    anchors.fill: parent
                    Component {
                        id: orderSelectedComponent
                        OrderDetails {
                            anchors.fill: parent
                            order: selectedOrder
                        }
                    }
                    Component {
                        id: noOrderSelectedComponent
                        Label {
                            text: ''
                        }
                    }

                    sourceComponent: selectedOrder ? orderSelectedComponent : noOrderSelectedComponent
                }
            } // details page
        }
    }

    Popup {
        id: createInvoicesWizard
        width: ( root.width / 3 ) * 2
        height: ( root.height / 5 ) * 4
        x: ( root.width - width ) / 2
        y: ( root.height - height ) / 2

        modal: true

        property var orders

        Column {
            id: wizardContent
            anchors.fill: parent

            property Order selectedOrder

            ListView {
                id: ordersListView
                anchors.fill: parent
                model: createInvoicesWizard.orders
                delegate: ItemDelegate {
                    width: parent.width
                    padding: 6
                    highlighted: wizardContent.selectedOrder === modelData
                    OrderDelegateContentItem {
                        width: parent.width - 50
                        order: modelData
                    }
                    onClicked: {
                        wizardContent.selectedOrder = modelData
                    }
                }
            }
        }
    }

    Component {
        id: messageComponent
        Page {
            Pane {
                anchors.centerIn: parent
                Text {
                    wrapMode: Text.Wrap
                    text: message
                }
            }
        }
    }

    sourceComponent: {
        if (orderBook && orderBook.orders.length > 0) return loadedComponent
        if (!orderBook) {
            message = 'you shouldnt see this'
        }
        else {
            switch (orderBook.state) {
            case OrderBook.STATE_LOADING: message = 'Loading...'; break
            case OrderBook.STATE_ERROR: message = 'Error: ' + orderBook.errorMessage; break
            default:
                 message = 'TODO:' + orderBook.state
            }
        }
        return messageComponent
    }
}
