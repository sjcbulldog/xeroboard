addEventListener("DOMContentLoaded", async (ev) => {
    //
    // Now get the app list to start
    //
    const applist = await window.electronAPI.getAppletList() ;
    const tabGroup = document.querySelector("tab-group");
    for(let tab of applist) {
        tabGroup.addTab({
            title: tab.title,
            src: tab.url
        });
    }
}) ;
