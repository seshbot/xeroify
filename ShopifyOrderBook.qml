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

    Popup {
        id: datePicker
        x: (root.width - width) / 2
        y: (root.height - height) / 2
        modal: true

        property string title
        property date selectedDate
        property var selectionHandler

        ColumnLayout {
            Label {
                Layout.fillWidth: true
                horizontalAlignment: Label.AlignHCenter
                font.bold: true
                text: datePicker.title
            }

            OldControls.Calendar {
                id: calendar
                selectedDate: datePicker.selectedDate
                maximumDate: new Date()
                onClicked: {
                    datePicker.selectionHandler( selectedDate )
                    datePicker.close()
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Button {
                    text: qsTr('Today')
                    onClicked: {
                        calendar.selectedDate = new Date()
                    }
                }
                Button {
                    text: qsTr('Cancel')
                    onClicked: {
                        datePicker.close()
                    }
                }
            }
        }
    }

    Component {
        id: loadedComponent

        RowLayout {
            Page {
                id: nav
                Layout.preferredWidth: 350
                Layout.fillHeight: true

                property bool isFilterShowing: false

                property string customerFilter
                property date startDate: new Date()
                property date endDate: new Date()


                ColumnLayout {
                    anchors.fill: parent
                    Pane {
                        id: navHeader
                        Layout.fillWidth: true
                        padding: 0
                        Button {
                            anchors.right: parent.right
                            text: qsTr('Filter')
                            onClicked: nav.isFilterShowing = !nav.isFilterShowing
                        }
                    }
                    Pane {
                        id: navFilter
                        Layout.fillWidth: true

                        visible: nav.isFilterShowing

                        function openStartDateDialog() {
                            datePicker.title = qsTr('Start Date')
                            datePicker.selectedDate = nav.startDate
                            datePicker.selectionHandler = function(d) { nav.startDate = root.orderBook.lastModifiedStart = d }
                            datePicker.open()
                        }

                        function openEndDateDialog() {
                            datePicker.title = qsTr('End Date')
                            datePicker.selectedDate = nav.endDate
                            datePicker.selectionHandler = function(d) { nav.endDate = root.orderBook.lastModifiedEnd = d }
                            datePicker.open()
                        }

                        GridLayout {
                            anchors.fill: parent
                            columns: 3

                            CheckBox {
                                id: startDateCheckBox
                                text: qsTr('Start Date')
                                checked: root.orderBook.filterByLastModifiedStart
                                onCheckedChanged: root.orderBook.filterByLastModifiedStart = checked
                            }
                            Label {
                                enabled: startDateCheckBox.checked
                                text: '<a href="xxx">' + nav.startDate.toDateString() + '</a>'
                                onLinkActivated: navFilter.openStartDateDialog()
                            }
                            Button {
                                text: qsTr('Change')
                                onClicked: navFilter.openStartDateDialog()
                            }

                            CheckBox {
                                id: endDateCheckBox
                                text: qsTr('End Date')
                                checked: root.orderBook.filterByLastModifiedEnd
                                onCheckedChanged: root.orderBook.filterByLastModifiedEnd = checked
                            }
                            Label {
                                text: '<a href="xxx">' + nav.endDate.toDateString() + '</a>'
                                onLinkActivated: navFilter.openEndDateDialog()
                            }
                            Button {
                                text: qsTr('Change')
                                onClicked: navFilter.openEndDateDialog()
                            }
                        }
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        width: parent.width
                        clip: true
                        model: orderBook.orders
                        delegate: CheckDelegate {
                            width: parent.width
                            highlighted: selectedOrder === modelData
                            GridLayout {
                                width: parent.width - 50
                                columns: 2
                                Text { text: modelData.name }
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

                                Text { text: modelData.customer.defaultAddress.name; color: 'darkgray' }
                                Row {
                                    Layout.alignment: Qt.AlignRight
                                    Text {
                                        text: modelData.financialStatusString
                                        color: modelData.financialStatus === Order.FINANCIAL_STATUS_PAID ? 'green' : 'blue'
                                    }
                                    Text { text: ' \u00B7 '; color: 'darkgray' }
                                    Text {
                                        text: modelData.fulfillmentStatusString
                                        color: modelData.fulfillmentStatus === Order.FULFILLMENT_STATUS_FULFILLED ? 'green' : 'blue'
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

                Flickable {
                    anchors.fill: parent
                    contentWidth: jsonText.width
                    contentHeight: jsonText.height
                    clip: true
                    flickableDirection: Flickable.VerticalFlick
                    TextEdit {
                        id: jsonText
                        readOnly: true
                        text: selectedOrder ? selectedOrder.json : ''
                    }
                    ScrollIndicator.vertical: ScrollIndicator { }
                }
            } // details page
        }
    }

    Component {
        id: messageComponent
        Label {
            anchors.centerIn: root
            text: message
        }
    }

    sourceComponent: {
        if (orderBook && orderBook.state === OrderBook.STATE_LOADED) return loadedComponent
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
