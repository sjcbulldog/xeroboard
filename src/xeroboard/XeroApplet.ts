import express, { Express, Request, Response } from 'express';
import path from 'path' ;
import { NTClient } from '../nt4/NTClient';
import { BrowserWindow } from 'electron';

export class XeroApplet {
    private name: string ;
    private nettable: NTClient | null ;
    private server: express | null ;
    private window: BrowserWindow | null ;

    public constructor(name: string) {
        this.nettable = null ;
        this.server = null ;
        this.window = null ;
        this.name = name ;
    }

    public start(nettable: NTClient, server: express) {
        this.nettable = nettable ;
        this.server = server ;
        this.window = new BrowserWindow(
            { 
                width: 800, 
                height: 600,
                webPreferences: {
                    nodeIntegration: false,
                    contextIsolation: true
                }
            }) ;
            
        const docurl = path.join(this.getContentDir(), 'html', this.getName() + ".html");
        this.window.loadURL(docurl);
    }

    public getName() : string {
        return this.name ;
    }

    protected getNetTables() : NTClient | null {
        return this.nettable ;
    }

    protected getServer() : express | null {
        return this.server ;
    }

    protected getContentDir() : string {
        return path.join(__dirname, '..', '..', 'content', 'applets', this.getName());
    }
}
