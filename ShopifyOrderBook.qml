import QtQuick 2.7
import QtQuick.Controls 1.4 as OldControls
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Loader {
    //anchors.fill: parent
    id: root
    property OrderBook orderBook
    property Order selectedOrder
    property string message

    Component {
        id: loadedComponent

        RowLayout {
            Page {
                id: nav
                Layout.preferredWidth: 350
                Layout.fillHeight: true

//                property bool isFilterShowing: false

//                property date startDate: new Date()
//                property date endDate: new Date()


                ColumnLayout {
                    anchors.fill: parent
//                    Pane {
//                        id: navHeader
//                        Layout.fillWidth: true
//                        padding: 0
//                        Button {
//                            anchors.right: parent.right
//                            text: orderBook.state === OrderBook.STATE_LOADING ? qsTr('Loading') : qsTr('Filter')
//                            onClicked: nav.isFilterShowing = !nav.isFilterShowing
//                        }
//                    }
//                    Pane {
//                        id: navFilter
//                        Layout.fillWidth: true

//                        visible: nav.isFilterShowing

//                        GridLayout {
//                            anchors.fill: parent
//                            columns: 3
//                            CheckBox {
//                                text: qsTr('Unshipped')
//                                checked: root.orderBook.showUnshipped
//                                onClicked: {
//                                    root.orderBook.showUnshipped = checked
//                                    checked = Qt.binding(function () {
//                                        return root.orderBook.showUnshipped
//                                    })
//                                }
//                            }
//                            CheckBox {
//                                text: qsTr('Partial')
//                                checked: root.orderBook.showPartial
//                                onClicked: {
//                                    root.orderBook.showPartial = checked
//                                    checked = Qt.binding(function () {
//                                        return root.orderBook.showPartial
//                                    })
//                                }
//                            }
//                            CheckBox {
//                                text: qsTr('Shipped')
//                                checked: root.orderBook.showShipped
//                                onClicked: {
//                                    root.orderBook.showShipped = checked
//                                    checked = Qt.binding(function () {
//                                        return root.orderBook.showShipped
//                                    })
//                                }
//                            }
//                        }
//                    }

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
                            GridLayout {
                                width: parent.width - 50
                                rowSpacing: 6
                                columns: 2
                                Text {
                                    Layout.leftMargin: 10
                                    text: modelData.name }
                                Row {
                                    Layout.alignment: Qt.AlignRight
                                    Text {
                                        text: modelData.currency
                                        color: 'darkgray'
                                    }
                                    Text {
                                        text: modelData.totalPrice
                                    }
                                }

                                Text {
                                    Layout.leftMargin: 10
                                    text: modelData.customer.defaultAddress.name; color: 'darkgray'
                                }
                                Row {
                                    Layout.alignment: Qt.AlignRight
                                    Text {
                                        text: modelData.financialStatusString
                                        color: modelData.financialStatus === Order.FINANCIAL_STATUS_PAID ? 'green' : 'blue'
                                    }
                                    Text { text: ' \u00B7 '; color: 'darkgray' }
                                    Text {
                                        text: modelData.fulfillmentStatusString
                                        color: modelData.fulfillmentStatus === Order.FULFILLMENT_STATUS_SHIPPED ? 'green' : 'blue'
                                    }
                                }
                            }
                            onClicked: {
                                console.log('clicked on ', modelData.name)
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
