import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    background: Rectangle { color: Theme.colorBg }

    header: ToolBar {
        background: Rectangle { color: Theme.colorPrimary }
        contentHeight: 56
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 4
            anchors.rightMargin: 16
            ToolButton {
                text: "←"; font.pixelSize: Theme.fsLG; font.bold: true
                onClicked: page.StackView.view.pop()
                contentItem: Label {
                    text: "←"; font: parent.font; color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                }
            }
            Label {
                text: "购药记录"; font.pixelSize: Theme.fsLG; font.bold: true; color: "#ffffff"
            }
        }
    }

    // ── 空状态 ────────────────────────────────
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        visible: orderList.count === 0
        ColumnLayout {
            anchors.centerIn: parent
            spacing: 12
            Label { text: "📋"; font.pixelSize: 48; Layout.alignment: Qt.AlignHCenter }
            Label {
                text: "暂无订单记录"; font.pixelSize: Theme.fsLG; font.bold: true; color: Theme.colorSubText
            }
        }
    }

    ListView {
        id: orderList
        anchors.fill: parent
        clip: true
        spacing: 8
        model: orderModel
        delegate: Component {
            Loader {
                source: "qrc:/qml/components/OrderItem.qml"
                asynchronous: false
                width: orderList.width - 24
                anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined

                onLoaded: {
                    item.orderId = model.id
                    item.orderTime = model.time
                    item.orderPrice = model.price
                    item.orderContent = model.content
                    item.removeRequested.connect(function() {
                        App.deleteOrder(model.id)
                        refresh()
                    })
                }
            }
        }
    }

    property var orderModel: ListModel { id: _orderModel }

    function refresh() {
        var data = App.getAllOrders()
        _orderModel.clear()
        for (var i = 0; i < data.length; i++) {
            _orderModel.append(data[i])
        }
    }

    Component.onCompleted: refresh()
}
