import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Page {
    id: root
    property MakeLeaps api
    property MakeLeapsEndpoint endpoint: api.apiRoot
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

    function deleteResource(resource) {
        console.log('deleting resource ', resource.url)
        var resourceEndpoint = api.createEndpoint(resource.url)
        updateResourcePopup.endpoint = resourceEndpoint
        updateResourcePopup.open()
        resourceEndpoint.deleteResource()
    }

    function editResource(resource) {
        console.log('editing ', prop)
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
        endpoint.getResource()
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
    Popup {
        id: updateResourcePopup
        x: ( parent.width - width ) / 2
        modal: true
        closePolicy: Popup.CloseOnEscape

        property MakeLeapsEndpoint endpoint

        Connections {
            target: updateResourcePopup.endpoint
            onStateChanged: {
                if ( updateResourcePopup.endpoint.state === MakeLeapsEndpoint.STATE_LOADED ) {
                    root.endpoint.load()
                    updateResourcePopup.close()
                }
                if ( updateResourcePopup.endpoint.state === MakeLeapsEndpoint.STATE_ABORTING ) {
                    updateResourcePopup.close()
                }
            }
        }

        ColumnLayout {
            anchors.fill: parent
            Loader {
                anchors.fill: parent
                Component {
                    id: noEndpoint
                    Label {
                        anchors.centerIn: parent
                        text: 'no endpoint'
                    }
                }
                Component {
                    id: loadingEndpoint
                    Label {
                        anchors.centerIn: parent
                        text: qsTr('Loading...')
                    }
                }
                Component {
                    id: errorEndpoint
                    Text {
                        anchors.centerIn: parent
                        text: updateResourcePopup.endpoint.lastErrorMessage || 'Error'
                    }
                }
                Component {
                    id: todoEndpoint
                    Label {
                        anchors.centerIn: parent
                        text: 'todo: ' + ( updateResourcePopup ? updateResourcePopup.endpoint.stateString : 'null' )
                    }
                }
                sourceComponent: {
                    if ( !updateResourcePopup.endpoint ) return noEndpoint
                    switch ( updateResourcePopup.endpoint.state ) {
                    case MakeLeapsEndpoint.STATE_INVALID: return todoEndpoint
                    case MakeLeapsEndpoint.STATE_LOADING: return loadingEndpoint
                    case MakeLeapsEndpoint.STATE_ABORTING: return todoEndpoint
                    case MakeLeapsEndpoint.STATE_LOADED: return todoEndpoint
                    case MakeLeapsEndpoint.STATE_NEEDS_AUTHENTICATION: return errorEndpoint
                    case MakeLeapsEndpoint.STATE_ERROR: return errorEndpoint
                    default: return todoEndpoint
                    }
                }
            } // loader
            RowLayout {
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    text: 'Abort'
                    enabled: updateResourcePopup.endpoint && updateResourcePopup.endpoint.state === MakeLeapsEndpoint.STATE_LOADING
                    onClicked: updateResourcePopup.endpoint.abort()
                }
                Button {
                    text: 'Close'
                    onClicked: updateResourcePopup.close()
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
                    var linkUrl = endpoint.url;
                    var linkName = endpoint.name ? endpoint.name : endpoint.url;
                    if (propertyIsResource(root.selectedProperty) || propertyIsResource(root.rootProperty)) {
                        var prop = propertyIsResource(root.selectedProperty)
                                ? root.selectedProperty
                                : root.rootProperty
                        var resource = prop.asObject
                        linkUrl = resource.url
                        linkName = resource.name
                        if (!linkName || linkName === '-') {
                            linkName = linkUrl
                        }
                    }

                    return '<a href="' + linkUrl + '">' + linkName + '</a>';
                }
                onLinkActivated: {
                    changeEndpointPopup.open()
                }
                function propertyIsResource(prop) {
                    return prop && prop.type === ApiProperty.TYPE_RESOURCE
                }
            }
            Label {
                color: 'lightgray'
                text: {
                    if (!root.selectedProperty) return ''
                    var countString = ''
                    switch (root.selectedProperty.type) {
                    case ApiProperty.TYPE_ARRAY: countString = '[' + root.selectedProperty.asArray.length + ']'; break;
                    }
                    return root.selectedProperty.typeString + countString
                }
            }
            Button {
                id: resourceMenuButton
                background: Rectangle {
                    implicitWidth: headerLabel.height
                    implicitHeight: headerLabel.height
                }
                text: qsTr('Options')

                onClicked: resourceMenu.open()
                Menu {
                    id: resourceMenu
                    y: resourceMenuButton.height
                    MenuItem {
                        text: qsTr('Add')
                        enabled: endpoint.isModifyable
                    }
                    MenuItem {
                        text: qsTr('Delete')
                        enabled: endpoint.isModifyable && root.selectedProperty && root.selectedProperty.type == ApiProperty.TYPE_RESOURCE
                        onTriggered: {
                            deleteResource(root.selectedProperty.asObject)
                        }
                    }
                    MenuItem {
                        text: qsTr('Edit')
                        enabled: endpoint.isModifyable && root.selectedProperty && root.selectedProperty.type == ApiProperty.TYPE_RESOURCE
                        onTriggered: {
                            editResource(root.selectedProperty.asObject)
                        }
                    }
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
                    onScalarItemClicked: {
                        root.selectedProperty = item
                    }
                    onObjectItemClicked: {
                        root.selectedProperty = item
                    }
                    onResourceItemClicked: {
                        root.selectedProperty = item
                    }
                    onArrayItemClicked: {
                        pushProperty(item)
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
