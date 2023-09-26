import { BrowserWindow } from 'electron';
import { NTConnection } from './nt4/NTConnection';
import path from 'path';

export default class Main {
    static connection: NTConnection ;

    static mainWindow: Electron.BrowserWindow;
    static application: Electron.App;
    static BrowserWindow;
    private static onWindowAllClosed() {
        if (process.platform !== 'darwin') {
            Main.application.quit();
        }
    }

    private static onClose() {
        // Dereference the window object. 
    }

    private static onReady() {
        let filename: string = path.join(__dirname, "../content/index.html");
        Main.mainWindow = new Main.BrowserWindow({ width: 800, height: 600 });
        Main.mainWindow
            .loadURL('file://' + filename) ;
        Main.mainWindow.on('closed', Main.onClose);

        this.connection = new NTConnection("ws://127.0.0.1:5810/") ;
    }

    static main(app: Electron.App, browserWindow: typeof BrowserWindow) {
        // we pass the Electron.App object and the  
        // Electron.BrowserWindow into this function 
        // so this class has no dependencies. This 
        // makes the code easier to write tests for 
        Main.BrowserWindow = browserWindow;
        Main.application = app;
        Main.application.on('window-all-closed', Main.onWindowAllClosed);
        Main.application.on('ready', Main.onReady);
    }
}