const { contextBridge, ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('electronAPI', {
  getAppletList: () => ipcRenderer.invoke('applets:getall')
})