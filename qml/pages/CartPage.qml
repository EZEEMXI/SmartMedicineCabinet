import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    background: Rectangle { color: Theme.colorBg }

    header: ToolBar {
        background: Rectangle { color: Theme.colorPrimary }
        contentHeight: 56
        Label {
            anchors.centerIn: parent
            text: "购物车"
            font.pixelSize: Theme.fsLG; font.bold: true; color: "#ffffff"
        }
    }

    // ── 空状态 ────────────────────────────────
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        visible: cartList.count === 0

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 12
            Label { text: "🛒"; font.pixelSize: 48; Layout.alignment: Qt.AlignHCenter }
            Label {
                text: "购物车是空的"
                font.pixelSize: Theme.fsLG; font.bold: true
                color: Theme.colorSubText
            }
            Label {
                text: "去药品货架逛逛吧~"
                font.pixelSize: Theme.fsSM; color: Theme.colorSubText
            }
        }
    }

    // ── 购物车列表 ────────────────────────────
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        ListView {
            id: cartList
            Layout.fillWidth: true; Layout.fillHeight: true
            model: cartModel
            clip: true
            spacing: 1

            delegate: Component {
                Loader {
                    source: "qrc:/qml/components/CartItem.qml"
                    asynchronous: false
                    width: cartList.width

                    onLoaded: {
                        item.cartId = model.id
                        item.cartName = model.name
                        item.cartPrice = model.price
                        item.cartNum = model.num
                        item.add.connect(function() { App.updateCartNum(model.id, 1); refresh() })
                        item.minus.connect(function() { App.updateCartNum(model.id, -1); refresh() })
                        item.remove.connect(function() { App.removeFromCart(model.id); refresh() })
                    }
                }
            }
        }
    }

    // ── 底部结算 ──────────────────────────────
    footer: Rectangle {
        width: parent.width; height: 64
        color: Theme.colorCard
        border.color: Theme.colorBorder
        border.width: 1
        visible: cartList.count > 0

        RowLayout {
            anchors.fill: parent
            anchors.margins: 16
            ColumnLayout {
                spacing: 2
                Label {
                    text: "共 " + totalItems + " 件"
                    font.pixelSize: Theme.fsSM; color: Theme.colorSubText
                }
                Label {
                    text: "合计 ¥" + totalPrice.toFixed(2)
                    font.pixelSize: Theme.fsLG; font.bold: true; color: Theme.colorDanger
                }
            }
            Item { Layout.fillWidth: true }
            Button {
                Layout.preferredWidth: 120; Layout.preferredHeight: 44
                background: Rectangle { radius: 12; color: Theme.colorAccent }
                contentItem: Label {
                    text: "提交订单"
                    font.pixelSize: Theme.fsMD; font.bold: true
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: confirmDialog.open()
            }
        }
    }

    // ── 数据 ──────────────────────────────────
    property var cartModel: ListModel { id: _cartModel }
    property double totalPrice: 0
    property int totalItems: 0

    function refresh() {
        var data = App.getAllCart()
        _cartModel.clear()
        totalPrice = 0
        totalItems = 0
        for (var i = 0; i < data.length; i++) {
            var item = data[i]
            _cartModel.append(item)
            totalPrice += item.price
            totalItems += item.num
        }
    }

    Component.onCompleted: refresh()
    Connections {
        target: App
        function onCartCountChanged() { refresh() }
    }

    // ── 确认对话框 ────────────────────────────
    Dialog {
        id: confirmDialog
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 320)
        modal: true

        background: Rectangle { radius: 16; color: Theme.colorCard }

        ColumnLayout {
            spacing: 16
            Label {
                text: "确认下单"
                font.pixelSize: Theme.fsLG; font.bold: true
                color: Theme.colorText
            }
            Label {
                text: "共 " + totalItems + " 件商品，合计 ¥" + totalPrice.toFixed(2) + "\n确认提交订单？"
                font.pixelSize: Theme.fsSM; color: Theme.colorSubText
                lineHeight: 1.6
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                spacing: 12
                Button {
                    text: "取消"; flat: true
                    onClicked: confirmDialog.close()
                }
                Button {
                    text: "确认下单"
                    background: Rectangle { radius: 8; color: Theme.colorAccent }
                    contentItem: Label {
                        text: parent.text; font.pixelSize: Theme.fsSM; font.bold: true
                        color: "#fff"; horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        if (App.submitOrder()) {
                            confirmDialog.close()
                            toast.show("下单成功！")
                            refresh()
                        }
                    }
                }
            }
        }
    }

    // ── Toast ──────────────────────────────────
    Rectangle {
        id: toast
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 80
        width: toastLabel.implicitWidth + 32; height: 40; radius: 20
        color: "#333333"; opacity: 0; visible: false

        function show(msg) {
            toastLabel.text = msg; opacity = 1; visible = true
            toastTimer.restart()
        }

        Label {
            id: toastLabel
            anchors.centerIn: parent
            color: "#ffffff"; font.pixelSize: Theme.fsSM
        }

        Timer {
            id: toastTimer; interval: 1800
            onTriggered: { toast.opacity = 0; toast.visible = false }
        }

        Behavior on opacity { NumberAnimation { duration: 300 } }
    }
}
