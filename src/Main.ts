import { BrowserWindow } from 'electron';
import path from 'path';
import { NTMessageLogger, NTMessageType } from './nt4/NTMessageLogger';
import { NTConsoleSink } from './nt4/NTConsoleSink';
import { NTClient } from './nt4/NTClient';
import { NTType, NTValue } from './nt4/NTValue';
import { NTTopic } from './nt4/NTTopic';

export default class Main {
    static client: NTClient ;
    static logger: NTMessageLogger ;

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
        Main.mainWindow.loadURL('file://' + filename) ;
        Main.mainWindow.on('closed', Main.onClose);

        this.client = new NTClient( 'xeroboard', '127.0.0.1', 5810) ;
        let sink = new NTConsoleSink();
        this.client.getLogger().addSink(NTMessageType.Debug, sink) ;
        this.client.getLogger().addSink(NTMessageType.Info, sink) ;
        this.client.getLogger().addSink(NTMessageType.Warning, sink) ;
        this.client.getLogger().addSink(NTMessageType.Error, sink) ;

        this.client.on("announce", (topic: NTTopic) => {
            if (topic.name_ === "/xyzzy") {
                const value: NTValue = new NTValue() ;
                value.initString("Hello World");
                this.client.setValue(topic, value) ;
            }
        }) ;

        this.client.on('connected', () => {
            this.client.subscribe(['/xyzzy'], { prefix: false, topicsonly: false });
            const lid : number = this.client.publish('/xyzzy', NTType.NT_STRING);
        }) ;
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