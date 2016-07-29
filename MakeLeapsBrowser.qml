import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Page {
    id: root
    property MakeLeapsEndpoint endpoint
    property ApiProperty rootProperty: endpoint.rootProperty
    property ApiProperty selectedProperty: endpoint.rootProperty

    Connections {
        target: endpoint
        onRootPropertyChanged: {
            if (endpoint.rootProperty) {
                root.rootProperty = endpoint.rootProperty
                switch (root.rootProperty.type) {
                case ApiProperty.TYPE_OBJECT:
                case ApiProperty.TYPE_RESOURCE:
                    //var obj = endpoint.rootProperty.asObject
                    root.selectedProperty = endpoint.rootProperty // obj.properties.length > 0 ? obj.properties[0] : null
                    break
                case ApiProperty.TYPE_ARRAY:
                    var array = endpoint.rootProperty.asArray
                    root.selectedProperty = array.length > 0 ? array[0] : null
                    break
                case ApiProperty.TYPE_NULL:
                case ApiProperty.TYPE_SCALAR:
                default:
                    root.selectedProperty = endpoint.rootProperty
                }

            }
        }
    }

    onEndpointChanged: {
        console.log(' *** endpoint changed: ', endpoint ? endpoint.url : 'null', endpoint.rootProperty ? 'set' : 'not set')
    }

    onRootPropertyChanged: {
        console.log(' *** root property changed: ', root.rootProperty ? ( 'name=' + root.rootProperty.name ) : 'null')
    }

    onSelectedPropertyChanged: {
        console.log(' *** current property changed: ', root.selectedProperty ? ( 'name=' + root.selectedProperty.name ) : 'null')
    }

    property bool invalid : !endpoint || endpoint.state == MakeLeapsEndpoint.STATE_INVALID
    property bool error : endpoint && endpoint.state == MakeLeapsEndpoint.STATE_ERROR
    property bool loading : endpoint && endpoint.state == MakeLeapsEndpoint.STATE_LOADING
    property bool loaded : endpoint && endpoint.state == MakeLeapsEndpoint.STATE_LOADED

    property int navbarWidth : root.width / 4

    property var history: []
    property var future: []
    property bool backEnabled: history.length > 0
    property bool forwardEnabled: future.length > 0

    function pushHistory(h) {
        history.push(h)
        future = []
        updateNavButtons()
    }

    function pushEndpoint(e) {
        pushHistory({ endpoint: root.endpoint, property: root.rootProperty })
        endpoint = e
        endpoint.load()
    }
    function pushProperty(p) {
        pushHistory({ endpoint: root.endpoint, property: root.rootProperty })
        root.rootProperty = p
    }
    function forward() {
        if (future.length > 0) {
            history.push({ endpoint: root.endpoint, property: root.rootProperty })
            var h = future.pop()
            root.endpoint = h.endpoint || root.endpoint
            root.rootProperty = h.property || root.rootProperty
        }
        updateNavButtons()
    }
    function back() {
        if (history.length > 0) {
            future.push({ endpoint: root.endpoint, property: root.rootProperty })
            var h = history.pop()
            root.endpoint = h.endpoint || root.endpoint
            root.rootProperty = h.property || root.rootProperty
        }
        updateNavButtons()
    }
    function updateNavButtons() {
        backEnabled = history.length > 0
        forwardEnabled = future.length > 0
    }


    Popup {
        id: changeEndpointPopup
        x: ( parent.width - width ) / 2
        ColumnLayout {
            Label {
                text: qsTr('Change Endpoint')
                anchors.horizontalCenter: parent.horizontalCenter
                font.bold: true
                padding: 10
            }
            RowLayout {
                TextField {
                    id: urlField
                    text: endpoint.url
                }
                Button {
                    text: 'Set'
                    enabled: urlField.text
                    onClicked: {
                        changeEndpointPopup.close()
                        endpointUrlSelected(urlField.text)
                    }
                }
            }
        }
    } // popup
    header: Pane {
        width: parent.width
        RowLayout {
            width: parent.width
            Button {
                text: "<"
                enabled: root.backEnabled
                background: Rectangle {
                    implicitWidth: headerLabel.height
                    implicitHeight: headerLabel.height
                }
                onClicked: {
                    back()
                }
            }
            Button {
                text: ">"
                enabled: root.forwardEnabled
                background: Rectangle {
                    implicitWidth: headerLabel.height
                    implicitHeight: headerLabel.height
                }
                onClicked: {
                    forward()
                }
            }
            Label {
                id: headerLabel
                Layout.fillWidth: true
                padding: 6
                horizontalAlignment: Text.AlignHCenter
                color: 'gray'
                text: {
                    if (invalid) return '';
                    if (error) return qsTr('Error: ') + endpoint.lastErrorMessage;
                    if (loading) return qsTr('loading');
                    var linkName = endpoint.name ? endpoint.name : endpoint.url;
                    return '<a href="' + endpoint.url + '">' + linkName + '</a>';
                }
                onLinkActivated: {
                    changeEndpointPopup.open()
                }
            }
            Label {
                text: {
                    if (!root.selectedProperty) return ''
                    var countString = ''
                    switch (root.selectedProperty.type) {
                    case ApiProperty.TYPE_ARRAY: countString = '[' + root.selectedProperty.asArray.length + ']'; break;
                    }
                    return root.selectedProperty.typeString + countString
                }
            }
        }
    } // header
    Loader {
        anchors.fill: parent
        Component {
            id: loadedPage
            RowLayout {
                anchors.fill: parent
                ApiNavBar {
                    id: navBar
                    currentProperty: root.rootProperty
                    selectedProperty: root.selectedProperty
                    Layout.minimumWidth: navbarWidth
                    Layout.fillHeight: true

                    onEndpointItemClicked: {
                        pushEndpoint(endpoint)
                    }
                    onArrayItemClicked: {
                        pushProperty(item)
                    }
                    onObjectItemClicked: {
                        root.selectedProperty = item
                    }
                    onResourceItemClicked: {
                        root.selectedProperty = item
                    }
                }
                ApiPropertyPage {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    currentProperty: root.selectedProperty

                    onEndpointItemClicked: {
                        pushEndpoint(endpoint)
                    }
                }
            }
        } // loadedPage
        Component {
            id: loadingPage
            Item {
                Label {
                    anchors.centerIn: parent
                    text: 'loading page'
                }
            }
        } // loadingPage
        sourceComponent: endpoint.rootProperty ? loadedPage : loadingPage
    }
}
