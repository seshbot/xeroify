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

    signal createInvoiceClicked(Order order)

    Component {
        id: loadedComponent

        RowLayout {
            id: orderBookRow

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
                        delegate: ItemDelegate {
                            width: parent.width
                            height: 60
                            padding: 6
                            highlighted: selectedOrder === modelData
                            OrderSummary {
                                anchors.verticalCenter: parent.verticalCenter
                                width: parent.width - 12
                                order: modelData
                            }
                            onClicked: {
                                selectedOrder = modelData
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
                            createInvoiceClicked(selectedOrder)
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
