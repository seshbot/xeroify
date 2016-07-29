import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Loader {
    property ApiProperty currentProperty

    signal endpointItemClicked(MakeLeapsEndpoint endpoint)

    Component {
        id: emptyPage
        Label {
            anchors.centerIn: parent
            text: '-empty-'
        }
    }
    Component {
        id: scalarPage
        Label {
            anchors.centerIn: parent
            text: {
                if (currentProperty.isEndpoint) return '<a href="' + currentProperty.value + '">' + currentProperty.value + '</a>'
                return 'scalar: ' + currentProperty.asScalar
            }
        }
    }
    Component {
        id: objectPage
        ListView {
            header: Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: 'object'
            }
            anchors.fill: parent
            model: ( currentProperty && currentProperty.asObject ) ? currentProperty.asObject.properties : []
            delegate: ItemDelegate {
                width: parent.width
                Row {
                    width: parent.width
                    anchors.verticalCenter: parent.verticalCenter
                    Label {
                        color: Material.color(Material.Grey)
                        width: parent.width / 3
                        text: modelData.name
                    }
                    Label {
                        text: {
                            switch (modelData.type) {
                            case ApiProperty.TYPE_SCALAR: {
                                var value = modelData.asScalar
                                if (modelData.isEndpoint) {
                                    value = '<a href="' + value + '">' + value + '</a>'
                                }
                                return value
                            }
                            case ApiProperty.TYPE_ARRAY: return modelData.asArray.length + ' values';
                            default: return modelData.typeString;
                            }
                        }
                        onLinkActivated: {
                            endpointItemClicked(modelData.asEndpoint)
                        }
                    }
                }
            }
            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }
    Component {
        id: arrayPage
        ListView {
            header: Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: 'array'
            }
            anchors.centerIn: parent
            model: currentProperty.asArray
            delegate: ItemDelegate {
                text: '-'
            }
            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }
    sourceComponent: {
        if (!currentProperty) return emptyPage
        switch (currentProperty.type) {
        case ApiProperty.TYPE_NULL: return emptyPage
        case ApiProperty.TYPE_SCALAR: return scalarPage
        case ApiProperty.TYPE_OBJECT:
        case ApiProperty.TYPE_RESOURCE: return objectPage
        case ApiProperty.TYPE_ARRAY: return arrayPage
        default: return emptyPage
        }
    }
}
