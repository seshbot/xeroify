import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import BusyBot 1.0

GridLayout {
    property Order order

    rowSpacing: 6
    columns: 2
    Text {
        Layout.leftMargin: 10
        text: order.name }
    Row {
        Layout.alignment: Qt.AlignRight
        Text {
            text: order.currency
            color: 'darkgray'
        }
        Text {
            text: order.totalPrice
        }
    }

    Text {
        Layout.leftMargin: 10
        text: order.customer.defaultAddress.name; color: 'darkgray'
    }
    Row {
        Layout.alignment: Qt.AlignRight
        Text {
            text: order.financialStatusString
            color: order.financialStatus === Order.FINANCIAL_STATUS_PAID ? 'green' : 'blue'
        }
        Text { text: ' \u00B7 '; color: 'darkgray' }
        Text {
            text: order.fulfillmentStatusString
            color: order.fulfillmentStatus === Order.FULFILLMENT_STATUS_SHIPPED ? 'green' : 'blue'
        }
    }
}
