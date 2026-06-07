import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    background: Rectangle { color: Theme.colorBg }

    property string newsTitle: ""
    property string newsEmoji: ""
    property string newsContent: ""

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
                text: "资讯详情"; font.pixelSize: Theme.fsLG; font.bold: true; color: "#ffffff"
                Layout.fillWidth: true; elide: Text.ElideRight
            }
        }
    }

    ScrollView {
        anchors.fill: parent

        ColumnLayout {
            width: parent.width; spacing: 0

            // 头部图
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 180
                color: Qt.lighter(Theme.colorPrimary, 1.7)
                Label {
                    anchors.centerIn: parent; text: newsEmoji || "📰"; font.pixelSize: 64
                }
            }

            // 标题
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: titleLabel.implicitHeight + 32
                color: Theme.colorCard
                Label {
                    id: titleLabel
                    anchors.fill: parent
                    anchors.margins: 16
                    text: newsTitle; font.pixelSize: Theme.fsXL; font.bold: true
                    color: Theme.colorText; wrapMode: Text.WordWrap
                }
            }

            // 正文
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: contentLabel.implicitHeight + 32
                color: Theme.colorCard
                Label {
                    id: contentLabel
                    anchors.fill: parent
                    anchors.margins: 16
                    text: newsContent; font.pixelSize: Theme.fsMD; color: Theme.colorText
                    wrapMode: Text.WordWrap; lineHeight: 1.8
                }
            }
        }
    }
}
