
function loadTree() {
    parttree = new mar10.Wunderbaum({
        id: "tree",
        debugLevel: 1,
        element: document.getElementById("app"),
        source: {
            url: "/tree/rest/tables"
        },
        columns: [
            { id: "*", title: "Name", width: "160px" },
            { id: "dtype", title: "Type", width: "120px" },
            { id: "value", title: "Value"}
        ],
        render: function (e) {
            const node = e.node;
            node.tooltip = node.data.desc;
            for (const col of Object.values(e.renderColInfosById)) {
                col.elem.textContent = node.data[col.id];
            }
        }
    });
}

$(document).ready(loadTree);