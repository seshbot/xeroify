import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Page {
    id: root
    property MakeLeapsEndpoint endpoint
    property bool invalid : !endpoint || endpoint.state == MakeLeapsEndpoint.STATE_INVALID
    property bool error : endpoint && endpoint.state == MakeLeapsEndpoint.STATE_ERROR
    property bool loading : endpoint && endpoint.state == MakeLeapsEndpoint.STATE_LOADING
    property bool loaded : endpoint && endpoint.state == MakeLeapsEndpoint.STATE_LOADED

    property int navbarWidth : root.width / 4

    property bool backEnabled: false
    property bool forwardEnabled: false

    signal endpointSelected(MakeLeapsEndpoint e)
    signal endpointUrlSelected(string url)
    signal backClicked()
    signal forwardClicked()

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
    }

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
                    backClicked()
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
                    forwardClicked()
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
                    // endpointUrlSelected(link)
                }
            }
            Label {
                text: {
                    if (!endpoint.rootProperty) return ''
                    var countString = ''
                    switch (endpoint.rootProperty.type) {
                    case MakeLeapsResourceProperty.TYPE_RESOURCE_ARRAY: countString = '[' + endpoint.rootProperty.resources.length + ']'; break;
                    case MakeLeapsResourceProperty.TYPE_VALUE_ARRAY: countString = '[' + endpoint.rootProperty.properties.length + ']'; break;
                    }
                    return endpoint.rootProperty.typeString + countString
                }
            }
        }
    }
    Component {
        id: loadingPage
        Label {
            text: 'loading'
        }
    }
    Component {
        id: todoPage
        Label {
            text: 'todo: ' + endpoint.rootProperty.typeString
        }
    }
    Component {
        id: singleResource
        RowLayout {
            id: singleResourcePage
            property var selectedResourceProperty
            anchors.fill: parent
            //
            // endpoint nav bar
            //
            ListView {
                id: childList
                Layout.minimumWidth: navbarWidth
                Layout.fillHeight: true
                currentIndex: -1
                highlightFollowsCurrentItem: true
                model: endpoint.rootProperty.resource.properties
                delegate: ItemDelegate {
                    text: {
                        var result = modelData.name
                        if (modelData.type === MakeLeapsResourceProperty.TYPE_ENDPOINT) {
                            result = '<a href="' + modelData.url + '">' + result + '</a>'
                        }
                        return result
                    }
                    width: parent.width
                    highlighted: singleResourcePage.selectedResourceProperty === modelData
                    onClicked: {
                        if (modelData.type === MakeLeapsResourceProperty.TYPE_ENDPOINT)
                        {
                            root.endpointSelected(modelData.endpoint)
                        }
                        else if (modelData.type === MakeLeapsResourceProperty.TYPE_RESOURCE_ARRAY)
                        {
                            childList.model = modelData.resources
                        }
                        else
                        {
                            singleResourcePage.selectedResourceProperty = modelData
                        }
                    }
                }
                ScrollIndicator.vertical: ScrollIndicator { }
            }
            Page {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Loader {
                    Component {
                        id: emptyInfo
                        Text {
                            text: 'No Endpoint'
                        }
                    }
                    Component {
                        id: todoPropertyInfo
                        Text {
                            text: 'todo: ' + selectedResourceProperty.typeString
                        }
                    }
                    Component {
                        id: noPropertyInfo
                        Text {
                            text: ''
                        }
                    }
                    Component {
                        id: endpointPropertyInfo
                        Pane {
                            property var endpoint: selectedResourceProperty.endpoint
                            anchors.centerIn: parent
                            Label {
                                anchors.centerIn: parent
                                text: endpoint ? endpoint.url : '-'
                            }
                        }
                    }
                    Component {
                        id: resourcePropertyInfo
                        Page {
                            anchors.fill: parent
                            ListView {
                                anchors.fill: parent
                                model: selectedResourceProperty.properties
                                delegate: ItemDelegate {
                                    width: parent.width
                                    Row {
                                        width: parent.width
                                        anchors.verticalCenter: parent.verticalCenter
                                        Label {
                                            color: Material.color(Material.Grey)
                                            width: 100
                                            text: modelData.name
                                        }
                                        Label {
                                            text: {
                                                switch (modelData.type) {
                                                case MakeLeapsResourceProperty.TYPE_VALUE: return modelData.value;
                                                case MakeLeapsResourceProperty.TYPE_VALUE_ARRAY: return modelData.properties.length + ' values';
                                                default: return modelData.typeString;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    Component {
                        id: resourceArrayPropertyInfo
                        ListView {
                            anchors.fill: parent
                            model: selectedResourceProperty.resources
                            delegate: ItemDelegate {
                                text: modelData.name
                                onClicked: {
                                    endpointSelected(modelData.properties[1].endpoint)
                                }
                            }
                        }
                    }
                    Component {
                        id: valueArrayPropertyInfo
                        ListView {
                            model: selectedResourceProperty.arrayValue
                            delegate: ItemDelegate {
                                text: modelData.json
                            }
                        }
                    }
                    Component {
                        id: valuePropertyInfo
                        Text {
                            text: selectedResourceProperty.value
                        }
                    }
                    sourceComponent: {
                        if (!singleResourcePage || !singleResourcePage.selectedResourceProperty)
                            return noPropertyInfo;

                        switch (singleResourcePage.selectedResourceProperty.type)
                        {
                        case MakeLeapsResourceProperty.TYPE_ENDPOINT: return endpointPropertyInfo;
                        case MakeLeapsResourceProperty.TYPE_VALUE: return valuePropertyInfo;
                        case MakeLeapsResourceProperty.TYPE_VALUE_ARRAY: return valueArrayPropertyInfo;
                        case MakeLeapsResourceProperty.TYPE_RESOURCE: return resourcePropertyInfo;
                        case MakeLeapsResourceProperty.TYPE_RESOURCE_ARRAY: return resourceArrayPropertyInfo;
                        default: return todoPropertyInfo;
                        }
                    }
                }
            }
        }
    }
    Component {
        id: resourceArray
        RowLayout {
            id: resourceArrayPage
            property var selectedResource
            ListView {
                Layout.minimumWidth: root.width / 4
                Layout.fillHeight: true
                model: endpoint.rootProperty.type === MakeLeapsResourceProperty.TYPE_RESOURCE_ARRAY ? endpoint.rootProperty.resources : endpoint.rootProperty.properties
                delegate: ItemDelegate {
                    text: modelData.name
                    width: parent.width
                    highlighted: resourceArrayPage.selectedResource === modelData
                    onClicked: {
                        resourceArrayPage.selectedResource = modelData.resource
                    }
                }
                ScrollIndicator.vertical: ScrollIndicator { }
            }
            Loader {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Component {
                    id: resourceDetailsPanel
                    Page {
                        anchors.fill: parent
                        ListView {
                            anchors.fill: parent
                            model: resourceArrayPage.selectedResource.properties
                            delegate: ItemDelegate {
                                width: parent.width
                                Row {
                                    width: parent.width
                                    anchors.verticalCenter: parent.verticalCenter
                                    Label {
                                        color: Material.color(Material.Grey)
                                        width: 100
                                        text: modelData.name
                                    }
                                    Label {
                                        text: {
                                            switch (modelData.type) {
                                            case MakeLeapsResourceProperty.TYPE_VALUE: return modelData.value;
                                            case MakeLeapsResourceProperty.TYPE_VALUE_ARRAY: return modelData.properties.length + ' values';
                                            default: return modelData.typeString;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                Component {
                    id: loadingPanel
                    Page {
                        anchors.centerIn: parent
                        Label {
                            text: ''
                        }
                    }
                }
                sourceComponent: resourceArrayPage.selectedResource ? resourceDetailsPanel : loadingPanel
            }
        }
    }
    Component {
        id: errorPage
        Page {
            anchors.fill: parent
            Button {
                anchors.centerIn: parent
                text: 'Reload'
                onClicked: {
                    root.endpoint.load()
                }
            }
        }
    }

    Loader {
        id: resourceLoader
        anchors.fill: parent
        sourceComponent: {
            if (root.error) return errorPage;
            if (!root.loaded) return loadingPage;
            switch (root.endpoint.rootProperty.type) {
            case MakeLeapsResourceProperty.TYPE_RESOURCE: return singleResource;
            case MakeLeapsResourceProperty.TYPE_RESOURCE_ARRAY: return resourceArray;
            case MakeLeapsResourceProperty.TYPE_VALUE_ARRAY: return resourceArray;
            default: return todoPage
            }
        }
    }
//    footer: Text {
//        text: 'footer'
//    }
}
