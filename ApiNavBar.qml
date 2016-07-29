import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Loader {
    property ApiProperty currentProperty
    property ApiProperty selectedProperty

    signal endpointItemClicked(MakeLeapsEndpoint endpoint)
    signal arrayItemClicked(ApiProperty item)
    signal objectItemClicked(ApiProperty item)
    signal resourceItemClicked(ApiProperty item)

    function itemClicked(item) {
        switch (item.type) {
        case ApiProperty.TYPE_OBJECT: objectItemClicked(item); break
        case ApiProperty.TYPE_RESOURCE: resourceItemClicked(item); break
        case ApiProperty.TYPE_ARRAY: arrayItemClicked(item); break
        case ApiProperty.TYPE_SCALAR: {
            if (item.isEndpoint) {
                endpointItemClicked(item.asEndpoint)
            }
            break
        }
        case ApiProperty.TYPE_NULL:
        default:
            console.log('ignoring click')
        }
    }

    Component {
        id: loadingPage
        Label {
            anchors.centerIn: parent
            text: 'loading'
        }
    }
    Component {
        id: nullPage
        Label {
            text: 'null'
        }
    }
    Component {
        id: scalarPage
        Label {
            text: 'scalar'
        }
    }
    Component {
        id: objectPage
        ListView {
            header: Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: 'object'
            }
            anchors.centerIn: parent
            model: !currentProperty ? [ 'null', 'null' ] : !currentProperty.asObject ? [ currentProperty.typeString, '2'] : currentProperty.asObject.properties
            delegate: ItemDelegate {
                width: parent.width
                text: {
                    if (typeof modelData === 'string') return modelData
                    var result = modelData.name
                    if (modelData.isEndpoint) {
                        result = '<a href="' + modelData.value + '">' + result + '</a>'
                    }
                    return result
                }
                highlighted: selectedProperty === modelData
                onClicked: {
                    itemClicked(modelData)
                }
            }
            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }
    Component {
        id: resourcePage
        ListView {
            header: Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: 'resource'
            }
            anchors.centerIn: parent
            model: currentProperty.asObject.properties
            delegate: ItemDelegate {
                width: parent.width
                text: {
                    var result = modelData.name
                    if (modelData.isEndpoint) {
                        result = '<a href="' + modelData.value + '">' + result + '</a>'
                    }
                    return result
                }
                highlighted: selectedProperty === modelData
                onClicked: {
                    itemClicked(modelData)
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
            model: currentProperty && currentProperty.asArray ? currentProperty.asArray : []
            delegate: ItemDelegate {
                width: parent.width
                text: {
                    switch (modelData.type) {
                    case ApiProperty.TYPE_NULL: return 'null'
                    case ApiProperty.TYPE_SCALAR: return modelData.value
                    case ApiProperty.TYPE_OBJECT:
                    case ApiProperty.TYPE_RESOURCE: return modelData.name || '-'
                    case ApiProperty.TYPE_ARRAY: return 'array [' + modelData.asArray.length + ']'
                    default: return 'todo: ' + modelData.typeString
                    }
                }
                highlighted: selectedProperty === modelData
                onClicked: {
                    console.log('array item clicked', modelData)
                    itemClicked(modelData)
                }
            }
            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }
    Component {
        id: todoPage
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: 'nav - todo: ' + currentProperty.typeString
        }
    }
    sourceComponent: {
        if (!currentProperty) return loadingPage;
        switch (currentProperty.type) {
        case ApiProperty.TYPE_NULL: return nullPage;
        case ApiProperty.TYPE_SCALAR: return scalarPage;
        case ApiProperty.TYPE_OBJECT: return objectPage;
        case ApiProperty.TYPE_RESOURCE: return resourcePage;
        case ApiProperty.TYPE_ARRAY: return arrayPage;
        default: return todoPage;
        }
    }
}
