import express, { Express, Request, Response } from 'express';
import { NTClient } from '../nt4/NTClient';
import { NTConsoleSink } from '../nt4/NTConsoleSink';
import { NTMessageType } from '../nt4/NTMessageLogger';
import { XeroApplet } from './XeroApplet';
import electron, { BrowserWindow, ipcMain } from 'electron';
import path from 'path';

const app = electron.app; 

export class XeroApp {
    private server: express | null ;
    private nettables: NTClient | null ;
    private applets: XeroApplet[] ;
    private restport: number ;
    private window: BrowserWindow | null ;

    public constructor() {
        this.server = null ;
        this.nettables = null ;
        this.applets = [] ;
        this.restport = -1 ;
        this.window = null ;
    }

    public addApplet(applet: XeroApplet) : void {
        this.applets.push(applet) ;
    }

    public httpAddress() : string {
        return "http://127.0.0.1:" + this.restport + "/" ;
    }

    public setURL(url: string) {
        if (this.window) {
            this.window.loadURL(url) ;
        }
    }

    public start(appid: string, ntaddr:string, ntport:number , restaddr: string, restport: number) {
        app.on('window-all-closed', () => { 
            if (process.platform !== 'darwin') {
                app.quit();
            }
        });

        app.on('ready', () => {
            this.restport = restport ;

            if (this.restport === -1) {
                this.restport = this.createRestPort() ;
            }
            this.server = new express() ;

            if (!this.isMultipleApplets()) {
                this.setupAppPaths() ;
            }

            this.server.listen(this.restport, restaddr, 4, () => {
                this.createNetworkTablesClient(appid, ntaddr, ntport) ;
            }) ;
        });
    }

    private isMultipleApplets() : boolean {
        return this.applets.length > 1 ;
    }

    private setupAppPaths() : void {
        let name = '/app/electron-tabs.js' ;
        this.server.get(name, (req, res) => {
            let filepath = path.join(__dirname, '../../node_modules/electron-tabs/dist/electron-tabs.js');
            let b: string = path.basename(filepath);
            res.contentType(b);
            res.sendFile(filepath);
        }) ;

        name = '/app/*' ;
        this.server.get(name, (req, res) => {
            let filepath = path.join(__dirname, '..', '..', 'content', req.path);
            let b: string = path.basename(filepath);
            res.contentType(b);
            res.sendFile(filepath);
        }) ;
    }

    private createNetworkTablesClient(appid: string, ntaddr: string, ntport: number) : boolean {
        if (this.nettables) {
            this.nettables.close() ;
            this.nettables = null ;
        }
        
        this.nettables = new NTClient(appid, ntaddr, ntport) ;
        let sink = new NTConsoleSink();
        this.nettables.getLogger().addSink(NTMessageType.Debug, sink) ;
        this.nettables.getLogger().addSink(NTMessageType.Info, sink) ;
        this.nettables.getLogger().addSink(NTMessageType.Warning, sink) ;
        this.nettables.getLogger().addSink(NTMessageType.Error, sink) ;

        this.nettables.connect() ;

        this.nettables!.on('connected', () => {
            this.createMainBrowserWindow() ;
            for(let applet of this.applets) {
                applet.start(this.nettables!, this.server!, this.isMultipleApplets());
            }
        }) ;

        return true ;
    }

    private getApplets() : Object[] {
        let list: Object[] = [] ;
        for(let applet of this.applets) {
            let obj = {
                url: applet.getStartFile(),
                title: applet.getTitle()
            }
            list.push(obj);
        }

        return list ;
    }

    private createMainBrowserWindow() : void {
        ipcMain.handle('applets:getall', () => {
            return this.getApplets() ;
        }) ;

        const preloadpath:string = path.join(__dirname, "preload.js");
        this.window = new BrowserWindow(
        { 
            width: 800, 
            height: 600,
            title: "XeroBoard",
            webPreferences: {
                nodeIntegration: false,
                contextIsolation: true,
                webviewTag: true,
                preload: preloadpath,
            }
        }) ;
        this.window.maximize();
        
        let docurl = this.httpAddress() + "app/index.html" ;
        this.window.loadURL(docurl);
    }

    private createRestPort() : number {
        return 9000 ;
    }
}