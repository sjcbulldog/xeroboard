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
            
        let docurl = "http://127.0.0.1:9000/" + this.getName() + "/html/" + this.getName() + ".html" ;
        this.window.loadURL(docurl);

        this.setupServerPaths() ;
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

    private setupServerPaths() : void {
        const name = '/' + this.getName() + '/*' ;
        this.server.get(name, (req, res) => {
            let prefix: string = '/' + this.getName() ;
            let url:string = req.url ;
            if (url.startsWith(prefix)) {
                url = url.substring(prefix.length);
            }
            let filepath = path.join(this.getContentDir(), url);
            let b: string = path.basename(filepath);
            res.contentType(b);
            res.sendFile(filepath);
        }) ;

        this.server.get("/", (req, res) => {
            console.log("get: " + req.url);
        })
    }
}
