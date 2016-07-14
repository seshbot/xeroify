import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Xeroify")

    property Shopify shopify: Shopify { }
    property Xero xero: Xero { }
    property MakeLeaps makeLeaps: MakeLeaps { }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: shopify.state === Shopify.STATE_LOADING ? 'Abort' : 'Load'
                onClicked: {
                    if ( swipeView.currentItem === orderSheet )
                    {
                        if (shopify.state === Shopify.STATE_LOADING) {
                            shopify.abort()
                        } else {
                            shopify.load()
                        }
                    }
                    else
                    {
                        if (makeLeaps.state === MakeLeaps.STATE_LOADING) {
                            makeLeaps.abort()
                        } else {
                            makeLeaps.load()
                        }
                    }
                }
            }
            Item {
                Layout.fillWidth: true
            }
            ToolButton {
                id: settingsMenuButton
                text: 'Settings'
                onClicked: settingsMenu.open()
                Menu {
                    id: settingsMenu
                    y: settingsMenuButton.height
                    MenuItem {
                        text: 'Shopify'
                        onClicked: settingsPopup.openShopify(shopify.settings)
                    }
                    MenuItem {
                        text: 'MakeLeaps'
                        onClicked: {
                            console.log('makeLeaps: ', makeLeaps)
                            console.log('makeLeaps settings: ', makeLeaps.settings)
                            settingsPopup.openMakeLeaps(makeLeaps.settings)
                        }
                    }
                }
            }
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Page {
            id: orderSheet
            ListView {
                id: orderList
                anchors.fill: parent
                model: shopify.orders
                delegate: ItemDelegate {
                    width: parent.width
                    RowLayout {
                        anchors.fill: parent
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            leftPadding: 10
                            text: modelData.name + ' (' + modelData.orderStatusString + ',' + modelData.financialStatusString + ',' + modelData.fulfillmentStatusString + ')'
                        }
                    }

                    highlighted: ListView.isCurrentItem
                    onClicked: orderList.currentIndex = index
                }
                ScrollIndicator.vertical: ScrollIndicator { }
            }
        }
        Page {
            id: invoiceSheet
            ListView {
                id: invoiceList
                anchors.fill: parent
                model: xero.invoices
                delegate: ItemDelegate {
                    width: parent.width
                    RowLayout {
                        anchors.fill: parent
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            leftPadding: 10
                            text: modelData.number + ' ' + modelData.reference
                        }
                    }

                    highlighted: ListView.isCurrentItem
                    onClicked: orderList.currentIndex = index
                }
                ScrollIndicator.vertical: ScrollIndicator { }
            }
        }
        Page {
            id: makeLeapsPage
            ListView {
                id: makeLeapsList
                anchors.fill: parent
                model: xero.invoices
                delegate: ItemDelegate {
                    width: parent.width
                    RowLayout {
                        anchors.fill: parent
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            leftPadding: 10
                            text: modelData.number + ' ' + modelData.reference
                        }
                    }

                    highlighted: ListView.isCurrentItem
                    onClicked: orderList.currentIndex = index
                }
                ScrollIndicator.vertical: ScrollIndicator { }
            }
        }
    }

    Popup {
        id: settingsPopup
        property SimpleHttpConnectionSettings shopifyApiSettings
        property OAuthZeroLeggedConnectionSettings xeroApiSettings
        property OAuth2Settings makeLeapsApiSettings
        x: (parent.width - width) / 2
        //y: parent.header / 6
        modal: true
        focus: true
        contentItem: RowLayout {
            ColumnLayout {
                spacing: 10
                Label {
                    text: "Settings "
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.bold: true
                    padding: 10
                }
                ShopifyApiSettingsPanel {
                    id: shopifyApiSettingsPanel
                    visible: settingsPopup.shopifyApiSettings != null
                    settings: settingsPopup.shopifyApiSettings
                }
                XeroApiSettingsPanel {
                    id: xeroApiSettingsPanel
                    visible: false
                    settings: settingsPopup.xeroApiSettings
                }
                MakeLeapsApiSettingsPanel {
                    id: makeLeapsApiSettingsPanel
                    visible: settingsPopup.makeLeapsApiSettings != null
                    settings: settingsPopup.makeLeapsApiSettings
                }
                RowLayout {
                    anchors.right: parent.right
                    Button {
                        text: "OK"
                        onClicked: {
                            settingsPopup.save()
                            settingsPopup.close()
                        }
                    }
                    Button {
                        text: "Cancel"
                        onClicked: {
                            settingsPopup.close()
                        }
                    }
                }
            }
        }
        function save() {
            if (shopifyApiSettings) shopifyApiSettingsPanel.save()
            if (xeroApiSettings) xeroApiSettingsPanel.save()
            if (makeLeapsApiSettings) {
                makeLeapsApiSettingsPanel.save()
                makeLeaps.reloadAccessToken()
            }
        }
        function openShopify(settings) {
            shopifyApiSettings = settings
            xeroApiSettings = null
            makeLeapsApiSettings = null
            open()
        }
        function openXero(settings) {
            shopifyApiSettings = null
            xeroApiSettings = settings
            makeLeapsApiSettings = null
            open()
        }
        function openMakeLeaps(settings) {
            shopifyApiSettings = null
            xeroApiSettings = null
            makeLeapsApiSettings = settings
            open()
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("Shopify")
        }
        TabButton {
            text: qsTr("Xero")
        }
        TabButton {
            text: qsTr("MakeLeaps")
        }
    }
}
