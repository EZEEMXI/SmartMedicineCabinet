import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    background: Rectangle { color: Theme.colorBg }

    // ── 内部 StackView ───────────────────────────
    StackView {
        id: drugStack
        anchors.fill: parent

        // 返回键：优先 pop 内部栈，不向上传递
        Keys.onBackPressed: function(event) {
            if (depth > 1) {
                pop()
                event.accepted = true
            }
        }

        initialItem: Item {
            width: drugStack.width
            height: drugStack.height

            // ── Header ───────────────────────────────
            ToolBar {
                id: headerBar
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                background: Rectangle { color: Theme.colorPrimary }
                contentHeight: 56
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    Label {
                        text: "药品货架"
                        font.pixelSize: Theme.fsLG; font.bold: true; color: "#ffffff"
                    }
                    Item { Layout.fillWidth: true }
                    Label {
                        text: App.userName
                        font.pixelSize: Theme.fsSM; color: "#bfdbfe"
                    }
                }
            }

            // ── 药品网格 ─────────────────────────────
            GridView {
                id: grid
                anchors.top: headerBar.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 12
                cellWidth: (parent.width - 24) / 2
                cellHeight: 200
                model: drugModel
                clip: true

                delegate: Component {
                    Loader {
                        source: "qrc:/qml/components/DrugCard.qml"
                        asynchronous: false
                        width: grid.cellWidth - 8
                        height: grid.cellHeight - 8

                        onLoaded: {
                            item.drugName = model.name
                            item.drugPrice = model.price
                            item.drugEmoji = model.emoji
                            item.drugColor = model.color
                            item.drugDesc = model.desc
                            item.clicked.connect(function() {
                                drugStack.push("qrc:/qml/pages/DrugDetailPage.qml", {
                                    drugName: model.name,
                                    drugPrice: model.price,
                                    drugEmoji: model.emoji,
                                    drugColor: model.color,
                                    drugDesc: model.desc,
                                    drugUsage: model.usage,
                                    drugWarning: model.warning
                                })
                            })
                        }
                    }
                }
            }
        }
    }

    // ── 药品数据 ────────────────────────────
    ListModel {
        id: drugModel
        ListElement {
            name: "布洛芬缓释胶囊"
            price: 18.0
            emoji: "💊"
            color: "#f97316"
            desc: "缓解轻至中度疼痛，如头痛、牙痛、关节痛、肌肉痛、神经痛、痛经。也用于普通感冒或流行性感冒引起的发热。"
            usage: "口服。成人一次1粒，一日2次（早晚各一次）。饭后服用，整粒吞服，不得打开或溶解后服用。"
            warning: "对本品过敏者禁用；服用期间不得饮酒；孕妇及哺乳期妇女慎用。"
        }
        ListElement {
            name: "连花清瘟胶囊"
            price: 15.0
            emoji: "🌿"
            color: "#22c55e"
            desc: "清瘟解毒，宣肺泄热。用于治疗流行性感冒属热毒袭肺证，症见发热或高热、恶寒、肌肉酸痛、鼻塞流涕、咳嗽、头痛、咽干咽痛。"
            usage: "口服。一次4粒，一日3次。儿童酌减，或遵医嘱。"
            warning: "风寒感冒者不适用；服药期间忌烟、酒及辛辣、生冷、油腻食物。"
        }
        ListElement {
            name: "阿莫西林胶囊"
            price: 58.0
            emoji: "🦠"
            color: "#3b82f6"
            desc: "广谱青霉素类抗生素。用于敏感菌所致的呼吸道感染、泌尿生殖道感染、皮肤软组织感染等。"
            usage: "口服。成人一次0.5g(2粒)，每6-8小时1次。须遵医嘱完成整个疗程。"
            warning: "青霉素过敏者禁用；用药前必须做青霉素皮试；肾功能不全者需调整剂量。"
        }
        ListElement {
            name: "氯雷他定片"
            price: 25.0
            emoji: "😤"
            color: "#a855f7"
            desc: "用于缓解过敏性鼻炎有关的症状，如喷嚏、流涕、鼻痒、鼻塞以及眼部痒及烧灼感。亦用于缓解慢性荨麻疹及其它过敏性皮肤病的症状。"
            usage: "口服。成人及12岁以上儿童：一日1次，一次1片(10mg)。"
            warning: "肝功能不全者应减量；妊娠期及哺乳期妇女慎用。"
        }
        ListElement {
            name: "蒙脱石散"
            price: 38.0
            emoji: "🧱"
            color: "#f59e0b"
            desc: "用于成人及儿童急、慢性腹泻。对消化道内的病毒、病菌及其产生的毒素有固定、抑制作用，同时修复消化道黏膜屏障。"
            usage: "口服。成人一次1袋(3g)，一日3次。将本品倒入半杯温水中，搅匀后服用。"
            warning: "服用其它药物应间隔1-2小时；治疗急性腹泻时应注意纠正脱水。"
        }
        ListElement {
            name: "盐酸二甲双胍片"
            price: 12.0
            emoji: "💉"
            color: "#ef4444"
            desc: "用于单纯饮食控制不满意的2型糖尿病病人，尤其是肥胖和伴高胰岛素血症者。可降低血糖，减轻体重和高胰岛素血症。"
            usage: "口服。成人开始一次0.25g(1片)，一日2-3次，以后根据疗效逐渐加量。"
            warning: "肾功能不全者禁用；严重感染、缺氧或接受大手术者禁用；用药期间定期监测肾功能。"
        }
    }
}
