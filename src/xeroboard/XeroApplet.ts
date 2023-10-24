import express, { Express, Request, Response } from 'express';
import path from 'path' ;
import { NTClient } from '../nt4/NTClient';
import { XeroApp } from './XeroApp';
import { TabGroup } from 'electron-tabs';

export interface RestHandler { (req: Request<{}, any, any, any, Record<string, any>>, res: Response<any, Record<string, any>>): void }

export class XeroApplet {
    private name: string ;
    private nettable: NTClient | null ;
    private server: express | null ;
    private handlers: Map<string, RestHandler> ;
    private mainapp: XeroApp ;

    public constructor(mainapp: XeroApp, name: string) {
        this.nettable = null ;
        this.server = null ;
        this.name = name ;
        this.handlers = new Map<string, RestHandler>() ;
        this.mainapp = mainapp;
    }

    public start(nettable: NTClient, server: express, multiple: boolean) {
        this.nettable = nettable ;
        this.server = server ;
        this.setupServerPaths() ;

        if (!multiple) {
            this.mainapp.setURL(this.getStartFile()) ;
        }
    }

    public getName() : string {
        return this.name ;
    }

    public getStartFile() : string {
        return this.mainapp.httpAddress() + this.getName() + "/html/" + this.getName() + ".html" ;
    }

    //
    // Derived classes can provide a more meaningful title.  This is only used to
    // show to humans and never used for paths or other computed stuff.
    //
    public getTitle() : string {
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

    protected registerRestRequest(req: string, handler: RestHandler) {
        this.handlers.set(req, handler) ;
    }

    private restRequest(req, res) {
        let prefix : string = '/' + this.getName() + '/rest/' ;
        let key : string = req.path ;

        if (key.startsWith(prefix)) {
            key = key.substring(prefix.length) ;
        }

        let handler : RestHandler | undefined = this.handlers.get(key) ;
        if (handler) {
            handler(req, res) ;
        }
        else {
            let obj = { 
                error: "unknown rest request '" + req.path + "'"
            } ;             
            res.json(obj) ;
        }
    }

    private setupServerPaths() : void {
        let name = '/' + this.getName() + '/rest/*' ;
        this.server.get(name, (req, res) => {
            this.restRequest(req, res) ;
        }) ;

        name = '/' + this.getName() + '/*' ;
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
