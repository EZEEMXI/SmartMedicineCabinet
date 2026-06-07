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
                text: "健康资讯"; font.pixelSize: Theme.fsLG; font.bold: true; color: "#ffffff"
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // 搜索栏
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 56
            color: Theme.colorCard
            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                TextField {
                    id: searchField
                    Layout.fillWidth: true
                    font.pixelSize: Theme.fsSM
                    placeholderText: "搜索健康资讯..."
                    background: Rectangle { radius: 8; color: Theme.colorBg }
                }
                Button {
                    text: "搜索"; font.pixelSize: Theme.fsSM
                    background: Rectangle { radius: 8; color: Theme.colorPrimary }
                    contentItem: Label {
                        text: parent.text; font: parent.font; color: "#fff"
                        horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        var kw = searchField.text.toLowerCase()
                        for (var i = 0; i < newsList.count; i++) {
                            var item = newsList.itemAtIndex(i)
                            item.visible = (kw === "" ||
                                item.title.toLowerCase().includes(kw) ||
                                item.summary.toLowerCase().includes(kw))
                        }
                    }
                }
            }
        }

        // 新闻列表
        ListView {
            Layout.fillWidth: true; Layout.fillHeight: true
            model: newsModel; clip: true; spacing: 1

            delegate: Rectangle {
                id: newsItem
                property string title: model.title
                property string summary: model.summary
                property string content: model.content
                property string emoji: model.emoji

                width: ListView.view.width; height: 80
                color: Theme.colorCard

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 12
                    Rectangle {
                        width: 56; height: 56; radius: 8
                        color: Qt.lighter(Theme.colorPrimary, 1.7)
                        Label {
                            anchors.centerIn: parent; text: emoji; font.pixelSize: 24
                        }
                    }
                    ColumnLayout { spacing: 4; Layout.fillWidth: true
                        Label {
                            text: title; font.pixelSize: Theme.fsSM; font.bold: true
                            color: Theme.colorText; elide: Text.ElideRight
                            Layout.fillWidth: true; maximumLineCount: 1
                        }
                        Label {
                            text: summary; font.pixelSize: Theme.fsXS; color: Theme.colorSubText
                            elide: Text.ElideRight; Layout.fillWidth: true; maximumLineCount: 2
                            lineHeight: 1.4
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: page.StackView.view.push("qrc:/qml/pages/NewsDetailPage.qml", {
                        newsTitle: title,
                        newsEmoji: emoji,
                        newsContent: content
                    })
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 1; color: Theme.colorBorder
                }
            }
        }
    }

    ListModel {
        id: newsModel
        ListElement {
            title: "春季养生正当时：中医推荐四大调理方法"
            summary: "春季是万物复苏的季节，中医认为此时应当顺应自然，养护肝脏。"
            emoji: "🌸"
            content: "春季养生正当时：中医推荐四大调理方法\n\n春季是万物复苏的季节，《黄帝内经》云：「春三月，此谓发陈，天地俱生，万物以荣。」中医认为春季属木，与肝相应，此时养生重点在于养肝护肝、顺应阳气生发。\n\n一、饮食调养：春季饮食宜「省酸增甘，以养脾气」。多吃山药、红枣、枸杞、荠菜、韭菜等甘味食物，少吃酸味收敛之品。推荐枸杞菊花茶、山药排骨汤等食疗方。\n\n二、起居有常：春应「夜卧早起，广步于庭」。早睡早起，舒展身体，保持心情舒畅。适当增加户外活动，接触阳光补充维生素D。\n\n三、情志调畅：肝主疏泄，喜条达而恶抑郁。春季应保持心情愉悦，避免暴怒、忧郁等不良情绪。可通过听音乐、踏青、太极拳等方式放松身心。\n\n四、运动锻炼：春季运动应「广步于庭，被发缓形」，以舒缓为主。推荐散步、慢跑、八段锦、太极拳等温和运动，避免剧烈运动。"
        }
        ListElement {
            title: "世界卫生组织发布新版健康饮食指南"
            summary: "WHO建议每日摄入至少400克蔬果，减少游离糖和饱和脂肪摄入。"
            emoji: "🥗"
            content: "世界卫生组织发布新版健康饮食指南\n\n世界卫生组织（WHO）近期发布了最新版的健康饮食指南，对全球居民的营养摄入提出了更具体的建议。\n\n核心建议包括：\n\n一、每日摄入至少400克（5份）蔬果，不包括土豆、红薯等根茎类蔬菜。\n\n二、总脂肪摄入量应低于总能量摄入的30%，其中饱和脂肪应低于10%，反式脂肪应低于1%。\n\n三、游离糖摄入应低于总能量摄入的10%，最好控制在5%以下。游离糖包括食品中添加的白糖、果糖、蜂蜜以及果汁中的糖分。\n\n四、每日食盐摄入量应低于5克，并确保食盐加碘。\n\nWHO强调，健康饮食不仅能预防各种形式的营养不良，还能降低非传染性疾病的发病风险，包括糖尿病、心脏病、中风和癌症。"
        }
        ListElement {
            title: "每天步行8000步可显著降低死亡风险"
            summary: "最新研究表明，每天坚持步行8000步能使全因死亡风险降低约50%。"
            emoji: "🚶"
            content: "研究表明：每天步行8000步可显著降低死亡风险\n\n一项发表在《美国医学会杂志》上的大规模队列研究，对超过4800名美国成年人进行了长达10年的追踪调查。\n\n研究结果：与每天步行少于4000步的组相比，每天步行8000步的人群全因死亡风险降低了约51%，心血管疾病死亡风险降低了约55%。\n\n关键发现：\n- 并非步数越多越好：每天12000步以上并未带来额外的显著获益。\n- 步速也很重要：同等步数下，步速较快者的获益更大。\n- 不分年龄性别：各年龄组和性别的人群均能从中获益。\n\n专家建议：将步行融入日常生活，如走楼梯而非乘电梯、提前一站下公交、饭后散步等。"
        }
        ListElement {
            title: "国家卫健委提醒：夏季谨防肠道传染病"
            summary: "夏季气温升高，细菌繁殖加快，是肠道传染病高发期。"
            emoji: "🦠"
            content: "国家卫健委提醒：夏季谨防肠道传染病\n\n夏季高温多湿，细菌繁殖速度加快，是霍乱、细菌性痢疾、伤寒、副伤寒等肠道传染病的高发季节。国家卫健委发布健康提示：\n\n一、注意饮食卫生：不喝生水，不吃腐败变质食物，生吃瓜果要洗净。\n\n二、养成良好卫生习惯：饭前便后洗手，不随地吐痰。\n\n三、生熟食品分开存放和处理，避免交叉污染。\n\n四、剩饭剩菜要彻底加热后再食用，存放超过2小时的熟食应冷藏。\n\n五、出现腹泻、呕吐等症状及时就医，避免自行用药。\n\n预防肠道传染病，关键是「把住病从口入关」。"
        }
        ListElement {
            title: "睡眠不足的危害不容忽视"
            summary: "专家建议成年人每晚应保证7-8小时的优质睡眠。"
            emoji: "😴"
            content: "睡眠不足的危害不容忽视：专家建议成年人每天睡足7小时\n\n充足的睡眠是健康的基础。然而《中国睡眠研究报告》显示，超过3亿中国人存在睡眠障碍。\n\n长期睡眠不足的危害：\n\n一、认知功能下降：注意力不集中、记忆力减退、决策能力下降。\n\n二、免疫力降低：睡眠不足会影响免疫细胞的活性和数量，增加感染风险。\n\n三、心血管风险增加：长期睡眠不足6小时的人群，高血压风险增加20%，冠心病风险增加48%。\n\n四、代谢紊乱：影响血糖调节，增加肥胖和糖尿病风险。\n\n五、心理健康问题：与焦虑、抑郁密切相关。\n\n改善睡眠建议：保持规律作息、睡前1小时远离手机、卧室温度控制在18-22℃、避免睡前大量进食或饮酒。"
        }
        ListElement {
            title: "心脏健康从日常做起：心血管疾病预防要点"
            summary: "心血管疾病是我国居民死亡的首要原因，但80%可以通过生活方式预防。"
            emoji: "❤️"
            content: "心脏健康从日常做起：心血管疾病预防五大要点\n\n心血管疾病是我国居民死亡的第一大原因，每年导致约400万人死亡。然而研究表明，80%的心血管疾病可以通过健康的生活方式来预防。\n\n五大预防要点：\n\n一、合理膳食：DASH饮食或地中海饮食模式被证实对心血管健康最有益。多吃蔬菜水果、全谷物、鱼类、坚果，减少红肉、加工肉制品和含糖饮料。\n\n二、坚持运动：每周至少150分钟中等强度有氧运动，或75分钟高强度有氧运动。快走、慢跑、游泳、骑行都是不错的选择。\n\n三、戒烟限酒：吸烟是心血管疾病的高危因素，戒烟后1年内心血管风险可降低50%。酒精摄入也应限制。\n\n四、控制体重：BMI应保持在18.5-24之间，腰围男性<90cm、女性<85cm。\n\n五、定期体检：每年至少检测一次血压、血脂、血糖，及早发现和控制危险因素。"
        }
    }
}
