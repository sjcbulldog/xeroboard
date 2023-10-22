import express, { Express, Request, Response } from 'express';
import { NTClient } from '../nt4/NTClient';
import { NTConsoleSink } from '../nt4/NTConsoleSink';
import { NTMessageType } from '../nt4/NTMessageLogger';
import { XeroApplet } from './XeroApplet';
import electron from 'electron';

const app = electron.app; 

export class XeroApp {
    private server: express | null ;
    private nettables: NTClient | null ;
    private applets: XeroApplet[] ;

    public constructor() {
        this.server = null ;
        this.nettables = null ;
        this.applets = [] ;
    }

    public addApplet(applet: XeroApplet) : void {
        this.applets.push(applet) ;
    }

    public start(appid: string, ntaddr:string, ntport:number , restaddr: string, restport: number) {
        app.on('window-all-closed', () => { 
            if (process.platform !== 'darwin') {
                app.quit();
            }
        });

        app.on('ready', () => {
            this.connectNetworkTables(appid, ntaddr, ntport) ;
            this.startLocalServer(restaddr, restport);

            for(let applet of this.applets) {
                applet.start(this.nettables!, this.server);
            }
        });
    }

    public startLocalServer(restaddr: string, restport: number) : boolean {
        if (restport === -1) {
            restport = this.createRestPort() ;
        }
        this.server = new express() ;
        this.server.listen(restport, restaddr, 4, () => {});
        return true;
    }

    private connectNetworkTables(appid: string, ntaddr: string, ntport: number) : boolean {
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

        return true ;
    }

    private createRestPort() : number {
        return 9000 ;
    }
}