import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: card
    radius: 12; color: Theme.colorCard
    border.color: Theme.colorBorder
    border.width: 1

    property string drugName: ""
    property double drugPrice: 0
    property string drugEmoji: "💊"
    property string drugColor: "#2563eb"
    property string drugDesc: ""

    signal clicked()

    // 卡片点击区域（放在底层，不拦截按钮）
    MouseArea {
        anchors.fill: parent
        z: 0
        onClicked: card.clicked()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 6

        // Emoji 图标
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 48; height: 48; radius: 12
            color: Qt.lighter(drugColor, 1.6)
            Label {
                anchors.centerIn: parent; text: drugEmoji; font.pixelSize: 24
            }
        }

        // 药品名
        Label {
            Layout.fillWidth: true
            text: drugName; font.pixelSize: Theme.fsSM; font.bold: true
            color: Theme.colorText; elide: Text.ElideRight; maximumLineCount: 1
            horizontalAlignment: Text.AlignHCenter
        }

        // 简述
        Label {
            Layout.fillWidth: true
            text: drugDesc; font.pixelSize: Theme.fsXS; color: Theme.colorSubText
            elide: Text.ElideRight; maximumLineCount: 2; wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }

        Item { Layout.fillHeight: true }

        // 价格
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "¥" + drugPrice.toFixed(2)
                font.pixelSize: Theme.fsMD; font.bold: true; color: Theme.colorDanger
            }
            Item { Layout.fillWidth: true }
            Rectangle {
                width: 28; height: 28; radius: 8; color: Theme.colorAccent
                Label {
                    anchors.centerIn: parent; text: "+"; font.pixelSize: 16; font.bold: true; color: "#fff"
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        App.addToCart(drugName, drugPrice)
                    }
                }
            }
        }
    }
}
